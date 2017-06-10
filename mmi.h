/*
 * Memory and registers read and write aplication.
 *
 * Warning! Reading of some registers may crash of you system.
 * Writing to some registers of some deviced i.e. PMIC may damage your hardware.
 * Do not try access to devices you don't know.
 * Use the application on your own risk.
 *
 * Copyright (C) 2017 Rostislav Stepanov
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * Written by:
 * Rostislav Stepanov <stepanovr@yahoo.com>
 */
#ifndef __MMI_H__
#define __MMI_H__

typedef enum {
	INIT_MMI_STATE = 0,
	S0,
	S1,
        S2,
        S3,
        S4,
        S5,
        S6,
        S7,
	MMI_STATES_NUM
}mmi_state_t;

typedef enum {
	NO_MMI_EVENT,
        MMI_EVENT0,
        MMI_EVENT1,
	/* UI events */
        MMI_CHANGE_STATE_BACK,
	MMI_CHANGE_STATE,
	/* Application events */

        MMI_STOP,
        MMI_EVENTS_NUM
}mmi_event_t;

#define MMI_ITEM(state, name)					\
        [state]{						\
                .display_interface = name##_window_on,		\
                .undisplay_interface = name##_window_off,	\
                .input_interface = name##_input			\
        }

#define API_ON(name) void name##_window_on(void)
#define API_OFF(name) void name##_window_off(void)
#define API_INPUT(name) void name##_input(void)


typedef struct _interface_item_t{
        void(*display_interface)(void);
	void(*undisplay_interface)(void);
        void(*input_interface)(void);
}interface_item_t;


typedef struct _mmi_t{
	mmi_state_t state;
	mmi_event_t event;
        void(*input_interface)(void);
	interface_item_t interface_array[];
}mmi_t;

#endif
