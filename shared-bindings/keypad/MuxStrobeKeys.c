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

#include "shared/runtime/context_manager_helpers.h"
#include "py/binary.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "shared-bindings/keypad/__init__.h"
#include "shared-bindings/keypad/Event.h"
#include "shared-bindings/keypad/MuxStrobeKeys.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/util.h"

//| class MuxStrobeKeys:
//|     """Manage a set of keys selected by a multiplexer."""
//|
//|     def __init__(
//|         self,
//|         *,
//|         sense_pins: Sequence[microcontroller.Pin],
//|         sense_value: bool = True,
//|         sense_pull: bool = True,
//|         sense_delay: float = 0,
//|         address_pins: Sequence[microcontroller.Pin],
//|         address_start: int = 0,
//|         address_end: Optional[int] = None,
//|         address_value: bool = True,
//|         enable_pin: Optional[microcontroller.Pin] = None,
//|         enable_value: bool = True,
//|         gate_pin: Optional[microcontroller.Pin] = None,
//|         gate_value: bool = True
//|     ) -> None:
//|         """
//|         Create a `Keys` object that will scan keys attached to a multiplexer like the 74159.
//|         An address is loaded into `address_pins` to select a key or set of keys similar to a `KeyMatrix` row.
//|         Key states are then read from `sense_pins` similar to a `KeyMatrix` column.
//|
//|         Key number 0 is the address times the number of sense pins plus the sense offset.
//|
//|         An `EventQueue` is created when this object is created and is available in the `events` attribute.
//|
//|         :param Sequence[microcontroller.Pin] sense_pins: The pins to read for a given address.
//|         :param bool sense_value: ``True`` if the pin reads high when the key is pressed.
//|           ``False`` if the pin reads low (is grounded) when the key is pressed.
//|         :param bool sense_pull: ``True`` if an internal pull-up or pull-down should be
//|           enabled on sense pins. A pull-up will be used if ``sense_value`` is ``False``;
//|           a pull-down will be used if it is ``True``.
//|         :param float sense_delay: Fraction of a second to delay after loading a new address before reading sense pins.
//|         :param Sequence[microcontroller.Pin] address_pins: The pins used to load an address into the multiplexer.
//|           The pins correspond to the bits of the address, least significant bit first.
//|         :param int address_start: The first used address.
//|           The default is ``0``.
//|         :param int address_end: The (exclusive) last used address.
//|           The default is the largest number using the given number of bits.
//|         :param bool address_value: ``True`` if the pin is set high for a ``1`` bit and low for a ``0`` bit.
//|           ``False`` for the other way around.
//|         :param Optional[microcontroller.Pin] enable_pin: A pin to set while scanning.
//|           The pin is set for the entire scan.
//|         :param bool enable_value: value to set ``enable_pin`` to while scanning.
//|         :param Optional[microcontroller.Pin] gate_pin: A pin to set while sensing.
//|           The pin is set and then unset for each address.
//|         :param bool gate_value: value to set ``gate_pin`` to while sensing.
//|         :param float interval: Scan keys no more often than ``interval`` to allow for debouncing.
//|           ``interval`` is in float seconds. The default is 0.020 (20 msecs).
//|         :param int max_events: maximum size of `events` `EventQueue`:
//|           maximum number of key transition events that are saved.
//|           Must be >= 1.
//|           If a new event arrives when the queue is full, the oldest event is discarded.
//|         """
//|         ...

