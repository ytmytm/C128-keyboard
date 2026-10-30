/*
  C64keyboard - Commodore Keyboard library

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


#ifndef C128keyboard_h
#define C128keyboard_h

#include <array>

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

//MT88xx control pins
#define ANALOG_SW_DATA      3 // Sets selected cross switch on/off
#define ANALOG_SW_STROBE    6 // Strobe timing pulse
#define ANALOG_SW_RESET     7 // Reset all MT88XX cross switches to off

// I2C DISPLAY
#define SDA_PIN             4 // just for reference, native SEEDUINO XIAO doesn't need these defines
#define SCL_PIN             5

// 74HC595 shift register pins
#define SH_CP_PIN           0 // SRCLK // 11th pin // clock
#define ST_CP_PIN           1 // RCLK  // 12th pin // latch
#define DS_PIN              2 // SER   // 14th pin // data

// bits coming out of shift register connected to MT8812/16
#define ANALOG_SH_AX0     0 // QA (15)
#define ANALOG_SH_AX1     1 // QB (1)
#define ANALOG_SH_AX2     2 // ...
#define ANALOG_SH_AY0     3
#define ANALOG_SH_AY1     4
#define ANALOG_SH_AY2     5
#define ANALOG_SH_AX3     6 // QG (6)
const uint8_t ANALOG_SH_ARRAY[] = { ANALOG_SH_AY2, ANALOG_SH_AY1, ANALOG_SH_AY0, ANALOG_SH_AX2, ANALOG_SH_AX1, ANALOG_SH_AX0 }; // ANALOG_SH_AX3 would be next, but is handled separately
// last, 7th bit unused

// these can't be driven directly, they need to go through transistors for open collector functionality (or 74'xx equivalent chip)
// C128 NMI setup
#define NMI_PIN           8
// C128 40/80
#define C128_40_80_PIN    9
// C128 CAPS
#define C128_CAPS_PIN     10

// C64/128 key codes / USB key codes defines
#include "keymapping.h"

class C64KeyInfo {
  public:
    constexpr C64KeyInfo(const uint16_t c, const char* name) :
    m_keycode(c), m_name(name)
    {};
    constexpr auto get_keycode() const { return m_keycode; }
    constexpr const char* get_name() const { return m_name; };
  private:
    const uint16_t m_keycode;
    const char *m_name;
};

class C128keyboard {
  public:
    C128keyboard();
    void c64key(uint16_t c, bool kpress, uint8_t kc);
    uint16_t mapkeycode(uint8_t kc);
    void clearAllKeys(void);
    void updateShiftSwitches(void);
    constexpr uint8_t getNumKeyMaps(void);
    const char* getCurrentKeyMapName(void);
  private:
    // A saved mapping can drive one matrix switch or a joystick diagonal.
    // Actions such as RESTORE must never alias a physical matrix address.
    static constexpr std::array<uint8_t, 2> getMatrixSwitches(uint16_t mapped) {
      const uint16_t primary = mapped & ~(MOD_SHIFT | MOD_JOY2 | MOD_JOY_UP | MOD_JOY_DOWN);
      if (primary >= 128) return {{0xff, 0xff}};
      const bool port2 = mapped & MOD_JOY2;
      uint8_t vertical = 0xff;
      if (mapped & MOD_JOY_UP) vertical = port2 ? JOY2_UP : JOY1_UP;
      else if (mapped & MOD_JOY_DOWN) vertical = port2 ? JOY2_DOWN : JOY1_DOWN;
      return {{static_cast<uint8_t>(primary), vertical}};
    }
};

class C64Key {
  public:
    constexpr C64Key(const int16_t keycode, const uint16_t unshifted, const uint16_t shifted) :
    m_keycode(keycode), m_unshifted(unshifted), m_shifted(shifted)
    {};
    constexpr auto get_keycode() const { return m_keycode; }
    constexpr uint16_t get(bool shifted = false) const {
      return shifted ? m_shifted : m_unshifted;
    }
  private:
    const int16_t m_keycode;
    const uint16_t m_unshifted, m_shifted;
};

using ModifierMap = std::array<uint16_t, 8>;

class C64KeyMap {
  public:
    constexpr C64KeyMap(const char* name, const C64Key *map, const uint16_t mapsize, const ModifierMap &mods) :
    m_name(name), m_map(map), m_mapsize(mapsize), m_mods(mods)
    {};
    constexpr const char* getName() const { return m_name; };
    constexpr const ModifierMap &getMods() const { return m_mods; };
    constexpr C64Key getKey(uint8_t keycode) const {
      for (uint16_t i = 0; i < m_mapsize; ++i) {
        if (m_map[i].get_keycode() == keycode) {
          return m_map[i];
        }
      }
      return C64Key(0, IGNORE_KEYCODE, IGNORE_KEYCODE);
    }
  private:
    const char *m_name;
    const C64Key *m_map;
    const uint16_t m_mapsize;
    const ModifierMap &m_mods;
};

#endif
