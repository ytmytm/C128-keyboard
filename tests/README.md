# Keyboard event regression tests

Run from the repository root:

```sh
python3 tests/test_keyboard_events.py
```

Requires Python 3 and GCC's g++ with AddressSanitizer and
UndefinedBehaviorSanitizer. Compilation uses `-Wall -Wextra -Werror`;
warnings fail the build. The language standard is C++14, matching the
[Seeed SAMD compiler configuration](https://github.com/Seeed-Studio/ArduinoCore-samd/blob/master/platform.txt).
Compile-time assertions also verify key lookup and modifier table sizes.

The Python runner reads functions directly from the firmware and compiles them
with the C++ scenarios in `keyboard_events.cpp`. It uses the real controller
class declaration, parser constructor and Parse implementation, controller
callbacks, the HID device release path, key maps, matrix event handling, constructor, reset and key-action callbacks.
Hardware I/O, USB transport, OLED output and ASCII conversion are mocked.
Temporary build files are removed automatically, including on test failure.

Tests cover:

- All 262,144 pairs of modifier masks across four keymaps, checking the expected
  matrix state, physical Shift flags and RESTORE pin after each transition.
- Ctrl + either Shift in both press/release orders, simultaneous changes,
  independent Shifts, and typing while Shift remains held.
- Six occupied HID slots, slot reordering, key replacement, repeated reports,
  reserved bytes, unknown usages, every error usage in every key slot, and
  report lengths from zero to seven bytes.
- Differential Shift in symbolic mappings and synthesized Shift for cursor
  and function keys; overlapping differential keys and Shift Lock with physical Shift.
- Both joystick ports, all directions, diagonals, three FIRE inputs, shared
  FIRE releases, route/count lifetime, and entering/leaving modes while held.
- Saved releases after changing a mapping to another key, IGNORE or an action.
- Reset, keymap switching and wrapping, momentary RESTORE, toggled 40/80 and
  CAPS pins (including reset), shared RESTORE sources, valid GPIO pin numbers,
  duplicate presses and unmatched releases.
- RESTORE versus DEL isolation and exact Shift/key write ordering.
- Disconnect with held modifiers, normal keys, FIRE, RESTORE and Shift Lock;
  reconnect with the same keys held; repeated releases and interfaces without
  a parser. The real HIDBoot release callback path is used with mocked USB I/O.

The transition count is a count of exercised cases, not a source coverage
percentage. These tests do not build the full Arduino sketch or verify
transport timing or electrical behavior.

For a hardware check, install the modified bundled
`SEEED/C64_USB_Host_Library_SAMD` library and rebuild the sketch; rebuilding
with an older installed library will not include the parser fix. Check Shift +
left Ctrl (C= in positional mode) in both press/release orders, then right Ctrl
and keypad 5 held together in each joystick mode. Releasing either FIRE input
must leave FIRE active until the other is also released.

The old `C128-keyboard-issue-3-shift.patch` is an alternative workaround and
must not be applied on top of the modifier event fix.
