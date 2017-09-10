/*
 * Ordered_container_array.c
 *
 *  Created on: Jan 15, 2014
 *      Author: Peter
 */
#include "Ordered_container.h"
#include "p1_globals.h"
#include "Utility.h"
#include <stdlib.h>
#include <assert.h>

#define INIT_SIZE 3

extern int g_Container_count;
extern int g_Container_items_in_use;
extern int g_Container_items_allocated;
/* A complete type declaration for Ordered_container implemented as an array */
struct Ordered_container {
	OC_comp_fp_t comp_fun;	/* pointer to comparison function  */
	void** array;			/* pointer to array of pointers to void */
	int allocation;			/* current size of array */
	int size;				/* number of items  currently in the array */
};

static int binsearch(const void *data_ptr, const struct Ordered_container* c_ptr, int *place, OC_comp_fp_t comp);


static void expand_array(struct Ordered_container *c_ptr);


struct Ordered_container* OC_create_container(OC_comp_fp_t f_ptr)
{
	struct Ordered_container* oc;
	assert(f_ptr);
	oc = NULL;

	oc = (struct Ordered_container*) malloc(sizeof(struct Ordered_container));
	if(!oc)
		mem_err_exit();
	g_Container_count++;

	oc->array = (void**) malloc(INIT_SIZE * sizeof(void*));
	if(!oc->array)
		mem_err_exit();
	g_Container_items_allocated += INIT_SIZE;

	oc->comp_fun = f_ptr;
	oc->allocation = INIT_SIZE;
	oc->size = 0;

	return oc;
}

void OC_destroy_container(struct Ordered_container* c_ptr)
{
	assert(c_ptr);

	free(c_ptr->array);
	g_Container_items_allocated -= c_ptr->allocation;
	g_Container_items_in_use -= c_ptr->size;

	free(c_ptr);
	g_Container_count--;
	return;
}

void OC_clear(struct Ordered_container* c_ptr)
{
	assert(c_ptr);

	free(c_ptr->array);
	g_Container_items_allocated -= c_ptr->allocation;
	g_Container_items_in_use -= c_ptr->size;

	c_ptr->array = (void**) malloc( INIT_SIZE * sizeof(void*) );
	if(!c_ptr->array)
		mem_err_exit();
	g_Container_items_allocated += INIT_SIZE;

	c_ptr->allocation = INIT_SIZE;
	c_ptr->size = 0;

	return;
}

int OC_get_size(const struct Ordered_container* c_ptr)
{
	assert(c_ptr);
	return c_ptr->size;
}

int OC_empty(const struct Ordered_container* c_ptr)
{
	assert(c_ptr);
	return c_ptr->size == 0;
}

void* OC_get_data_ptr(const void* item_ptr)
{
	void** vpp;
	assert(item_ptr);
	vpp = (void**) item_ptr;
	return *vpp;
}
/* Delete the specified item.
Caller is responsible for any deletion of the data pointed to by the item. */
void OC_delete_item(struct Ordered_container* c_ptr, void* item_ptr)
{
	void** next;
	void** prev;

	prev = next = NULL;

	assert(c_ptr); assert(item_ptr);

	next = (void**) item_ptr;
	prev = next++;

	while(next - c_ptr->array < c_ptr->size)
	{
		*prev++ = *next++;
	}
	c_ptr->size--;
	g_Container_items_in_use--;
	return;
}

void OC_insert(struct Ordered_container* c_ptr, void* data_ptr)
{
	int place, i;
	void *prev, *next;

	prev = next = NULL;
	place = -1;

	assert(c_ptr);

	if(c_ptr->size == c_ptr->allocation)
		expand_array(c_ptr);

	if(OC_empty(c_ptr))
		place = 0;
	else
		binsearch(data_ptr, c_ptr, &place, c_ptr->comp_fun);


	prev = data_ptr;
	for(i = place; i <= c_ptr->size; i++)			/*starting at place and going until one after the last object in the array*/
	{
		next = c_ptr->array[i];
		c_ptr->array[i] = prev;
		prev = next;
	}

	c_ptr->size++;
	g_Container_items_in_use++;

	return;
}

void* OC_find_item(const struct Ordered_container* c_ptr, const void* data_ptr)
{
	int place = -1;

	assert(c_ptr);

	if(binsearch(data_ptr, c_ptr, &place, c_ptr->comp_fun))
	{
		return (void*) &(c_ptr->array[place]);
	}
	return NULL;
}

void* OC_find_item_arg(const struct Ordered_container* c_ptr, const void* arg_ptr, OC_find_item_arg_fp_t fafp)
{
	int place = -1;

	assert(c_ptr); assert(arg_ptr); assert(fafp);

	if(binsearch(arg_ptr, c_ptr, &place, fafp))
	{
		return (void*) &(c_ptr->array[place]);
	}
	return NULL;
}

void OC_apply(const struct Ordered_container* c_ptr, OC_apply_fp_t afp)
{
	int i;

	assert(c_ptr); assert(afp);

	for(i = 0; i < c_ptr->size; i++)
	{
		(*afp)(c_ptr->array[i]);
	}
	return;
}

int OC_apply_if(const struct Ordered_container* c_ptr, OC_apply_if_fp_t afp)
{
	int i, result;
	i = result = 0;

	assert(c_ptr); assert(afp);

	while(!result && i < c_ptr->size)
		result = (*afp)(c_ptr->array[i++]);
	return result;
}

void OC_apply_arg(const struct Ordered_container* c_ptr, OC_apply_arg_fp_t afp, void* arg_ptr)
{
	int i;
	assert(c_ptr); assert(arg_ptr); assert(afp);

	for(i = 0; i < c_ptr->size; i++)
	{
		(*afp)(c_ptr->array[i], arg_ptr);
	}
	return;
}

int OC_apply_if_arg(const struct Ordered_container* c_ptr, OC_apply_if_arg_fp_t afp, void* arg_ptr)
{
	int i, result;
	i = result = 0;

	assert(c_ptr); assert(arg_ptr); assert(afp);

	while(!result && i < c_ptr->size)
		result = (*afp)(c_ptr->array[i++], arg_ptr);
	return result;
}

static int binsearch(const void *data_ptr, const struct Ordered_container* c_ptr, int *place, OC_comp_fp_t comp)
/*return non-zero if successful and 0 otherwise. Also stores the index where the object would go into the address place points to*/
{
	int cond;
	int low, high, mid;
	low = 0;
	high = c_ptr->size - 1;
	while (low <= high)
	{
		mid = low + (high-low) / 2;
		if ((cond = (*comp)(data_ptr, c_ptr->array[mid])) < 0)
			high = mid - 1;
		else if (cond > 0)
			low = mid + 1;
		else
		{
			*place = mid;
			return 1;
		}
	}
	*place = high + 1;
	return 0;
}

static void expand_array(struct Ordered_container *c_ptr)
/*expands the array of the container, copies the data over and updates global vars accordingly*/
{
	int new_size, i;
	void **new_array;

	new_size = 2*(c_ptr->size + 1);

	new_array = (void**) malloc( new_size * sizeof(void*) );
	if(!new_array)
		mem_err_exit();
	g_Container_items_allocated += new_size - c_ptr->allocation;
	c_ptr->allocation = new_size;


	for(i = 0; i < c_ptr->size; i++)
	{
		new_array[i] = c_ptr->array[i];
	}
	free(c_ptr->array);
	c_ptr->array = new_array;

	return;
}
