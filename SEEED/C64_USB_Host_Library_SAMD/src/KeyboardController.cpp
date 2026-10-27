/*
 Copyright (c) 2012 Arduino.  All right reserved.

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <KeyboardController.h>

extern "C" {
void __keyboardControllerEmptyCallback() { }
}

void keyPressed()  __attribute__ ((weak, alias("__keyboardControllerEmptyCallback")));
void keyReleased() __attribute__ ((weak, alias("__keyboardControllerEmptyCallback")));
void keyDisconnected() __attribute__ ((weak, alias("__keyboardControllerEmptyCallback")));

void KeyboardController::OnKeyDown(uint8_t _mod, uint8_t _oemKey) {
	modifiers = _mod;
	keyOem = _oemKey;
	key = OemToAscii(_mod, _oemKey);
	keyPressed();
}

void KeyboardController::OnKeyUp(uint8_t _mod, uint8_t _oemKey) {
	modifiers = _mod;
	keyOem = _oemKey;
	key = OemToAscii(_mod, _oemKey);
	keyReleased();
}

// Give each modifier a stable HID usage, independent of the other held bits.
void KeyboardController::OnControlKeysChanged(uint8_t before, uint8_t after) {
    for (uint8_t i = 0; i < 8; i++) {
        const uint8_t mask = uint8_t(1U << i);
        if ((before & mask) && !(after & mask)) OnKeyUp(after, 0xe0 + i);
    }
    for (uint8_t i = 0; i < 8; i++) {
        const uint8_t mask = uint8_t(1U << i);
        if (!(before & mask) && (after & mask)) OnKeyDown(after, 0xe0 + i);
    }
}

void KeyboardController::OnDisconnect() {
    // A new device must not inherit the previous keyboard's held usages.
    for (auto &byte : prevState.bInfo) byte = 0;
    kbdLockingKeys.bLeds = 0;
    key = 0;
    keyOem = 0;
    modifiers = 0;
    keyDisconnected();
}
