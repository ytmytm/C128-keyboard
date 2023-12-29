
/* Maps are positional in relation to keyboard USB codes.
Place C64 key byte (Chart is found in KEYBIN spreadsheet)
into the equivalent keyboard USB code code position.
For C64 keyboards normally USB code set 2.

Each map has a normal (without shift) and shifted (with shift) array.

In situations where there is a shift difference, add 64 to the C64 key byte. 
This is when a key is shifted on keyboard, but not shifted on the C64 or vise-versa.

Unused USB codes need to have the IGNORE_KEYCODE value in place.(Default 170)

https://deskthority.net/wiki/Scancode#USB_HID_Usage_Codes

https://sites.google.com/site/h2obsession/CBM/C128/keyboard-scan

*/

//  Modified for C128 by Maciej 'YTM/Elysium' Witkowiak

// order of this table is so weird (if you look into C64/C128 keyboard matrix) because
// of the way how original (PS/2) code had MTxxxx lines attached
// it's [AX0..2,AY0..2] (AX*8+AY) but 3 bits of AX and 3 bits of AY are both in reverse order
// there are 6 bits of key address total,
// AX3 is used for logic table gap on MT8812/16 for X6/X7 and for C128 keys (lines X8/X9/X10)
// anything above $00ff is a special modifier or action key

