// Copyright 2023 QMK
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

#include "raw_hid.h"
#include "analog.h"
#include "print.h"

#define RAW_HID_BUFFER_SIZE 32


// Post Init Konfigurationen
void keyboard_post_init_user(void) {
  analogReference(ADC_REF_POWER);
  debug_enable=true;
  debug_matrix=false;
  //debug_keyboard=true;
  //debug_mouse=true;
}


// Keymaps definieren
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [0] = LAYOUT_1x1(
        KC_AUDIO_MUTE
    )
};


// ADC lesen und per RAW-HID senden
static uint16_t last_time = 0;
static uint8_t last_percent = 0;

void matrix_scan_user(void) {
    if (timer_elapsed(last_time) < 100) return;
    last_time = timer_read();

    uint16_t adc_value = analogReadPin(ADC_PIN);
    uint32_t scaled = ((uint32_t)adc_value * 100 + 512) >> 10;
    uint8_t percent = (uint8_t)scaled;

    uprintf("ADC: %u%%\n", percent);

    if (percent != last_percent) {
        last_percent = percent;

        uint8_t buf[32] = {0};
        buf[0] = percent;

        raw_hid_send(buf, 32);
    }
}

