/*
 * p1_globals.c
 *
 *  Created on: Jan 16, 2014
 *      Author: Peter
 */
#include "p1_globals.h"

int g_Container_count = 0;		/* number of Ordered_containers currently allocated */
int g_Container_items_in_use = 0;	/* number of Ordered_container items currently in use */
int g_Container_items_allocated = 0;	/* number of Ordered_container items currently allocated */
int g_string_memory = 0;			/* number of bytes used in C-strings */