enum C128_Key {
   // C64 8x8 keymap
   KEY_DEL = 0x00           // 0 DEL
  ,KEY_F1 = 0x01            // 1 F1
  ,KEY_HORZ_CSR = 0x02      // 2 HORZ CSR
  ,KEY_F5 = 0x03            // 3 F5
  ,KEY_RETURN = 0x04        // 4 RETURN
  ,KEY_F3 = 0x05            // 5 F3
  ,KEY_F7 = 0x06            // 6 F7
  ,KEY_VERT_CSR = 0x07      // 7 VERT CSR
  ,KEY_9 = 0x08             // 8 9
  ,KEY_M = 0x09             // 9 M
  ,KEY_J = 0x0A             // 10 J
  ,KEY_O = 0x0B             // 11 O
  ,KEY_I = 0x0C             // 12 I
  ,KEY_K = 0x0D             // 13 K
  ,KEY_ZERO = 0x0E          // 14 0
  ,KEY_N = 0x0F             // 15 N
  ,KEY_5 = 0x10             // 16 5
  ,KEY_C = 0x11             // 17 C
  ,KEY_D = 0x12             // 18 D
  ,KEY_T = 0x13             // 19 T
  ,KEY_R = 0x14             // 20 R
  ,KEY_F = 0x15             // 21 F
  ,KEY_6 = 0x16             // 22 6
  ,KEY_X = 0x17             // 23 X
  ,KEY_POUND = 0x18         // 24 POUND
  ,KEY_RSHIFT = 0x19        // 25 RSHIFT
  ,KEY_SEMICOLON = 0x1A     // 26 ;
  ,KEY_ARR_UP = 0x1B        // 27 ^
  ,KEY_STAR = 0x1C          // 28 *
  ,KEY_EQUALS = 0x1D        // 29 =
  ,KEY_HOME = 0x1E          // 30 HOME
  ,KEY_SLASH = 0x1F         // 31 /
  ,KEY_3 = 0x20             // 32 3
  ,KEY_Z = 0x21             // 33 Z
  ,KEY_A = 0x22             // 34 A
  ,KEY_E = 0x23             // 35 E
  ,KEY_W = 0x24             // 36 W
  ,KEY_S = 0x25             // 37 S
  ,KEY_4 = 0x26             // 38 4
  ,KEY_LSHIFT = 0x27        // 39 LSHIFT
  ,KEY_PLUS = 0x28          // 40 +
  ,KEY_DOT = 0x29           // 41 .
  ,KEY_L = 0x2A             // 42 L
  ,KEY_AT = 0x2B            // 43 @
  ,KEY_P = 0x2C             // 44 P
  ,KEY_COLON = 0x2D         // 45 :
  ,KEY_MINUS = 0x2E         // 46 -
  ,KEY_COMMA = 0x2F         // 47 ,
  ,KEY_7 = 0x30             // 48 7
  ,KEY_B = 0x31             // 49 B
  ,KEY_G = 0x32             // 50 G
  ,KEY_U = 0x33             // 51 U
  ,KEY_Y = 0x34             // 52 Y
  ,KEY_H = 0x35             // 53 H
  ,KEY_8 = 0x36             // 54 8
  ,KEY_V = 0x37             // 55 V
  ,KEY_1 = 0x38             // 56 1
  ,KEY_SPACE = 0x39         // 57 SPACE
  ,KEY_CTRL = 0x3A          // 58 CTRL
  ,KEY_Q = 0x3B             // 59 Q
  ,KEY_ARR_LEFT = 0x3C      // 60 <-
  ,KEY_CBM = 0x3D           // 61 CBM
  ,KEY_2 = 0x3E             // 62 2
  ,KEY_RUN_STOP = 0x3F      // 63 RUN/STOP
  // C128 extended keyboard (X8/X9/X10, AX3=1 always)
  ,C128_KEY_HELP = 0x50     // 80 HELP
  ,C128_KEYPAD_2 = 0x51     // 81 keypad 2
  ,C128_KEYPAD_5 = 0x52     // 82 keypad 5
  ,C128_KEYPAD_7 = 0x53     // 83 keypad 7
  ,C128_KEYPAD_8 = 0x54     // 84 keypad 8
  ,C128_KEYPAD_4 = 0x55     // 85 keypad 4
  ,C128_KEY_TAB = 0x56      // 86 TAB
  ,C128_KEYPAD_1 = 0x57     // 87 keypad 1
  ,C128_KEY_ESC = 0x70      // 112 ESC
  ,C128_KEYPAD_ENTER = 0x71 // 113 keypad ENTER
  ,C128_KEYPAD_MINUS = 0x72 // 114 keypad -
  ,C128_KEYPAD_9 = 0x73     // 115 keypad 9
  ,C128_KEYPAD_PLUS = 0x74  // 116 keypad +
  ,C128_KEYPAD_6 = 0x75     // 117 keypad 6
  ,C128_KEY_LINEFEED = 0x76 // 118 LINE FEED
  ,C128_KEYPAD_3 = 0x77     // 119 keypad 3
  ,C128_KEY_ALT = 0x48      // 72 ALT
  ,C128_KEY_DOWN = 0x49     // 73 cursor DOWN
  ,C128_KEYPAD_DOT = 0x4a   // 74 keypad .
  ,C128_KEY_RIGHT = 0x4b    // 75 cursor RIGHT
  ,C128_KEYPAD_0 = 0x4c     // 76 keypad 0
  ,C128_KEY_LEFT = 0x4d     // 77 cursor LEFT
  ,C128_KEY_UP = 0x4e       // 78 cursor UP
  ,C128_KEY_NOSCROLL = 0x4f // 79 NO SCROLL
  // joystick #1, if X11 line connected to GND
  ,JOY1_UP = 0x68           // 104 joystick 1 UP
  ,JOY1_DOWN = 0x6c         // 108 joystick 1 DOWN
  ,JOY1_LEFT = 0x6a         // 106 joystick 1 LEFT
  ,JOY1_RIGHT = 0x6e        // 110 joystick 1 RIGHT
  ,JOY1_FIRE = 0x69         // 105 joystick 1 FIRE
//  ,JOY2_ROUTE_GND = 0x6f    // 111 route GND from X11 to Y7 so that joystick 2 can be mapped as shorts between X0-4 and Y7
  ,JOY2_ROUTE_GND = 0x6b    // 107 route GND from X11 to Y6 so that joystick 2 can be mapped as shorts between X0-4 and Y6
  // joystick #2, if Y7 is connected to GND (through X11 via JOY2_ROUTE_GND), add MOD_JOY2 to all these codes in handler routine
  /*
  ,JOY2_UP = 0x07           // X=0, Y=7 -> X11=GND
  ,JOY2_DOWN = 0x27         // X=1, Y=7 -> X11=GND
  ,JOY2_LEFT = 0x17         // X=2, Y=7 -> X11=GND
  ,JOY2_RIGHT = 0x37        // X=3, Y=7 -> X11=GND
  ,JOY2_FIRE = 0x0f         // X=4, Y=7 -> X11=GND
  */
  // joystick #2, if Y6 is connected to GND (through X11 via JOY2_ROUTE_GND), add MOD_JOY2 to all these codes in handler routine
  ,JOY2_UP = 0x03           // X=0, Y=6 -> X11=GND
  ,JOY2_DOWN = 0x23         // X=1, Y=6 -> X11=GND
  ,JOY2_LEFT = 0x13         // X=2, Y=6 -> X11=GND
  ,JOY2_RIGHT = 0x33        // X=3, Y=6 -> X11=GND
  ,JOY2_FIRE = 0x0b         // X=4, Y=6 -> X11=GND

  // modifiers
  ,MOD_SHIFT = 0x100        // virtual shift modifier (add shift on base map, remove shift on shifted map) (this is used as a bit modifier)
  ,MOD_JOY2 = 0x200         // switch JOY2_ROUTE_GND to connect Y7 to X11(GND) so that connections X0-4 to Y7 act as joystick #2
  // special actions
  ,KEY_RESTORE = 0x400      // special action: RESTORE (NMI)
  ,KEY_KEYMAP = 0x401       // special action: switch to next keymap
  ,KEY_MT_RESET = 0x402     // special action: reset MT88XX chip
  ,KEY_SHIFT_LOCK = 0x403   // special action: shift lock (left shift toggle)
  ,C128_40_80_KEY = 0x404   // special action: C128 40/80 key (toggle)
  ,C128_CAPS_KEY = 0x405    // special action: C128 CAPS/DIN key (toggle)
  ,KEY_JOY1_MODE = 0x406    // special action: toggle joystick #1 mode on numpad + right control
  ,KEY_JOY2_MODE = 0x407    // special action: toggle joystick #2 mode on numpad + right control
  //
  ,IGNORE_KEYCODE = 0xFFF   // special: ignore keycode
};
