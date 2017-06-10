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
#include "debug.h"
#include "app/memdirect.h"

extern panel_context_t* contexts[PANELS_NUM];
extern ITEM **my_items;
extern int n_choices;


#define WINDOW_INDEX	S0

#define MAIN_W		60
#define MAIN_H		20

#define MARGIN_FIELDS	2
#define MARGIN_BUTTONS	2
#define MARGIN_MENU MARGIN_BUTTONS
#define NB_FIELDS	4

static panel_init_t panel_init = {
        .win_title =            "Select memory region",
        .title_color =          GREEN_COLOR,
        .main_width =           MAIN_W,
        .main_heigth =          MAIN_H,
        .main_h_offset =        5,
        .main_v_offset =        3,
        .button_text =  {[0] = "OK_", [1] = "QUIT", [2] = "test"},
//        .field =        {[0]{.name ="Address:", .field_text = "00000000"},
//                         [1]{.name = "Amount:", .field_text = "0"}},
        .menu_width =           22,
        .menu_heigth =          28,
	.index =		WINDOW_INDEX
};

extern major_data_struct_t major_data_struct;



static int input_interface(panel_context_t* ctx, int ch);
static int open_window(panel_context_t* ctx);
static int close_window(panel_context_t* ctx);
static int init_win(panel_context_t* ctx);
static int state(panel_context_t* ctx, mmi_t* mmi);
void options();
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

void init_window0()
{
	contexts[WINDOW_INDEX] = &context;
}

static int handle_buttons(panel_context_t* ctx, ITEM *item)
{
        char *name = (char*)item_name(item);
	mem_record_ty* m_rec;
	major_data_struct_t *mds = &major_data_struct;
        int i;
DEBUG_S("RS1111")
DEBUG_S(name)

	m_rec =  queue_find_by_name(mds, name);

	mds->start = m_rec->start;
        mds->end = m_rec->end;
	strncpy(mds->name, name, FIELD_NAME_LEN-1);
	mds->name[FIELD_NAME_LEN-1] = '\0';	/* more reliable code*/

DEBUG_S(mds->name)

mvprintw(LINES-2, 1, "                                                           ");
mvprintw(LINES-2, 1, "[*] %X -- %X  : %s", m_rec->start, m_rec->end, m_rec->name);
        if (strcmp(name, "OK_") == 0) {
//                mvprintw(LINES-2, 1, "[*] OK clicked:\t");
/*
                for (i = 0; i < ctx->form->maxfield; i++) {
                        printw("%s", field_buffer(ctx->fields[i], 0));

                        if (field_opts(ctx->fields[i]) & O_ACTIVE)
                                printw("\t");
                }
*/

        } else if (strcmp(name, "QUIT") == 0){
//                mvprintw(LINES-2, 1, "[*] QUIT clicked, 'F1' to quit\n");
                return 0;
        }

        refresh();
        return 1;
}

