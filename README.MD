# C128 USB Keyboard

This project is designed to allow a modern USB keyboard to be used with a Commodore 128D/DCR in place of the original Commodore keyboard.

This project was originally forked from [Hartland/C64-Keyboard](https://github.com/Hartland/C64-Keyboard).
I made extensive hardware and software changes to support the full C128 keyboard.

I kept only the USB interface, as PS/2 keyboards are becoming retro themselves.

This project requires a Seeed Studio XIAO SAMD21, which has a USB-C socket. I use a USB-C male to USB-A female OTG adapter to connect a standard PC keyboard.

<img src="media/03.assembled.jpg" alt="Assembled adapter with OTG adapter and wireless keyboard dongle" width=800>

-- Maciej 'YTM/Elysium' Witkowiak

<a href="https://www.pcbway.com/project/shareproject/C128_USB_Keyboard_adapter_dcf19251.html"><img src="https://www.pcbway.com/project/img/images/frompcbway-1220.png" alt="PCB from PCBWay" /></a>

# Features

- Works with compatible USB HID boot-protocol keyboards, including wireless models with USB receivers.
- Maps all C128 keys, including `40/80`, `CAPS` and the second set of cursor keys.
- Provides four keyboard layouts that can be switched on the fly.
- Supports a VICE-style joystick mode on the keypad, selected with NumLock or Shift+NumLock (may not work with some games).
- Supports an external power supply for an unmodified C128DCR.
- Supports an OLED display.
- Can be plugged directly into the keyboard port or mounted internally.

# PCB and schematic

Use KiCad to open the current [schematic](kicad-c128keyb/kicad-c128keyb.kicad_sch)
and [PCB](kicad-c128keyb/kicad-c128keyb.kicad_pcb) in the
[kicad-c128keyb](kicad-c128keyb/) folder. The legacy `kicad-c128keyb.sch` and
`.net` files describe an older Pro Micro/MT8808 design; use the `.kicad_sch`
and `.kicad_pcb` files for this XIAO/MT8816 adapter.

The [kicad-c128keyb/plots](kicad-c128keyb/plots) folder contains Gerber files ready for manufacturing
and a [PDF schematic](kicad-c128keyb/plots/kicad-c128keyb.pdf).

To order PCBs, open the [shared project on PCBWay](https://www.pcbway.com/project/shareproject/C128_USB_Keyboard_adapter_dcf19251.html) and add the boards to your cart.

## Parts

<img src="media/04.pcb.jpg" alt="PCB top-down view" width=800>

* Seeed Studio XIAO SAMD21 (SAMD21, not RP2040)
* U1: MT8816 or MT8812
* U2: 74HC/HCT/LS595
* R1, R2, R3: 10 kΩ resistors
* Q1, Q2, Q3: 2N3904 transistors (or another NPN type, such as S9013)
* C1, C2: 100 nF capacitors
* PCB-mount male DB25 connector (option 1)
* 26-pin ribbon cable connector / 2 × 13-pin headers (option 2)
* Pin headers and sockets
* 128 × 32 SSD1306 I²C OLED display (optional)

Check the pinout of any substitute NPN transistor before fitting it to the PCB.

<img src="media/02.parts.jpg" alt="Components for the adapter" width=800>

## Connection options

<img src="kicad-c128keyb/plots/kicad-c128keyb-pcb.jpg" alt="Adapter overview" width=800>

- Plugs directly into the keyboard port when fitted with a male DB25 connector.
- Can use a ribbon cable for either an internal or external connection.
- Can be powered from the keyboard port or an external source, selected by a jumper.
- Can be fitted with a 128 × 32 I²C OLED display for status and debugging.

### Power (C128DCR only)

The C128D supplies +5 V on pin 4 of the keyboard port; the C128DCR does not.
An internal modification can add this connection to the C128DCR.

The photo below shows a wire on the underside of the C128DCR mainboard connecting +5 V from the joystick port to pin 4 of the keyboard port.

<img src="media/00.c128dcr-keyboard-5v.jpg" alt="C128DCR modification to route +5V to the keyboard port" width=800>

If you prefer not to open your C128DCR, you can supply external power from the nearby cassette or joystick port.
In this case, leave jumper JP1 open and connect +5 V to the `Ext. power` connector. Observe the polarity.

Options:

- JP1 open: supply power through `Ext. power` for an unmodified C128DCR.
- JP1 closed: leave `Ext. power` disconnected; use this setting for a C128D or a modified C128DCR.

### Flat cable: 26 pins (socket) to 25 pins (DB25 male)

If you prefer not to have the adapter protruding from the computer, you can make an extension cable.

You need:

* A 26-wire ribbon cable
* 26-pin ribbon cable connectors (a male PCB connector and a female cable connector)
* A male DB25 insulation-displacement connector for ribbon cable

<img src="media/01.flatcable.jpg" alt="Completed 26-to-25 flat ribbon cable" width=800>

Steps:

1. Solder the 26-pin male ribbon cable connector to J1.
2. Crimp the 26-pin female connector onto the cable, aligning the red wire (wire 1) with the triangular pin-1 mark.
3. Separate wire 26 (at the opposite edge from the red wire) from the ribbon and cut it as close to the female connector as possible.
4. Align the cable with the DB25 connector and crimp it, checking that the red wire connects to pin 1.

# Firmware

The sketch is in [SEEED/C128_keyboard](SEEED/C128_keyboard/). Keep all files in
that folder together when copying it to another computer.

## Install and upload with Arduino IDE

1. Install the **Seeed SAMD Boards** package and select the **Seeeduino XIAO**
   board (SAMD21). Follow the [Seeed setup instructions](https://wiki.seeedstudio.com/Seeeduino-XIAO/#software)
   to add the board package URL if necessary.
2. Use the Library Manager to install **[U8g2](https://github.com/olikraus/U8g2_Arduino)**
   and **[ShiftRegister74HC595](https://github.com/Simsso/ShiftRegister74HC595)**.
   U8g2 is required to compile the sketch even when no OLED is connected.
3. Download **`SEEED/C64_USB_Host_Library_SAMD.zip` from this repository**
   ([download the matching ZIP](SEEED/C64_USB_Host_Library_SAMD.zip)). In the IDE,
   choose **Sketch > Include Library > Add .ZIP Library** and select that local
   file. Alternatively, replace the installed library with this checkout's
   [source directory](SEEED/C64_USB_Host_Library_SAMD).
   A generic USB host library is not a substitute for this modified version.
4. Open [C128_keyboard.ino](SEEED/C128_keyboard/C128_keyboard.ino) in the Arduino IDE.
5. Disconnect the USB keyboard/OTG adapter and connect the XIAO's USB-C port
   to the computer with a USB data cable. Select the board's port and upload.
   If the port is missing or upload fails, enter the bootloader as described
   below, select its port, and try again.
6. Disconnect the programming cable, reconnect the keyboard through the OTG
   adapter, and power the adapter using the configuration described above.

Install the matching USB library on **the computer that compiles the sketch**.
Updating only the sketch or the Git checkout does not update an independently
installed library. The sketch rejects older library headers at compile time.

### Replacing the library on Windows

1. Close Arduino IDE and locate the sketchbook folder shown in its Preferences.
   Its library directory is usually `Documents\Arduino\libraries`, but the
   sketchbook may be elsewhere, including under OneDrive.
2. Move the old `C64_USB_Host_Library_SAMD` folder outside the `libraries`
   directory, then extract the current ZIP there. Replace the whole folder.
3. Check that the resulting layout is
   `libraries\C64_USB_Host_Library_SAMD\library.properties`, with no extra
   nested `C64_USB_Host_Library_SAMD` directory.
4. Restart the IDE and compile again. If multiple libraries are reported for
   `KeyboardController.h`, enable verbose compilation output and check the
   selected path. Remove or replace any outdated copy being selected.

On systems that support symbolic links, the installed library directory can
instead link to this checkout's `SEEED/C64_USB_Host_Library_SAMD` directory.
Move the old copy outside `libraries` and restart the IDE after changing it.

## Compile with the checkout's library using Arduino CLI

Run from the repository root. The Seeed SAMD board package, U8g2 and
ShiftRegister74HC595 must already be installed for Arduino CLI.

Bash (Linux/macOS):

```sh
arduino-cli compile \
  --fqbn Seeeduino:samd:seeed_XIAO_m0 \
  --library "$PWD/SEEED/C64_USB_Host_Library_SAMD" \
  SEEED/C128_keyboard
```

PowerShell (Windows):

```powershell
arduino-cli compile --fqbn Seeeduino:samd:seeed_XIAO_m0 --library ".\SEEED\C64_USB_Host_Library_SAMD" ".\SEEED\C128_keyboard"
```

The explicit [`--library` option](https://arduino.github.io/arduino-cli/1.5/sketch-build-process/#dependency-resolution)
has priority over libraries installed in the sketchbook. These commands use
the source directory directly, so no USB library ZIP installation is needed.
They compile the firmware; use the Arduino IDE or an appropriate CLI upload
command to upload it to the board.

## Entering the bootloader (double reset)

The original XIAO SAMD21 has reset pads rather than an onboard reset button.
To enter its bootloader:

1. Disconnect the keyboard/OTG adapter and connect the XIAO to the computer
   with a USB data cable.
2. Briefly bridge the two **RST reset pads twice in quick succession**, removing
   the bridge between the two touches. See [Seeed's reset-pad illustration](https://wiki.seeedstudio.com/Seeeduino-XIAO/#enter-bootloader-mode)
   for the pad locations. If your board has a reset button, press it twice.
3. The bootloader LED should pulse. With a UF2-enabled bootloader, a removable
   drive normally named **Arduino** appears, containing `INFO_UF2.TXT`.
   A bootloader serial port may also appear; on Windows its COM number may
   differ from the application port. Select that port for an IDE upload.

A single reset restarts the application. **Print Screen on the USB keyboard
only resets the adapter's keys and settings; it does not enter the bootloader.**

## Uploading a UF2 file by copying it to the boot drive

The [XIAO SAMD21 UF2 bootloader supports drag-and-drop flashing](https://wiki.seeedstudio.com/Seeeduino-XIAO-CircuitPython/#installing-circuitpython).
With a UF2-enabled bootloader and a UF2 build of **this project's firmware for
XIAO SAMD21**, no Arduino IDE is needed on the computer doing the upload:

1. Enter the bootloader using the double-reset procedure above.
2. Copy the firmware `.uf2` file to the **Arduino** removable drive using
   Windows File Explorer or your system's file manager.
3. Wait for copying to finish. The board normally restarts automatically and
   the boot drive disappears. Reconnect the keyboard/OTG adapter for use.

The repository currently contains source code and the USB library ZIP, but
**does not include a prebuilt firmware UF2**. The library ZIP is installed
on the build computer; it is not copied to the board's boot drive.
A `.bin` file cannot be used by simply renaming it to `.uf2`.

### Creating a UF2 from an Arduino build

1. Build for **Seeeduino XIAO (SAMD21)** with the matching USB library, then
   use **Sketch > Export Compiled Binary** in the Arduino IDE. Locate the
   application binary, normally named `C128_keyboard.ino.bin`.
2. Download [uf2conv.py](https://github.com/microsoft/uf2/blob/master/utils/uf2conv.py)
   and [uf2families.json](https://github.com/microsoft/uf2/blob/master/utils/uf2families.json)
   from Microsoft's UF2 project and place them alongside the binary.
3. In that directory, run the conversion with Python 3 (adjust the binary
   filename if your build uses a different one):

   ```sh
   python uf2conv.py --convert --family SAMD21 --base 0x2000 --output C128_keyboard.uf2 C128_keyboard.ino.bin
   ```

Use the application binary from this sketch's XIAO SAMD21 build, without a
bootloader image prepended. The `0x2000` application address matches the
[Seeed XIAO linker configuration](https://github.com/Seeed-Studio/ArduinoCore-samd/blob/master/variants/XIAO_m0/linker_scripts/gcc/flash_with_bootloader.ld).
The conversion only creates the UF2 file; copy it to the boot drive separately.

## OLED and firmware version

The optional display is a **128 × 32 SSD1306 I²C OLED at address `0x3C`**.
The firmware probes for it at startup and skips display operations if it does
not respond. The keyboard and joystick functions work without it.

The OLED shows the firmware release date below startup USB status messages,
for example `USB Configuring` / `FW 2026-10-30`. The version also appears below
`USB Running`. Status messages are cleared after about five seconds of
inactivity; key events replace them with the key display.

Set the release date in [firmware_version.h](SEEED/C128_keyboard/firmware_version.h)
when preparing a new firmware release. Recompiling the same source does not
change its version.

## Keyboard compatibility

The USB driver uses HID boot-protocol reports with six ordinary key slots and
separate modifier bits. A keyboard that only supports a proprietary or
NKRO-only report format may not work. Wireless keyboards need a compatible
USB receiver; Bluetooth-only keyboards cannot connect directly.

The adapter reads raw USB HID usages, so the PC's operating-system keyboard
layout does not configure it. The symbolic maps assume US-style key labels.
For troubleshooting, Right Shift should appear as HID usage `0xE5` with
modifier mask `0x20`. `getOemKey()` returns the HID usage; `getKey()` returns
ASCII and is not used to map the Commodore keyboard matrix.

# Keyboard mapping

Key maps are defined in [SEEED/C128_keyboard/keymaps.h](SEEED/C128_keyboard/keymaps.h). You can modify existing ones or add your own.

After defining a new keymap, add it to the `keymaps[]` array at the bottom of the file. The first entry is the default layout.

Press **Pause** to cycle through C128 positional → C128 symbolic → C64
positional → C64 symbolic → C128 positional. Startup and **Print Screen**
reset select C128 positional. Selection is not saved across power cycles.
The C64 names describe keyboard layouts; the hardware still uses the C128D/DCR
keyboard connector.

The following controls apply in all four layouts:

| USB key | Action |
| --- | --- |
| Print Screen | Reset adapter keys and settings to startup defaults; does not reset the C128 |
| Scroll Lock | Toggle Shift Lock |
| Pause | Select the next keymap |
| Caps Lock | RUN/STOP; use Scroll Lock for Shift Lock |
| Page Up or AltGr | Hold RESTORE while the key is held |
| NumLock | Toggle joystick mode and select port 2 |
| Shift+NumLock | Toggle joystick mode and select port 1 |

After resetting the adapter, release any keys that were held during the reset.
Disconnecting the USB keyboard releases held keys, joystick outputs, RESTORE
and Shift Lock, but preserves the selected keymap and joystick mode.


## C128 positional (default)

The top row follows the C128 keyboard layout. This layout is useful if you are familiar with the original keyboard.

| USB key | C128 key |
|---------|----------|
| ESC | RUN/STOP |
| Tab | CTRL |
| Caps Lock | RUN/STOP |
| F1 | ESC |
| F2 | TAB |
| F3 | ALT |
| F4 | CAPS/DIN (toggle) |
| F5 | HELP |
| F6 | LINEFEED |
| F7 | 40/80 (toggle) |
| F8 | NO SCROLL |
| F9 | F1/F2 |
| F10 | F3/F4 |
| F11 | F5/F6 |
| F12 | F7/F8 |
| Print Screen | reset adapter keys and settings |
| Scroll Lock | Shift Lock |
| Pause | switch to next keymap |
| Insert | POUND |
| Home | CLR/HOME |
| Page Up | RESTORE |
| End | RUN/STOP |
| NumLock | enable/disable joystick #2 on keypad |
| Shift+NumLock | enable/disable joystick #1 on keypad |
| Keypad digits, `+`, `-`, Enter and `.` | C128 keypad equivalents (outside joystick mode) |
| left CTRL | C= key |
| left SHIFT | left SHIFT |
| left ALT | C128 top cursor down |
| left Win | C128 top cursor up |
| Menu/Application | C128 top cursor left |
| right CTRL | C128 top cursor right |
| right SHIFT | right SHIFT |
| AltGr | RESTORE |
| right Win | ignored |

## C128 symbolic

Symbols follow US-style PC keycaps; for example, Shift+2 produces `@` rather than `"`.

Other changes:

- C128 top cursor keys are available via Shift+arrow keys.
- Both Ctrl keys map to CTRL.
- Both Windows keys map to the C= key.
- Left Alt maps to C128 ALT.
- AltGr maps to RESTORE.

The function row and several editing keys also differ from C128 positional:

| USB key | C128 key/action |
| --- | --- |
| Esc | ESC |
| Tab | TAB |
| F1–F8 | F1–F8 (even function keys use synthetic Shift) |
| F9 | HELP |
| F10 | NO SCROLL |
| F11 | 40/80 (toggle) |
| F12 | CAPS/DIN (toggle) |
| Insert | INST/DEL with Shift (insert) |
| Delete | INST/DEL |
| Shift+Return | LINEFEED |

Menu/Application still maps to the C128 top left cursor key.

## C64 positional

This uses the positional character and modifier mappings, with these changes:

- Keypad digits, `+`, `-`, Enter and `.` map to the Commodore's main digit
  keys, PLUS, MINUS, RETURN and full stop.
- F1–F8 map to the corresponding Commodore function keys, using synthetic
  Shift for even-numbered function keys.
- F9 and F10 are ignored.
- F11 toggles 40/80; F12 toggles CAPS/DIN.

For this reason, the C128 positional function-row table does not apply to this
layout. The common adapter controls and joystick mode are still available.

## C64 symbolic

This uses the C128 symbolic mapping, except that keypad digits, `+`, `-`,
Enter and `.` map to the Commodore's main digit keys, PLUS, MINUS,
RETURN and full stop. Joystick mode overrides keypad direction and FIRE
keys in the same way as in the other layouts.

# Joystick mode

Press **NumLock** to toggle joystick mode and select port 2, or
**Shift+NumLock** to toggle it and select port 1. Both combinations toggle
the same on/off state. To change ports while the mode is enabled, first
turn it off, then enable the desired port with the appropriate combination.

The OLED reports the selected mode and port. The USB library toggles the
NumLock LED separately; it is not resynchronised when Print Screen resets
the adapter, so the LED alone is not a reliable mode indicator after a reset.

The keypad directions are the same for both joystick ports:

| Keypad key | Direction |
| --- | --- |
| 7 | up + left |
| 8 | up |
| 9 | up + right |
| 4 | left |
| 6 | right |
| 1 | down + left |
| 2 | down |
| 3 | down + right |
| 5, 0, right Ctrl | FIRE |

A diagonal key is equivalent to holding its two direction keys together.
A direction remains active while any held key still requires it. For example,
releasing keypad 1 while holding keypad 4 leaves LEFT active.

This mode is not intended to replace a physical joystick: some games may interpret joystick #2 inputs as keypresses.

# Development and tests

With Python 3 and GCC's `g++` with AddressSanitizer and UndefinedBehaviorSanitizer
available (for example, on Linux or WSL), run from the repository root:

```sh
python3 tests/test_keyboard_events.py
```

The host tests compile the actual event-handling functions as C++14, with
`-Wall -Wextra -Werror`. They use mocked USB transport, GPIO and OLED drawing;
they do not replace an Arduino build or a hardware test. See
[tests/README.md](tests/README.md) for coverage and hardware checks.

After changing the bundled USB library, rebuild its tracked ZIP:

```sh
python3 tools/package_usb_library.py
```

The host tests verify that the ZIP matches the source directory. Both must
be committed together when that library changes. Firmware-only changes do
not require a new USB library ZIP. On Windows, use `python` instead of
`python3` if that is the installed command name.
