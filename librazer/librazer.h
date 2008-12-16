/*
 *   Razer device access library
 *
 *   Copyright (C) 2007 Michael Buesch <mb@bu3sch.de>
 *
 *   This program is free software; you can redistribute it and/or
 *   modify it under the terms of the GNU General Public License
 *   as published by the Free Software Foundation; either version 2
 *   of the License, or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 */

#ifndef LIB_RAZER_H_
#define LIB_RAZER_H_

#if defined(c_plusplus) || defined(__cplusplus)
extern "C" {
#endif


#define RAZER_IDSTR_MAX_SIZE	128

struct razer_mouse;
struct razer_mouse_base_ops;


/** enum razer_led_state - The LED state value
  * @RAZER_LED_OFF: The LED is turned off
  * @RAZER_LED_ON: The LED is turned on
  * @RAZER_LED_UNKNOWN: The LED is in an unknown state (on or off)
  */
enum razer_led_state {
	RAZER_LED_OFF		= 0,
	RAZER_LED_ON		= 1,
	RAZER_LED_UNKNOWN,
};

/** struct razer_led - A LED on a razer device.
  *
  * @next: The next LED device in the linked list.
  *
  * @name: The human readable name string for the LED.
  * @id: A unique ID cookie
  * @state: The state of the LED (on, off, unknown)
  *
  * @toggle_state: Toggle the state. Note that a new_state of
  * 	RAZER_LED_UNKNOWN result in an error.
  *
  * @u: This union contains a pointer to the parent device.
  */
struct razer_led {
	struct razer_led *next;

	const char *name;
	unsigned int id;
	enum razer_led_state state;

	int (*toggle_state)(struct razer_led *led,
			    enum razer_led_state new_state);

	union {
		struct razer_mouse *mouse;
	} u;
};

/** enum razer_mouse_freq - Mouse scan frequency
  * @RAZER_MOUSE_FREQ_UNKNOWN: Unknown scan frequency
  * @RAZER_MOUSE_FREQ_125HZ: 100Hz scan frequency
  * @RAZER_MOUSE_FREQ_500HZ: 500Hz scan frequency
  * @RAZER_MOUSE_FREQ_1000HZ: 1000Hz scan frequency
  */
enum razer_mouse_freq {
	RAZER_MOUSE_FREQ_UNKNOWN	= 0,
	RAZER_MOUSE_FREQ_125HZ		= 125,
	RAZER_MOUSE_FREQ_500HZ		= 500,
	RAZER_MOUSE_FREQ_1000HZ		= 1000,
};

/** enum razer_mouse_res - Mouse scan resolutions
  * @RAZER_MOUSE_RES_UNKNOWN: Unknown scan resolution
  * @RAZER_MOUSE_RES_400DPI: 400DPI scan resolution
  * @RAZER_MOUSE_RES_450DPI: 450DPI scan resolution
  * @RAZER_MOUSE_RES_900DPI: 900DPI scan resolution
  * @RAZER_MOUSE_RES_1600DPI: 1600DPI scan resolution
  * @RAZER_MOUSE_RES_1800DPI: 1800DPI scan resolution
  */
enum razer_mouse_res {
	RAZER_MOUSE_RES_UNKNOWN		= 0,
	RAZER_MOUSE_RES_400DPI		= 400,
	RAZER_MOUSE_RES_450DPI		= 450,
	RAZER_MOUSE_RES_900DPI		= 900,
	RAZER_MOUSE_RES_1600DPI		= 1600,
	RAZER_MOUSE_RES_1800DPI		= 1800,
};

/** enum razer_mouse_type
  * @RAZER_MOUSETYPE_DEATHADDER: A "DeathAdder" mouse
  * @RAZER_MOUSETYPE_KRAIT: A "Krait" mouse
  * @RAZER_MOUSETYPE_LACHESIS: A "Lachesis" mouse
  */
enum razer_mouse_type {
	RAZER_MOUSETYPE_DEATHADDER,
	RAZER_MOUSETYPE_KRAIT,
	RAZER_MOUSETYPE_LACHESIS,
};

/** struct razer_mouse - Representation of a mouse device
  *
  * @next: Linked list to the next mouse.
  *
  * @idstr: A system wide unique ID string for the device.
  *
  * @type: The mouse type
  *
  * @claim: Claim and open the backend device (USB).
  * 	As long as the device is claimed, it is not operable by the user!
  *
  * @release: Release a claimed backend device.
  *
  * @get_fw_version: Read the firmware version from the device.
  *     Returns the firmware version or a negative error code.
  *
  * @get_leds: Get a linked list of available LEDs.
  * 	Returns the number of LEDs or a negative error code.
  * 	leds_list points to the first LED in the list.
  * 	The caller is responsible to free every item in leds_list.
  *
  * @supported_freqs: Get an array of supported scan frequencies.
  * 	Returns the array size or a negative error code.
  * 	freq_ptr points to the array.
  * 	The caller is responsible to free freq_ptr.
  *
  * @get_freq: Get the currently used scan frequency.
  *
  * @set_freq: Change the mouse scan frequency.
  *
  * @supported_resolutions: Get an array of supported scan resolutions.
  * 	Returns the array size or a negative error code.
  * 	res_ptr points to the array.
  * 	The caller is responsible to free res_ptr.
  *
  * @get_resolution: Get the currently used scan resolution.
  *
  * @set_resolution: Change the mouse scan resolution.
  */
struct razer_mouse {
	struct razer_mouse *next;

	char idstr[RAZER_IDSTR_MAX_SIZE];

	enum razer_mouse_type type;

	int (*claim)(struct razer_mouse *m);
	void (*release)(struct razer_mouse *m);

	int (*get_fw_version)(struct razer_mouse *m);

	int (*get_leds)(struct razer_mouse *m,
			struct razer_led **leds_list);

	int (*supported_freqs)(struct razer_mouse *m,
			       enum razer_mouse_freq **freq_ptr);
	enum razer_mouse_freq (*get_freq)(struct razer_mouse *m);
	int (*set_freq)(struct razer_mouse *m, enum razer_mouse_freq freq);

	int (*supported_resolutions)(struct razer_mouse *m,
				     enum razer_mouse_res **res_ptr);
	enum razer_mouse_res (*get_resolution)(struct razer_mouse *m);
	int (*set_resolution)(struct razer_mouse *m, enum razer_mouse_res res);

	/* Do not touch these pointers. */
	const struct razer_mouse_base_ops *base_ops;
	void *internal;
};

/** razer_free_freq_list - Free an array of frequencies.
  * This function frees a whole array of frequencies as returned
  * by the device methods.
  */
void razer_free_freq_list(enum razer_mouse_freq *freq_list, int count);

/** razer_free_resolution_list - Free an array of resolutions.
  * This function frees a whole array of resolutions as returned
  * by the device methods.
  */
void razer_free_resolution_list(enum razer_mouse_res *res_list, int count);

/** razer_free_leds - Free a linked list of struct razer_led.
  * This function frees a whole linked list of struct razer_led,
  * as returned by the device methods. Note that you can
  * also free a single struct razer_led with this function, if
  * you assign a NULL pointer to led_list->next before calling this.
  */
void razer_free_leds(struct razer_led *led_list);

/** razer_rescan_mice - Rescan for connected razer mice.
  * Returns a pointer to the linked list of mice, or a NULL pointer
  * in case of an error.
  */
struct razer_mouse * razer_rescan_mice(void);

/** razer_init - LibRazer initialization
  * Call this before any other library function.
  */
int razer_init(void);

/** razer_exit - LibRazer cleanup
  * Call this after any operation with the library.
  */
void razer_exit(void);


#if defined(c_plusplus) || defined(__cplusplus)
} /* extern "C" */
#endif

#endif /* LIB_RAZER_H_ */
