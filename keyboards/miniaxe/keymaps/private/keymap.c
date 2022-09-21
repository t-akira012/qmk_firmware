/* Copyright 2018 ENDO Katsuhiro <ka2hiro@curlybracket.co.jp>
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

// Defines the keycodes used by our macros in process_record_user
enum layer_names {
    _QWERTY,
    _LOWER,
    _RAISE,
    _ADJUST
};

enum custom_keycodes {
  QWERTY = SAFE_RANGE,
  LOWER,
  RAISE,
  ADJUST,
  KC_L1_HOLD,
  NN_LANG2,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

/* Qwerty */
[_QWERTY] = LAYOUT_split_3x5_3(
      KC_Q,           KC_W, KC_E,            KC_R,        KC_T,             KC_Y,            KC_U,             KC_I,    KC_O,    KC_P,
      LSFT_T(KC_A),   KC_S, KC_D,            KC_F,        KC_G,             KC_H,            KC_J,             KC_K,    KC_L,    KC_SCLN,
      LT(1,KC_Z),     KC_X, KC_C,            KC_V,        KC_B,             KC_N,            KC_M,             KC_COMM, KC_DOT,  LT(2, KC_SLSH),
                            KC_ESC,          NN_LANG2,    LCTL_T(KC_SPC),   LSFT_T(KC_ENT),  LALT_T(KC_LANG1), KC_COLN
),

/* Raise */
[_RAISE] = LAYOUT_split_3x5_3(
      KC_1,           KC_2,    KC_3,    KC_4,    KC_5,          KC_6,    KC_7,    KC_8,    KC_9,    KC_0,
      LSFT_T(KC_GRV), KC_LBRC, KC_RBRC, KC_LBRC, KC_RBRC,       KC_MINS, KC_PLUS, KC_DQUO, KC_QUOT, KC_COLN,
      KC_NO,          KC_LPRN, KC_RPRN, KC_LT,   KC_GT,         KC_UNDS, KC_EQL,  KC_BSLS, KC_NO,   KC_NO,
                               KC_TRNS, KC_TRNS, KC_TRNS,       KC_TRNS, KC_TRNS, KC_TRNS
),

/* Lower */
[_LOWER] = LAYOUT_split_3x5_3(
      KC_TAB,  KC_ESC,  KC_UP,   KC_LGUI,     KC_BSPC,       KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,
      KC_LSFT, KC_LEFT, KC_DOWN, KC_RGHT,     KC_NO,         KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,
      KC_NO,   KC_NO,   KC_NO,   KC_NO,       KC_NO,         KC_F11,  KC_F12,  KC_TRNS, KC_TRNS, KC_TRNS,
                        KC_TRNS, KC_TRNS,     KC_TRNS,       KC_TRNS, KC_TRNS, KC_TRNS
),


/* Adjust (Lower + Raise) */
[_ADJUST] =  LAYOUT_split_3x5_3(
  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,        KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,
  KC_F11,  KC_F12,  _______, _______, _______,      _______, _______, _______, _______, _______,
  QK_BOOT,   _______, _______, _______, _______,      KC_MPRV, KC_MNXT, KC_VOLD, KC_VOLU, KC_MPLY,
                    _______, _______, _______,      _______,  _______, _______
)
};

bool get_ignore_mod_tap_interrupt(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LSFT_T(KC_Z):
            return true;
        case LCTL_T(KC_A):
            return true;
        case LCTL_T(KC_SCLN):
            return true;
        case LCTL_T(KC_GRV):
            return true;
        default:
            return false;
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  switch (keycode) {
    case NN_LANG2:
      if(record->event.pressed) {
          tap_code(KC_LANG2);
          register_code(KC_LEFT_GUI);
      } else {
          unregister_code(KC_LEFT_GUI);
      }
      return false;
      break;
    case KC_L1_HOLD:
      if(record->event.pressed) {
          tap_code(KC_LANG1);
      } else {
          tap_code(KC_LANG2);
      }
      return false;
      break;
    case QWERTY:
      if (record->event.pressed) {
        // persistant_default_layer_set(1UL<<_QWERTY);
        set_single_persistent_default_layer(_QWERTY);
      }
      return false;
      break;
    case LOWER:
      if (record->event.pressed) {
        layer_on(_LOWER);
        update_tri_layer(_LOWER, _RAISE, _ADJUST);
      } else {
        layer_off(_LOWER);
        update_tri_layer(_LOWER, _RAISE, _ADJUST);
      }
      return false;
      break;
    case RAISE:
      if (record->event.pressed) {
        layer_on(_RAISE);
        update_tri_layer(_LOWER, _RAISE, _ADJUST);
      } else {
        layer_off(_RAISE);
        update_tri_layer(_LOWER, _RAISE, _ADJUST);
      }
      return false;
      break;
    case ADJUST:
      if (record->event.pressed) {
        layer_on(_ADJUST);
      } else {
        layer_off(_ADJUST);
      }
      return false;
      break;
  }
  return true;
}
