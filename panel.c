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

/********************************/

extern mmi_t mmi_instance;

void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string, chtype color);
void delete_panel(panel_context_t* ctx);

panel_context_t* contexts[PANELS_NUM];

void init_display()
{
        /* Initialize curses */
        initscr();
        start_color();
        cbreak();
        noecho();
        keypad(stdscr, TRUE);
        INIT_COLOR(BLACK);
        INIT_COLOR(BLUE);
        INIT_COLOR(RED);
        INIT_COLOR(MAGENTA);
        INIT_COLOR(GREEN);
        INIT_COLOR(CYAN);
        INIT_COLOR(YELLOW);
        INIT_COLOR(WHITE);
	init_window_init();
        init_window0();
        init_window1();
        init_window2();
        init_window3();
}


void disable_display(panel_context_t* ctx)
{
	delete_panel(ctx);
        endwin();
}

panel_context_t* init_panel(panel_context_t* ctx)
{
        if(ctx){
                ctx->wins[MAIN_WINDOW] = newwin(ctx->h, ctx->w, ctx->y, ctx->x);
                ctx->panel = new_panel(ctx->wins[MAIN_WINDOW]);
        }
        win_show(ctx->wins[MAIN_WINDOW], ctx->title, TITLE_COLOR);
        set_panel_userptr(ctx->panel, ctx);
	return ctx;
}

panel_context_t* create_panel(panel_context_t* ctx, char* title, int x, int y, int w, int h)
{
        ctx->title = title;
        ctx->x = x;
        ctx->y = y;
        ctx->w = w;
        ctx->h = h;
//	contexts[index] = ctx;
	init_panel(ctx);
//assign to contexts[]
	return ctx;
}


void delete_panel(panel_context_t* ctx)
{
	int i;
	WINDOW *win;
        MENU *menu;

        if(ctx->items){
                free(ctx->items);
        }

        if(ctx->form){
	        free_form(ctx->form);
	}

	if(ctx->fields){
		free(ctx->fields);
	}
        for(i = 0; i < MENUS_NUM; i++){
                menu = ctx->menus[i];
                if(menu){
                        free_menu(menu);
                }
        }

        del_panel(ctx->panel);

	for(i = 0; i < WINDOWS_NUM; i++){
		win = ctx->wins[i];
		if(win){
        		delwin(win);
		}
	}
}

void create_menu(panel_context_t* ctx, int rows, int cols, char **buttons, int nb_buttons)
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

//void create_fields(panel_context_t* ctx, int rows, int cols, char **requests, int nb_fields)
void create_fields(panel_context_t* ctx, char **requests, int nb_fields)
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

void make_menu(panel_context_t* ctx)
{
        char *buttons[] = { BUTTON_0, BUTTON_1 };
	create_menu(ctx, MENU_ROWS, MENU_COLS, buttons, 2);
}


void make_fields(panel_context_t* ctx, int nb_fields)
{
        char *requests[] = { TITLE_FIELD_0, FILL_FIELD_0, TITLE_FIELD_1, FILL_FIELD_1};
	create_fields(ctx, requests, nb_fields);
}

/*
 * Actions for 'return' on a button
 */
int driver_buttons(panel_context_t* ctx, ITEM *item)
{
        const char *name = item_name(item);
        int i;

        if (strcmp(name, "OK") == 0) {
                mvprintw(LINES-2, 1, "[*] OK clicked:\t");

                for (i = 0; i < ctx->form->maxfield; i++) {
                        printw("%s", field_buffer(ctx->fields[i], 0));

                        if (field_opts(ctx->fields[i]) & O_ACTIVE)
                                printw("\t");
                }

        } else if (strcmp(name, "QUIT") == 0){
                mvprintw(LINES-2, 1, "[*] QUIT clicked, 'F1' to quit\n");
                return 0;
        }

        refresh();
        return 1;
}

/*
 * When you want to change between the form and the buttons
 */
void switch_to_buttons(panel_context_t* ctx)
{
	int status;
        // Those 2 lines allow the field buffer to be set
        form_driver(ctx->form, REQ_PREV_FIELD);
        form_driver(ctx->form, REQ_NEXT_FIELD);

        menu_driver(ctx->menus[MENU_0], REQ_FIRST_ITEM);
        ctx->input_state = ON_BUTTONS_STATE;
        status = set_menu_fore(ctx->menus[MENU_0], A_REVERSE); // "show" the button
}

/* Show the window with a border and a label */
void win_show(WINDOW *win, char *label, int label_color)
{	int startx, starty, height, width;

	getbegyx(win, starty, startx);
	getmaxyx(win, height, width);

	box(win, 0, 0);
	mvwaddch(win, 2, 0, ACS_LTEE);
	mvwhline(win, 2, 1, ACS_HLINE, width - 2);
	mvwaddch(win, 2, width - 1, ACS_RTEE);

	print_in_middle(win, 1, 0, width, label, COLOR_PAIR(label_color));
}

void print_in_middle(WINDOW *win, int starty, int startx, int width, char *string, chtype color)
{	int length, x, y;
	float temp;

	if(win == NULL)
		win = stdscr;
	getyx(win, y, x);
	if(startx != 0)
		x = startx;
	if(starty != 0)
		y = starty;
	if(width == 0)
		width = 80;

	length = strlen(string);
	temp = (width - length)/ 2;
	x = startx + (int)temp;
	wattron(win, color);
	mvwprintw(win, y, x, "%s", string);
	wattroff(win, color);
	refresh();
}

panel_context_t* entry_state(mmi_state_t state)
{
	panel_context_t* ctx;
	mmi_t *mmi;
        ctx = contexts[state];
	mmi = &mmi_instance;
	mmi->state = state;
DEBUG_I(mmi->state);
DEBUG_X(&mmi)
        if(!ctx){
                return ctx;
        }

	if(ctx->win_dev->init){
	        ctx->win_dev->init(ctx);
	}
	if(ctx->win_dev->start){
	        ctx->win_dev->start(ctx);
	}
	return ctx;
}

void leave_state()
{
	panel_context_t* ctx;
        mmi_t *mmi;
        mmi = &mmi_instance;
DEBUG_I(mmi->state)
	ctx = contexts[mmi->state];
	ctx->win_dev->stop(ctx);
}

panel_context_t* get_context()
{
        mmi_t *mmi;
        mmi = &mmi_instance;
	return contexts[mmi->state];
}

int main_test()
{
        int ch;
        panel_context_t* ctx;

        init_display();

	ctx = entry_state(0);
	if(!ctx){
		return -1;
	}
        while ((ch = getch()) != KEY_ESC)
        {
		if(0 == ctx->win_dev->input(ctx, ch)) break;
        }
	leave_state();

        ctx = entry_state(1);
        if(!ctx){
                return -1;
        }
        while ((ch = getch()) != KEY_ESC)
        {
                if(0 == ctx->win_dev->input(ctx, ch)) break;
        }
	leave_state();

	usleep(100000);


        disable_display(ctx);
        return 0;
}


