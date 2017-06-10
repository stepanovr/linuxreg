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
#include <sys/io.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdint.h>
#include "memdirect.h"

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>

#define MAP_PAGESIZE 4096UL
static sigjmp_buf sj_env;

void sig_handler(int);



void sig_handler(int sig) {
    switch (sig) {
    case SIGBUS:
    case SIGSEGV:
	siglongjmp (sj_env, 1);
//        abort();
	break;
    default:
//        abort();
	break;
    }
}


//typedef long	address_type_t;

/*
typedef enum{
	NO_MEM_ACCESS_ERROR,
	MEM_ACCESS_RANGE_ERROR,
	MEM_ACCESS_NOT_ROOT,
	MEM_ACCESS_FILE_ERROR,
	MEM_ACCESS_MAP_ERROR
}mem_access_errors_t;
*/


mem_access_errors_t read_mem(address_type_t address, address_type_t* first_val, uint32_t range, char *str)
{
	long page;
	long offset;
	int i;
	unsigned long *pLong, *pFirst;
	int fd;
	void *pMem;

	signal(SIGBUS, sig_handler);
	signal(SIGSEGV, sig_handler);


	if(range == 0){
		return MEM_ACCESS_RANGE_ERROR;
	}

	if (geteuid() != 0){
		return MEM_ACCESS_NOT_ROOT;
	}

	page = (address / MAP_PAGESIZE) * MAP_PAGESIZE;
	offset = address % MAP_PAGESIZE;

	/*
	 * Open memory device for read/write
	 */

	if((fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0)
	{
		return MEM_ACCESS_FILE_ERROR;
	}
	/*
	 * get pointer to memory device
	 */
	pMem = mmap(NULL,MAP_PAGESIZE,(PROT_READ | PROT_WRITE),MAP_SHARED,
			fd,page);

	if((pMem == MAP_FAILED) || (pMem == NULL))
	{
		return MEM_ACCESS_MAP_ERROR;
	}

	pLong = pMem + offset;
	pFirst = pLong;
	{
		bool success = true;

		for(i = 0; i < 8; i++)
		{
			value_type_t val = *pLong;
			if (sigsetjmp(sj_env, 1)) {
				success = false;
			} else {
				success = true;
			}

			if(success){
				if(pFirst == pLong){
					*first_val = *pLong;
				}
				sprintf(str + 9*i, "%08lX ",val);
			} else {
				sprintf(str + 9*i, "-------- ",val);
			}
			if(--range == 0)
				break;
			pLong++;
		}
	}

	close(fd);
	return NO_MEM_ACCESS_ERROR;
}



mem_access_errors_t read_one(address_type_t address, address_type_t* val)
{
        long page;
        long offset;
        int i;
        unsigned long *pLong;
        int fd;
        void *pMem;



        if (geteuid() != 0){
                return MEM_ACCESS_NOT_ROOT;
        }

        page = (address / MAP_PAGESIZE) * MAP_PAGESIZE;
        offset = address % MAP_PAGESIZE;

        /*
         * Open memory device for read/write
         */

        if((fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0)
        {
                return MEM_ACCESS_FILE_ERROR;
        }
        /*
         * get pointer to memory device
         */
        pMem = mmap(NULL,MAP_PAGESIZE,(PROT_READ | PROT_WRITE),MAP_SHARED, fd,page);

        if((pMem == MAP_FAILED) || (pMem == NULL))
        {
                return MEM_ACCESS_MAP_ERROR;
        }

        pLong = pMem + offset;

        *val = *pLong;
        close(fd);
        return NO_MEM_ACCESS_ERROR;
}


mem_access_errors_t write_one(address_type_t address, address_type_t* val)
{
        long page;
        long offset;
        int i;
        unsigned long *pLong;
        int fd;
        void *pMem;



        if (geteuid() != 0){
                return MEM_ACCESS_NOT_ROOT;
        }

        page = (address / MAP_PAGESIZE) * MAP_PAGESIZE;
        offset = address % MAP_PAGESIZE;

        /*
         * Open memory device for read/write
         */

        if((fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0)
        {
                return MEM_ACCESS_FILE_ERROR;
        }
        /*
         * get pointer to memory device
         */
        pMem = mmap(NULL,MAP_PAGESIZE,(PROT_READ | PROT_WRITE),MAP_SHARED, fd,page);

        if((pMem == MAP_FAILED) || (pMem == NULL))
        {
                return MEM_ACCESS_MAP_ERROR;
        }

        pLong = pMem + offset;

        *pLong = *val;


        close(fd);
        return NO_MEM_ACCESS_ERROR;
}
