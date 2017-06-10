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
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#include <asm/errno.h>

#include <panel.h>
#include <form.h>
#include <menu.h>

#include "mmi.h"
#include "win_interface.h"
#include "debug.h"

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD 	4

void region_select_window_on(void);
void region_select_window_off(void);
void region_select_input(void);

void operation_select_window_on(void);
void operation_select_window_off(void);
void operation_select_input(void);

void read_params_window_on(void);
void read_params_window_off(void);
void read_params_input(void);

void mmi_state_machine(mmi_t* mmi);
void set_new_state(mmi_t* mmi, mmi_state_t state);

mmi_t mmi_instance = {
	.interface_array = {
	        [INIT_MMI_STATE] = {
        	        .display_interface = NULL,
	                .undisplay_interface = NULL,
                	.input_interface = NULL,
        	},
	}
};

pthread_t mmi_thread_id;
pthread_t input_thread_id;



void* mmi_thread(void *arg)
{
	int stop = 0;
	mmi_t *mmi = &mmi_instance;
	panel_context_t* ctx;
DEBUG_I(mmi->state)
DEBUG_X(mmi)
DEBUG_S("INIT_MMI_STATE")
	leave_state();
	set_new_state(mmi, S0);

	do{
		usleep(10000);
//		mmi_state_machine(mmi);
		ctx = get_context();
		if(ctx && ctx->win_dev && ctx->win_dev->state){
			ctx->win_dev->state(ctx, mmi);
		}
	}
	while(!stop);
}

void* input_thread(void *arg)
{
        int stop = 0, ch;
	panel_context_t* ctx;

        mmi_t *mmi = &mmi_instance;
	mmi->event = NO_MMI_EVENT;
        do{
//DEBUG_I(1)
                usleep(10000);
		ctx = get_context();
		ch = getch();
//DEBUG_X(ctx->win_dev->input)
		if(ctx->win_dev->input){
//			mmi->event = ctx->win_dev->input(ctx, ch);

			switch(mmi->event){
				case MMI_CHANGE_STATE:
					break;
				default:
					break;
			}
			stop = (mmi->event == MMI_STOP);
		}
        }
        while(!stop);
}



int init_mmi_thread()
{
	int err;
DEBUG_I(1);
        err = pthread_create(&mmi_thread_id, NULL, &mmi_thread, NULL);
        if (err != 0){
            return -ESRCH;
            printf("\ncan't create thread :[%s]", strerror(err));
        }

//        err = pthread_create(&input_thread_id, NULL, &input_thread, NULL);
        if (err != 0){
	    return -ESRCH;
            printf("\ncan't create thread :[%s]", strerror(err));
	}
        else{
	    return 0;
            printf("\n Thread created successfully\n");
	}
}



void mmi_state_machine(mmi_t* mmi)
{
	static int counter = 0;
//	printf("event is: %c %X\n", mds->mmi_event, mds->mmi_event);
/*
	if(mmi->event != INIT_MMI_STATE){
		printf("event is: %c %X\n", mmi->event, mmi->event);
	}
*/
        switch(mmi->state){
                case INIT_MMI_STATE:
DEBUG_I(mmi->state)
DEBUG_X(mmi)
DEBUG_S("INIT_MMI_STATE")
			leave_state();
                        set_new_state(mmi, S0);
                        break;
	        case S0:
			/* Init here */
//			if(--counter == 0){
//	                        counter = 10;
//				set_new_state(mmi, S1);
//			}
			if(mmi->event == MMI_CHANGE_STATE){
				leave_state();
				set_new_state(mmi, S1);
			}
if(mmi->event != NO_MMI_EVENT){
	DEBUG_I(mmi->event)
}
			mmi->event = NO_MMI_EVENT;
//DEBUG_I(mmi->state)
                        break;
                case S1:
//                        if(--counter == 0){
//                                counter = 10;
//	                        set_new_state(mmi, S2);
//			}
                        if(mmi->event == MMI_CHANGE_STATE){
                                leave_state();
                                set_new_state(mmi, S2);
                        }
if(mmi->event != NO_MMI_EVENT){
        DEBUG_I(mmi->event)
}

                        mmi->event = NO_MMI_EVENT;
//DEBUG_I(mmi->state)
                        break;
                case S2:
                        if(mmi->event == MMI_CHANGE_STATE){
                                leave_state();
                                set_new_state(mmi, S3);
                        }
if(mmi->event != NO_MMI_EVENT){
        DEBUG_I(mmi->event)
}
                        mmi->event = NO_MMI_EVENT;
//DEBUG_I(mmi->state)
                        break;
	        case S3:
                        if(mmi->event == MMI_CHANGE_STATE){
                                leave_state();
                                set_new_state(mmi, S0);
                        }
if(mmi->event != NO_MMI_EVENT){
        DEBUG_I(mmi->event)
}
                        mmi->event = NO_MMI_EVENT;
//DEBUG_I(mmi->state)
                        break;
		default:
if(mmi->event != NO_MMI_EVENT){
        DEBUG_I(mmi->event)
}
                        mmi->event = NO_MMI_EVENT;
//DEBUG_I(mmi->state)
			break;
	}

}



void set_new_state(mmi_t* mmi, mmi_state_t state)
{
//	leave_state();
DEBUG_I(state)
	entry_state(state);
}

//void options();

int main()
{
	int ch = 0;
	mmi_t *mmi;
        panel_context_t* ctx;
DEBUG_INIT();
        init_display();

        mmi = &mmi_instance;

//options();
//	set_new_state(mmi, S0);
	set_new_state(mmi, INIT_MMI_STATE);
        init_mmi_thread();


        ctx = entry_state(INIT_MMI_STATE);
        if(!ctx){
                return -1;
        }


	while((mmi->event != MMI_STOP))
	{

	        ch = getch();
		ctx =  get_context();
//DEBUG_X(ctx->win_dev->input)
        	if(ctx->win_dev->input){
		        mmi->event = ctx->win_dev->input(ctx, ch);

        		switch(mmi->event){
                		case MMI_CHANGE_STATE:
                        		break;
	                	default:
        		                break;
	                }
	        }
	}
	disable_display(ctx);
}





