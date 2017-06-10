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
#include "app/memdirect.h"
#include "win_interface.h"
#include "debug.h"

extern panel_context_t* contexts[PANELS_NUM];

#define WINDOW_INDEX	S2
#define MAIN_W          80
#define MAIN_H          40

#define MARGIN_FIELDS   2
#define MARGIN_BUTTONS  2
#define MARGIN_MENU MARGIN_BUTTONS
#define BUTTONS_NUM_LOCAL	2
#define NB_FIELDS	4
extern major_data_struct_t major_data_struct;

static panel_init_t panel_init = {
        .win_title =            "Write memory",
        .title_color =          GREEN_COLOR,
        .main_width =           MAIN_W,
        .main_heigth =          MAIN_H,
        .main_h_offset =        5,
        .main_v_offset =        3,
        .button_text =  {[1] = "Write", [0] = "Cancel"},
        .field =        {[0]{.name ="Address:", .field_text = "00000000"},
                         [1]{.name = "Amount:", .field_text = "00"}},
        .menu_width =           20,
        .menu_heigth =          20,
	.index =		WINDOW_INDEX
};

static int input_interface(panel_context_t* ctx, int ch);
static int open_window(panel_context_t* ctx);
static int close_window(panel_context_t* ctx);
static int init_win(panel_context_t* ctx);
static int state(panel_context_t* ctx, mmi_t* mmi);
static void delete_state_window(panel_context_t* ctx, int nb_buttons, int nb_fields);



static win_dev_t windev = {
//	.init = init_win,
	.input = input_interface,
	.start = open_window,
	.stop = close_window,
	.state = state
};

static panel_context_t context = {
	.init = &panel_init,
	.win_dev = &windev
};

void init_window2()
{
	contexts[WINDOW_INDEX] = &context;
}


validate_addr_t validate_addr(address_type_t addr)
{
	major_data_struct_t *mds = &major_data_struct;
	if(addr < mds->start){
		return ADDRESS_TOO_LOW;
	}
        if(addr > mds->end){
                return ADDRESS_TOO_HIGH;
        }
        if(addr % sizeof(address_type_t) != 0){
                return ADDRESS_ALIGN_ERROR;
        }
	return ADDRESS_VALID;
}

void write_value(panel_context_t* ctx, address_type_t addr, address_type_t val)
{
	int x, y;
	getmaxyx(ctx->wins[MAIN_WINDOW], y, x);

	mvwprintw(ctx->wins[MAIN_WINDOW], y - 4 , 3, "                                      ");
	switch(validate_addr(addr)){
                case ADDRESS_TOO_LOW:
			mvwprintw(ctx->wins[MAIN_WINDOW], y - 4 , 3, "----Address is below the range");
                        break;
                case ADDRESS_TOO_HIGH:
			mvwprintw(ctx->wins[MAIN_WINDOW], y - 4 , 3, "----Address is abowe the range");
                        break;
                case ADDRESS_ALIGN_ERROR:
			mvwprintw(ctx->wins[MAIN_WINDOW], y - 4 , 3, "----Address alignment error");
                        break;
                case ADDRESS_VALID:
			write_one(addr, &val);
                        break;
		default:
			break;
	}
}

static int handle_buttons(panel_context_t* ctx, ITEM *item)
{
        const char *name = item_name(item);
        int i;
	address_type_t addr, val;
        if (strcmp(name, "Write") == 0) {
                form_driver(ctx->form, REQ_VALIDATION);

//You might want to check for validation errors:

                if (form_driver(ctx->form, REQ_VALIDATION) != E_OK) {
                    // do something
                }
//DEBUG_S(field_buffer(ctx->fields[0], 0))
DEBUG_S(field_buffer(ctx->fields[1], 0))
addr=a2hex(field_buffer(ctx->fields[1], 0));
DEBUG_X(addr)
DEBUG_I(validate_addr(addr))
//DEBUG_S(field_buffer(ctx->fields[2], 0))
DEBUG_S(field_buffer(ctx->fields[3], 0))
val=a2hex(field_buffer(ctx->fields[3], 0));
DEBUG_X(val)
		write_value(ctx, addr, val);


//                snprintf(inputstring, 200, "%s", field_buffer(field[0], 0));
//                set_field_buffer(field[0], 0, "");

//                mvprintw(LINES-2, 1, "[*] OK clicked:\t");

        } else if (strcmp(name, "Cancel") == 0){
//                mvprintw(LINES-2, 1, "[*] Cancel clicked, 'F1' to Cancel\n");
                return 0;
        }

        refresh();
        return 1;
}


