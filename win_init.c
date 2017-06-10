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
#include <panel.h>
#include <stdlib.h>
#include <string.h>
#include <form.h>
#include <menu.h>
#include <stdio.h>
#include <assert.h>

#include "mmi.h"
#include "win_interface.h"

extern panel_context_t* contexts[PANELS_NUM];

#define WINDOW_INDEX	INIT_MMI_STATE
static panel_init_t panel_init = {
        .win_title =            "None",
        .title_color =          GREEN_COLOR,
        .main_width =           80,
        .main_heigth =          40,
        .main_h_offset =        5,
        .main_v_offset =        3,
        .button_text =  {[0] = "OK", [1] = "QUIT"},
        .field =        {[0]{.name ="Address:", .field_text = "00000000"},
                         [1]{.name = "Amount:", .field_text = "0"}},
        .menu_width =           20,
        .menu_heigth =          20,
	.index =		WINDOW_INDEX
};

static int input_interface(panel_context_t* ctx, int ch);
static int open_window(panel_context_t* ctx);
static int close_window(panel_context_t* ctx);
static int init_win(panel_context_t* ctx);


static win_dev_t windev = {
	.init = init_win,
/*
	.input = input_interface,
	.start = open_window,
*/
	.stop = close_window
};

static panel_context_t context = {
	.init = &panel_init,
	.win_dev = &windev
};

void init_window_init()
{
	contexts[WINDOW_INDEX] = &context;
}

static int input_interface(panel_context_t* ctx, int ch)
{
	return MMI_CHANGE_STATE;
}

static int open_window(panel_context_t* ctx)
{
}


static int close_window(panel_context_t* ctx)
{
}

static int init_win(panel_context_t* ctx)
{
}
