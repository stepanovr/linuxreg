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

#include <form.h>
#include <menu.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>


WINDOW *win_body, *win_form, *win_menu;
FORM *form;
FIELD **fields;
MENU *menu;
ITEM **items;
bool is_on_button; // Used to know the "case" we're in

void create_menu(int rows, int cols, char **buttons, int nb_buttons)
{
	int tmp, i;
        WINDOW *inner;

        items = malloc(sizeof(ITEM *) * (nb_buttons+1));
        assert(items);
        for (i = 0; i < nb_buttons; i++) {
                items[i] = new_item(buttons[i], "");
                assert(items[i] != NULL);
        }

        items[i] = NULL;
        menu = new_menu(items);
        win_menu = derwin(win_body, 3, cols-2, rows-4, 1);
        assert(menu != NULL && win_menu != NULL);
        box(win_menu, 0, 0);
        set_menu_win(menu, win_menu);
        set_menu_format(menu, 1, nb_buttons);
        tmp = menu->fcols * (menu->namelen + menu->spc_rows);
        tmp--;
        inner = derwin(win_menu, 2, tmp, 1, (cols-3-tmp)/2);
        assert(inner != NULL);
        set_menu_sub(menu, inner);
        set_menu_mark(menu, "");
//	return tmp;
}



void create_menu1(int rows, int cols, char **buttons, int nb_buttons)
{
        int tmp, i;
        WINDOW *inner;

        items = malloc(sizeof(ITEM *) * (nb_buttons+1));
        assert(items);
        for (i = 0; i < nb_buttons; i++) {
                items[i] = new_item(buttons[i], "");
                assert(items[i] != NULL);
        }

        items[i] = NULL;
        menu = new_menu(items);
//        win_menu = derwin(win_body, 3, cols-2, rows-4, 1);
        win_menu = derwin(win_body, 3, cols-2, rows-5, 1);
        assert(menu != NULL && win_menu != NULL);
        box(win_menu, 0, 0);
        set_menu_win(menu, win_menu);
        set_menu_format(menu, 1, nb_buttons);
        tmp = menu->fcols * (menu->namelen + menu->spc_rows);
        tmp--;
        inner = derwin(win_menu, 2, tmp, 1, (cols-3-tmp)/2);
        assert(inner != NULL);
        set_menu_sub(menu, inner);
        set_menu_mark(menu, "");
//      return tmp;
}

void create_fields(int rows, int cols, char **requests, int nb_fields)
{
	int i, cury = 0, curx = 1;
	WINDOW *inner;

	fields = malloc(sizeof(FIELD *) * (nb_fields+1));
        assert(fields);

        for (i = 0; i < nb_fields; i++) {
                fields[i] = new_field(1, 8, cury, curx, 0, 0);
                assert(fields[i] != NULL);
                set_field_buffer(fields[i], 0, requests[i]);

                if (i % 2 == 1) {
                        cury = cury+1;
                        curx = 1;
                        field_opts_on(fields[i], O_ACTIVE);
                        field_opts_on(fields[i], O_EDIT);
                        set_field_back(fields[i], A_UNDERLINE);
                } else {
                        curx = 20;
                        field_opts_off(fields[i], O_ACTIVE);
                        field_opts_off(fields[i], O_EDIT);
                }
        }

        fields[i] = NULL;
        form = new_form(fields);
        assert(form != NULL);
        win_form = derwin(win_body, rows-5, cols-2, 1, 1);
        box(win_form, 0, 0);
        assert(form != NULL && win_form != NULL);
        set_form_win(form, win_form);
        inner = derwin(win_form, form->rows+1, form->cols+1, 1, 1);
        assert(inner != NULL);
        set_form_sub(form, inner);
}

