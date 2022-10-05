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
  NN_L2_GUI,
  NN_ESC,
  NN_L2_ESC_GUI,
  NN_LANG1,
  NN_LANG1_GUI,
  NN_LANG1_ALT,
  NN_LANG1_LOW,
  NN_LANG1_SFT,
  NN_ENT,
  NN_RPIN1,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

/* Qwerty */
[_QWERTY] = LAYOUT_split_3x5_3(
      KC_Q,           KC_W,  KC_E,      KC_R,            KC_T,               KC_Y,    KC_U,         KC_I,    KC_O,    KC_P,
      LCTL_T(KC_A),   KC_S,  KC_D,      KC_F,            KC_G,               KC_H,    KC_J,         KC_K,    KC_L,    KC_COLN,
      LSFT_T(KC_Z),   KC_X,  KC_C,      KC_V,            KC_B,               KC_N,    KC_M,         KC_COMM, KC_DOT,  NN_RPIN1,
                             KC_ESC,    NN_L2_ESC_GUI,   RCTL_T(KC_SPC),     NN_ENT,  NN_LANG1_ALT, LT(3,KC_TAB)
),

/* Lower */
/*  1 2 3 4 5  6 7 8 9 0
    ( ) { } `  - = " ' :
    [ ] < > |  _ + ~ \ NO
 */
[_LOWER] = LAYOUT_split_3x5_3(
      KC_1,               KC_2,    KC_3,    KC_4,      KC_5,          KC_6,    KC_7,    KC_8,    KC_9,    KC_0,
      KC_LPRN,            KC_RPRN, KC_LCBR, KC_RCBR,   KC_GRV,        KC_MINS, KC_EQL,  KC_DQUO, KC_QUOT, KC_SCLN,
      KC_LBRC,            KC_RBRC, KC_LT,   KC_GT,     KC_PIPE,       KC_UNDS, KC_PLUS, KC_TILD, KC_BSLS, KC_NO,
                                   KC_TRNS, KC_TRNS,   KC_TRNS,       KC_TRNS, KC_TRNS, KC_TRNS
),

/* Raise */
/*  ! @ # $ %  ^ & * ? BS
    ( ) { } `  - = " ' :
    [ ] < > |  _ + ~ \ NO
*/
[_RAISE] = LAYOUT_split_3x5_3(
      KC_EXLM,            KC_AT,   KC_HASH, KC_DLR,    KC_PERC,       KC_CIRC, KC_AMPR, KC_ASTR, KC_QUES, KC_BSPC,
      KC_LPRN,            KC_RPRN, KC_LCBR, KC_RCBR,   KC_GRV,        KC_MINS, KC_EQL,  KC_DQUO, KC_QUOT, KC_SCLN,
      KC_LBRC,            KC_RBRC, KC_LT,   KC_GT,     KC_PIPE,       KC_UNDS, KC_PLUS, KC_TILD, KC_BSLS, KC_NO,
                                   KC_TRNS, KC_TRNS,   KC_TRNS,       KC_TRNS, KC_TRNS, KC_TRNS
),
/* Adjust (Lower + Raise) */
[_ADJUST] =  LAYOUT_split_3x5_3(
  KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,        KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,
  KC_F11,  KC_F12,  _______, _______, _______,      _______, _______, _______, KC_UP,   _______,
  KC_NO,   _______, _______, _______, _______,      KC_NO,   KC_NO,   KC_LEFT, KC_DOWN, KC_RGHT,
                    _______, _______, _______,      _______,  _______, _______
)
};

bool get_ignore_mod_tap_interrupt(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case LCTL_T(KC_A):
            return true;
        // hold shift のつもりが z tap に化ける事象が多発するため false
        case LSFT_T(KC_Z):
            return false;
        case LT(2,KC_SLSH):
            return true;
        case RCTL_T(KC_SPC):
            return true;
        default:
            return false;
    }
}
// uint16_t get_tapping_term(uint16_t keycode, keyrecord_t *record) {
//     switch (keycode) {
//         case LT(2,KC_SLSH):
//             return 300;
//         case LCTL_T(KC_A):
//             return 225;
//         case RCTL_T(KC_SPC):
//             return 100;
//         default:
//             return TAPPING_TERM;
//     }
// }

