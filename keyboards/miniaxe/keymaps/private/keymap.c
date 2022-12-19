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
  NN_RSUM1,
  NN_RSUM2,
  NN_LSUM2,
  NN_RPIN1,
  EMOJI,
  M_COP,
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

/* Qwerty */
[_QWERTY] = LAYOUT_split_3x5_3(
      KC_Q,           KC_W,  KC_E,      KC_R,            KC_T,               KC_Y,     KC_U,         KC_I,    KC_O,    KC_P,
      KC_A,           KC_S,  KC_D,      KC_F,            KC_G,               KC_H,     KC_J,         KC_K,    KC_L,    LT(3,KC_SCLN),
      KC_Z,           KC_X,  KC_C,      KC_V,            KC_B,               KC_N,     KC_M,         KC_COMM, KC_DOT,  NN_RPIN1,
                             KC_LALT,   NN_LSUM2,        RCTL_T(KC_SPC),     NN_RSUM1, NN_RSUM2, KC_DEL
),

/* Lower */
/*  1 2 3 4 5  6 7 8 9 0
    ( ) { } \  - = " ' :
    [ ] < > |  _ + ` ~ N
 */
[_LOWER] = LAYOUT_split_3x5_3(
      KC_1,         KC_2,    KC_3,    KC_4,      KC_5,          KC_6,    KC_7,    KC_8,    KC_9,    KC_0,
      KC_LPRN,      KC_RPRN, KC_LCBR, KC_RCBR,   KC_BSLS,       KC_MINS, KC_EQL,  KC_DQUO, KC_QUOT, KC_COLN,
      KC_LBRC,      KC_RBRC, KC_LT,   KC_GT,     KC_PIPE,       KC_UNDS, KC_PLUS, KC_GRV,  KC_TILD, KC_NO,
                             M_COP,   KC_TRNS,   KC_TRNS,       MO(_RAISE), KC_TRNS, KC_TRNS
),

/* Raise */
/*  ! @ # $ %  ^ & * ? BS
    ( ) { } \  - = " ' :
    [ ] < > |  _ + ` ~ N
*/
[_RAISE] = LAYOUT_split_3x5_3(
      KC_EXLM,      KC_AT,   KC_HASH, KC_DLR,    KC_PERC,       KC_CIRC, KC_AMPR, KC_ASTR, KC_QUES, KC_BSPC,
      KC_LPRN,      KC_RPRN, KC_LCBR, KC_RCBR,   KC_BSLS,       KC_MINS, KC_EQL,  KC_DQUO, KC_QUOT, KC_COLN,
      KC_LBRC,      KC_RBRC, KC_GRV,  KC_TILD,   KC_PIPE,       KC_UNDS, KC_PLUS, KC_LT,   KC_GT,   KC_NO,
                             EMOJI,   KC_TRNS,   KC_TRNS,       KC_NO,   KC_TRNS, KC_TRNS
),
/* Adjust (Lower + Raise) */
[_ADJUST] =  LAYOUT_split_3x5_3(
  KC_F1,           KC_F2,   KC_F3,   KC_F4,   KC_F5,        KC_NO,   KC_UP,   KC_NO,   KC_NO,   KC_NO,
  LCTL_T(KC_F6),   KC_F7,   KC_F8,   KC_F9,   KC_F10,       KC_LEFT, KC_DOWN, KC_RGHT, KC_NO,   KC_NO,
  LSFT_T(KC_F11),  KC_F12,  _______, _______, _______,      KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,
                            EMOJI,   KC_TRNS, KC_TRNS,      KC_ENT,  KC_TRNS, KC_TRNS
)
};

