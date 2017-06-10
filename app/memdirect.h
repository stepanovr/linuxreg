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
#ifndef __MEMDIRECT_H__
#define __MEMDIRECT_H__

#include <sys/queue.h>
#include <stdint.h>
#include <menu.h>
#include <panel.h>


#define MEM_MAP	"/proc/iomem"

#define STR_LEN	100
#define FIELD_NAME_LEN 150

/* Length of two addreses of Start and Stop addresses. For 64 bit 16 characters per address */
#define START_END_LEN	(16*2 + 10)

#define CHOICES_LIMIT 150

typedef long    address_type_t;
typedef long    value_type_t;


WINDOW *mem_win;
PANEL  *mem_panel;

typedef enum{
        INIT_MMU_STATE = 0,
	REG_SEL_MMU_STATE,
        OPERATION_SEL_MMU_STATE,
        READ_PARMETERS_MMU_STATE,
        WRITE_PARMETERS_MMU_STATE,
        SAVE_READ_PARMETERS_MMU_STATE,
        DISPLAY_READ_MMU_STATE,
        STATUS_WRITE_MMU_STATE,
        STATUS_WRITE_FILE_MMU_STATE,
}en_mmu_state_t;


typedef enum{
        NO_MEM_ACCESS_ERROR,
        MEM_ACCESS_RANGE_ERROR,
        MEM_ACCESS_NOT_ROOT,
        MEM_ACCESS_FILE_ERROR,
        MEM_ACCESS_MAP_ERROR
}mem_access_errors_t;


typedef enum{
        NO_MMU_EVENT    = 0,
        CR_MMU_EVENT    = '\n',
        R_MMU_EVENT     = 'R',
        W_MMU_EVENT     = 'W',
        S_MMU_EVENT     = 'S',
        ESC_MMU_EVENT   = 27,
}en_mmu_events_t;



typedef enum{
	ERROR_OK,
	ERROR_MEM,
	ERROR_OPEN
}error_ty;

typedef enum{
        ADDRESS_VALID,
        ADDRESS_TOO_LOW,
        ADDRESS_TOO_HIGH,
        ADDRESS_ALIGN_ERROR
}validate_addr_t;

#define 	ORIGINAL 0
//#define 	FIELD_NAME_LEN 50


typedef struct _mem_record_ty{
        LIST_ENTRY(_mem_record_ty) list;
        uint32_t start;
        uint32_t end;
        char    name[FIELD_NAME_LEN];
	char	*menu_str;
	int	index;
}mem_record_ty;


typedef LIST_HEAD(mem_record_head, _mem_record_ty) mem_record_head;

/* Main data structure */
typedef struct _major_data_struct_t{
        mem_record_head mem_rec_head;
	int total;
	en_mmu_state_t	mmu_state;
	en_mmu_events_t mmu_event;
        address_type_t start;
	address_type_t page_start;
	address_type_t end;
	char    name[FIELD_NAME_LEN];
        value_type_t first_value;
}major_data_struct_t;

typedef struct _menu_item_t{
        char *choices;
        mem_record_ty *mem_rec;
}menu_item_t;



void init_mem_rec(major_data_struct_t* mds);
void insert2mem_record_queue(mem_record_head *head, mem_record_ty *item);
mem_record_ty* queue_find_by_name(major_data_struct_t* mds, char* str);
int get_fields(void);
mem_access_errors_t read_mem(address_type_t address, address_type_t* first_val, uint32_t range, char *str);
mem_access_errors_t read_one(address_type_t address, address_type_t* val);
mem_access_errors_t write_one(address_type_t address, address_type_t* val);
uint32_t a2hex(char *str);


#endif
