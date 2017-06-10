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
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/queue.h>
#include <stdlib.h>
#include "memdirect.h"
#include "../debug.h"

major_data_struct_t major_data_struct;


char buff_str[STR_LEN];

char *teststr = "aBc";

uint32_t a2hex(char *str){
	uint32_t res = 0;
	char ch;
	while(*str){
		ch = *str++;
		if((0 != res) && (' ' == ch)){
			break;
		}
                res <<= 4;
		if(('0' <= ch) && ('9' >= ch)){
			res |= ch - '0';
		} else if (('a' <= ch) && ('f' >= ch)){
			res |= 10 + ch -'a';
		} else if (('A' <= ch) && ('F' >= ch)){
			res |= 10 + ch -'A';
		}
	}
	return res;
}

error_ty parse_mem_field_line(mem_record_ty* mem_rec, char* buff_str)
{
	char* str;

	if((!mem_rec) || (!buff_str)){
		return ERROR_MEM;
	}

	str = strtok(buff_str, "-");
        mem_rec->start = a2hex(str);
DEBUG_X(mem_rec->start)
	str = strtok(NULL, " ");
	mem_rec->end = a2hex(str);
DEBUG_X(mem_rec->end)
	str = strtok(NULL, " ");
	str = strtok(NULL, "\0");
	strncpy(mem_rec->name, str, FIELD_NAME_LEN);
}

int get_fields(void)
{
	FILE* fp;
	char* str;
	int res;
	int index = 0;
	mem_record_ty *mem_rec;
        major_data_struct_t *mds = &major_data_struct;

	init_mem_rec(mds);

	res = a2hex(teststr);

	fp = fopen(MEM_MAP, "r");

	if(NULL == fp){
		perror(MEM_MAP);
		return ERROR_OPEN;
	}
	while(fgets(buff_str, STR_LEN, fp)){
		if(' ' == buff_str[0]){
			continue;
		}
		mem_rec = (mem_record_ty*)malloc(sizeof(mem_record_ty));
		buff_str[strcspn(buff_str, "\n")] = '\0';

		parse_mem_field_line(mem_rec, buff_str);
		insert2mem_record_queue(&mds->mem_rec_head, mem_rec);
		mem_rec->index = index;

		mem_rec->menu_str = malloc(strlen(mem_rec->name) + START_END_LEN);
#ifdef FULL_MEMORY_LINE
		snprintf(mem_rec->menu_str, (strlen(mem_rec->name) + START_END_LEN), "%08X -- %08X :: %s",
                        mem_rec->start, mem_rec->end,  mem_rec->name);
#else
                snprintf(mem_rec->menu_str, (strlen(mem_rec->name) + START_END_LEN), "%s", mem_rec->name);

#endif

//                printf("%08X  %08X   %s\n\n", mem_rec->start, mem_rec->end, mem_rec->name);
		index++;
	}
	fclose(fp);

	mds->total = index;

//        LIST_FOREACH(mem_rec, &mds->mem_rec_head, list) {
//		printf("%d %08X  %08X   %s\n", mem_rec->index, mem_rec->start, mem_rec->end, mem_rec->name);
//        }


//mem_rec = queue_find_by_name(mds, "bcm2708_dma.0");
//if(mem_rec){
//	printf("\n\n%d %08X  %08X   %s  total:%d\n", mem_rec->index, mem_rec->start, mem_rec->end, mem_rec->name, mds->total);
//}

}

menu_item_t choices[CHOICES_LIMIT];
ITEM **my_items;
int n_choices;

void options()
{
        int i;
        mem_record_ty *mem_rec;
        major_data_struct_t *mds = &major_data_struct;

        get_fields();
        n_choices = mds->total;

        /* Initialize items */
        i = 0;

        LIST_FOREACH(mem_rec, &mds->mem_rec_head, list) {
                choices[i].choices = mem_rec->menu_str;
                choices[i].mem_rec = mem_rec;
                i++;
        }


        my_items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));
        for(i = 0; i < n_choices; ++i){
                my_items[i] = new_item(choices[i].choices, "");
        }
        my_items[n_choices] = (ITEM *)NULL;
}
