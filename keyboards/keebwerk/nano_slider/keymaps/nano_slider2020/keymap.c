/* Copyright 2020 Duckle
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
#include QMK_KEYBOARD_H
#include "analog.h"
#include "qmk_midi.h"


bool is_alt_tab_active = false;
uint16_t alt_tab_timer = 0;

// Defines names for use in layer keycodes and the keymap
enum layer_names {
    _BASE,
    _FN,
    _MEDIA
};

// Defines the keycodes used by our macros in process_record_user
enum custom_keycodes {
    ALT_TAB = SAFE_RANGE,
    ALT_STAB
};


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    /* Base */
    [_BASE] = LAYOUT(
        TG(1),
        KC_PAUSE,    ALT_TAB,   LGUI(KC_C),
        KC_SCROLLLOCK,    ALT_STAB,    LGUI(KC_V),    LGUI(LCTL(KC_Q))
    ),
    [_FN] = LAYOUT(
        TG(1),
        KC__VOLDOWN, KC__MUTE,  KC__VOLUP,
        KC_MEDIA_REWIND,  KC_MEDIA_PLAY_PAUSE, KC_MEDIA_FAST_FORWARD, LGUI(LCTL(KC_Q))
    ),    
        [_MEDIA] = LAYOUT(
        TO(_BASE),
        KC_VOLD, KC_VOLU, KC_F24,
        KC_MRWD, KC_MFFD, KC_F23, KC_MPLY
    )
};
   

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case ALT_TAB:
      if (record->event.pressed) {
        if (!is_alt_tab_active) {
             is_alt_tab_active = true;
             register_code(KC_LGUI);
            }
            alt_tab_timer = timer_read();
            register_code(KC_TAB);
        }   else {
        unregister_code(KC_TAB);
      }
      break;
      case ALT_STAB:                        
      if (record->event.pressed) {
        if (!is_alt_tab_active) {
             is_alt_tab_active = true;
             register_code(KC_LGUI);
             register_code(KC_LSHIFT);
            }
            alt_tab_timer = timer_read();
            register_code(KC_TAB);
        }   else {
        unregister_code(KC_TAB);
      }
      break;

  }
  return true;
}

uint8_t divisor = 0;

void slider(void) {
    if (divisor++) { // only run the slider function 1/256 times it's called
        return;
    }

    midi_send_cc(&midi_device, 2, 0x3E, 0x7F - (analogReadPin(SLIDER_PIN) >> 3));
}

void matrix_scan_user(void) {
  if (is_alt_tab_active) {
    if (timer_elapsed(alt_tab_timer) > 500) {
      unregister_code(KC_LGUI);
      unregister_code(KC_LSHIFT);
          is_alt_tab_active = false;
    }
  }
  slider();
}