void new_popup(int rows, int cols, int posy, int posx, char **buttons,
		int nb_buttons, char **requests, int nb_fields)
{
	int i, cury = 0, curx = 1, tmp;
	WINDOW *inner;

	win_body = newwin(rows, cols, posy, posx);
	assert(win_body != NULL);
	box(win_body, 0, 0);

	create_menu1(rows, cols, buttons, nb_buttons);
	create_fields(rows, cols, requests, nb_fields);

/* create form */
/*
	fields = malloc(sizeof(FIELD *) * (nb_fields+1));
	assert(fields);

	for (i = 0; i < nb_fields; i++) {
		fields[i] = new_field(1, 8, cury, curx, 0, 0);
		assert(fields[i] != NULL);
		set_field_buffer(fields[i], 0, requests[i]);

		if (i % 2 == 1) {
			cury = cury+1;
			curx = 1;
			field_opts_on(fields[i], O_ACTIVE);
			field_opts_on(fields[i], O_EDIT);
			set_field_back(fields[i], A_UNDERLINE);
		} else {
			curx = 20;
			field_opts_off(fields[i], O_ACTIVE);
			field_opts_off(fields[i], O_EDIT);
		}
	}

	fields[i] = NULL;
	form = new_form(fields);
	assert(form != NULL);
	win_form = derwin(win_body, rows-5, cols-2, 1, 1);
	box(win_form, 0, 0);
	assert(form != NULL && win_form != NULL);
	set_form_win(form, win_form);
	inner = derwin(win_form, form->rows+1, form->cols+1, 1, 1);
	assert(inner != NULL);
	set_form_sub(form, inner);
*/
	assert(post_form(form) == E_OK);
	assert(post_menu(menu) == E_OK);
	is_on_button = true;
	pos_menu_cursor(menu);
}

void delete_popup(void)
{
	int i;

	unpost_form(form);
	unpost_menu(menu);

	for (i = 0; fields[i] != NULL; i++) {
		free_field(fields[i]);
	}

	for (i = 0; items[i] != NULL; i++) {
		free_item(items[i]);
	}

	free_menu(menu);
	free_form(form);
	delwin(win_form);
	delwin(win_menu);
	delwin(win_body);
}

/*
 * Actions for 'return' on a button
 */
int driver_buttons(ITEM *item)
{
	const char *name = item_name(item);
	int i;

	if (strcmp(name, "OK_1") == 0) {
		mvprintw(LINES-2, 1, "[*] OK clicked:\t");

		for (i = 0; i < form->maxfield; i++) {
			printw("%s", field_buffer(fields[i], 0));

			if (field_opts(fields[i]) & O_ACTIVE)
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
void switch_to_buttons(void)
{
	// Those 2 lines allow the field buffer to be set
	form_driver(form, REQ_PREV_FIELD);
	form_driver(form, REQ_NEXT_FIELD);

	menu_driver(menu, REQ_FIRST_ITEM);
	is_on_button = true;
	set_menu_fore(menu, A_REVERSE); // "show" the button
}

int driver(int ch)
{
	switch (ch) {
		case KEY_DOWN:
			if (is_on_button)
				break;

			if (form->current == fields[form->maxfield-1])
				switch_to_buttons();
			else
				form_driver(form, REQ_NEXT_FIELD);
			break;

		case KEY_UP:
			if (is_on_button) {
				is_on_button = false;
				set_menu_fore(menu, A_NORMAL); // "hide" the button
			} else
				form_driver(form, REQ_PREV_FIELD);
			break;

		case KEY_LEFT:
			if (is_on_button)
				menu_driver(menu, REQ_LEFT_ITEM);
			else
				form_driver(form, REQ_LEFT_FIELD);
			break;

		case KEY_RIGHT:
			if (is_on_button)
				menu_driver(menu, REQ_RIGHT_ITEM);
			else
				form_driver(form, REQ_RIGHT_FIELD);
			break;

		case 10:
			if (!is_on_button)
				switch_to_buttons();
			else
				if(0 == driver_buttons(current_item(menu))){
					return 0;
				}

			break;

		default:
			if (!is_on_button)
				form_driver(form, ch);

			break;

	}

	if (is_on_button)
		pos_menu_cursor(menu);
	else
		pos_form_cursor(form);

	wrefresh(win_body);
	return 1;
}

int main()
{
	char *buttons[] = { "OK_1", "QUIT" };
	char *requests[] = { "Password:", "pass", "Id:", "id" };
	int ch;

	initscr();
	noecho();
	cbreak();
	keypad(stdscr, TRUE);

	new_popup(24, 80, (LINES-25)/2, (COLS-81)/2, buttons, 2, requests, 4);
	refresh();
	wrefresh(win_body);
	wrefresh(win_form);
	wrefresh(win_menu);

	while ((ch = getch()) != KEY_F(1))
		if(0 == driver(ch)) break;

	delete_popup();
	endwin();

	return 0;
}

