/*
 * This file is part of the Micro Python project, http://micropython.org/
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2022 xxx for Adafruit Industries
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

#ifndef MICROPY_INCLUDED_SHARED_BINDINGS_KEYPAD_MUXSTROBEKEYS_H
#define MICROPY_INCLUDED_SHARED_BINDINGS_KEYPAD_MUXSTROBEKEYS_H

#include "py/objlist.h"
#include "shared-module/keypad/MuxStrobeKeys.h"

extern const mp_obj_type_t keypad_muxstrobekeys_type;

void common_hal_keypad_muxstrobekeys_construct(keypad_muxstrobekeys_obj_t *self, size_t num_sense_pins, const mcu_pin_obj_t *sense_pins[], bool sense_value, bool sense_pull, mp_float_t sense_delay, size_t num_address_pins, const mcu_pin_obj_t *address_pins[], mp_int_t address_start, mp_int_t address_end, bool address_value, const mcu_pin_obj_t *enable_pin, bool enable_value, const mcu_pin_obj_t *gate_pin, bool gate_value, mp_float_t interval, size_t max_events);

void common_hal_keypad_muxstrobekeys_deinit(keypad_muxstrobekeys_obj_t *self);

#endif  // MICROPY_INCLUDED_SHARED_BINDINGS_KEYPAD_MUXSTROBEKEYS_H
