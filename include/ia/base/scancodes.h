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
    ia_scancode_unknown = 0,

    /** These values are from usage page 0x07 (USB keyboard page) */
    ia_scancode_a = 4,
    ia_scancode_b = 5,
    ia_scancode_c = 6,
    ia_scancode_d = 7,
    ia_scancode_e = 8,
    ia_scancode_f = 9,
    ia_scancode_g = 10,
    ia_scancode_h = 11,
    ia_scancode_i = 12,
    ia_scancode_j = 13,
    ia_scancode_k = 14,
    ia_scancode_l = 15,
    ia_scancode_m = 16,
    ia_scancode_n = 17,
    ia_scancode_o = 18,
    ia_scancode_p = 19,
    ia_scancode_q = 20,
    ia_scancode_r = 21,
    ia_scancode_s = 22,
    ia_scancode_t = 23,
    ia_scancode_u = 24,
    ia_scancode_v = 25,
    ia_scancode_w = 26,
    ia_scancode_x = 27,
    ia_scancode_y = 28,
    ia_scancode_z = 29,

    ia_scancode_1 = 30,
    ia_scancode_2 = 31,
    ia_scancode_3 = 32,
    ia_scancode_4 = 33,
    ia_scancode_5 = 34,
    ia_scancode_6 = 35,
    ia_scancode_7 = 36,
    ia_scancode_8 = 37,
    ia_scancode_9 = 38,
    ia_scancode_0 = 39,
 
    ia_scancode_return = 40,
    ia_scancode_escape = 41,
    ia_scancode_backspace = 42,
    ia_scancode_tab = 43,
    ia_scancode_space = 44,
 
    ia_scancode_minus = 45,
    ia_scancode_equals = 46,
    ia_scancode_leftbracket = 47,
    ia_scancode_rightbracket = 48,
    /** Located at the lower left of the return key on ISO keyboards and 
     *  at the right end of the QWERTY row on ANSI keyboards. Produces 
     *  REVERSE SOLIDUS (backslash) and VERTICAL LINE in a US layout, 
     *  REVERSE SOLIDUS and VERTICAL LINE in a UK Mac layout, NUMBER 
     *  SIGN and TILDE in a UK Windows layout, DOLLAR SIGN and POUND 
     *  SIGN in a Swiss German layout, NUMBER SIGN and APOSTROPHE in 
     *  a German layout, GRAVE ACCENT and POUND SIGN in a French Mac
     *  layout, and ASTERISK and MICRO SIGN in a French Windows layout. */
    ia_scancode_backslash = 49,
    /** ISO USB keyboards actually use this code instead of 49 for the 
     *  same key, but all OSes I've seen treat the two codes identically. 
     *  So, as an implementor, unless your keyboard generates both of 
     *  those codes and your OS treats them differently, you should 
     *  generate lake_scancode_backlash instead of this code. */
    ia_scancode_nonushash = 50,
    ia_scancode_semicolon = 51,
    ia_scancode_apostrophe = 52,
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
    ia_scancode_grave = 53,
    ia_scancode_comma = 54,
    ia_scancode_period = 55,
    ia_scancode_slash = 56,
    ia_scancode_capslock = 57,

    ia_scancode_f1 = 58,
    ia_scancode_f2 = 59,
    ia_scancode_f3 = 60,
    ia_scancode_f4 = 61,
    ia_scancode_f5 = 62,
    ia_scancode_f6 = 63,
    ia_scancode_f7 = 64,
    ia_scancode_f8 = 65,
    ia_scancode_f9 = 66,
    ia_scancode_f10 = 67,
    ia_scancode_f11 = 68,
    ia_scancode_f12 = 69,

    ia_scancode_printscreen = 70,
    ia_scancode_scrolllock = 71,
    ia_scancode_pause = 72,
    ia_scancode_insert = 73, /**< insert on PC, help on some Mac keyboards (but does send code 73, not 117) */
    ia_scancode_home = 74,
    ia_scancode_pageup = 75,
    ia_scancode_delete = 76,
    ia_scancode_end = 77,
    ia_scancode_pagedown = 78,
    ia_scancode_right = 79,
    ia_scancode_left = 80,
    ia_scancode_down = 81,
    ia_scancode_up = 82,

    ia_scancode_numlockclear = 83, /**< num lock on PC, clear on Mac keyboards */
    ia_scancode_kp_divide = 84,
    ia_scancode_kp_multiply = 85,
    ia_scancode_kp_minus = 86,
    ia_scancode_kp_plus = 87,
    ia_scancode_kp_enter = 88,
    ia_scancode_kp_1 = 89,
    ia_scancode_kp_2 = 90,
    ia_scancode_kp_3 = 91,
    ia_scancode_kp_4 = 92,
    ia_scancode_kp_5 = 93,
    ia_scancode_kp_6 = 94,
    ia_scancode_kp_7 = 95,
    ia_scancode_kp_8 = 96,
    ia_scancode_kp_9 = 97,
    ia_scancode_kp_0 = 98,
    ia_scancode_kp_period = 99,

    /** This is the additional key that ISO keyboards have over ANSI ones, 
     *  located between left shift and Y. Produces GRAVE ACCENT and TILDE 
     *  in a US or UK Mac layout, REVERSE SOLIDUS (backslash) and VERTICAL 
     *  LINE in a US or UK Windows layout, and LESS-THAN SIGN and GREATER-THAN 
     *  SIGN in a Swiss German, German, or French layout. */
    ia_scancode_nonusbackslash = 100,
    ia_scancode_application = 101, /**< windows contextual menu, compose */
    /** The USB document says this is a status flag, not a physical key, 
     *  but some Mac keyboards do have a power key. */
    ia_scancode_power = 102,
    ia_scancode_kp_equals = 103,
    ia_scancode_f13 = 104,
    ia_scancode_f14 = 105,
    ia_scancode_f15 = 106,
    ia_scancode_f16 = 107,
    ia_scancode_f17 = 108,
    ia_scancode_f18 = 109,
    ia_scancode_f19 = 110,
    ia_scancode_f20 = 111,
    ia_scancode_f21 = 112,
    ia_scancode_f22 = 113,
    ia_scancode_f23 = 114,
    ia_scancode_f24 = 115,
    ia_scancode_execute = 116,
    ia_scancode_help = 117,    /**< AL Integrated Help Center */
    ia_scancode_menu = 118,    /**< Menu (show menu) */
    ia_scancode_select = 119,
    ia_scancode_stop = 120,    /**< AC Stop */
    ia_scancode_again = 121,   /**< AC Redo/Repeat */
    ia_scancode_undo = 122,    /**< AC Undo */
    ia_scancode_cut = 123,     /**< AC Cut */
    ia_scancode_copy = 124,    /**< AC Copy */
    ia_scancode_paste = 125,   /**< AC Paste */
    ia_scancode_find = 126,    /**< AC Find */
    ia_scancode_mute = 127,
    ia_scancode_volumeup = 128,
    ia_scancode_volumedown = 129,
    ia_scancode_lockingcapslock = 130,
    ia_scancode_lockingnumlock = 131,
    ia_scancode_lockingscrolllock = 132,
    ia_scancode_kp_comma = 133,
    ia_scancode_kp_equalsas400 = 134,

    ia_scancode_international1 = 135, /**< used on Asian keyboards, see footnotes in USB doc */
    ia_scancode_international2 = 136,
    ia_scancode_international3 = 137, /**< Yen */
    ia_scancode_international4 = 138,
    ia_scancode_international5 = 139,
    ia_scancode_international6 = 140,
    ia_scancode_international7 = 141,
    ia_scancode_international8 = 142,
    ia_scancode_international9 = 143,
    ia_scancode_lang1 = 144, /**< Hangul/English toggle */
    ia_scancode_lang2 = 145, /**< Hanja conversion */
    ia_scancode_lang3 = 146, /**< Katakana */
    ia_scancode_lang4 = 147, /**< Hiragana */
    ia_scancode_lang5 = 148, /**< Zenkaku/Hankaku */
    ia_scancode_lang6 = 149, /**< reserved */
    ia_scancode_lang7 = 150, /**< reserved */
    ia_scancode_lang8 = 151, /**< reserved */
    ia_scancode_lang9 = 152, /**< reserved */

    ia_scancode_alterase = 153,    /**< Erase-Eaze */
    ia_scancode_sysreq = 154,
    ia_scancode_cancel = 155,      /**< AC Cancel */
    ia_scancode_clear = 156,
    ia_scancode_prior = 157,
    ia_scancode_return2 = 158,
    ia_scancode_separator = 159,
    ia_scancode_out = 160,
    ia_scancode_oper = 161,
    ia_scancode_clearagain = 162,
    ia_scancode_crsel = 163,
    ia_scancode_exsel = 164,

    ia_scancode_kp_00 = 176,
    ia_scancode_kp_000 = 177,
    ia_scancode_thousandsseparator = 178,
    ia_scancode_decimalseparator = 179,
    ia_scancode_currencyunit = 180,
    ia_scancode_currencysubunit = 181,
    ia_scancode_kp_leftparen = 182,
    ia_scancode_kp_rightparen = 183,
    ia_scancode_kp_leftbrace = 184,
    ia_scancode_kp_rightbrace = 185,
    ia_scancode_kp_tab = 186,
    ia_scancode_kp_backspace = 187,
    ia_scancode_kp_a = 188,
    ia_scancode_kp_b = 189,
    ia_scancode_kp_c = 190,
    ia_scancode_kp_d = 191,
    ia_scancode_kp_e = 192,
    ia_scancode_kp_f = 193,
    ia_scancode_kp_xor = 194,
    ia_scancode_kp_power = 195,
    ia_scancode_kp_percent = 196,
    ia_scancode_kp_less = 197,
    ia_scancode_kp_greater = 198,
    ia_scancode_kp_ampersand = 199,
    ia_scancode_kp_dblampersand = 200,
    ia_scancode_kp_verticalbar = 201,
    ia_scancode_kp_dblverticalbar = 202,
    ia_scancode_kp_colon = 203,
    ia_scancode_kp_hash = 204,
    ia_scancode_kp_space = 205,
    ia_scancode_kp_at = 206,
    ia_scancode_kp_exclam = 207,
    ia_scancode_kp_memstore = 208,
    ia_scancode_kp_memrecall = 209,
    ia_scancode_kp_memclear = 210,
    ia_scancode_kp_memadd = 211,
    ia_scancode_kp_memsubtract = 212,
    ia_scancode_kp_memmultiply = 213,
    ia_scancode_kp_memdivide = 214,
    ia_scancode_kp_plusminus = 215,
    ia_scancode_kp_clear = 216,
    ia_scancode_kp_clearentry = 217,
    ia_scancode_kp_binary = 218,
    ia_scancode_kp_octal = 219,
    ia_scancode_kp_decimal = 220,
    ia_scancode_kp_hexadecimal = 221,

    ia_scancode_lctrl = 224,
    ia_scancode_lshift = 225,
    ia_scancode_lalt = 226, /**< alt, option */
    ia_scancode_lgui = 227, /**< windows, command (apple), meta */
    ia_scancode_rctrl = 228,
    ia_scancode_rshift = 229,
    ia_scancode_ralt = 230, /**< alt gr, option */
    ia_scancode_rgui = 231, /**< windows, command (apple), meta */
    ia_scancode_mode = 257,

    /* These values are mapped from usage page 0x0C (USB consumer page). There are 
     * way more keys in the spec than we can represent in the current scancode range, 
     * so pick the ones that commonly come up in real world usage. */

    ia_scancode_sleep = 258,                /**< Sleep */
    ia_scancode_wake = 259,                 /**< Wake */

    ia_scancode_channel_increment = 260,    /**< Channel Increment */
    ia_scancode_channel_decrement = 261,    /**< Channel Decrement */

    ia_scancode_media_play = 262,           /**< Play */
    ia_scancode_media_pause = 263,          /**< Pause */
    ia_scancode_media_record = 264,         /**< Record */
    ia_scancode_media_fast_forward = 265,   /**< Fast Forward */
    ia_scancode_media_rewind = 266,         /**< Rewind */
    ia_scancode_media_next_track = 267,     /**< Next Track */
    ia_scancode_media_previous_track = 268, /**< Previous Track */
    ia_scancode_media_stop = 269,           /**< Stop */
    ia_scancode_media_eject = 270,          /**< Eject */
    ia_scancode_media_play_pause = 271,     /**< Play / Pause */
    ia_scancode_media_select = 272,         /* Media Select */

    ia_scancode_ac_new = 273,               /**< AC New */
    ia_scancode_ac_open = 274,              /**< AC Open */
    ia_scancode_ac_close = 275,             /**< AC Close */
    ia_scancode_ac_exit = 276,              /**< AC Exit */
    ia_scancode_ac_save = 277,              /**< AC Save */
    ia_scancode_ac_print = 278,             /**< AC Print */
    ia_scancode_ac_properties = 279,        /**< AC Properties */

    ia_scancode_ac_search = 280,            /**< AC Search */
    ia_scancode_ac_home = 281,              /**< AC Home */
    ia_scancode_ac_back = 282,              /**< AC Back */
    ia_scancode_ac_forward = 283,           /**< AC Forward */
    ia_scancode_ac_stop = 284,              /**< AC Stop */
    ia_scancode_ac_refresh = 285,           /**< AC Refresh */
    ia_scancode_ac_bookmarks = 286,         /**< AC Bookmarks */

    /* these are values that are often used on mobile phones */

    /** Usually situated below the display on phones and used as a 
     *  multi-function feature key for selecting a software defined 
     *  function shown on the bottom left of the display. */
    ia_scancode_softleft = 287,
    /** Usually situated below the display on phones and used as a 
     *  multi-function feature key for selecting a software defined 
     *  function shown on the bottom right of the display. */
    ia_scancode_softright = 288,
    ia_scancode_call = 289, /**< Used for accepting phone calls. */
    ia_scancode_endcall = 290, /**< Used for rejecting phone calls. */

    /* add any other keys here. */

    ia_scancode_reserved = 400, /**< 400-500 reserved for dynamic keycodes */

    ia_scancode_count = 512 /**< Not a key, just marks the number of scancodes for array bounds */
} ia_scancode;
