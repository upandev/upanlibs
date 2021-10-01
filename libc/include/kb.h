/*
 *	Upanix - An x86 based Operating System
 *  Copyright (C) 2011 'Prajwala Prabhakar' 'srinivasa.prajwal@gmail.com'
 *
 *  I am making my contributions/submissions to this project solely in
 *  my personal capacity and am not conveying any rights to any
 *  intellectual property of any third parties.
 *                                                                          
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *                                                                          
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *                                                                          
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/
 */
#ifndef _KEYBOARD_H_
#define _KEYBOARD_H_

# include <ctype.h>

#if defined __cplusplus
extern "C" {
#endif

typedef enum {
  Keyboard_CTRL_AT = 0,
  Keyboard_CTRL_A,
  Keyboard_CTRL_B,
  Keyboard_CTRL_C,
  Keyboard_CTRL_D,
  Keyboard_CTRL_E,
  Keyboard_CTRL_F,
  Keyboard_CTRL_G,
  Keyboard_CTRL_H,
  Keyboard_BACKSPACE = Keyboard_CTRL_H,
  Keyboard_CTRL_I,
  Keyboard_TAB = Keyboard_CTRL_I,
  Keyboard_CTRL_J,
  Keyboard_CTRL_K,
  Keyboard_CTRL_L,
  Keyboard_CTRL_M,
  Keyboard_ENTER = Keyboard_CTRL_M,
  Keyboard_CTRL_N,
  Keyboard_CTRL_O,
  Keyboard_CTRL_P,
  Keyboard_CTRL_Q,
  Keyboard_CTRL_R,
  Keyboard_CTRL_S,
  Keyboard_CTRL_T,
  Keyboard_CTRL_U,
  Keyboard_CTRL_V,
  Keyboard_CTRL_W,
  Keyboard_CTRL_X,
  Keyboard_CTRL_Y,
  Keyboard_CTRL_Z,
  Keyboard_CTRL_OPEN_BRACKET,
  Keyboard_ESC = Keyboard_CTRL_OPEN_BRACKET,
  Keyboard_CTRL_BACKSLASH,
  Keyboard_CTRL_CLOSE_BRACKET,
  Keyboard_CTRL_CARROT,
  Keyboard_CTRL_UNDERSCORE,

  Keyboard_SPACE = ' ',
  Keyboard_EXCLAIMATION = '!',
  Keyboard_DOUBLEQUOTE = '"',
  Keyboard_HASH = '#',
  Keyboard_DOLLAR = '$',
  Keyboard_PERCENT = '%',
  Keyboard_AND = '&',
  Keyboard_SINGLEQUOTE = '\'',
  Keyboard_OPEN_PAREN = '(',
  Keyboard_CLOSE_PAREN = ')',
  Keyboard_STAR_MULTIPLY = '*',
  Keyboard_PLUS = '+',
  Keyboard_COMMA = ',',
  Keyboard_MINUS = '-',
  Keyboard_FULLSTOP = '.',
  Keyboard_SLASH_DIVIDE = '/',

  Keyboard_0 = '0',
  Keyboard_1 = '1',
  Keyboard_2 = '2',
  Keyboard_3 = '3',
  Keyboard_4 = '4',
  Keyboard_5 = '5',
  Keyboard_6 = '6',
  Keyboard_7 = '7',
  Keyboard_8 = '8',
  Keyboard_9 = '9',

  Keyboard_COLON = ':',
  Keyboard_SEMICOLON = ';',
  Keyboard_LESSER = '<',
  Keyboard_EQUAL = '=',
  Keyboard_GREATER = '>',
  Keyboard_QUESTION = '?',
  Keyboard_AT = '@',

  Keyboard_A = 'A',
  Keyboard_B = 'B',
  Keyboard_C = 'C',
  Keyboard_D = 'D',
  Keyboard_E = 'E',
  Keyboard_F = 'F',
  Keyboard_G = 'G',
  Keyboard_H = 'H',
  Keyboard_I = 'I',
  Keyboard_J = 'J',
  Keyboard_K = 'K',
  Keyboard_L = 'L',
  Keyboard_M = 'M',
  Keyboard_N = 'N',
  Keyboard_O = 'O',
  Keyboard_P = 'P',
  Keyboard_Q = 'Q',
  Keyboard_R = 'R',
  Keyboard_S = 'S',
  Keyboard_T = 'T',
  Keyboard_U = 'U',
  Keyboard_V = 'V',
  Keyboard_W = 'W',
  Keyboard_X = 'X',
  Keyboard_Y = 'Y',
  Keyboard_Z = 'Z',

  Keyboard_OPEN_BRACKET = '[',
  Keyboard_BACKSLASH = '\\',
  Keyboard_CLOSE_BRACKET = ']',
  Keyboard_CARROT = '^',
  Keyboard_UNDERSCORE = '_',
  Keyboard_BACKQUOTE = '`',

  Keyboard_a = 'a',
  Keyboard_b = 'b',
  Keyboard_c = 'c',
  Keyboard_d = 'd',
  Keyboard_e = 'e',
  Keyboard_f = 'f',
  Keyboard_g = 'g',
  Keyboard_h = 'h',
  Keyboard_i = 'i',
  Keyboard_j = 'j',
  Keyboard_k = 'k',
  Keyboard_l = 'l',
  Keyboard_m = 'm',
  Keyboard_n = 'n',
  Keyboard_o = 'o',
  Keyboard_p = 'p',
  Keyboard_q = 'q',
  Keyboard_r = 'r',
  Keyboard_s = 's',
  Keyboard_t = 't',
  Keyboard_u = 'u',
  Keyboard_v = 'v',
  Keyboard_w = 'w',
  Keyboard_x = 'x',
  Keyboard_y = 'y',
  Keyboard_z = 'z',

  Keyboard_OPEN_BRACE = '{',
  Keyboard_OR = '|',
  Keyboard_CLOSE_BRACE = '}',
  Keyboard_TILDE = '~',
  Keyboard_DEL,

  Keyboard_LEFT_CTRL,
  Keyboard_LEFT_SHIFT,
  Keyboard_LEFT_ALT,
  Keyboard_RIGHT_SHIFT,
  Keyboard_RIGHT_CTRL,
  Keyboard_RIGHT_ALT,
  Keyboard_CAPS_LOCK,
  Keyboard_F1,
  Keyboard_F2,
  Keyboard_F3,
  Keyboard_F4,
  Keyboard_F5,
  Keyboard_F6,
  Keyboard_F7,
  Keyboard_F8,
  Keyboard_F9,
  Keyboard_F10,
  Keyboard_F11,
  Keyboard_F12,
  Keyboard_KEY_UP,
  Keyboard_KEY_DOWN,
  Keyboard_KEY_LEFT,
  Keyboard_KEY_RIGHT,
  Keyboard_KEY_HOME,
  Keyboard_KEY_END,
  Keyboard_KEY_INST,
  Keyboard_KEY_DEL,
  Keyboard_KEY_PG_UP,
  Keyboard_KEY_PG_DOWN,
  Keyboard_KEY_NUM,

  Keyboard_NA_CHAR = 0xFF
} KeyboardKeys ;

bool is_command_key(int ch);

#if defined __cplusplus
}
#endif

#endif

