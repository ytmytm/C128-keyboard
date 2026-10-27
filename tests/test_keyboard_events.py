#!/usr/bin/env python3
"""Run the firmware's actual parser, callbacks and key mapping with mocked I/O.

Requires Python 3 and g++; no Arduino libraries or connected board are needed.
"""
from pathlib import Path
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]
SRC = ROOT / 'SEEED/C64_USB_Host_Library_SAMD/src'
FW = ROOT / 'SEEED/C128_keyboard'


def read(path):
    return path.read_text()


def function(source, signature):
    start = source.index(signature)
    brace = source.index('{', start)
    depth = 1
    end = brace + 1
    while depth:
        depth += (source[end] == '{') - (source[end] == '}')
        end += 1
    return source[start:end]


firmware = read(FW / 'C128keyboard.c.ino')
sketch = read(FW / 'C128_keyboard.ino')
header = read(FW / 'C128keyboard.h')
classes = header[header.index('class C64KeyInfo'):header.rindex('#endif')]
parser_header = read(SRC / 'hidboot.h')
constructor = function(parser_header, 'KeyboardReportParser()')
source = r'''
#include <cassert>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#define HIGH true
#define LOW false
#define OUTPUT 1
#define NMI_PIN 8
#define C128_40_80_PIN 9
#define C128_CAPS_PIN 10
#define ANALOG_SW_DATA 3
#define ANALOG_SW_RESET 7
#define ANALOG_SW_STROBE 6
#define bitRead(value, bit) (((value) >> (bit)) & 1U)
using String = std::string;
struct HID {};
struct KeyboardReportParser {
    struct { uint8_t bInfo[8]; } prevState;
    struct { uint8_t bLeds; } kbdLockingKeys;
    CONSTRUCTOR
    void Parse(HID *, uint32_t, uint32_t, uint8_t *);
    virtual void OnControlKeysChanged(uint8_t, uint8_t) {}
    virtual void OnKeyDown(uint8_t, uint8_t) {}
    virtual void OnKeyUp(uint8_t, uint8_t) {}
    uint8_t HandleLockingKeys(HID *, uint8_t) { return 0; }
    uint8_t OemToAscii(uint8_t, uint8_t) { return 0; }
};
struct USBHost {};
#define HID_PROTOCOL_KEYBOARD 1
template <uint8_t> struct HIDBoot {
    explicit HIDBoot(USBHost *) {}
    void SetReportParser(uint8_t, KeyboardReportParser *) {}
};
void keyPressed();
void keyReleased();
'''.replace('CONSTRUCTOR', constructor)
# Compile the real controller declaration as well as its callback definitions.
source += '\n' + read(SRC / 'KeyboardController.h').replace('#include <hidboot.h>', '')
source += '\nUSBHost usb;\nKeyboardController keyboard(usb);\n'
source += '\n' + read(FW / 'keymapping.h') + '\n' + classes
source += '\n' + read(FW / 'keymaps.h')
source += r'''
uint8_t kc = 0, mod = 0, keymapnum = 0, joynum = 2, joykeycount = 0;
uint16_t kc_mapped = 0;
bool joymode = false, kpress = false, c128_4080 = false, c128_caps = false;
bool lshift = false, rshift = false, shiftlock = false, virtshift = false;
unsigned long lastPressedMillis = 0, currentMillis = 0;
std::map<uint8_t, uint16_t> keyStateTable;
std::map<uint8_t, bool> matrix;
C128keyboard *ckey;
unsigned matrixWrites = 0;
void setswitch(uint8_t key, bool down) { matrix[key] = down; ++matrixWrites; }
std::map<int, bool> pins;
void digitalWrite(int pin, bool value) { assert(pin >= 0 && pin <= 10); pins[pin] = value; }
void pinMode(int pin, int) { assert(pin >= 0 && pin <= 10); }
void delay(int) {}
void displayKey() {}
void displayState(const char *) {}
void handle_joymode() {}
void displayKeyMap() {}

'''
source += '\n' + function(read(SRC / 'hidboot.cpp'), 'void KeyboardReportParser::Parse(')
controller = read(SRC / 'KeyboardController.cpp')
for name in ('OnKeyDown', 'OnKeyUp', 'OnControlKeysChanged'):
    source += '\n' + function(controller, 'void KeyboardController::' + name + '(')
source += '\n' + function(firmware, 'constexpr uint8_t C128keyboard::getNumKeyMaps(')
source += '\n' + function(firmware, 'void resetMT88(')
source += '\n' + function(firmware, 'C128keyboard::C128keyboard(')
for name in ('mapkeycode', 'updateShiftSwitches', 'c64key', 'clearAllKeys'):
    source += '\n' + function(firmware, ('uint16_t' if name == 'mapkeycode' else 'void') + ' C128keyboard::' + name + '(')
for name in ('next_keymap', 'handle_c128_4080', 'handle_c128_caps', 'keyPressed', 'keyReleased'):
    source += '\n' + function(sketch, 'void ' + name + '(')
source += '\n' + read(ROOT / 'tests/keyboard_events.cpp')
with tempfile.TemporaryDirectory(prefix='c128-events-') as tmp:
    cpp = Path(tmp) / 'events.cpp'
    exe = Path(tmp) / 'events'
    cpp.write_text(source)
    subprocess.run(['g++', '-std=c++20', '-Wall', '-Wextra', '-Werror',
                    '-fsanitize=address,undefined', '-g', str(cpp), '-o', str(exe)], check=True)
    subprocess.run([str(exe)], check=True)
