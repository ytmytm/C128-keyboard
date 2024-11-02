
// IDE: Seeeduino XIAO SAMD

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

#include "C128keyboard.h"


#include <KeyboardController.h>


// Initialize USB Controller
USBHost usb;

// Attach keyboard controller to USB
KeyboardController keyboard(usb);


C128keyboard *ckey;
static volatile uint8_t kc, mod, modbuff;
static volatile uint16_t kc_mapped;
static volatile uint8_t keymapnum = 0; // current keymap number

static volatile bool joymode=false;
static volatile uint8_t joynum=2, joykeycount=0;
static volatile bool kpress=false, c128_4080 = false, c128_caps = false;
static volatile bool lshift=false, rshift=false, shiftlock=false, virtshift=false; // 4 shift states: left, right, lock(left), virtual (from mapping)

static volatile unsigned long lastPressedMillis = 0, currentMillis = 0;
constexpr const unsigned long inactivePeriodMillis = 5000; // keep the message on OLED for 5 seconds after a keypress

//////////////////////////////////////////////////////////////
// I2C
#include <Wire.h>

//////////////////////////////////////////////////////////////
// OLED display
// 128x32
// https://github.com/olikraus/u8g2

bool displayPresent = false;

#include <U8g2lib.h>

U8G2_SSD1306_128X32_UNIVISION_1_HW_I2C u8g2(U8G2_R0);

//////////////////////////////////////////////////////////////

void handle_c128_4080() {
    if (c128_4080) {
      digitalWrite (C128_40_80_PIN, HIGH);
      displayState("80col");
    } else {
      digitalWrite (C128_40_80_PIN, LOW);
      displayState("40col");
    }
}

void handle_c128_caps() {
    if (c128_caps) {
      digitalWrite (C128_CAPS_PIN, HIGH);
      displayState("CAPS ON");
    } else {
      digitalWrite (C128_CAPS_PIN, LOW);
      displayState("CAPS OFF");
    }
}

void handle_joymode() {
  if (joymode) {
    String s;
    s = "joy #" + String(joynum) + " mode ON";
    displayState(s.c_str());
  } else {
    displayState("joy mode OFF");
  }
}

void next_keymap() {
    keymapnum++;
    if (keymapnum == ckey->getNumKeyMaps()) keymapnum = 0;
    displayKeyMap();
}

// This function intercepts key press
void keyPressed() {
  lastPressedMillis = currentMillis;
  kc = keyboard.getOemKey();
  mod = keyboard.getModifiers();
  kpress = true;
  kc_mapped = ckey->mapkeycode(kc,mod);
  displayKey();

  // handle special key functions
  switch (kc_mapped) {
    case IGNORE_KEYCODE:
      break;
    case KEY_MT_RESET:
      resetMT88();
      ckey->clearAllKeys();
      displayState("reset MT");
      break;
    case KEY_KEYMAP:
      next_keymap();
      break;
    case KEY_SHIFT_LOCK:
      shiftlock = !shiftlock;
      ckey->c64key(KEY_LSHIFT, shiftlock);
      if (shiftlock) {
        displayState("shift lock on");
      } else {
        displayState("shift lock off");
      }
      break;
    case KEY_RESTORE:
      digitalWrite (NMI_PIN, HIGH);
      displayState("NMI");
      break;
    case C128_40_80_KEY:
      c128_4080 = !c128_4080;
      handle_c128_4080();
      break;
    case C128_CAPS_KEY:
      c128_caps = !c128_caps;
      handle_c128_caps();
      break;
    case KEY_JOY1_MODE:
      joymode = !joymode;
      joykeycount = 0;
      joynum = 1;
      handle_joymode();
      break;
    case KEY_JOY2_MODE:
      joymode = !joymode;
      joykeycount = 0;
      joynum = 2;
      handle_joymode();
      break;
    default:
      ckey->c64key(kc_mapped, kpress);
      break;
  }
}

// This function intercepts key release
void keyReleased() {
  lastPressedMillis = currentMillis;
  kc = keyboard.getOemKey();
  mod = keyboard.getModifiers();
  kpress = false;
  kc_mapped = ckey->mapkeycode(kc,mod);
  displayKey();

  // handle special key functions
  switch (kc_mapped) {
    case IGNORE_KEYCODE:
      break;
    case KEY_RESTORE:
      digitalWrite (NMI_PIN, LOW);
      break;
    default:
      ckey->c64key(kc_mapped, kpress);
      break;
  }
}

//////////////////////////////////////////////////////////////

void displayKeyMap() {
  if (!displayPresent) { return; }
  u8g2.firstPage();
  do {
   u8g2.setFlipMode(true);
   u8g2.setFont(u8g2_font_7x13B_tr);
   u8g2.setCursor(0,14);
   String s;
   s = "New keymap: " + String(keymapnum);
   u8g2.print(s);
   u8g2.setCursor(0,31);
   s = ckey->getCurrentKeyMapName();
   u8g2.print(s);
  } while ( u8g2.nextPage() );
}


void displayKey() {
  if (!displayPresent) { return; }
  u8g2.firstPage();
  do {
   u8g2.setFlipMode(true);
   u8g2.setFont(u8g2_font_7x13B_tr);
   u8g2.setCursor(0,14);
   String s;
   s = "Key: " + String(kc,HEX) + " Mod: " + String(mod, HEX);
   u8g2.print(s);
   u8g2.setCursor(0,31);
   s = "map:"+String(kc_mapped, HEX) + " " + String(get_mapped_key_name(kc_mapped));
   u8g2.print(s);
  } while ( u8g2.nextPage() );
}


void displayState(const char* s) {
  if (!displayPresent) { return; }
  u8g2.firstPage();
  do {
   u8g2.setFlipMode(true);
   u8g2.setFont(u8g2_font_7x13B_tr);
   u8g2.setCursor(0,14);
   u8g2.print(s);
  } while ( u8g2.nextPage() );
}

//////////////////////////////////////////////////////////////

void setup() {
  ckey = new C128keyboard();

  Wire.begin();

  // detect if display is connected
  Wire.beginTransmission(0x3C);
  displayPresent = (0 == Wire.endTransmission());

  if (displayPresent) {
    u8g2.begin();
  }

  usb.Init();

  displayState("Hello world!");

  ckey->clearAllKeys(); // just in case RESET line is not used
}

uint32_t lastUSBstate = 0;

void loop() {
  currentMillis = millis();
  if (displayPresent && (currentMillis - lastPressedMillis > inactivePeriodMillis)) {
    // clear screen if there was no activity for inactivePeriodMillis
    u8g2.firstPage();
    do { } while ( u8g2.nextPage() );
    lastPressedMillis = currentMillis; // prevent screen clearing and i2c access in every following loop during inactivity
  }

  // Process USB tasks
  usb.Task();

  uint32_t currentUSBstate = usb.getUsbTaskState();
  String state;
  if (lastUSBstate != currentUSBstate) {
    switch (currentUSBstate) {
      case USB_ATTACHED_SUBSTATE_SETTLE: state="Device Attached"; break;
      case USB_DETACHED_SUBSTATE_WAIT_FOR_DEVICE: state="Detached, waiting for Device"; break;
      case USB_ATTACHED_SUBSTATE_RESET_DEVICE: state="Resetting Device"; break;
      case USB_ATTACHED_SUBSTATE_WAIT_RESET_COMPLETE: state="Reset complete"; break;
      case USB_STATE_CONFIGURING: state="USB Configuring"; break;
      case USB_STATE_RUNNING: state="USB Running"; break;
    }
    displayState(state.c_str());
    lastUSBstate = currentUSBstate;
  }
}
