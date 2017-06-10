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
#include <sys/types.h>
#include <sys/queue.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "memdirect.h"

extern major_data_struct_t major_data_struct;


void init_mem_rec(major_data_struct_t* mds)
{
	LIST_INIT(&mds->mem_rec_head);
}

void insert2mem_record_queue(mem_record_head *head, mem_record_ty *item)
{
	LIST_INSERT_HEAD(head, item, list);
}

mem_record_ty* queue_find_by_name(major_data_struct_t* mds, char* str)
{
	mem_record_ty *mem_rec;
	LIST_FOREACH(mem_rec, &mds->mem_rec_head, list) {
		if(0 == strcmp(mem_rec->name, str)){
			return	mem_rec;
		}
	}
	return (mem_record_ty*)NULL;
}

/*
int main(int argc, char *argv[])
{
        ssize_t i;
        mem_record_ty *np, *prevp;
	major_data_struct_t *mds = &major_data_struct;

	init_mem_rec(mds);

        for (i = 2; i <= 11; ++i) {
                np = malloc(sizeof(mem_record_ty));
                np->val = i;
//                LIST_INSERT_HEAD(&mds->mem_rec_head, np, list);
		insert2mem_record_queue(&mds->mem_rec_head, np);
        }


        LIST_FOREACH(np, &mds->mem_rec_head, list) {
                fprintf(stdout, "%d ", np->val);
        }
        fprintf(stdout, "\n");


        return EXIT_SUCCESS;
}
*/
