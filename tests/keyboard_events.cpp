// Compiled by test_keyboard_events.py together with functions read from firmware.
void report(uint8_t modifiers, std::initializer_list<uint8_t> keys = {}) {
    uint8_t data[8] = {modifiers, 0};
    unsigned i = 2;
    for (auto key : keys) { assert(i < 8); data[i++] = key; }
    keyboard.Parse(nullptr, 0, 8, data);
}
void clean() {
    assert(keyStateTable.empty());
    assert(!lshift && !rshift);
    assert(joykeycount == 0);
    for (const auto &entry : matrix) assert(!entry.second);
}
int main() {
    C128keyboard instance;
    ckey = &instance;
    // Both press orders, both release orders, and simultaneous transitions.
    for (uint8_t shift : {2, 32}) {
        for (bool ctrlFirst : {false, true}) {
            for (bool ctrlReleasedFirst : {false, true}) {
                report(ctrlFirst ? 1 : shift);
                report(1 | shift);
                assert(matrix[KEY_CBM]);
                assert(matrix[shift == 2 ? KEY_LSHIFT : KEY_RSHIFT]);
                report(ctrlReleasedFirst ? shift : 1);
                assert(matrix[KEY_CBM] == !ctrlReleasedFirst);
                assert(matrix[shift == 2 ? KEY_LSHIFT : KEY_RSHIFT] == ctrlReleasedFirst);
                report(0);
                clean();
            }
        }
        report(1 | shift);
        assert(matrix[KEY_CBM]);
        report(0);
        clean();
    }
    // Every modifier transition in one report must be handled, not just one bit.
    report(0xff);
    assert(matrix[KEY_CBM] && matrix[KEY_LSHIFT] && matrix[KEY_RSHIFT]);
    assert(matrix[C128_KEY_DOWN] && matrix[C128_KEY_UP] && matrix[C128_KEY_RIGHT]);
    for (const auto &entry : keyStateTable) assert(entry.first >= 0xe0 && entry.first <= 0xe7);
    report(0);
    clean();
    // Normal keys in the same report as modifier changes, in every keymap.
    for (keymapnum = 0; keymapnum < nkeymaps; keymapnum++) {
        report(2, {4});
        assert(lshift && matrix[KEY_A]);
        report(0);
        clean();
    }
    keymapnum = 0;
    // Left and right Shift are independent, including their release order.
    for (bool leftReleasedFirst : {false, true}) {
        report(0x22);
        assert(lshift && rshift && matrix[KEY_LSHIFT] && matrix[KEY_RSHIFT]);
        report(leftReleasedFirst ? 0x20 : 0x02);
        assert(lshift == !leftReleasedFirst && rshift == leftReleasedFirst);
        assert(matrix[KEY_LSHIFT] == !leftReleasedFirst);
        assert(matrix[KEY_RSHIFT] == leftReleasedFirst);
        report(0);
        clean();
    }
    // Releasing ordinary keys must leave the held Shift active.
    for (uint8_t shift : {2, 32}) {
        report(shift);
        for (uint8_t key : {4, 5, 6}) {
            report(shift, {key});
            assert(keyStateTable.count(key));
            report(shift);
            assert(keyStateTable.size() == 1);
            assert(matrix[shift == 2 ? KEY_LSHIFT : KEY_RSHIFT]);
        }
        report(0);
        clean();
    }
    // Release must use the saved mapping if Ctrl changes from C= to CTRL.
    report(1);
    assert(matrix[KEY_CBM]);
    keymapnum = 1;
    report(0);
    assert(!matrix[KEY_CBM] && !matrix[KEY_CTRL]);
    clean();
    keymapnum = 0;
    // Slot permutations do not create key transitions.
    report(0, {4, 5});
    report(0, {5, 4});
    assert(keyStateTable.size() == 2);
    report(0);
    clean();
    // A repeated report produces no extra presses, releases or matrix writes.
    report(3, {4});
    const auto savedKeys = keyStateTable;
    const auto savedMatrix = matrix;
    const auto savedWrites = matrixWrites;
    report(3, {4});
    assert(keyStateTable == savedKeys && matrix == savedMatrix);
    assert(matrixWrites == savedWrites);
    report(0);
    clean();
    // Rollover/error and short reports must preserve the last valid state.
    report(1, {4});
    report(0, {1});
    assert(matrix[KEY_CBM] && matrix[KEY_A]);
    uint8_t shortReport[2] = {};
    keyboard.Parse(nullptr, 0, 2, shortReport);
    assert(matrix[KEY_CBM] && matrix[KEY_A]);
    report(0);
    clean();
    // Shift Lock must not overwrite the physical Shift state or swallow its release.
    report(2);
    report(2, {0x47});
    assert(shiftlock && lshift);
    report(0);
    assert(!lshift && matrix[KEY_LSHIFT] && keyStateTable.empty());
    report(0, {0x47});
    report(0);
    assert(!shiftlock);
    clean();
    // Right Ctrl and keypad FIRE share a matrix switch; either can release first.
    for (joynum = 1; joynum <= 2; joynum++) {
        joymode = true;
        for (bool ctrlReleasedFirst : {false, true}) {
            const uint8_t fire = joynum == 1 ? JOY1_FIRE : JOY2_FIRE;
            report(0x12, {0x5d});
            assert(matrix[fire] && lshift);
            assert(joykeycount == (joynum == 2 ? 2 : 0));
            if (ctrlReleasedFirst) report(2, {0x5d});
            else report(0x12);
            assert(matrix[fire]);
            assert(joykeycount == (joynum == 2 ? 1 : 0));
            report(0);
            clean();
        }
        // Release uses the mapping at press time after leaving joystick mode.
        report(0x10);
        joymode = false;
        report(0);
        clean();
    }
    joynum = 2;
    // Toggling joystick mode while a key is held must not reset its count.
    joymode = true;
    report(0x10);
    assert(joykeycount == 1);
    report(0x10, {0x53});
    assert(!joymode && joykeycount == 1);
    report(0);
    clean();
    // Reset clears saved physical usages, allowing the same key to work again.
    report(1);
    ckey->clearAllKeys();
    report(0);
    clean();
    report(1);
    assert(matrix[KEY_CBM]);
    report(0);
    clean();
    // No HID usage may collide with the modifier mask (Ctrl bit equals usage A).
    report(4, {4});
    assert(matrix[KEY_A] && matrix[C128_KEY_DOWN]);
    report(0, {4});
    assert(matrix[KEY_A] && !matrix[C128_KEY_DOWN]);
    report(0);
    clean();
    // Exhaust every pair of modifier masks against independent expected mappings.
    // This covers aliases (both Ctrls -> CTRL, both GUIs -> C=) and mixed releases.
    const uint16_t modifierTargets[2][8] = {
        {KEY_CBM, KEY_LSHIFT, C128_KEY_DOWN, C128_KEY_UP, C128_KEY_RIGHT,
         KEY_RSHIFT, IGNORE_KEYCODE, IGNORE_KEYCODE},
        {KEY_CTRL, KEY_LSHIFT, C128_KEY_ALT, KEY_CBM, KEY_CTRL,
         KEY_RSHIFT, IGNORE_KEYCODE, KEY_CBM}
    };
    unsigned modifierCases = 0;
    for (keymapnum = 0; keymapnum < nkeymaps; keymapnum++) {
        for (unsigned before = 0; before < 256; ++before) {
            for (unsigned after = 0; after < 256; ++after) {
                report(before);
                report(after);
                bool expected[128] = {};
                for (unsigned bit = 0; bit < 8; ++bit) {
                    const auto target = modifierTargets[keymapnum % 2][bit];
                    if (target < 128 && (after & (1U << bit))) expected[target] = true;
                }
                for (const auto &entry : matrix) assert(entry.second == expected[entry.first]);
                assert(lshift == bool(after & 2));
                assert(rshift == bool(after & 32));
                assert(pins[NMI_PIN] == bool(after & 64));
                report(0);
                clean();
                ++modifierCases;
            }
        }
    }
    keymapnum = 0;
    std::cout << "PASS: " << modifierCases << " exhaustive modifier transitions\n";

    // Six-key rollover, slot changes and replacement without an all-keys-up report.
    report(0, {4, 5, 6, 7, 8, 9});
    assert(keyStateTable.size() == 6);
    const auto writesBeforeReorder = matrixWrites;
    report(0, {9, 8, 7, 6, 5, 4});
    assert(matrixWrites == writesBeforeReorder);
    report(0, {4, 5, 6, 7, 8, 10});
    assert(!matrix[KEY_F] && matrix[KEY_G]);
    report(0);
    clean();

    // All HID error usages in every slot, and all short report lengths.
    report(3, {4, 5});
    const auto keysBeforeInvalid = keyStateTable;
    const auto writesBeforeInvalid = matrixWrites;
    for (uint8_t error : {1, 2, 3}) {
        for (unsigned slot = 2; slot < 8; ++slot) {
            uint8_t data[8] = {};
            data[slot] = error;
            keyboard.Parse(nullptr, 0, 8, data);
        }
    }
    uint8_t emptyReport[8] = {};
    for (unsigned len = 0; len < 8; ++len) keyboard.Parse(nullptr, 0, len, emptyReport);
    assert(keyStateTable == keysBeforeInvalid && matrixWrites == writesBeforeInvalid);
    report(0);
    clean();

    // Reserved byte is not a usage, even if it contains the code of a held key.
    uint8_t reservedReport[8] = {0, 4};
    const auto writesBeforeReserved = matrixWrites;
    keyboard.Parse(nullptr, 0, 8, reservedReport);
    assert(matrixWrites == writesBeforeReserved);
    report(0);
    report(0, {0xff, 0x4e}); // unknown usage and explicitly ignored PageDown
    assert(keyStateTable.empty());
    report(0);
    clean();

    // Differential Shift: F2 synthesizes Shift; symbolic Shift+2 removes it.
    for (keymapnum = 0; keymapnum < nkeymaps; ++keymapnum) {
        report(0, {uint8_t(keymapnum == 0 ? 0x50 : 0x3b)});
        assert(matrix[keymapnum == 0 ? KEY_HORZ_CSR : KEY_F1]);
        assert(matrix[KEY_LSHIFT] && !lshift);
        report(0);
        clean();
        if (keymapnum % 2 == 1) {
            for (uint8_t shift : {2, 32}) {
                report(shift, {0x1f});
                assert(matrix[KEY_AT] && !matrix[KEY_LSHIFT] && !matrix[KEY_RSHIFT]);
                report(shift);
                assert(!matrix[KEY_AT]);
                assert(matrix[shift == 2 ? KEY_LSHIFT : KEY_RSHIFT]);
                report(0);
                clean();
            }
        }
    }
    keymapnum = 0;

    // Two differential keys share synthetic Shift until both have been released.
    keymapnum = 2;
    report(0, {0x3b, 0x3d}); // F2 and F4 -> shifted F1 and F3
    assert(matrix[KEY_F1] && matrix[KEY_F3] && matrix[KEY_LSHIFT]);
    report(0, {0x3d});
    assert(!matrix[KEY_F1] && matrix[KEY_F3] && matrix[KEY_LSHIFT]);
    report(0);
    clean();
    // A physical Shift release cannot cancel another key's differential mapping.
    keymapnum = 1;
    report(0x22, {0x1f});
    assert(matrix[KEY_AT] && !matrix[KEY_LSHIFT] && !matrix[KEY_RSHIFT]);
    report(2, {0x1f});
    assert(lshift && !rshift && !matrix[KEY_LSHIFT] && !matrix[KEY_RSHIFT]);
    report(2);
    assert(matrix[KEY_LSHIFT] && !matrix[KEY_AT]);
    report(0);
    clean();
    keymapnum = 0;

    // Shift Lock off while physical Shift is held must keep that Shift down.
    report(0, {0x47});
    report(2);
    report(2, {0x47});
    assert(!shiftlock && lshift && matrix[KEY_LSHIFT]);
    report(0);
    clean();

    // All joystick directions plus keypad FIRE, including diagonals and releases.
    const uint8_t joyUsages[] = {0x60, 0x5a, 0x5c, 0x5e, 0x5d, 0x62};
    const uint8_t joyTargets[2][6] = {
        {JOY1_UP, JOY1_DOWN, JOY1_LEFT, JOY1_RIGHT, JOY1_FIRE, JOY1_FIRE},
        {JOY2_UP, JOY2_DOWN, JOY2_LEFT, JOY2_RIGHT, JOY2_FIRE, JOY2_FIRE}
    };
    for (joynum = 1; joynum <= 2; ++joynum) {
        joymode = true;
        for (unsigned i = 0; i < 6; ++i) {
            report(0, {joyUsages[i]});
            assert(matrix[joyTargets[joynum - 1][i]]);
            assert(joykeycount == (joynum == 2 ? 1 : 0));
            report(0);
            clean();
        }
        report(0, {0x60, 0x5e, 0x62});
        assert(matrix[joyTargets[joynum - 1][0]]);
        assert(matrix[joyTargets[joynum - 1][3]]);
        assert(matrix[joyTargets[joynum - 1][5]]);
        report(0, {0x5e});
        assert(!matrix[joyTargets[joynum - 1][0]]);
        assert(matrix[joyTargets[joynum - 1][3]]);
        assert(!matrix[joyTargets[joynum - 1][5]]);
        assert(matrix[JOY2_ROUTE_GND] == (joynum == 2));
        report(0);
        clean();
        joymode = false;
    }
    joynum = 2;

    // Enter joystick mode with right Ctrl already held as a keyboard cursor.
    report(0x10);
    assert(matrix[C128_KEY_RIGHT]);
    report(0x10, {0x53});
    assert(joymode && joynum == 2 && joykeycount == 0);
    report(0);
    clean();
    joymode = false;

    // Shift+NumLock selects joystick 1; a repeated report must not toggle it.
    report(2, {0x53});
    assert(joymode && joynum == 1);
    report(2, {0x53});
    assert(joymode && joynum == 1);
    report(0);
    clean();
    report(2, {0x53});
    assert(!joymode && joynum == 1);
    report(0);
    clean();

    // Releasing a held key still works if its new mapping is ignored or an action.
    report(0, {0x42}); // F9 -> F1 in C128 positional
    assert(matrix[KEY_F1]);
    keymapnum = 2;     // F9 ignored in C64 positional
    report(0);
    clean();
    keymapnum = 0;
    report(0, {0x44}); // F11 -> F5 in C128 positional
    assert(matrix[KEY_F5]);
    keymapnum = 2;     // F11 -> 40/80 action in C64 positional
    report(0);
    assert(!c128_4080);
    clean();
    keymapnum = 0;

    // Matrix reset uses the firmware implementation and removes saved states.
    report(3, {4});
    report(3, {4, 0x46});
    assert(!lshift && !rshift && !shiftlock && !virtshift);
    assert(!joymode && joynum == 2 && keymapnum == 0);
    report(0);
    clean();

    // Keymap selection, including wrapping, is an action only on key-down.
    for (unsigned i = 0; i < nkeymaps; ++i) {
        report(0, {0x48});
        assert(keymapnum == (i + 1) % nkeymaps);
        report(0, {0x48});
        assert(keymapnum == (i + 1) % nkeymaps);
        report(0);
        clean();
    }
    // Independent control pins: RESTORE momentary, 40/80 and CAPS toggled.
    report(0x40);
    assert(pins[NMI_PIN]);
    report(0);
    assert(!pins[NMI_PIN]);
    report(0, {0x4b});
    assert(pins[NMI_PIN]);
    report(0);
    assert(!pins[NMI_PIN]);
    for (bool enabled : {true, false}) {
        report(0, {0x40});
        assert(c128_4080 == enabled && pins[C128_40_80_PIN] == enabled);
        report(0, {0x40});
        assert(c128_4080 == enabled);
        report(0);
    }
    keymapnum = 1;
    for (bool enabled : {true, false}) {
        report(0, {0x45});
        assert(c128_caps == enabled && pins[C128_CAPS_PIN] == enabled);
        report(0);
    }
    keymapnum = 0;
    clean();

    // Two physical sources can hold RESTORE; check both release orders.
    for (bool altReleasedFirst : {false, true}) {
        report(0x40, {0x4b});
        assert(pins[NMI_PIN]);
        if (altReleasedFirst) report(0, {0x4b});
        else report(0x40);
        assert(pins[NMI_PIN]);
        report(0);
        assert(!pins[NMI_PIN]);
        clean();
    }
    // Reset must clear the actual CAPS pin, rather than a key action code.
    keymapnum = 1;
    report(0, {0x45});
    report(0x40);
    assert(c128_caps && pins[C128_CAPS_PIN] && pins[NMI_PIN]);
    report(0x40, {0x46});
    assert(!c128_caps && !pins[C128_CAPS_PIN] && !pins[NMI_PIN]);
    report(0);
    keymapnum = 0;
    clean();

    // Defensive API behavior: duplicate presses and unmatched releases are harmless.
    ckey->c64key(KEY_A, true, 4);
    const auto writesBeforeDuplicate = matrixWrites;
    ckey->c64key(KEY_A, true, 4);
    ckey->c64key(KEY_B, false, 5);
    assert(matrixWrites == writesBeforeDuplicate);
    assert(matrix[KEY_A] && keyStateTable.size() == 1);
    ckey->c64key(IGNORE_KEYCODE, false, 4);
    clean();
    std::cout << "PASS: six slots, invalid reports, differential Shift, directions, reset, keymap and control pins\n";
    std::cout << "PASS: modifier orders, simultaneous changes, both Shifts, typing, all keymaps, slots, repeated reports, errors, Shift Lock, joystick FIRE and saved releases\n";
}
