#!/usr/bin/env python3
"""Run the firmware's actual parser, callbacks and key mapping with mocked I/O.

Requires Python 3 and g++; no Arduino libraries or connected board are needed.
"""
from pathlib import Path
import subprocess
import sys
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


subprocess.run([sys.executable, str(ROOT / 'tools/package_usb_library.py'), '--check'], check=True)

firmware = read(FW / 'C128keyboard.c.ino')
sketch = read(FW / 'C128_keyboard.ino')
guard_start = sketch.index('#if !defined(C128_USB_HOST_EVENT_API_VERSION)')
api_guard = sketch[guard_start:sketch.index('#endif', guard_start) + len('#endif')]
for old_header in ('', '#define C128_USB_HOST_EVENT_API_VERSION 0\n'):
    rejected = subprocess.run(['g++', '-E', '-x', 'c++', '-'], input=old_header + api_guard,
                              text=True, capture_output=True)
    assert rejected.returncode != 0 and 'Outdated USB host library' in rejected.stderr
print('PASS: outdated USB library headers are rejected at compile time', flush=True)
header = read(FW / 'C128keyboard.h')
classes = header[header.index('class C64KeyInfo'):header.rindex('#endif')]
parser_header = read(SRC / 'hidboot.h')
constructor = function(parser_header, 'KeyboardReportParser()')
hid_header = read(SRC / 'hid.h')
parser_interface = hid_header[hid_header.index('class HIDReportParser {'):hid_header.index('class HID :')]
source = r'''
#include <array>
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
PARSER_INTERFACE
struct KeyboardReportParser : HIDReportParser {
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
struct USBHost {
    struct AddressPool { void FreeAddress(uint32_t) {} } pool;
    AddressPool &GetAddressPool() { return pool; }
};
#define HID_PROTOCOL_KEYBOARD 1
#define HID_PROTOCOL_MOUSE 2
#define epMUL(p) ((((p) & HID_PROTOCOL_KEYBOARD)? 1 : 0) + (((p) & HID_PROTOCOL_MOUSE)? 1 : 0))
template <const uint8_t BOOT_PROTOCOL> struct HIDBoot {
    USBHost *pUsb;
    HIDReportParser *pRptParser[epMUL(BOOT_PROTOCOL)] = {};
    uint32_t bConfNum = 0, bIfaceNum = 0, bNumEP = 1, bAddress = 0, qNextPollTime = 0;
    bool bPollEnable = false;
    explicit HIDBoot(USBHost *host) : pUsb(host) {}
    void SetReportParser(uint8_t id, HIDReportParser *parser) { pRptParser[id] = parser; }
    uint32_t Release();
};
void keyPressed();
void keyReleased();
void keyDisconnected();
'''.replace('CONSTRUCTOR', constructor).replace('PARSER_INTERFACE', parser_interface)
source += '\ntemplate <const uint8_t BOOT_PROTOCOL>\n' + function(parser_header, 'uint32_t HIDBoot<BOOT_PROTOCOL>::Release(')
# Compile the real controller declaration as well as its callback definitions.
source += '\n' + read(SRC / 'KeyboardController.h').replace('#include <hidboot.h>', '')
source += '\n' + api_guard + '\n'
source += '\nUSBHost usb;\nKeyboardController keyboard(usb);\n'
source += '\n' + read(FW / 'keymapping.h') + '\n' + classes
source += '\n' + read(FW / 'keymaps.h')
source += '\n' + read(FW / 'firmware_version.h')
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
bool captureTransitions = false;
std::vector<std::pair<uint8_t, bool>> transitions;
void setswitch(uint8_t key, bool down) {
    matrix[key] = down;
    ++matrixWrites;
    if (captureTransitions) transitions.emplace_back(key, down);
}
std::map<int, bool> pins;
void digitalWrite(int pin, bool value) { assert(pin >= 0 && pin <= 10); pins[pin] = value; }
void pinMode(int pin, int) { assert(pin >= 0 && pin <= 10); }
void delay(int) {}
void displayKey() {}
bool displayPresent = false;
constexpr uint8_t u8g2_font_7x13B_tr[] = {0};
struct MockOled {
    std::array<std::string, 2> lines;
    unsigned currentLine = 0, frames = 0;
    void firstPage() { lines = {{"", ""}}; ++frames; }
    void setFlipMode(bool enabled) { assert(enabled); }
    void setFont(const uint8_t *) {}
    void setCursor(unsigned x, unsigned y) {
        assert(x == 0 && (y == 14 || y == 31));
        currentLine = y == 31 ? 1 : 0;
    }
    void print(const char *text) { lines[currentLine] += text; }
    bool nextPage() { return false; }
} u8g2;
void displayState(const char *, bool = false);
void handle_joymode() {}
void displayKeyMap() {}

'''
source += '\n' + function(read(SRC / 'hidboot.cpp'), 'void KeyboardReportParser::Parse(')
controller = read(SRC / 'KeyboardController.cpp')
for name in ('OnKeyDown', 'OnKeyUp', 'OnControlKeysChanged', 'OnDisconnect'):
    source += '\n' + function(controller, 'void KeyboardController::' + name + '(')
source += '\n' + function(firmware, 'constexpr uint8_t C128keyboard::getNumKeyMaps(')
source += '\n' + function(firmware, 'const char* get_mapped_key_name(')
source += '\n' + function(firmware, 'void resetMT88(')
source += '\n' + function(firmware, 'C128keyboard::C128keyboard(')
for name in ('mapkeycode', 'updateShiftSwitches', 'c64key', 'clearAllKeys'):
    source += '\n' + function(firmware, ('uint16_t' if name == 'mapkeycode' else 'void') + ' C128keyboard::' + name + '(')
source += '\n' + function(sketch, 'void displayState(const char* s, bool showVersion) {')
for name in ('next_keymap', 'handle_c128_4080', 'handle_c128_caps', 'keyPressed', 'keyReleased', 'keyDisconnected'):
    source += '\n' + function(sketch, 'void ' + name + '(')
source += '\n' + read(ROOT / 'tests/keyboard_events.cpp')
with tempfile.TemporaryDirectory(prefix='c128-events-') as tmp:
    cpp = Path(tmp) / 'events.cpp'
    exe = Path(tmp) / 'events'
    cpp.write_text(source)
    subprocess.run(['g++', '-std=c++14', '-Wall', '-Wextra', '-Werror',
                    '-fsanitize=address,undefined', '-g', str(cpp), '-o', str(exe)], check=True)
    subprocess.run([str(exe)], check=True)