STATIC mp_obj_t keypad_muxstrobekeys_make_new(const mp_obj_type_t *type, size_t n_args, size_t n_kw, const mp_obj_t *all_args) {
    keypad_muxstrobekeys_obj_t *self = m_new_obj(keypad_muxstrobekeys_obj_t);
    self->base.type = &keypad_muxstrobekeys_type;
    enum { ARG_sense_pins, ARG_sense_value, ARG_sense_pull, ARG_sense_delay, ARG_address_pins, ARG_address_start, ARG_address_end, ARG_address_value, ARG_enable_pin, ARG_enable_value, ARG_gate_pin, ARG_gate_value, ARG_interval, ARG_max_events };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_sense_pins, MP_ARG_KW_ONLY | MP_ARG_REQUIRED | MP_ARG_OBJ },
        { MP_QSTR_sense_value, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
        { MP_QSTR_sense_pull, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
        { MP_QSTR_sense_delay, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_address_pins, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_address_start, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_address_end, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 0} },
        { MP_QSTR_address_value, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
        { MP_QSTR_enable_pin, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_enable_value, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
        { MP_QSTR_gate_pin, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = mp_const_none} },
        { MP_QSTR_gate_value, MP_ARG_KW_ONLY | MP_ARG_BOOL, {.u_bool = true} },
        { MP_QSTR_interval, MP_ARG_KW_ONLY | MP_ARG_OBJ, {.u_obj = MP_OBJ_NULL} },
        { MP_QSTR_max_events, MP_ARG_KW_ONLY | MP_ARG_INT, {.u_int = 64} },
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all_kw_array(n_args, n_kw, all_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    mp_obj_t sense_pins = args[ARG_sense_pins].u_obj;
    const size_t num_sense_pins = (size_t)MP_OBJ_SMALL_INT_VALUE(mp_obj_len(sense_pins));
    const bool sense_value = args[ARG_sense_value].u_bool;
    const bool sense_pull = args[ARG_sense_pull].u_bool;
    const mp_float_t sense_delay = mp_arg_validate_obj_float_non_negative(args[ARG_sense_delay].u_obj, 0.0f, MP_QSTR_sense_delay);

    mp_obj_t address_pins = args[ARG_address_pins].u_obj;
    const size_t num_address_pins = address_pins == MP_OBJ_NULL ? 0 : (size_t)MP_OBJ_SMALL_INT_VALUE(mp_obj_len(address_pins));
    const mp_int_t max_address = 1 << num_address_pins;
    const mp_int_t address_start = mp_arg_validate_int_max(args[ARG_address_start].u_int, max_address, MP_QSTR_address_start);
    const mp_int_t address_end = args[ARG_address_end].u_int == 0 ? max_address :
        mp_arg_validate_int_range(args[ARG_address_end].u_int, address_start + 1, max_address, MP_QSTR_address_end);
    const bool address_value = args[ARG_address_value].u_bool;

    const mcu_pin_obj_t *enable_pin = validate_obj_is_free_pin_or_none(args[ARG_enable_pin].u_obj, MP_QSTR_enable_pin);
    const bool enable_value = args[ARG_enable_value].u_bool;
    const mcu_pin_obj_t *gate_pin = validate_obj_is_free_pin_or_none(args[ARG_gate_pin].u_obj, MP_QSTR_gate_pin);
    const bool gate_value = args[ARG_gate_value].u_bool;

    const mp_float_t interval =
        mp_arg_validate_obj_float_non_negative(args[ARG_interval].u_obj, 0.020f, MP_QSTR_interval);
    const size_t max_events = (size_t)mp_arg_validate_int_min(args[ARG_max_events].u_int, 1, MP_QSTR_max_events);

    if (num_address_pins > 0) {
        validate_no_duplicate_pins_2(sense_pins, address_pins, MP_QSTR_sense_pins, MP_QSTR_address_pins);
    }

    const mcu_pin_obj_t *sense_pins_array[num_sense_pins];
    for (size_t index = 0; index < num_sense_pins; index++) {
        const mcu_pin_obj_t *pin =
            validate_obj_is_free_pin(mp_obj_subscr(sense_pins, MP_OBJ_NEW_SMALL_INT(index), MP_OBJ_SENTINEL), MP_QSTR_sense_pins);
        sense_pins_array[index] = pin;
    }

    const mcu_pin_obj_t *address_pins_array[num_address_pins];
    for (size_t index = 0; index < num_address_pins; index++) {
        const mcu_pin_obj_t *pin =
            validate_obj_is_free_pin(mp_obj_subscr(address_pins, MP_OBJ_NEW_SMALL_INT(index), MP_OBJ_SENTINEL), MP_QSTR_address_pins);
        address_pins_array[index] = pin;
    }

    common_hal_keypad_muxstrobekeys_construct(self,
        num_sense_pins, sense_pins_array, sense_value, sense_pull, sense_delay,
        num_address_pins, address_pins_array, address_start, address_end, address_value,
        enable_pin, enable_value, gate_pin, gate_value,
        interval, max_events);

    return MP_OBJ_FROM_PTR(self);
}

//|     def deinit(self) -> None:
//|         """Stop scanning and release the pins."""
//|         ...
STATIC mp_obj_t keypad_muxstrobekeys_deinit(mp_obj_t self_in) {
    keypad_muxstrobekeys_obj_t *self = MP_OBJ_TO_PTR(self_in);

    common_hal_keypad_muxstrobekeys_deinit(self);
    return MP_ROM_NONE;
}
MP_DEFINE_CONST_FUN_OBJ_1(keypad_muxstrobekeys_deinit_obj, keypad_muxstrobekeys_deinit);

//|     def __enter__(self) -> Keys:
//|         """No-op used by Context Managers."""
//|         ...
//  Provided by context manager helper.

//|     def __exit__(self) -> None:
//|         """Automatically deinitializes when exiting a context. See
//|         :ref:`lifetime-and-contextmanagers` for more info."""
//|         ...
STATIC mp_obj_t keypad_muxstrobekeys___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_keypad_muxstrobekeys_deinit(args[0]);
    return MP_ROM_NONE;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(keypad_muxstrobekeys___exit___obj, 4, 4, keypad_muxstrobekeys___exit__);

//|     def reset(self) -> None:
//|         """Reset the internal state of the scanner to assume that all keys are now released.
//|         Any key that is already pressed at the time of this call will therefore immediately cause
//|         a new key-pressed event to occur.
//|         """
//|         ...

//|     key_count: int
//|     """The number of keys that are being scanned. (read-only)
//|     """
//|

//|     events: EventQueue
//|     """The `EventQueue` associated with this `Keys` object. (read-only)
//|     """
//|

STATIC const mp_rom_map_elem_t keypad_muxstrobekeys_locals_dict_table[] = {
    { MP_ROM_QSTR(MP_QSTR_deinit),       MP_ROM_PTR(&keypad_muxstrobekeys_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__),    MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__),     MP_ROM_PTR(&keypad_muxstrobekeys___exit___obj) },

    { MP_ROM_QSTR(MP_QSTR_events),       MP_ROM_PTR(&keypad_generic_events_obj) },
    { MP_ROM_QSTR(MP_QSTR_key_count),    MP_ROM_PTR(&keypad_generic_key_count_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset),        MP_ROM_PTR(&keypad_generic_reset_obj) },
};

STATIC MP_DEFINE_CONST_DICT(keypad_muxstrobekeys_locals_dict, keypad_muxstrobekeys_locals_dict_table);

const mp_obj_type_t keypad_muxstrobekeys_type = {
    { &mp_type_type },
    .name = MP_QSTR_MuxStrobeKeys,
    .make_new = keypad_muxstrobekeys_make_new,
    .locals_dict = (mp_obj_t)&keypad_muxstrobekeys_locals_dict,
};
