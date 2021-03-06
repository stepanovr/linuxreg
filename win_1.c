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
#include "app/memdirect.h"

#include "debug.h"

extern panel_context_t* contexts[PANELS_NUM];
extern major_data_struct_t major_data_struct;

#define WINDOW_INDEX	S1

#define MAIN_W          90
#define MAIN_H          40

#define MARGIN_FIELDS   2
#define MARGIN_BUTTONS  2
#define MARGIN_MENU MARGIN_BUTTONS

static char data_buffer[100];

#define COL_AMOUNT	8
#define ROW_AMOUNT      24
#define READ_PER_LINE (COL_AMOUNT * sizeof(address_type_t))
#define PAGE_SIZE	(READ_PER_LINE * ROW_AMOUNT)

#define BUTTONS_NUM_LOCAL	2
#define NB_FIELDS	4
static panel_init_t panel_init = {
        .win_title =            "Read memory",
        .title_color =          GREEN_COLOR,
        .main_width =           MAIN_W,
        .main_heigth =          MAIN_H,
        .main_h_offset =        5,
        .main_v_offset =        3,
        .button_text =  {[0] = "Back", [1] = "Write"},
        .field =        {[0]{.name ="Address:", .field_text = "00000000"},
                         [1]{.name = "Value:", .field_text = "0"}},
        .menu_width =           30,
        .menu_heigth =          30,
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

void init_window1()
{
	contexts[WINDOW_INDEX] = &context;
}

static int handle_buttons(panel_context_t* ctx, ITEM *item)
{
        const char *name = item_name(item);
        int i;

        if (strcmp(name, "Back") == 0) {
		return MMI_CHANGE_STATE_BACK;
//                mvprintw(LINES-4, 1, "[*] OK clicked:\t");
/*
                for (i = 0; i < ctx->form->maxfield; i++) {
                        printw("%s", field_buffer(ctx->fields[i], 0));

                        if (field_opts(ctx->fields[i]) & O_ACTIVE)
                                printw("\t");
                }
*/

        } else if (strcmp(name, "Write") == 0){
//                mvprintw(LINES-4, 1, "[*] QUIT clicked, 'F1' to quit\n");
                return MMI_CHANGE_STATE;
        }

        refresh();
        return 1;
}


static int state(panel_context_t* ctx, mmi_t* mmi)
{
	if(mmi->event == MMI_CHANGE_STATE){
		leave_state();
		set_new_state(mmi, S2);
	}

        if(mmi->event == MMI_CHANGE_STATE_BACK){
                leave_state();
                set_new_state(mmi, S0);
        }

if(mmi->event != NO_MMI_EVENT){
        DEBUG_I(mmi->event)
}

	mmi->event = NO_MMI_EVENT;

}


void read_from_memory(panel_context_t* ctx)
{
	int i;
	address_type_t first_val;
	mem_access_errors_t mem_access;
	major_data_struct_t *mds = &major_data_struct;
        address_type_t addr = mds->page_start;
        address_type_t num, end = mds->end + sizeof(address_type_t) -1;

	for(i = 0; i < ROW_AMOUNT; i++){
                num = addr + READ_PER_LINE;

                if(end <= num){
                        if(end > addr){
                                num = (end - addr)/sizeof(address_type_t);
                        } else {
                                break;
                        }
                }
		if(0 == num){
			break;
		}

//DEBUG_X(num)
		mem_access = read_mem(addr, &first_val, num, data_buffer);
		mds->first_value = first_val;

DEBUG_X(mds->page_start)
DEBUG_X(mds->first_value);
		switch(mem_access){
	        	case NO_MEM_ACCESS_ERROR:
				break;
	        	case MEM_ACCESS_RANGE_ERROR:
        	                mvprintw(LINES-2, 1, "                                          ");
				mvprintw(LINES-2, 1, "Range error");
                        	break;
	                case MEM_ACCESS_NOT_ROOT:
        	                mvprintw(LINES-2, 1, "                                          ");
                	        mvprintw(LINES-2, 1, "Root only allowed");
                        	break;
	                case MEM_ACCESS_FILE_ERROR:
        	                mvprintw(LINES-2, 1, "                                          ");
                	        mvprintw(LINES-2, 1, "File error");
                        	break;
	                case MEM_ACCESS_MAP_ERROR:
        	                mvprintw(LINES-2, 1, "                                          ");
                	        mvprintw(LINES-2, 1, "Mapping error");
                        	break;
			default:
        	                mvprintw(LINES-2, 1, "                                          ");
                	        mvprintw(LINES-2, 1, "Non specified error");
                        	break;
		}
DEBUG_X(addr)
DEBUG_X(end)
DEBUG_S(data_buffer)
		mvwprintw(ctx->wins[MAIN_WINDOW], 5 + i, 2, "%08X: ", addr);
	        mvwprintw(ctx->wins[MAIN_WINDOW], 5 + i, 12, "%s", data_buffer);

		addr += READ_PER_LINE;
	}
}

static int input_interface(panel_context_t* ctx, int ch)
{
        int status;
	major_data_struct_t *mds = &major_data_struct;
        switch (ch) {
/*
                case KEY_DOWN:
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
                        if (ctx->input_state == ON_BUTTONS_STATE) {
                                ctx->input_state = ON_FIELDS_STATE;
                                status = set_menu_fore(ctx->menus[MENU_0], A_NORMAL); // "hide" the button

                        } else
                                form_driver(ctx->form, REQ_PREV_FIELD);
                        break;
*/
                case KEY_LEFT:
                        if (ctx->input_state == ON_BUTTONS_STATE)
                                menu_driver(ctx->menus[MENU_0], REQ_LEFT_ITEM);
                        else
                                form_driver(ctx->form, REQ_LEFT_FIELD);
                        break;

                case KEY_RIGHT:
                        if (ctx->input_state == ON_BUTTONS_STATE)
                                menu_driver(ctx->menus[MENU_0], REQ_RIGHT_ITEM);
                        else
                                form_driver(ctx->form, REQ_RIGHT_FIELD);
                        break;

		case KEY_NPAGE: /* Page down */
			{
				address_type_t  addr = mds->page_start;
				addr += PAGE_SIZE;
				if(addr < mds->end){
					mds->page_start = addr;
					read_from_memory(ctx);
				}
			}
DEBUG_S("PAGE_DOWN")
			break;

		case KEY_PPAGE: /* Page up */
			{
				address_type_t  addr = mds->page_start;
				if(addr > PAGE_SIZE){
					addr -= PAGE_SIZE;
				}
				if(addr >= mds->start){
					mds->page_start = addr;
					read_from_memory(ctx);
				}
			}DEBUG_S("PAGE_UP")
			break;

                case '\n':
                        if (!(ctx->input_state == ON_BUTTONS_STATE))
                                switch_to_buttons(ctx);
                        else{
				mmi_event_t event;
				event = handle_buttons(ctx, current_item(ctx->menus[MENU_0]));
                                return event;
			}
                        break;

                default:
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
        return 1;
}

static int open_window(panel_context_t* ctx)
{
	major_data_struct_t *mds = &major_data_struct;
	init_win(ctx);
        show_panel(ctx->panel);

	mds->page_start = mds->start;
	read_from_memory(ctx);
        update_panels();
        doupdate();
}


static int close_window(panel_context_t* ctx)
{
	int i;
        for(i = 0; i < ROW_AMOUNT; i++){
                mvwprintw(ctx->wins[MAIN_WINDOW], 5 + i, 2,
                "                                                                                 ");
        }

	hide_panel(ctx->panel);
	update_panels();
	doupdate();
	delete_state_window(ctx, BUTTONS_NUM_LOCAL, NB_FIELDS);
}

static void create_menu_local(panel_context_t* ctx, int rows, int cols, char **buttons, int nb_buttons)
{
        int tmp, i;

DEBUG_I(rows)
DEBUG_I(cols)

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
        create_fields_local(ctx, requests, nb_fields);
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
//	make_fields_local(ctx, 4);
	wrefresh(ctx->wins[MAIN_WINDOW]);

        ctx->input_state = ON_FIELDS_STATE;
        set_menu_fore(ctx->menus[MENU_0], A_NORMAL); // "hide" the button
        switch_to_buttons(ctx);
	return 0;
}

static void delete_fields_local(panel_context_t* ctx, int nb_fields)
{
/*
Not used in the window
	int i;
	delwin(ctx->wins[FIELDS_WINDOW]);
	free_form(ctx->form);

        for (i = 0; i < nb_fields; i++) {
                free_field(ctx->fields[i]);
        }
        free(ctx->fields);
*/
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


