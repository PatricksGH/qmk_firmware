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


// Globale Variablen
static uint16_t last_time = 0;
static uint8_t last_percent = 0;
static uint8_t oled_percent = 0;
static uint8_t last_oled_drawn = 255;


// Keymaps definieren
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    [0] = LAYOUT_1x1(
        KC_AUDIO_MUTE
    )
};


// ADC lesen und periodisch per RAW-HID senden
void matrix_scan_user(void) {
    if (timer_elapsed(last_time) < 100) return;
    last_time = timer_read();

    uint16_t adc_value = analogReadPin(ADC_PIN);
    uint32_t scaled = ((uint32_t)adc_value * 100 + 512) >> 10;
    uint8_t percent = (uint8_t)scaled;

    oled_percent = percent;

    uprintf("ADC: %u%%\n", percent);

    if (percent != last_percent) {
        last_percent = percent;

        uint8_t buf[32] = {0};
        buf[0] = percent;

        raw_hid_send(buf, 32);
    }
}


// OLED-Display mit ADC-Wert aktualisieren
bool oled_task_user(void) {
    if (oled_percent == last_oled_drawn) {
        return false; // nichts neu zeichnen
    }
    last_oled_drawn = oled_percent;

    oled_clear();

    oled_write_ln_P(PSTR("ADC VALUE"), false);
    oled_write_ln_P(PSTR("--------"), false);

    char buf[6];
    snprintf(buf, sizeof(buf), "%3u", oled_percent);
    oled_write_ln(buf, false);

    return false;
}


