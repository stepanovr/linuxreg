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
#ifndef __WININTERFACE_H__
#define __WININTERFACE_H__

/**************************************/
#define PANELS_NUM 20
/**************************************/

#define KEY_ESC	27

#define BUTTONS_NUM     30
#define FIELDS_NUM      30

typedef enum{
        BLACK_COLOR,
        BLUE_COLOR,
        RED_COLOR,
        MAGENTA_COLOR,
        GREEN_COLOR,
        CYAN_COLOR,
        YELLOW_COLOR,
        WHITE_COLOR
}color_id_t;

#define INIT_COLOR(C) init_pair(C##_COLOR, COLOR_##C, COLOR_BLACK);

typedef enum _event_t
{
        NO_EVENT = 0,
        GOT_DATA_EVENT,
        EXIT_WINDOW_EVENT,
        STOP_APP_EVENT,
        NUM_EVENTS
}event_t;


typedef enum _window_id_t
{
        MAIN_WINDOW,
        MENU_WINDOW,
        MENU_INNER_WINDOW,
        FIELDS_WINDOW,
        FIELDS_INNER_WINDOW,
        WINDOWS_NUM
}window_id_t;

typedef enum _menu_id_t
{
        MENU_0,
        MENUS_NUM
}menu_id_t;

typedef enum _input_state_t
{
        ON_BUTTONS_STATE, /* menu */
        ON_FIELDS_STATE,  /* forms */
        INPUT_STATES_NUM
}input_state_t;


typedef struct _field_name_t{
        char*   name;
        char*   field_text;
}field_name_t;


typedef struct _panel_init_t{
        char*   win_title;
        color_id_t title_color;
        int     main_width;
        int     main_heigth;
        int     main_h_offset;
        int     main_v_offset;
        char*   button_text[BUTTONS_NUM];
        field_name_t    field[FIELDS_NUM];
        int     menu_width;
        int     menu_heigth;
        int     index;
}panel_init_t;


struct _win_dev_t;

typedef struct _panel_context_t{
        WINDOW *wins[WINDOWS_NUM];
        MENU *menus[MENUS_NUM];
        FIELD **fields;
        FORM *form;
        ITEM **items;
        PANEL  *panel;
        input_state_t input_state;
        event_t event;
        void*   data;
        int x;
        int y;
        int w;
        int h;
        char *title;
        chtype color;
	int	index;
	panel_init_t*	init;
	struct _win_dev_t* win_dev;
}panel_context_t;


typedef struct _win_dev_t{
	int (*init)(panel_context_t*);
        int (*input)(panel_context_t*, int);
        int (*start)(panel_context_t*);
        int (*stop)(panel_context_t*);
	int (*state)(panel_context_t*, mmi_t*);
}win_dev_t;


#define WIN_TITLE       ctx->init->win_title
#define TITLE_COLOR     ctx->init->title_color
#define MAIN_WIDTH      ctx->init->main_width
#define MAIN_HEIGHT     ctx->init->main_heigth
#define MAIN_H_OFFSET   ctx->init->main_h_offset
#define MAIN_V_OFFSET   ctx->init->main_v_offset
#define BUTTON_0        ctx->init->button_text[0]
#define BUTTON_1        ctx->init->button_text[1]
#define TITLE_FIELD_0   ctx->init->field[0].name
#define FILL_FIELD_0    ctx->init->field[0].field_text
#define TITLE_FIELD_1   ctx->init->field[1].name
#define FILL_FIELD_1    ctx->init->field[1].field_text
#define MENU_ROWS       ctx->init->menu_width
#define MENU_COLS       ctx->init->menu_heigth



void init_wins(WINDOW **wins, int n);
void win_show(WINDOW *win, char *label, int label_color);
void init_display();
void disable_display();
panel_context_t* create_panel(panel_context_t* ctx, char* title, int x, int y, int w, int h);
void make_menu(panel_context_t* ctx);
void make_fields(panel_context_t* ctx, int nb_fields);
panel_context_t* entry_state(mmi_state_t state);
void leave_state();
panel_context_t* get_context();



void init_window0();
void init_window1();

#endif

