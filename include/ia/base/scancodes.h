#pragma once
/** @file ia/base/scancodes.h
 *  @brief A scancode is the physical representation of a key on the keyboard,
 *  independent of language and keyboard mapping.
 * 
 *  The values in this enumeration are based on the USB usage page standard:
 *  https://usb.org/sites/default/files/hut1_5.pdf
 */
#include <ia/base/types.h>

typedef enum ia_scancode : i16 {
    IA_SCANCODE_UNKNOWN = 0,

    /** These values are from usage page 0x07 (USB keyboard page) */
    IA_SCANCODE_A = 4,
    IA_SCANCODE_B = 5,
    IA_SCANCODE_C = 6,
    IA_SCANCODE_D = 7,
    IA_SCANCODE_E = 8,
    IA_SCANCODE_F = 9,
    IA_SCANCODE_G = 10,
    IA_SCANCODE_H = 11,
    IA_SCANCODE_I = 12,
    IA_SCANCODE_J = 13,
    IA_SCANCODE_K = 14,
    IA_SCANCODE_L = 15,
    IA_SCANCODE_M = 16,
    IA_SCANCODE_N = 17,
    IA_SCANCODE_O = 18,
    IA_SCANCODE_P = 19,
    IA_SCANCODE_Q = 20,
    IA_SCANCODE_R = 21,
    IA_SCANCODE_S = 22,
    IA_SCANCODE_T = 23,
    IA_SCANCODE_U = 24,
    IA_SCANCODE_V = 25,
    IA_SCANCODE_W = 26,
    IA_SCANCODE_X = 27,
    IA_SCANCODE_Y = 28,
    IA_SCANCODE_Z = 29,

    IA_SCANCODE_1 = 30,
    IA_SCANCODE_2 = 31,
    IA_SCANCODE_3 = 32,
    IA_SCANCODE_4 = 33,
    IA_SCANCODE_5 = 34,
    IA_SCANCODE_6 = 35,
    IA_SCANCODE_7 = 36,
    IA_SCANCODE_8 = 37,
    IA_SCANCODE_9 = 38,
    IA_SCANCODE_0 = 39,
 
    IA_SCANCODE_RETURN = 40,
    IA_SCANCODE_ESCAPE = 41,
    IA_SCANCODE_BACKSPACE = 42,
    IA_SCANCODE_TAB = 43,
    IA_SCANCODE_SPACE = 44,
 
    IA_SCANCODE_MINUS = 45,
    IA_SCANCODE_EQUALS = 46,
    IA_SCANCODE_LEFTBRACKET = 47,
    IA_SCANCODE_RIGHTBRACKET = 48,
    /** Located at the lower left of the return key on ISO keyboards and 
     *  at the right end of the QWERTY row on ANSI keyboards. Produces 
     *  REVERSE SOLIDUS (backslash) and VERTICAL LINE in a US layout, 
     *  REVERSE SOLIDUS and VERTICAL LINE in a UK Mac layout, NUMBER 
     *  SIGN and TILDE in a UK Windows layout, DOLLAR SIGN and POUND 
     *  SIGN in a Swiss German layout, NUMBER SIGN and APOSTROPHE in 
     *  a German layout, GRAVE ACCENT and POUND SIGN in a French Mac
     *  layout, and ASTERISK and MICRO SIGN in a French Windows layout. */
    IA_SCANCODE_BACKSLASH = 49,
    /** ISO USB keyboards actually use this code instead of 49 for the 
     *  same key, but all OSes I've seen treat the two codes identically. 
     *  So, as an implementor, unless your keyboard generates both of 
     *  those codes and your OS treats them differently, you should 
     *  generate lake_scancode_backlash instead of this code. */
    IA_SCANCODE_NONUSHASH = 50,
    IA_SCANCODE_SEMICOLON = 51,
    IA_SCANCODE_APOSTROPHE = 52,
    /** Located in the top left corner (on both ANSI and ISO keyboards). 
     *  Produces GRAVE ACCENT and TILDE in a US Windows layout and in US 
     *  and UK Mac layouts on ANSI keyboards, GRAVE ACCENT and NOT SIGN 
     *  in a UK Windows layout, SECTION SIGN and PLUS-MINUS SIGN in US 
     *  and UK Mac layouts on ISO keyboards, SECTION SIGN and DEGREE SIGN 
     *  in a Swiss German layout (Mac: only on ISO keyboards), CIRCUMFLEX 
     *  ACCENT and DEGREE SIGN in a German layout (Mac: only on ISO 
     *  keyboards), SUPERSCRIPT TWO and TILDE in a French Windows layout, 
     *  COMMERCIAL AT and NUMBER SIGN in a French Mac layout on ISO 
     *  keyboards, and LESS-THAN SIGN and GREATER-THAN SIGN in a Swiss 
     *  German, German, or French Mac layout on ANSI keyboards. */
    IA_SCANCODE_GRAVE = 53,
    IA_SCANCODE_COMMA = 54,
    IA_SCANCODE_PERIOD = 55,
    IA_SCANCODE_SLASH = 56,
    IA_SCANCODE_CAPSLOCK = 57,

    IA_SCANCODE_F1 = 58,
    IA_SCANCODE_F2 = 59,
    IA_SCANCODE_F3 = 60,
    IA_SCANCODE_F4 = 61,
    IA_SCANCODE_F5 = 62,
    IA_SCANCODE_F6 = 63,
    IA_SCANCODE_F7 = 64,
    IA_SCANCODE_F8 = 65,
    IA_SCANCODE_F9 = 66,
    IA_SCANCODE_F10 = 67,
    IA_SCANCODE_F11 = 68,
    IA_SCANCODE_F12 = 69,

    IA_SCANCODE_PRINTSCREEN = 70,
    IA_SCANCODE_SCROLLLOCK = 71,
    IA_SCANCODE_PAUSE = 72,
    IA_SCANCODE_INSERT = 73, /**< insert on PC, help on some Mac keyboards (but does send code 73, not 117) */
    IA_SCANCODE_HOME = 74,
    IA_SCANCODE_PAGEUP = 75,
    IA_SCANCODE_DELETE = 76,
    IA_SCANCODE_END = 77,
    IA_SCANCODE_PAGEDOWN = 78,
    IA_SCANCODE_RIGHT = 79,
    IA_SCANCODE_LEFT = 80,
    IA_SCANCODE_DOWN = 81,
    IA_SCANCODE_UP = 82,

    IA_SCANCODE_NUMLOCKCLEAR = 83, /**< num lock on PC, clear on Mac keyboards */
    IA_SCANCODE_KP_DIVIDE = 84,
    IA_SCANCODE_KP_MULTIPLY = 85,
    IA_SCANCODE_KP_MINUS = 86,
    IA_SCANCODE_KP_PLUS = 87,
    IA_SCANCODE_KP_ENTER = 88,
    IA_SCANCODE_KP_1 = 89,
    IA_SCANCODE_KP_2 = 90,
    IA_SCANCODE_KP_3 = 91,
    IA_SCANCODE_KP_4 = 92,
    IA_SCANCODE_KP_5 = 93,
    IA_SCANCODE_KP_6 = 94,
    IA_SCANCODE_KP_7 = 95,
    IA_SCANCODE_KP_8 = 96,
    IA_SCANCODE_KP_9 = 97,
    IA_SCANCODE_KP_0 = 98,
    IA_SCANCODE_KP_PERIOD = 99,

    /** This is the additional key that ISO keyboards have over ANSI ones, 
     *  located between left shift and Y. Produces GRAVE ACCENT and TILDE 
     *  in a US or UK Mac layout, REVERSE SOLIDUS (backslash) and VERTICAL 
     *  LINE in a US or UK Windows layout, and LESS-THAN SIGN and GREATER-THAN 
     *  SIGN in a Swiss German, German, or French layout. */
    IA_SCANCODE_NONUSBACKSLASH = 100,
    IA_SCANCODE_APPLICATION = 101, /**< windows contextual menu, compose */
    /** The USB document says this is a status flag, not a physical key, 
     *  but some Mac keyboards do have a power key. */
    IA_SCANCODE_POWER = 102,
    IA_SCANCODE_KP_EQUALS = 103,
    IA_SCANCODE_F13 = 104,
    IA_SCANCODE_F14 = 105,
    IA_SCANCODE_F15 = 106,
    IA_SCANCODE_F16 = 107,
    IA_SCANCODE_F17 = 108,
    IA_SCANCODE_F18 = 109,
    IA_SCANCODE_F19 = 110,
    IA_SCANCODE_F20 = 111,
    IA_SCANCODE_F21 = 112,
    IA_SCANCODE_F22 = 113,
    IA_SCANCODE_F23 = 114,
    IA_SCANCODE_F24 = 115,
    IA_SCANCODE_EXECUTE = 116,
    IA_SCANCODE_HELP = 117,    /**< AL Integrated Help Center */
    IA_SCANCODE_MENU = 118,    /**< Menu (show menu) */
    IA_SCANCODE_SELECT = 119,
    IA_SCANCODE_STOP = 120,    /**< AC Stop */
    IA_SCANCODE_AGAIN = 121,   /**< AC Redo/Repeat */
    IA_SCANCODE_UNDO = 122,    /**< AC Undo */
    IA_SCANCODE_CUT = 123,     /**< AC Cut */
    IA_SCANCODE_COPY = 124,    /**< AC Copy */
    IA_SCANCODE_PASTE = 125,   /**< AC Paste */
    IA_SCANCODE_FIND = 126,    /**< AC Find */
    IA_SCANCODE_MUTE = 127,
    IA_SCANCODE_VOLUMEUP = 128,
    IA_SCANCODE_VOLUMEDOWN = 129,
    IA_SCANCODE_LOCKINGCAPSLOCK = 130,
    IA_SCANCODE_LOCKINGNUMLOCK = 131,
    IA_SCANCODE_LOCKINGSCROLLLOCK = 132,
    IA_SCANCODE_KP_COMMA = 133,
    IA_SCANCODE_KP_EQUALSAS400 = 134,

    IA_SCANCODE_INTERNATIONAL1 = 135, /**< used on Asian keyboards, see footnotes in USB doc */
    IA_SCANCODE_INTERNATIONAL2 = 136,
    IA_SCANCODE_INTERNATIONAL3 = 137, /**< Yen */
    IA_SCANCODE_INTERNATIONAL4 = 138,
    IA_SCANCODE_INTERNATIONAL5 = 139,
    IA_SCANCODE_INTERNATIONAL6 = 140,
    IA_SCANCODE_INTERNATIONAL7 = 141,
    IA_SCANCODE_INTERNATIONAL8 = 142,
    IA_SCANCODE_INTERNATIONAL9 = 143,
    IA_SCANCODE_LANG1 = 144, /**< Hangul/English toggle */
    IA_SCANCODE_LANG2 = 145, /**< Hanja conversion */
    IA_SCANCODE_LANG3 = 146, /**< Katakana */
    IA_SCANCODE_LANG4 = 147, /**< Hiragana */
    IA_SCANCODE_LANG5 = 148, /**< Zenkaku/Hankaku */
    IA_SCANCODE_LANG6 = 149, /**< reserved */
    IA_SCANCODE_LANG7 = 150, /**< reserved */
    IA_SCANCODE_LANG8 = 151, /**< reserved */
    IA_SCANCODE_LANG9 = 152, /**< reserved */

    IA_SCANCODE_ALTERASE = 153,    /**< Erase-Eaze */
    IA_SCANCODE_SYSREQ = 154,
    IA_SCANCODE_CANCEL = 155,      /**< AC Cancel */
    IA_SCANCODE_CLEAR = 156,
    IA_SCANCODE_PRIOR = 157,
    IA_SCANCODE_RETURN2 = 158,
    IA_SCANCODE_SEPARATOR = 159,
    IA_SCANCODE_OUT = 160,
    IA_SCANCODE_OPER = 161,
    IA_SCANCODE_CLEARAGAIN = 162,
    IA_SCANCODE_CRSEL = 163,
    IA_SCANCODE_EXSEL = 164,

    IA_SCANCODE_KP_00 = 176,
    IA_SCANCODE_KP_000 = 177,
    IA_SCANCODE_THOUSANDSSEPARATOR = 178,
    IA_SCANCODE_DECIMALSEPARATOR = 179,
    IA_SCANCODE_CURRENCYUNIT = 180,
    IA_SCANCODE_CURRENCYSUBUNIT = 181,
    IA_SCANCODE_KP_LEFTPAREN = 182,
    IA_SCANCODE_KP_RIGHTPAREN = 183,
    IA_SCANCODE_KP_LEFTBRACE = 184,
    IA_SCANCODE_KP_RIGHTBRACE = 185,
    IA_SCANCODE_KP_TAB = 186,
    IA_SCANCODE_KP_BACKSPACE = 187,
    IA_SCANCODE_KP_A = 188,
    IA_SCANCODE_KP_B = 189,
    IA_SCANCODE_KP_C = 190,
    IA_SCANCODE_KP_D = 191,
    IA_SCANCODE_KP_E = 192,
    IA_SCANCODE_KP_F = 193,
    IA_SCANCODE_KP_XOR = 194,
    IA_SCANCODE_KP_POWER = 195,
    IA_SCANCODE_KP_PERCENT = 196,
    IA_SCANCODE_KP_LESS = 197,
    IA_SCANCODE_KP_GREATER = 198,
    IA_SCANCODE_KP_AMPERSAND = 199,
    IA_SCANCODE_KP_DBLAMPERSAND = 200,
    IA_SCANCODE_KP_VERTICALBAR = 201,
    IA_SCANCODE_KP_DBLVERTICALBAR = 202,
    IA_SCANCODE_KP_COLON = 203,
    IA_SCANCODE_KP_HASH = 204,
    IA_SCANCODE_KP_SPACE = 205,
    IA_SCANCODE_KP_AT = 206,
    IA_SCANCODE_KP_EXCLAM = 207,
    IA_SCANCODE_KP_MEMSTORE = 208,
    IA_SCANCODE_KP_MEMRECALL = 209,
    IA_SCANCODE_KP_MEMCLEAR = 210,
    IA_SCANCODE_KP_MEMADD = 211,
    IA_SCANCODE_KP_MEMSUBTRACT = 212,
    IA_SCANCODE_KP_MEMMULTIPLY = 213,
    IA_SCANCODE_KP_MEMDIVIDE = 214,
    IA_SCANCODE_KP_PLUSMINUS = 215,
    IA_SCANCODE_KP_CLEAR = 216,
    IA_SCANCODE_KP_CLEARENTRY = 217,
    IA_SCANCODE_KP_BINARY = 218,
    IA_SCANCODE_KP_OCTAL = 219,
    IA_SCANCODE_KP_DECIMAL = 220,
    IA_SCANCODE_KP_HEXADECIMAL = 221,

    IA_SCANCODE_LCTRL = 224,
    IA_SCANCODE_LSHIFT = 225,
    IA_SCANCODE_LALT = 226, /**< alt, option */
    IA_SCANCODE_LGUI = 227, /**< windows, command (apple), meta */
    IA_SCANCODE_RCTRL = 228,
    IA_SCANCODE_RSHIFT = 229,
    IA_SCANCODE_RALT = 230, /**< alt gr, option */
    IA_SCANCODE_RGUI = 231, /**< windows, command (apple), meta */
    IA_SCANCODE_MODE = 257,

    /* These values are mapped from usage page 0x0C (USB consumer page). There are 
     * way more keys in the spec than we can represent in the current scancode range, 
     * so pick the ones that commonly come up in real world usage. */

    IA_SCANCODE_SLEEP = 258,                /**< Sleep */
    IA_SCANCODE_WAKE = 259,                 /**< Wake */

    IA_SCANCODE_CHANNEL_INCREMENT = 260,    /**< Channel Increment */
    IA_SCANCODE_CHANNEL_DECREMENT = 261,    /**< Channel Decrement */

    IA_SCANCODE_MEDIA_PLAY = 262,           /**< Play */
    IA_SCANCODE_MEDIA_PAUSE = 263,          /**< Pause */
    IA_SCANCODE_MEDIA_RECORD = 264,         /**< Record */
    IA_SCANCODE_MEDIA_FAST_FORWARD = 265,   /**< Fast Forward */
    IA_SCANCODE_MEDIA_REWIND = 266,         /**< Rewind */
    IA_SCANCODE_MEDIA_NEXT_TRACK = 267,     /**< Next Track */
    IA_SCANCODE_MEDIA_PREVIOUS_TRACK = 268, /**< Previous Track */
    IA_SCANCODE_MEDIA_STOP = 269,           /**< Stop */
    IA_SCANCODE_MEDIA_EJECT = 270,          /**< Eject */
    IA_SCANCODE_MEDIA_PLAY_PAUSE = 271,     /**< Play / Pause */
    IA_SCANCODE_MEDIA_SELECT = 272,         /* Media Select */

    IA_SCANCODE_AC_NEW = 273,               /**< AC New */
    IA_SCANCODE_AC_OPEN = 274,              /**< AC Open */
    IA_SCANCODE_AC_CLOSE = 275,             /**< AC Close */
    IA_SCANCODE_AC_EXIT = 276,              /**< AC Exit */
    IA_SCANCODE_AC_SAVE = 277,              /**< AC Save */
    IA_SCANCODE_AC_PRINT = 278,             /**< AC Print */
    IA_SCANCODE_AC_PROPERTIES = 279,        /**< AC Properties */

    IA_SCANCODE_AC_SEARCH = 280,            /**< AC Search */
    IA_SCANCODE_AC_HOME = 281,              /**< AC Home */
    IA_SCANCODE_AC_BACK = 282,              /**< AC Back */
    IA_SCANCODE_AC_FORWARD = 283,           /**< AC Forward */
    IA_SCANCODE_AC_STOP = 284,              /**< AC Stop */
    IA_SCANCODE_AC_REFRESH = 285,           /**< AC Refresh */
    IA_SCANCODE_AC_BOOKMARKS = 286,         /**< AC Bookmarks */

    /* these are values that are often used on mobile phones */

    /** Usually situated below the display on phones and used as a 
     *  multi-function feature key for selecting a software defined 
     *  function shown on the bottom left of the display. */
    IA_SCANCODE_SOFTLEFT = 287,
    /** Usually situated below the display on phones and used as a 
     *  multi-function feature key for selecting a software defined 
     *  function shown on the bottom right of the display. */
    IA_SCANCODE_SOFTRIGHT = 288,
    IA_SCANCODE_CALL = 289, /**< Used for accepting phone calls. */
    IA_SCANCODE_ENDCALL = 290, /**< Used for rejecting phone calls. */

    /* add any other keys here. */

    IA_SCANCODE_RESERVED = 400, /**< 400-500 reserved for dynamic keycodes */

    IA_SCANCODE_COUNT = 512 /**< Not a key, just marks the number of scancodes for array bounds */
} ia_scancode;
