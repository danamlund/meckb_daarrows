/* Copyright 2017 REPLACE_WITH_YOUR_NAME
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "daarrows.h"
#include "tetris_text.h"

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
[0] = KEYMAP( /* Base */
  MO(1),   KC_UP,   MO(2),   \
  KC_LEFT, KC_DOWN, KC_RIGHT \
),
[1] = KEYMAP(
  KC_TRNS, KC_PGUP,   TO(3), \
  KC_HOME, KC_PGDOWN, KC_END \
),
[2] = KEYMAP(
  TO(3),   KC_PGUP,   KC_TRNS, \
  KC_HOME, KC_PGDOWN, KC_END   \
),
[3] = KEYMAP(
  TO(0), F(0), F(1), \
  RGB_TOG,  RGB_MOD, F(2) \
),
[4] = KEYMAP(
  KC_ESC,  KC_UP,   KC_ESC,  \
  KC_LEFT, KC_DOWN, KC_RIGHT \
)
};

const uint16_t PROGMEM fn_actions[] = {
  [0] = ACTION_FUNCTION(0),  // Calls action_function()
  [1] = ACTION_FUNCTION(1),  // Calls action_function()
  [2] = ACTION_FUNCTION(2),
};

const macro_t *action_get_macro(keyrecord_t *record, uint8_t id, uint8_t opt)
{
  // MACRODOWN only works in this function
      switch(id) {
        case 0:
          if (record->event.pressed) {
            register_code(KC_RSFT);
          } else {
            unregister_code(KC_RSFT);
          }
        break;
      }
    return MACRO_NONE;
};


void matrix_init_user(void) {

}

void led_set_user(uint8_t usb_led) {

}

static uint8_t daarrows_ascii = 0;
static uint8_t daarrows_i = 0;

uint16_t ascii_to_keycode(uint8_t ascii) {
  switch (ascii) {
  case 0x08: return KC_BSPACE;
  case 0x09: return KC_TAB;
  case 0x0A: return KC_ENTER;
  case 0x1B: return KC_ESCAPE;
  case 0x20: return KC_SPACE;
  case '0': return KC_0;
  case '1': return KC_1;
  case '2': return KC_2;
  case '3': return KC_3;
  case '4': return KC_4;
  case '5': return KC_5;
  case '6': return KC_6;
  case '7': return KC_7;
  case '8': return KC_8;
  case '9': return KC_9;
  case 'q': case 'Q': return KC_Q;
  case 'w': case 'W': return KC_W;
  case 'e': case 'E': return KC_E;
  case 'r': case 'R': return KC_R;
  case 't': case 'T': return KC_T;
  case 'y': case 'Y': return KC_Y;
  case 'u': case 'U': return KC_U;
  case 'i': case 'I': return KC_I;
  case 'o': case 'O': return KC_O;
  case 'p': case 'P': return KC_P;
  case 'a': case 'A': return KC_A;
  case 's': case 'S': return KC_S;
  case 'd': case 'D': return KC_D;
  case 'f': case 'F': return KC_F;
  case 'g': case 'G': return KC_G;
  case 'h': case 'H': return KC_H;
  case 'j': case 'J': return KC_J;
  case 'k': case 'K': return KC_K;
  case 'l': case 'L': return KC_L;
  case 'z': case 'Z': return KC_Z;
  case 'x': case 'X': return KC_X;
  case 'c': case 'C': return KC_C;
  case 'v': case 'V': return KC_V;
  case 'b': case 'B': return KC_B;
  case 'n': case 'N': return KC_N;
  case 'm': case 'M': return KC_M;
  }
  return KC_DOT;
}

static uint8_t tetris_key_presses = 0;
static uint16_t tetris_timer = 0;
static uint8_t tetris_running = 0;
static int tetris_keypress = 0;

void action_function(keyrecord_t *record, uint8_t id, uint8_t opt) {
  if (id == 2 && record->event.pressed) {
    tetris_running = ! tetris_running;
    if (tetris_running) {
      layer_on(4);
      tetris_keypress = 0;
      tetris_timer = 0;
      // set randomness using number of keys pressed
      tetris_start(tetris_key_presses);
    }
  } else {
    if (record->event.pressed) {
      daarrows_ascii = (daarrows_ascii << 1) + id;
      daarrows_i++;
      if (daarrows_i >= 8) {
        uint16_t k = ascii_to_keycode(daarrows_ascii);
       if (k != 0) {
          add_key(k);
          send_keyboard_report();
        }
      }
    } else {
      if (daarrows_i >= 8) {
        uint16_t k = ascii_to_keycode(daarrows_ascii);
        if (k != 0) {
          del_key(k);
          send_keyboard_report();
        }

        daarrows_ascii = 0;
        daarrows_i = 0;
      }
    }
  }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  if (record->event.pressed) {
    tetris_key_presses++;
  }

  if (tetris_running && record->event.pressed) {
    tetris_keypress = 0;
    switch (keycode) {
    case KC_UP: tetris_keypress = 1; break;
    case KC_LEFT: tetris_keypress = 2; break;
    case KC_DOWN: tetris_keypress = 3; break;
    case KC_RIGHT: tetris_keypress = 4; break;
    case KC_ESC: tetris_running = 0; layer_off(4); return false; // ESC quits tetris
    }
    if (tetris_keypress != 0) {
      return false;
    }
  }

  return true;
}


void matrix_scan_user(void) {
  if (tetris_running) {
    tetris_timer++;
    if (tetris_timer > 1000) {
      // every 1000 times this is run is about 100 ms.
      if (!tetris_tick(100)) {
        // game over
        tetris_running = 0;
      }
      tetris_timer = 0;
    }
  }      
}

void send_keycode(uint16_t keycode) {
  register_code(keycode);
  unregister_code(keycode);
}

void send_keycode_shift(uint16_t keycode) {
  register_code(KC_LSFT);
  register_code(keycode);
  unregister_code(keycode);
  unregister_code(KC_LSFT);
}

void tetris_send_up(void) {
  send_keycode(KC_UP);
}
void tetris_send_left(void) {
  send_keycode(KC_LEFT);
}
void tetris_send_down(void) {
  send_keycode(KC_DOWN);
}
void tetris_send_right(void) {
  send_keycode(KC_RGHT);
}

void tetris_send_backspace(void) {
  send_keycode(KC_BSPC);
}
void tetris_send_delete(void) {
  send_keycode(KC_DEL);
}

void tetris_send_string(const char *s) {
  for (int i = 0; s[i] != 0; i++) {
    if (s[i] >= 'a' && s[i] <= 'z') {
      send_keycode(KC_A + (s[i] - 'a'));
    } else if (s[i] >= 'A' && s[i] <= 'Z') {
      send_keycode_shift(KC_A + (s[i] - 'A'));
    } else if (s[i] >= '1' && s[i] <= '9') {
      send_keycode(KC_1 + (s[i] - '1'));
    } else {
      switch (s[i]) {
      case ' ': send_keycode(KC_SPACE); break;
      case '.': send_keycode(KC_DOT); break;
      case '0': send_keycode(KC_0); break;
      }
    }
  }
}

void tetris_send_newline(void) {
  send_keycode(KC_ENT);
}

int tetris_get_keypress(void) {
  int out = tetris_keypress;
  tetris_keypress = 0;
  return out;
}