static int state(panel_context_t* ctx, mmi_t* mmi)
{
	/* the state corresponding to the window handler */
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

//	ctx->input_state = ON_BUTTONS_STATE;
//	switch_to_buttons(ctx);

        switch (ch) {
                case KEY_DOWN:
//DEBUG_S("Down")
			menu_driver(ctx->menus[MENU_0], REQ_DOWN_ITEM);
//                        menu_driver(ctx->menus[MENU_0], REQ_SCR_UPAGE);

//			menu_driver(ctx->menus[MENU_0], REQ_SCR_ULINE);
DEBUG_S(item_name(current_item(ctx->menus[MENU_0])))
                        break;

                case KEY_UP:
DEBUG_S(item_name(current_item(ctx->menus[MENU_0])))

//DEBUG_S("Up")
			menu_driver(ctx->menus[MENU_0], REQ_UP_ITEM);
//                        menu_driver(ctx->menus[MENU_0], REQ_SCR_DPAGE);

//                        menu_driver(ctx->menus[MENU_0], REQ_SCR_DLINE);

			break;

                case KEY_LEFT:
//DEBUG_S("Left")

                        menu_driver(ctx->menus[MENU_0], REQ_LEFT_ITEM);
                        break;

                case KEY_RIGHT:
//DEBUG_S("Right")
                        menu_driver(ctx->menus[MENU_0], REQ_RIGHT_ITEM);
                        break;

                case '\n':
//DEBUG_S("CR")
                        if(0 == handle_buttons(ctx, current_item(ctx->menus[MENU_0]))){
                                return MMI_CHANGE_STATE;
                        }
			return MMI_CHANGE_STATE;

                        break;

                default:
//DEBUG_S("default")
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
	init_win(ctx);
        show_panel(ctx->panel);
        update_panels();
        doupdate();
}


static int close_window(panel_context_t* ctx)
{
	hide_panel(ctx->panel);
	update_panels();
	doupdate();
	delete_state_window(ctx, n_choices, NB_FIELDS);
}

static void create_menu_local(panel_context_t* ctx, int rows, int cols, char **buttons, int nb_buttons)
{
        int tmp, i;
	int margin = MARGIN_MENU;
	int buttons_num = 0;

        options();
//TODO get rid from a global n_choices.
	nb_buttons = n_choices;
	buttons_num = n_choices;


        ctx->items = malloc(sizeof(ITEM *) * (buttons_num+1));
//        ctx->items = malloc(sizeof(ITEM *) * (nb_buttons+1));
        assert(ctx->items);
DEBUG_I(nb_buttons)
/*
        for (i = 0; i < nb_buttons; i++) {
                ctx->items[i] = new_item(buttons[i], "");
                assert(ctx->items[i] != NULL);
        }
*/

//TODO get rid from a global my_items.
	ctx->menus[MENU_0] = new_menu(my_items);
        ctx->wins[MENU_WINDOW] = derwin(ctx->wins[MAIN_WINDOW], rows, cols, margin+2, margin);
        assert(ctx->menus[MENU_0] != NULL && ctx->wins[MENU_WINDOW] != NULL);
        box(ctx->wins[MENU_WINDOW], 0, 0);
        keypad(ctx->wins[MENU_WINDOW], TRUE);

        set_menu_win(ctx->menus[MENU_0], ctx->wins[MENU_WINDOW]);
        set_menu_format(ctx->menus[MENU_0],  rows-2, 1);
//        tmp = ctx->menus[MENU_0]->fcols * (ctx->menus[MENU_0]->namelen + ctx->menus[MENU_0]->spc_rows);
        tmp = nb_buttons/* + 4*/;
//DEBUG_I(tmp);
//DEBUG_I(rows);


        ctx->wins[MENU_INNER_WINDOW] = derwin(ctx->wins[MENU_WINDOW], rows - (2 * margin), cols -(2 * margin), margin, margin);
        assert(ctx->wins[MENU_INNER_WINDOW] != NULL);
//box(ctx->wins[MENU_INNER_WINDOW], 0, 0);

        set_menu_sub(ctx->menus[MENU_0], ctx->wins[MENU_INNER_WINDOW]);

        set_menu_mark(ctx->menus[MENU_0], "");
        assert(post_menu(ctx->menus[MENU_0]) == E_OK);
        ctx->input_state = ON_BUTTONS_STATE;
        switch_to_buttons(ctx);
}

static void make_menu_local(panel_context_t* ctx)
{
        char *buttons[] = { BUTTON_0, BUTTON_1, ctx->init->button_text[2] };
        create_menu_local(ctx, MAIN_H - 2*(MARGIN_MENU+1), MAIN_W - 2*MARGIN_MENU, buttons, n_choices);

}


int init_win(panel_context_t* ctx)
{
/*
	static bool init_done = FALSE;
	if(init_done){
		return 0;
	}
	init_done = TRUE;
*/
        create_panel(ctx, WIN_TITLE, MAIN_H_OFFSET, MAIN_V_OFFSET, MAIN_WIDTH, MAIN_HEIGHT);
DEBUG_S(__func__)
        update_panels();
	make_menu_local(ctx);
//	make_fields(ctx);
	wrefresh(ctx->wins[MAIN_WINDOW]);

        ctx->input_state = ON_FIELDS_STATE;
//        set_menu_fore(ctx->menus[MENU_0], A_NORMAL); // "hide" the button selection
	return 0;
}

static void delete_fields_local(panel_context_t* ctx, int nb_fields)
{
/*
No fields in the window
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
/*
        for (i = 0; i < nb_buttons; i++) {
                free_item(ctx->items[i]);
        }
*/
        for(i = 0; i < n_choices; ++i){
                free(my_items[i]);
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


