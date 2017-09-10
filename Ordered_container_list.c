/*
 * Ordered_container_list.c
 *
 *  Created on: Jan 15, 2014
 *      Author: Peter
 */
#include "Ordered_container.h"
#include "p1_globals.h"
#include "Utility.h"
#include <stdlib.h>
#include <assert.h>


extern int g_Container_count;
extern int g_Container_items_in_use;
extern int g_Container_items_allocated;

/* struct LL_Node structure declaration. This declaration is local to this file.
This is a two-way or doubly-linked list. Each node has a pointer to the previous
node and another pointer to the next node in the list. This means insertions or
removals of a node can be made in constant time, once the location has been
determined. */
struct LL_Node {
    struct LL_Node* prev;      /* pointer to the previous node */
	struct LL_Node* next;		/* pointer to the next node */
	void* data_ptr; 			/* uncommitted pointer to the data item */
};

/* Declaration for Ordered_container. This declaration is local to this file.
A pointer is maintained to the last node in the list as well as the first,
meaning that additions to the end of the list can be made in constant time.
The number of nodes in the list is kept up-to-date in the size member
variable, so that the size of the list can be accessed in constant time. */
struct Ordered_container {
	OC_comp_fp_t comp_func;
	struct LL_Node* first;
	struct LL_Node* last;
	int size;
};

struct Ordered_container* OC_create_container(OC_comp_fp_t f_ptr)
{
	struct Ordered_container* oc;
	assert(f_ptr);
	oc = (struct Ordered_container*) malloc(sizeof(struct Ordered_container));
	if(!oc)
		mem_err_exit();
	g_Container_count++;

	oc->comp_func = f_ptr;
	oc->first = oc->last = NULL;
	oc->size = 0;
	return oc;
}

void OC_destroy_container(struct Ordered_container* c_ptr)
{
	assert(c_ptr);
	OC_clear(c_ptr);
	free(c_ptr);
	g_Container_count--;

	return;
}

void OC_clear(struct Ordered_container* c_ptr)
{
	struct LL_Node* cur_ptr;
	struct LL_Node* next_ptr;
	assert(c_ptr); 						/*c_ptr is not NULL*/

	cur_ptr = c_ptr->last;

	while(cur_ptr)						/*while there are still nodes to delete*/
	{
		next_ptr = cur_ptr->prev;

		free(cur_ptr);

		cur_ptr = next_ptr;
	}
	c_ptr->first = c_ptr->last = NULL;
	g_Container_items_in_use -= c_ptr->size;
	g_Container_items_allocated -= c_ptr->size;
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
	struct LL_Node* nptr;

	assert(item_ptr);
	nptr = (struct LL_Node*) item_ptr;
	return nptr->data_ptr;
}

void OC_delete_item(struct Ordered_container* c_ptr, void* item_ptr)
{
	struct LL_Node* nptr;
	struct LL_Node* prev;
	struct LL_Node* next;
	assert(c_ptr); assert(item_ptr);


	nptr = (struct LL_Node*) item_ptr;
	prev = nptr->prev;
	next = nptr->next;
	if(prev)
		prev->next = next;
	else c_ptr->first = next;
	if(next)
		next->prev = prev;
	else c_ptr->last = prev;

	free(item_ptr);
	g_Container_items_in_use--;
	g_Container_items_allocated--;
	c_ptr->size--;
	return;
}

void OC_insert(struct Ordered_container* c_ptr, void* data_ptr)
{
	struct LL_Node* new_node;
	struct LL_Node* cur;
	struct LL_Node* prev;
	int result;
	assert(c_ptr); assert(data_ptr);


	new_node = (struct LL_Node*) malloc(sizeof(struct LL_Node));
	if(!new_node)
		mem_err_exit();
	new_node->data_ptr = data_ptr;

	g_Container_items_in_use++;
	g_Container_items_allocated++;

	if(OC_empty(c_ptr))
	{
		new_node->next = new_node->prev = NULL;
		c_ptr->first = c_ptr->last = new_node;
		c_ptr->size++;
		return;
	}

	cur = c_ptr->first;
	prev = NULL;
	while(cur)
	{
		result = (*(c_ptr->comp_func))(data_ptr, cur->data_ptr);
		if(result <= 0) break;

		prev = cur;
		cur = cur->next;
	}

	new_node->next = cur;
	new_node->prev = prev;

	if(prev)
		prev->next = new_node;
	else
		c_ptr->first = new_node;

	if(cur)
		cur->prev = new_node;
	else
		c_ptr->last = new_node;

	c_ptr->size++;
	return;
}

void* OC_find_item(const struct Ordered_container* c_ptr, const void* data_ptr)
{
	struct LL_Node* cur;
	int result;
	assert(c_ptr); assert(data_ptr);

	if(OC_empty(c_ptr)) return NULL;

	cur = c_ptr->first;
	while(cur)
	{
		result = (*(c_ptr->comp_func))(data_ptr, cur->data_ptr);
		if(result <= 0) break;
		cur = cur->next;
	}

	if(cur && result == 0)
		return (void*) cur;
	else
		return NULL;
}

void* OC_find_item_arg(const struct Ordered_container* c_ptr, const void* arg_ptr, OC_find_item_arg_fp_t fafp)
{
	struct LL_Node* cur;
	int result;
	assert(c_ptr); assert(fafp); assert(arg_ptr);
	if(OC_empty(c_ptr)) return NULL;

	cur = c_ptr->first;
	while(cur)
	{
		result = (*fafp)(arg_ptr, cur->data_ptr);
		if(result <= 0) break;
		cur = cur->next;
	}
	if(cur && (*fafp)(arg_ptr, cur->data_ptr) == 0)
		return (void*) cur;
	return NULL;
}

void OC_apply(const struct Ordered_container* c_ptr, OC_apply_fp_t afp)
{
	struct LL_Node* cur;

	assert(c_ptr); assert(afp);

	if( OC_empty(c_ptr) ) return;

	cur = c_ptr->first;
	while(cur)
	{
		(*afp)(cur->data_ptr);
		cur = cur->next;
	}
	return;
}

int OC_apply_if(const struct Ordered_container* c_ptr, OC_apply_if_fp_t afp)
{
	struct LL_Node* cur;
	int result;

	assert(c_ptr); assert(afp);

	if( OC_empty(c_ptr) ) return 0;
	cur = c_ptr->first;
	result = 0;
	while(cur && result == 0)
	{
		result = (*afp)(cur->data_ptr);
		cur = cur->next;
	}
	return result;
}

void OC_apply_arg(const struct Ordered_container* c_ptr, OC_apply_arg_fp_t afp, void* arg_ptr)
{
	struct LL_Node* cur;

	assert(c_ptr); assert(arg_ptr); assert(afp);
	if( OC_empty(c_ptr) ) return;
	cur = c_ptr->first;
	while(cur)
	{
		(*afp)(cur->data_ptr, arg_ptr);
		cur = cur->next;
	}
	return;

}

int OC_apply_if_arg(const struct Ordered_container* c_ptr, OC_apply_if_arg_fp_t afp, void* arg_ptr)
{
	struct LL_Node* cur;
	int result;

	assert(c_ptr); assert(arg_ptr); assert(afp);

	cur = c_ptr->first;
	result = 0;
	while(cur && result == 0)
	{
		result = (*afp)(cur->data_ptr, arg_ptr);
		cur = cur->next;
	}
	return result;
}
