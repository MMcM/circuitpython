/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 xx for Adafruit Industries
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <string.h>

#include "py/gc.h"
#include "py/runtime.h"
#include "shared-bindings/digitalio/DigitalInOut.h"
#include "shared-bindings/keypad/EventQueue.h"
#include "shared-bindings/keypad/MuxStrobeKeys.h"
#include "shared-bindings/keypad/__init__.h"
#include "shared-bindings/microcontroller/__init__.h"
#include "shared-bindings/supervisor/__init__.h"
#include "supervisor/port.h"
#include "supervisor/shared/tick.h"

static void muxstrobekeys_scan_now(void *self, mp_obj_t timestamp);
static size_t muxstrobekeys_get_key_count(void *self);

static keypad_scanner_funcs_t muxstrobekeys_funcs = {
    .scan_now = muxstrobekeys_scan_now,
    .get_key_count = muxstrobekeys_get_key_count,
};

void common_hal_keypad_muxstrobekeys_construct(keypad_muxstrobekeys_obj_t *self, size_t num_sense_pins, const mcu_pin_obj_t *sense_pins[], bool sense_value, bool sense_pull, mp_float_t sense_delay, size_t num_address_pins, const mcu_pin_obj_t *address_pins[], mp_int_t address_start, mp_int_t address_end, bool address_value, const mcu_pin_obj_t *enable_pin, bool enable_value, const mcu_pin_obj_t *gate_pin, bool gate_value, mp_float_t interval, size_t max_events) {

    digitalio_pull_t pull = sense_pull ? (sense_value ? PULL_DOWN : PULL_UP) : PULL_NONE;
    mp_obj_t sense_dios[num_sense_pins];
    for (size_t index = 0; index < num_sense_pins; index++) {
        digitalio_digitalinout_obj_t *dio = m_new_obj(digitalio_digitalinout_obj_t);
        dio->base.type = &digitalio_digitalinout_type;
        common_hal_digitalio_digitalinout_construct(dio, sense_pins[index]);
        common_hal_digitalio_digitalinout_switch_to_input(dio, pull);
        sense_dios[index] = dio;
    }
    self->sense_digitalinouts = mp_obj_new_tuple(num_sense_pins, sense_dios);
    self->sense_value = sense_value;
    self->sense_delay_us = (mp_uint_t)(sense_delay * 1000000);
    
    mp_obj_t address_dios[num_address_pins];
    for (size_t index = 0; index < num_address_pins; index++) {
        digitalio_digitalinout_obj_t *dio = m_new_obj(digitalio_digitalinout_obj_t);
        dio->base.type = &digitalio_digitalinout_type;
        common_hal_digitalio_digitalinout_construct(dio, address_pins[index]);
        common_hal_digitalio_digitalinout_switch_to_output(dio, !address_value, DRIVE_MODE_PUSH_PULL);
        address_dios[index] = dio;
    }
    self->address_digitalinouts = mp_obj_new_tuple(num_address_pins, address_dios);
    self->address_start = address_start;
    self->address_end = address_end;
    self->address_value = address_value;

    if (enable_pin != NULL) {
        digitalio_digitalinout_obj_t *enable = m_new_obj(digitalio_digitalinout_obj_t);
        enable->base.type = &digitalio_digitalinout_type;
        common_hal_digitalio_digitalinout_construct(enable, enable_pin);
        common_hal_digitalio_digitalinout_switch_to_output(enable, !enable_value, DRIVE_MODE_PUSH_PULL);
        self->enable_digitalinout = enable;
        self->enable_value = enable_value;
    } else {
        self->enable_digitalinout = MP_ROM_NONE;
    }

    if (gate_pin != NULL) {
        digitalio_digitalinout_obj_t *gate = m_new_obj(digitalio_digitalinout_obj_t);
        gate->base.type = &digitalio_digitalinout_type;
        common_hal_digitalio_digitalinout_construct(gate, gate_pin);
        common_hal_digitalio_digitalinout_switch_to_output(gate, !gate_value, DRIVE_MODE_PUSH_PULL);
        self->gate_digitalinout = gate;
        self->gate_value = gate_value;
    } else {
        self->gate_digitalinout = MP_ROM_NONE;
    }

    self->funcs = &muxstrobekeys_funcs;
    keypad_construct_common((keypad_scanner_obj_t *)self, interval, max_events);
}

void common_hal_keypad_muxstrobekeys_deinit(keypad_muxstrobekeys_obj_t *self) {
    if (common_hal_keypad_deinited(self)) {
        return;
    }

    // Remove self from the list of active keypad scanners first.
    keypad_deregister_scanner((keypad_scanner_obj_t *)self);


    if (self->gate_digitalinout != MP_ROM_NONE) {
        common_hal_digitalio_digitalinout_deinit(self->gate_digitalinout);
        self->gate_digitalinout = MP_ROM_NONE;
    }

    if (self->enable_digitalinout != MP_ROM_NONE) {
        common_hal_digitalio_digitalinout_deinit(self->enable_digitalinout);
        self->enable_digitalinout = MP_ROM_NONE;
    }

    for (size_t index = 0; index < self->address_digitalinouts->len; index++) {
        common_hal_digitalio_digitalinout_deinit(self->address_digitalinouts->items[index]);
    }
    self->address_digitalinouts = MP_ROM_NONE;

    for (size_t index = 0; index < self->sense_digitalinouts->len; index++) {
        common_hal_digitalio_digitalinout_deinit(self->sense_digitalinouts->items[index]);
    }
    self->sense_digitalinouts = MP_ROM_NONE;

    common_hal_keypad_deinit_core(self);
}

size_t muxstrobekeys_get_key_count(void *self_in) {
    keypad_muxstrobekeys_obj_t *self = self_in;
    return (self->address_end - self->address_start) * self->sense_digitalinouts->len;
}

static void muxstrobekeys_scan_now(void *self_in, mp_obj_t timestamp) {
    keypad_muxstrobekeys_obj_t *self = self_in;

    if (self->enable_digitalinout != MP_ROM_NONE) {
        common_hal_digitalio_digitalinout_set_value(self->enable_digitalinout, self->enable_value);
    }

    mp_uint_t key_number = 0;
    for (size_t address = self->address_start; address < self->address_end; address++) {
        for (size_t bit = 0; bit < self->address_digitalinouts->len; bit++) {
            common_hal_digitalio_digitalinout_set_value(self->address_digitalinouts->items[bit],
                                                        (address & (1 << bit)) ? self->address_value : !self->address_value);
        }
        if (self->gate_digitalinout != MP_ROM_NONE) {
            common_hal_digitalio_digitalinout_set_value(self->gate_digitalinout, self->gate_value);
        }
        if (self->sense_delay_us) {
            common_hal_mcu_delay_us(self->sense_delay_us);
        }
        for (size_t index = 0; index < self->sense_digitalinouts->len; index++) {
            const bool previous = self->currently_pressed[key_number];
            self->previously_pressed[key_number] = previous;
            const bool current = common_hal_digitalio_digitalinout_get_value(self->sense_digitalinouts->items[index]) == self->sense_value;
            self->currently_pressed[key_number] = current;
            if (previous != current) {
                keypad_eventqueue_record(self->events, key_number, current, timestamp);
            }
            key_number++;
        }
        if (self->gate_digitalinout != MP_ROM_NONE) {
            common_hal_digitalio_digitalinout_set_value(self->gate_digitalinout, !self->gate_value);
        }
    }

    if (self->enable_digitalinout != MP_ROM_NONE) {
        common_hal_digitalio_digitalinout_set_value(self->enable_digitalinout, !self->enable_value);
    }
}