static bool hold_rsum1  = false;
static bool hold_rpin1  = false;
static bool hold_sum    = false;
static bool hold_sus    = false;
static bool hold_ctl    = false;
static bool hold_low    = false;
static bool hold_sft    = false;
static bool hold_alt    = false;
static bool hold_esc    = false;
static bool lang1_on    = false;
static uint16_t tap_timer;
bool process_record_user(uint16_t keycode, keyrecord_t *record) {

  // CTL + H to absolute BackSpace
  // if (keycode == KC_H) {
  //   if(record->event.pressed) {
  //     if( hold_sum || hold_ctl ){
  //       unregister_code(KC_LCTRL);
  //       unregister_code(KC_RCTRL);
  //       register_code(KC_BSPACE);
  //       return false;
  //     }
  //   }else{
  //     unregister_code(KC_BSPACE);
  //   }
  // }
  // Q or TAB
  if (keycode == KC_Q) {
    if(hold_sum){
      if (record->event.pressed) {
        unregister_code(KC_RCTL);
        tap_code(KC_TAB);
        return false;
      }
    }
    if(hold_sus){
      if (record->event.pressed) {
        tap_code(KC_TAB);
        return false;
      }
    }
  }

  switch (keycode) {
    case NN_RPIN1:
       if (record->event.pressed) {
        hold_rpin1 = true;
        layer_on(_LOWER);
        tap_timer = timer_read();
       } else {
        layer_off(_LOWER);
        if (hold_rpin1 && timer_elapsed(tap_timer) < 200) {
          tap_code(KC_SLSH);
        }
        hold_rpin1 = false;
       }
      return false;
      break;
    case NN_ENT:
       if (record->event.pressed) {
        hold_rsum1 = true;
        register_code(KC_RSFT);
        tap_timer = timer_read();
       } else {
        unregister_code(KC_RSFT);
        if (hold_rsum1 && timer_elapsed(tap_timer) < 200) {
          tap_code(KC_ENT);
        }
        hold_rsum1 = false;
       }
      return false;
      break;
    case LCTL_T(KC_A):
       if (record->event.pressed) {
        hold_ctl = true;
       } else {
        hold_ctl = false;
       }
      return true;
      break;
    case RCTL_T(KC_SPC):
      if (record->event.pressed) {
        hold_sum = true;
      } else {
        hold_sum = false;
      }
      return true;
      break;
    case NN_LANG1:
      if (record->event.pressed) {
        tap_code(KC_LANG1);
        lang1_on = true;
      }
      return false;
      break;
    case NN_LANG1_SFT:
      if (record->event.pressed) {
        hold_sft = true;
        register_code(KC_LSFT);
      } else {
        unregister_code(KC_LSFT);
        if (hold_sft) {
          lang1_on = true;
          tap_code(KC_LANG1);
        }
        hold_sft = false;
      }
      return false;
      break;
    case NN_LANG1_LOW:
      if (record->event.pressed) {
        hold_low = true;
        layer_on(_LOWER);
      } else {
        layer_off(_LOWER);
        if (hold_low) {
          lang1_on = true;
          tap_code(KC_LANG1);
        }
        hold_low = false;
      }
      return false;
      break;
    case NN_LANG1_ALT:
      if (record->event.pressed) {
        hold_alt = true;
        register_code(KC_RIGHT_ALT);
      } else {
        unregister_code(KC_RIGHT_ALT);
        if (hold_alt) {
          lang1_on = true;
          tap_code(KC_LANG1);
        }
        hold_alt = false;
      }
      return false;
      break;
    case NN_LANG1_GUI:
      if (record->event.pressed) {
        tap_code(KC_LANG1);
        lang1_on = true;
        register_code(KC_RIGHT_GUI);
      } else {
        unregister_code(KC_RIGHT_GUI);
      }
      return false;
      break;
    case NN_L2_ESC_GUI:
      if (record->event.pressed) {
        hold_esc = true;
        hold_sus = true;
        tap_code(KC_LANG2);
        register_code(KC_LEFT_GUI);
        tap_timer = timer_read();
      } else {
        unregister_code(KC_LEFT_GUI);
        if (!lang1_on && hold_esc && timer_elapsed(tap_timer) < 300) {
          tap_code(KC_ESC);
        }
        hold_esc = false;
        hold_sus = false;
        lang1_on = false;
      }
      return false;
      break;
    case NN_ESC:
      if(record->event.pressed) {
        tap_code(KC_LANG2);
        tap_code(KC_ESC);
      }
      return false;
      break;
    case NN_L2_GUI:
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
    default:
      hold_esc   = false;
      hold_alt   = false;
      hold_rsum1 = false;
      hold_rpin1 = false;
      if (hold_low) {
        tap_code(KC_LANG2);
        hold_low = false;
      }
      if (hold_sft) {
        tap_code(KC_LANG2);
        hold_sft = false;
      }
      break;
  }
  return true;
}