static bool presscheck_low  = false;
static bool presscheck_ent  = false;
static bool presscheck_ln1  = false;
static bool presscheck_esc  = false;
static bool presscheck_z    = false;
static bool hold_rsum1      = false;
static bool hold_lsum1      = false;
static bool hold_lsum2      = false;
static bool pressed_sft     = false;
static bool pressed_gui     = false;
static bool lang1_on        = false;
static bool bspc_on         = true;
static uint16_t tap_timer;
bool process_record_user(uint16_t keycode, keyrecord_t *record) {

  // GUI + M の無効化
  if ( hold_lsum2 && keycode == KC_M ) {
    return false;
  }

  // emacs like keybind
  if ( bspc_on ) {
    if ( !(keycode == KC_H && keycode == KC_BSPC) ) {
      unregister_code(KC_BSPACE);
      bspc_on = false;
    }
  }
  if ( hold_lsum1 && !(pressed_gui || pressed_sft) ) {
    // lsum1 押下中なら ctrl on にする
    if (keycode && record->event.pressed) {
      register_code(KC_RCTL);
    }

    // Absolute Backspace
    if (keycode == KC_H) {
      if(record->event.pressed) {
        bspc_on = true;
        unregister_code(KC_LCTL);
        unregister_code(KC_RCTL);
        register_code(KC_BSPACE);
        return false;
      }else{
        unregister_code(KC_BSPACE);
        bspc_on = false;
        return false;
      }
    }

    // Absolute TAB
    if (keycode == KC_I) {
      if(record->event.pressed) {
        unregister_code(KC_RCTL);
        tap_code(KC_TAB);
        return false;
      }
    }

    // Absolute ENT
    if (keycode == KC_M) {
      if(record->event.pressed) {
        unregister_code(KC_RCTL);
        tap_code(KC_ENT);
        return false;
      }
    }
  }


  // CTL + Q to TAB
  if ( hold_lsum1 && keycode == KC_Q ){
    if (record->event.pressed) {
      unregister_code(KC_RCTL);
      tap_code(KC_TAB);
      return false;
    }
  }
  if ( hold_lsum2 && keycode == KC_Q ){
    if (record->event.pressed) {
      tap_code(KC_TAB);
      return false;
    }
  }

  switch (keycode) {
    case M_COP:
      if(record->event.pressed){
        register_code(KC_LSFT);
        register_code(KC_LGUI);
        tap_code(KC_C);
      }else{
        unregister_code(KC_LSFT);
        unregister_code(KC_LGUI);
      }
      return false;
      break;
    case EMOJI:
      if(record->event.pressed){
        register_code(KC_LCTL);
        register_code(KC_LGUI);
        tap_code(KC_SPC);
      }else{
        unregister_code(KC_LCTL);
        unregister_code(KC_LGUI);
      }
      return false;
      break;
    case KC_A:
      if(record->event.pressed){
        presscheck_low = false;
        presscheck_esc = false;
        presscheck_ent = false;
        presscheck_ln1 = false;
        if(hold_lsum2||hold_lsum1||hold_rsum1){
          register_code(KC_LCTL);
        }
        tap_timer = timer_read();
      } else {
        unregister_code(KC_LCTL);
        if(!(hold_lsum2||hold_lsum1||hold_rsum1)||timer_elapsed(tap_timer) < 200){
          tap_code(KC_A);
        }
      }
      return false;
      break;
    case KC_Z:
      if(record->event.pressed){
        presscheck_low = false;
        presscheck_esc = false;
        presscheck_ent = false;
        presscheck_ln1 = false;
        presscheck_z = true;
        if(hold_lsum2||hold_lsum1){
          pressed_sft = true;
          register_code(KC_LSFT);
        }
        tap_timer = timer_read();
      } else {
        pressed_sft = false;
        unregister_code(KC_LSFT);
        if(!(hold_lsum2||hold_lsum1||presscheck_z)||timer_elapsed(tap_timer) < 200){
          tap_code(KC_Z);
        }
        presscheck_z = false;
      }
      return false;
      break;
    case NN_RPIN1:
      if (record->event.pressed) {
        presscheck_esc = false;
        presscheck_ent = false;
        presscheck_ln1 = false;
        layer_on(_LOWER);
        presscheck_low = true;
        tap_timer = timer_read();
      } else {
        layer_off(_LOWER);
        if (presscheck_low && timer_elapsed(tap_timer) < 200) {
          tap_code(KC_SLSH);
        }
        presscheck_low = false;
      }
      return false;
      break;
    case NN_RSUM1:
      if (record->event.pressed) {
        hold_rsum1 = true;
        presscheck_ent = true;
        tap_timer = timer_read();
        pressed_sft = true;
        register_code(KC_RSFT);
      } else {
        pressed_sft = false;
        unregister_code(KC_RSFT);
        if (presscheck_ent && timer_elapsed(tap_timer) < 180) {
          if (presscheck_z){
            register_code(KC_LSFT);
          }
          tap_code(KC_ENT);
          unregister_code(KC_LSFT);
        }
        presscheck_ent = false;
        hold_rsum1 = false;
      }
      return false;
      break;
    case RCTL_T(KC_SPC):
      presscheck_esc = false;
      presscheck_ent = false;
      presscheck_ln1 = false;
      if (record->event.pressed) {
        hold_lsum1 = true;
        hold_lsum1 = true;
      } else {
        hold_lsum1 = false;
      }
      return true;
      break;
    case NN_RSUM2:
      if (record->event.pressed) {
        presscheck_ln1 = true;
        register_code(KC_RIGHT_ALT);
        tap_timer = timer_read();
      } else {
        unregister_code(KC_RIGHT_ALT);
        if (presscheck_ln1 && timer_elapsed(tap_timer) < 180) {
          lang1_on = true;
          tap_code(KC_LANG1);
        } else {
          lang1_on = false;
          tap_code(KC_LANG2);
        }
        presscheck_ln1 = false;
      }
      return false;
      break;
    case NN_LSUM2:
      if (record->event.pressed) {
        presscheck_esc = true;
        hold_lsum2 = true;
        pressed_gui = true;
        tap_code(KC_LANG2);
        register_code(KC_LEFT_GUI);
        tap_timer = timer_read();
      } else {
        unregister_code(KC_LEFT_GUI);
        if (!lang1_on && presscheck_esc && timer_elapsed(tap_timer) < 150) {
          tap_code(KC_ESC);
        }
        presscheck_esc = false;
        pressed_gui = false;
        hold_lsum2 = false;
        lang1_on = false;
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
      presscheck_z     = false;
      presscheck_esc   = false;
      presscheck_ln1   = false;
      presscheck_ent   = false;
      presscheck_low   = false;
      break;
  }
  return true;
}