static int state(panel_context_t* ctx, mmi_t* mmi)
{
	if(mmi->event == MMI_CHANGE_STATE){
		leave_state();
		set_new_state(mmi, S1);
	}
if(mmi->event != NO_MMI_EVENT){
        DEBUG_I(mmi->event)
}

	mmi->event = NO_MMI_EVENT;

}


static int input_interface(panel_context_t* ctx, int ch)
{
        int status;

//        initscr();
//        keypad(stdscr, true);
//DEBUG_S("======================")
//do{
//DEBUG_S("======================")
//ch = getch();
//DEBUG_X(ch)

        switch (ch) {
                case KEY_DOWN:
//DEBUG_S("Down")
                        if (ctx->input_state == ON_BUTTONS_STATE){
                                break;
                        }

                        if (ctx->form->current == ctx->fields[ctx->form->maxfield-1]){
                                switch_to_buttons(ctx);
                        } else {
                                form_driver(ctx->form, REQ_NEXT_FIELD);
                        }
                        break;

                case KEY_UP:
//DEBUG_S("Up")

                        if (ctx->input_state == ON_BUTTONS_STATE) {
                                ctx->input_state = ON_FIELDS_STATE;
                                status = set_menu_fore(ctx->menus[MENU_0], A_NORMAL); // "hide" the button

                        } else
                                form_driver(ctx->form, REQ_PREV_FIELD);
                        break;

                case KEY_LEFT:
//DEBUG_S("Left")

                        if (ctx->input_state == ON_BUTTONS_STATE){

                                menu_driver(ctx->menus[MENU_0], REQ_LEFT_ITEM);
                        } else {
                                form_driver(ctx->form, REQ_PREV_CHAR /*REQ_LEFT_FIELD*/);
			}
                        break;

                case KEY_RIGHT:
//DEBUG_S("Right")
                        if (ctx->input_state == ON_BUTTONS_STATE) {
                                menu_driver(ctx->menus[MENU_0], REQ_RIGHT_ITEM);
                        } else {
                                form_driver(ctx->form, REQ_NEXT_CHAR /*REQ_RIGHT_FIELD*/);
			}
                        break;

                case '\n':
//DEBUG_S("CR")
                        if (!(ctx->input_state == ON_BUTTONS_STATE))
                                switch_to_buttons(ctx);
                        else
                                if(0 == handle_buttons(ctx, current_item(ctx->menus[MENU_0]))){
                                        return MMI_CHANGE_STATE;
                                }
                        break;
/*
                case KEY_LEFT:
                        form_driver(ctx->form, REQ_PREV_CHAR);
                        break;

                case KEY_RIGHT:
                        form_driver(ctx->form, REQ_NEXT_CHAR);
                        break;
*/
                // Delete the char before cursor
                case KEY_BACKSPACE:
                case 127:
                        form_driver(ctx->form, REQ_DEL_PREV);
                        break;

                // Delete the char under the cursor
                case KEY_DC:
                        form_driver(ctx->form, REQ_DEL_CHAR);
                        break;
                default:
//DEBUG_S("default")
                        if (!(ctx->input_state == ON_BUTTONS_STATE)){
                                if(((ch >= '0') && (ch <= '9')) ||
                                   ((ch >= 'a') && (ch <= 'f')) ||
                                   ((ch >= 'A') && (ch <= 'F'))){
                                        form_driver(ctx->form, ch);
                                }
                        }

                        break;

        }

        if (ctx->input_state == ON_BUTTONS_STATE)
                pos_menu_cursor(ctx->menus[MENU_0]);
        else
                pos_form_cursor(ctx->form);

        wrefresh(ctx->wins[MAIN_WINDOW]);

//}while(1);
        return 1;
}

