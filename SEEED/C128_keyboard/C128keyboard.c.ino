/*
  C128keyboard - Commodore Keyboard library

  Copyright (c) 2022 Hartland PC LLC
  Written by Robert VanHazinga

  Modified for C128 by Maciej 'YTM/Elysium' Witkowiak


    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

*/

#include <map>

#include "C128keyboard.h"

// static keymap definitions, include only once
#include "keymaps.h"

#include <ShiftRegister74HC595.h>
ShiftRegister74HC595<1> sr(DS_PIN, SH_CP_PIN, ST_CP_PIN); // data, clock, latch

void resetMT88(void) {

  digitalWrite(NMI_PIN, LOW);
  digitalWrite(C128_40_80_PIN, LOW);
  digitalWrite(C128_CAPS_KEY, LOW);

  digitalWrite(ANALOG_SW_DATA , LOW);
  digitalWrite(ANALOG_SW_RESET, HIGH);
  digitalWrite(ANALOG_SW_STROBE, HIGH);
  delay(20);
  digitalWrite(ANALOG_SW_RESET, LOW);
  digitalWrite(ANALOG_SW_STROBE, LOW);
  digitalWrite(ANALOG_SW_DATA , HIGH);

  lshift = false;
  rshift = false;
  shiftlock = false;
  virtshift = false;

  keymapnum = 0;
  modbuff = 0;
  joymode = false;
  joynum = 2;
  joykeycount = 0;
  kpress = false;
  c128_4080 = false;
  c128_caps = false;
}


void setswitch(uint8_t c,bool kstate) {
  int bitr;
  bool state;
  uint8_t ax3;

  ax3 = 0;
  digitalWrite(ANALOG_SW_DATA , kstate);
  // Fix logic table hole in MT8812/16
  // Convert x12 & x13 to x6 & x7. AX3 line control
  if ((c > 55 && c < 64) || (c > 23 && c < 32 )) {
     c -= 24;
     ax3 = 1;
  }
  // C128 extra keys use only 3 rows, so we don't reach x12 & x13 anyway but ax3 must be active for x8/x9/x10
  if (c >= 64) {
    c -= 64;
    ax3 = 1;
  }
  if (c < 64) {
    for (uint8_t i = 0; i<(sizeof(ANALOG_SH_ARRAY)/sizeof(ANALOG_SH_ARRAY[0])); i++) {
      state =  bitRead(c, i);
      sr.setNoUpdate(ANALOG_SH_ARRAY[i], state);
    }
    sr.setNoUpdate(ANALOG_SH_AX3, ax3);
    sr.updateRegisters(); // update the pins to the set values
    digitalWrite( ANALOG_SW_STROBE, HIGH);
    digitalWrite( ANALOG_SW_STROBE, LOW);
  }

  // Reset switch state pin to default high state (Key press).
  //digitalWrite(ANALOG_SW_DATA , HIGH);
}

//////////////////////////////////////////////////////////////

const char* get_mapped_key_name(auto keycode) {
      if (keycode & MOD_SHIFT) { keycode -= MOD_SHIFT; };
      for (auto i=0;i<key_infos_nsize;i++) {
        if (key_infos[i].get_keycode() == keycode) {
          return key_infos[i].get_name();
        }
      }
      return NULL;
}

//////////////////////////////////////////////////////////////

// This map will store the original keycode for each pressed key
std::map<uint8_t, uint16_t> keyStateTable;

constexpr const uint8_t C128keyboard::getNumKeyMaps(void) { return nkeymaps; };

const char* C128keyboard::getCurrentKeyMapName(void) { return keymaps[keymapnum].getName(); };

