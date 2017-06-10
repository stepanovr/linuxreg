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
#ifndef __DEBUG_H__
#define __DEBUG_H__

#include <stdio.h>

#define DEBUG_FILE "log_debug"

#define DEBUG_INIT()                                                      \
                        {                                               \
                                FILE* f;                                \
                                f = fopen(DEBUG_FILE, "w");             \
                                if(f){                                  \
                                        fclose(f);                              \
                                }                                       \
                        }


#define DEBUG_I(i)							\
			{						\
				FILE* f;				\
				f = fopen(DEBUG_FILE, "a");		\
				if(f){					\
					fprintf(f, "%s %d\n", __func__, i);	\
					fclose(f);				\
				}					\
			}

#define DEBUG_S(s)                                                      \
                        {                                               \
                                FILE* f;                                \
                                f = fopen(DEBUG_FILE, "a");             \
                                if(f){                                  \
        	                        fprintf(f, "%s %s\n", __func__, s);     \
	                                fclose(f);                              \
                                }                                       \
                        }



#define DEBUG_IS(i, s)                                                  \
                        {                                               \
                                FILE* f;                                \
                                f = fopen(DEBUG_FILE, "a");             \
                                if(f){                                  \
        	                        fprintf(f, "%s %d %s\n", __func__, i, s);     \
	                                fclose(f);                              \
                                }                                       \
                        }

#define DEBUG_X(i)                                                      \
                        {                                               \
                                FILE* f;                                \
                                f = fopen(DEBUG_FILE, "a");             \
                                if(f){                                  \
                                        fprintf(f, "%s %X\n", __func__, i);     \
                                        fclose(f);                              \
                                }                                       \
                        }


#endif