static int open_window(panel_context_t* ctx)
{
	major_data_struct_t *mds = &major_data_struct;
	address_type_t val;
	char str[20];
	int y;
	int x;
	init_win(ctx);

	sprintf(str, "%08X", mds->page_start);
	set_field_buffer(ctx->fields[1], 0, str);

	read_one(mds->page_start, &val);


        sprintf(str, "%08X", val);
        set_field_buffer(ctx->fields[3], 0, str);

DEBUG_S("RS1112")
DEBUG_S(mds->name)

getmaxyx(ctx->wins[MAIN_WINDOW], y, x);
mvwprintw(ctx->wins[MAIN_WINDOW], y - 3 , 3, "name:  %s", mds->name);
mvwprintw(ctx->wins[MAIN_WINDOW], y - 2 , 3, "range: %08X -- %08X", mds->start, mds->end);

        show_panel(ctx->panel);
        update_panels();
        doupdate();
}


static int close_window(panel_context_t* ctx)
{
	hide_panel(ctx->panel);
	update_panels();
	doupdate();
	delete_state_window(ctx, BUTTONS_NUM_LOCAL, NB_FIELDS);
}

static void create_menu_local(panel_context_t* ctx, int rows, int cols, char **buttons, int nb_buttons)
{
        int tmp, i;

        ctx->items = malloc(sizeof(ITEM *) * (nb_buttons+1));
        assert(ctx->items);
        for (i = 0; i < nb_buttons; i++) {
                ctx->items[i] = new_item(buttons[i], "");
                assert(ctx->items[i] != NULL);
        }
        ctx->items[i] = NULL;
        ctx->menus[MENU_0] = new_menu(ctx->items);
        ctx->wins[MENU_WINDOW] = derwin(ctx->wins[MAIN_WINDOW], 4, cols-2, rows+5, 4);
        assert(ctx->menus[MENU_0] != NULL && ctx->wins[MENU_WINDOW] != NULL);
        box(ctx->wins[MENU_WINDOW], 0, 0);
        set_menu_win(ctx->menus[MENU_0], ctx->wins[MENU_WINDOW]);
        set_menu_format(ctx->menus[MENU_0], 1, nb_buttons);
        tmp = ctx->menus[MENU_0]->fcols * (ctx->menus[MENU_0]->namelen + ctx->menus[MENU_0]->spc_rows);
        tmp--;
        ctx->wins[MENU_INNER_WINDOW] = derwin(ctx->wins[MENU_WINDOW], 2, tmp, 1, (cols-3-tmp)/2);
        assert(ctx->wins[MENU_INNER_WINDOW] != NULL);
        set_menu_sub(ctx->menus[MENU_0], ctx->wins[MENU_INNER_WINDOW]);
        set_menu_mark(ctx->menus[MENU_0], "");
        assert(post_menu(ctx->menus[MENU_0]) == E_OK);
}

