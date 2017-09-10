/*
 * Collection.c
 *
 *  Created on: Jan 15, 2014
 *      Author: Peter
 */
#include "Collection.h"
#include "Ordered_container.h"
#include "Utility.h"
#include "Record.h"
#include "p1_globals.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

extern int g_string_memory;
const char * NUM_REC_FMT = "Collection %s contains:";
const char * NO_REC = " None\n";
/* a Collection contains a pointer to a C-string name and a container
that holds pointers to Records - the members. */
struct Collection {
	char* name;
	struct Ordered_container* members;
};

static void save_Record_title(struct Record* data_ptr, FILE *outfile);

struct Collection* create_Collection(const char* name)
{
	struct Collection * coll;
	int name_size;

	assert(name);

	coll = (struct Collection *) malloc( sizeof(struct Collection) );

	name_size = strlen(name) + 1;		/*for NULL byte*/

	coll->name = (char *) malloc( name_size );
	if(!coll->name)
		mem_err_exit();
	g_string_memory += name_size;
	strncpy(coll->name, name, name_size);

	coll->members = OC_create_container((OC_comp_fp_t)compare_Record_titles);

	return coll;

}

void destroy_Collection(struct Collection* collection_ptr)
{
	assert(collection_ptr);

	g_string_memory -= strlen(collection_ptr->name) + 1;
	free(collection_ptr->name);

	OC_destroy_container(collection_ptr->members);
	return;
}

const char* get_Collection_name(const struct Collection* collection_ptr)
{
	assert(collection_ptr);
	return collection_ptr->name;
}

int Collection_empty(const struct Collection* collection_ptr)
{
	assert(collection_ptr);
	return OC_empty(collection_ptr->members);
}

int add_Collection_member(struct Collection* collection_ptr, const struct Record* record_ptr)
{
	assert(collection_ptr); assert(record_ptr);

	if( is_Collection_member_present(collection_ptr, record_ptr) )
		return 1;
	OC_insert(collection_ptr->members, (void *) record_ptr);
	return 0;
}

int is_Collection_member_present(const struct Collection* collection_ptr, const struct Record* record_ptr)
{
	assert(collection_ptr); assert(record_ptr);

	if(  OC_find_item(collection_ptr->members, record_ptr) )
		return 1;

	return 0;
}

int remove_Collection_member(struct Collection* collection_ptr, const struct Record* record_ptr)
{
	void **item_ptr;

	assert(collection_ptr); assert(record_ptr);

	if( !is_Collection_member_present(collection_ptr, record_ptr) )
		return 1;

	item_ptr = OC_find_item(collection_ptr->members, record_ptr);
	OC_delete_item(collection_ptr->members, (void*) item_ptr);

	return 0;
}

void print_Collection(const struct Collection* collection_ptr)
{
	assert(collection_ptr);

	printf(NUM_REC_FMT, collection_ptr->name);
	if( Collection_empty(collection_ptr) )
	{
		printf(NO_REC);
		return;
	}
	printf("\n");
	OC_apply(collection_ptr->members, (OC_apply_fp_t) print_Record);
	return;
}

void save_Collection(const struct Collection* collection_ptr, FILE* outfile)
{
	assert(collection_ptr); assert(outfile);

	fprintf( outfile, "%s %d\n", collection_ptr->name, OC_get_size(collection_ptr->members) );
	OC_apply_arg(collection_ptr->members, (OC_apply_arg_fp_t) save_Record_title, outfile);

	return;
}

struct Collection* load_Collection(FILE* input_file, const struct Ordered_container* records)
{
	char coll_name[COLL_ARY_SIZE];
	char rec_name[TITLE_ARY_SIZE];
	int num_records;
	void **item_ptr;
	struct Record *rec_ptr;
	struct Collection *coll_ptr;
	assert(records); assert(input_file);

	if( fscanf(input_file, COLL_NAME_ARY_FMT, coll_name) != 1 ) return NULL;		/*read collection name*/
	if( fscanf(input_file, "%d", &num_records) != 1 ) return NULL;					/*read number of records*/

	coll_ptr = create_Collection(coll_name);					/*create a collection*/

	for(; num_records > 0; num_records--)
	{
		fscanf(input_file, " ");
		if( !read_Record_title(input_file, rec_name) )
		{
			destroy_Collection(coll_ptr);				/*if there is a title read error, delete the collection*/
			return NULL;
		}
		item_ptr = OC_find_item_arg(records, rec_name, (OC_find_item_arg_fp_t) comp_Record_to_title);
		if(!item_ptr)
		{
			destroy_Collection(coll_ptr);				/*if the record doesn't exist, delete the collection*/
			return NULL;
		}
		rec_ptr = OC_get_data_ptr(item_ptr);
		add_Collection_member(coll_ptr, rec_ptr);
	}
	return coll_ptr;
}


static void save_Record_title(struct Record* data_ptr, FILE *outfile)
{
	assert(data_ptr); assert(outfile);
	fprintf( outfile, "%s\n", get_Record_title(data_ptr) );
}