uint16_t C128keyboard::mapkeycode(uint8_t kc, uint8_t mod) {

    if (joymode && joynum==1) {
      // in joystick mode map numpad 4/6/2/8 to directions and 5/numpad 0/right control to FIRE
      // pass through to current keymap for any other key
      if (bitRead(mod,4) != bitRead(modbuff,4)) { // bit 4 / 0x10 is right CTRL
        modbuff = mod;
        return JOY1_FIRE;
      }
      switch(kc) {
        case 0x60:  // keypad 8
          return JOY1_UP;
          break;
        case 0x5a:  // keypad 2
          return JOY1_DOWN;
          break;
        case 0x5c:  // keypad 4
          return JOY1_LEFT;
          break;
        case 0x5e:  // keypad 6
          return JOY1_RIGHT;
          break;
        case 0x5d:  // keypad 5
        case 0x62:  // keypad 0
          return JOY1_FIRE;
          break;
        default:
          break;
      }
    }

    if (joymode && joynum==2) {
      // in joystick mode map numpad 4/6/2/8 to directions and 5/numpad 0/right control to FIRE
      // pass through to current keymap for any other key
      if (bitRead(mod,4) != bitRead(modbuff,4)) { // bit 4 / 0x10 is right CTRL
        modbuff = mod;
        return JOY2_FIRE | MOD_JOY2;
      }
      switch(kc) {
        case 0x60:  // keypad 8
          return JOY2_UP | MOD_JOY2;
          break;
        case 0x5a:  // keypad 2
          return JOY2_DOWN | MOD_JOY2;
          break;
        case 0x5c:  // keypad 4
          return JOY2_LEFT | MOD_JOY2;
          break;
        case 0x5e:  // keypad 6
          return JOY2_RIGHT | MOD_JOY2;
          break;
        case 0x5d:  // keypad 5
        case 0x62:  // keypad 0
          return JOY2_FIRE | MOD_JOY2;
          break;
        default:
          break;
      }
    }

    // handle modifier keys
    if (mod != modbuff) {
      uint16_t c;
      auto MODKEYS = keymaps[keymapnum].getMods();
      for (uint8_t i=0;i<8; i++) {
        if (bitRead(modbuff,i) != bitRead(mod,i)) {
          c = MODKEYS[i];
          modbuff = mod;
          return c;
        }
      }
    }

    // scan for any other key
    return keymaps[keymapnum].getKey(kc).get(lshift || rshift || shiftlock || virtshift);
}


void C128keyboard::c64key(uint16_t c, bool kpress, uint8_t kc) {

    // return early
    if (c == IGNORE_KEYCODE) return;

    // update shift states
    if (c == KEY_LSHIFT) lshift = kpress;
    if (c == KEY_RSHIFT) rshift = kpress;

    // lshift key release can be ignored if shiftlock/virtual shift are still on
    if (!kpress && c == KEY_LSHIFT && (shiftlock || virtshift)) {
      return;
    }

    if (c & MOD_JOY2) {
      c -= MOD_JOY2;
      // change count
      if (kpress) { joykeycount++; } else { joykeycount--; };
      if ((kpress && joykeycount==1) || (!kpress && joykeycount==0)) {
        // add/remove route between X11 and Y6/Y7
        // only after first press or after last release
        setswitch(JOY2_ROUTE_GND, kpress);
      }
    }

    if (kpress) {
        // Map keycode and store in key state table
        keyStateTable[kc] = c;
    } else {
        // Key release event
        // Retrieve keycode from key state table
        // Retrieve the stored keycode
        auto it = keyStateTable.find(kc);
        if (it != keyStateTable.end()) {
            c = it->second;
            keyStateTable.erase(it);  // Remove the key from the state table
        } else {
            // If the key wasn't found, ignore the release event
            return;
        }
    }

    // Differential shift conversion
    if (c & MOD_SHIFT ) {
      c -= MOD_SHIFT;
      if (kpress) {
        // setup shift key state if this is key press
        if (lshift || rshift || shiftlock || virtshift) { // if any of the shifts is active right now
          setswitch(KEY_RSHIFT,LOW); // disable all of them
          setswitch(KEY_LSHIFT,LOW);
        } else {
          setswitch(KEY_LSHIFT,HIGH); // if any of the shifts is inactive right now, enable left shift
        }
      } else {
      // Restore shift states if this was key release
        if (!kpress) {
          setswitch(KEY_LSHIFT, lshift || shiftlock || virtshift);
          setswitch(KEY_RSHIFT, rshift);
        }
      }
    }

    // Process normal keypress
    if (c != IGNORE_KEYCODE){
      setswitch(c & 0xff,kpress);
    }
}


void C128keyboard::clearAllKeys(void) {
  // MT88xx RESET substitute, just go through all the switches in the matrix and clear them
  for (uint8_t c=0;c<128;c++) { // we have 7 address bits only (AX0..3, AY0..2)
    setswitch(c,LOW);
  }
}


C128keyboard::C128keyboard() {

  // initialize the pins
  pinMode(ANALOG_SW_STROBE, OUTPUT);  // MT88XX strobe
  pinMode(ANALOG_SW_DATA, OUTPUT);   // MT88XX data
  pinMode(ANALOG_SW_RESET, OUTPUT); // MT88XX reset

  pinMode(NMI_PIN, OUTPUT);
  pinMode(C128_40_80_PIN, OUTPUT);
  pinMode(C128_CAPS_KEY, OUTPUT);

  resetMT88();

}