static void create_fields_local(panel_context_t* ctx, char **requests, int nb_fields)
{
        int i, cury = 0, curx = 1;
        WINDOW *inner;

        ctx->fields = malloc(sizeof(FIELD *) * (nb_fields+1));
        assert(ctx->fields);

        for (i = 0; i < nb_fields; i++) {
                ctx->fields[i] = new_field(1, 8, cury, curx, 0, 0);
                assert(ctx->fields[i] != NULL);
                set_field_buffer(ctx->fields[i], 0, requests[i]);

                if (i % 2 == 1) {
                        cury = cury+1;
                        curx = 1;
                        field_opts_on(ctx->fields[i], O_ACTIVE);
                        field_opts_on(ctx->fields[i], O_EDIT);
                        set_field_back(ctx->fields[i], A_UNDERLINE);
                } else {
                        curx = 20;
                        field_opts_off(ctx->fields[i], O_ACTIVE);
                        field_opts_off(ctx->fields[i], O_EDIT);
                }
        }

        ctx->fields[i] = NULL;
        ctx->form = new_form(ctx->fields);
DEBUG_X(ctx)
DEBUG_X(ctx->form )

        assert(ctx->form != NULL);
        ctx->wins[FIELDS_WINDOW] = derwin(ctx->wins[MAIN_WINDOW], ctx->form->rows+5, ctx->form->cols+4, 5, 5);
        box(ctx->wins[FIELDS_WINDOW], 0, 0);
        assert(ctx->form != NULL && ctx->wins[FIELDS_WINDOW] != NULL);
        set_form_win(ctx->form, ctx->wins[FIELDS_WINDOW]);

        ctx->wins[FIELDS_INNER_WINDOW] = derwin(ctx->wins[FIELDS_WINDOW], ctx->form->rows+2, ctx->form->cols+2, 2, 1);

        assert(ctx->wins[FIELDS_INNER_WINDOW] != NULL);
        set_form_sub(ctx->form, ctx->wins[FIELDS_INNER_WINDOW]);
        assert(post_form(ctx->form) == E_OK);
}

static void make_menu_local(panel_context_t* ctx)
{
        char *buttons[] = { BUTTON_0, BUTTON_1 };
        create_menu_local(ctx, MENU_ROWS, MENU_COLS, buttons, BUTTONS_NUM_LOCAL);
}

static void make_fields_local(panel_context_t* ctx, int nb_fields)
{
        char *requests[] = { TITLE_FIELD_0, FILL_FIELD_0, TITLE_FIELD_1, FILL_FIELD_1};
        create_fields_local(ctx, requests, NB_FIELDS);
}
static int init_win(panel_context_t* ctx)
{
/*
        static bool init_done = FALSE;
        if(init_done){
                return 0;
        }
        init_done = TRUE;
*/
        create_panel(ctx, WIN_TITLE, MAIN_H_OFFSET, MAIN_V_OFFSET, MAIN_WIDTH, MAIN_HEIGHT);

        update_panels();
	make_menu_local(ctx);
	make_fields_local(ctx, NB_FIELDS);
	wrefresh(ctx->wins[MAIN_WINDOW]);

        ctx->input_state = ON_FIELDS_STATE;
        set_menu_fore(ctx->menus[MENU_0], A_NORMAL); // "hide" the button
        switch_to_buttons(ctx);
	return 0;
}

static void delete_fields_local(panel_context_t* ctx, int nb_fields)
{
	int i;
	delwin(ctx->wins[FIELDS_WINDOW]);
	free_form(ctx->form);

        for (i = 0; i < nb_fields; i++) {
                free_field(ctx->fields[i]);
        }
        free(ctx->fields);
}


static void delete_menu_local(panel_context_t* ctx, int nb_buttons)
{
        int tmp, i;
	delwin(ctx->wins[MENU_INNER_WINDOW]);
	delwin(ctx->wins[MENU_WINDOW]);
	free_menu(ctx->menus[MENU_0]);
        for (i = 0; i < nb_buttons; i++) {
                free_item(ctx->items[i]);
        }
	free(ctx->items);
}

static void delete_panel(panel_context_t* ctx)
{
                del_panel(ctx->panel);
                delwin(ctx->wins[MAIN_WINDOW]);
}

static void delete_state_window(panel_context_t* ctx, int nb_buttons, int nb_fields)
{
	delete_fields_local(ctx, nb_fields);
	delete_menu_local(ctx, nb_buttons);
	delete_panel(ctx);
}


