/*
 * main.c
 *
 *  Created on: Jan 15, 2014
 *      Author: Peter
 */
#include "Ordered_container.h"
#include "Record.h"
#include "Collection.h"
#include "Utility.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

#define MAX_RATING 5
#define NUM_CMDS 2

extern int g_Container_count;
extern int g_Container_items_in_use;
extern int g_Container_items_allocated;
extern int g_string_memory;
/*top level messages*/
const char * const CMD_PROMPT = "\nEnter command: ";
const char * const END_STR = "Done\n";
/*Bad Input data*/
const char * const BAD_FILE_NAME = "Could not open file!\n";
const char * const BAD_FILE_DATA = "Invalid data found in file!\n";
const char * const DELETE_MEMBER_ERR = "Record is not a member in the collection!\n";
const char * const BAD_RATING = "Rating is out of range!\n";
const char * const WRONG_ID = "No record with that ID!\n";
const char * const WRONG_TITLE = "No record with that title!\n";
const char * const WRONG_NAME = "No collection with that name!\n";
const char * const DUPLICATE_RECORD_LIB = "Library already has a record with this title!\n";
const char * const DUPLICATE_RECORD_COLL = "Record is already a member in the collection!\n";
const char * const DUPLICATE_COLLECTION = "Catalog already has a collection with this name!\n";
const char * const BAD_INT = "Could not read an integer value!\n";
const char * const BAD_TITLE = "Could not read a title!\n";
const char * const BAD_CMD = "Unrecognized command!\n";
/*input output messages*/
const char * const DEL_ALL_COLLECTION = "All collections deleted\n";
const char * const COLLECTIONS_NOT_EMPTY = "Cannot clear all records unless all collections are empty!\n";
const char * const COLLECTION_ADDED = "Collection %s added\n";
const char * const DELETE_COLLECTION = "Collection %s deleted\n";
const char * const MEMBER_ADDED = "Member %d %s added\n";
const char * const DELETE_MEMBER = "Member %d %s deleted\n";
const char * const DELETE_RECORD_ERR = "Cannot delete a record that is a member of a collection!\n";
const char * const LIB_EMPTY = "Library is empty\n";
const char * const LIB_CONTAINS = "Library contains %d records:\n";
const char * const CATALOG_EMPTY = "Catalog is empty\n";
const char * const CATALOG_CONTAINS = "Catalog contains %d collections:\n";
const char * const CHANGE_RATING = "Rating for record %d changed to %d\n";
const char * const ADD_RECORD = "Record %d added\n";
const char * const DELETE_ALL_RECORDS = "All records deleted\n";
const char * const DELETE_RECORD = "Record %d %s deleted\n";
const char * const SAVED = "Data saved\n";
const char * const LOADED = "Data loaded\n";

/*create Objects*/
void read_make_Record(struct Ordered_container * name_lib, struct Ordered_container * ID_lib);
void read_make_Collection(struct Ordered_container * catalog);
/*save and restore*/
void restore_all(struct Ordered_container * catalog, struct Ordered_container * name_lib, struct Ordered_container * ID_lib);
int restore_data(FILE* infile, struct Ordered_container *name_lib, struct Ordered_container *id_lib, struct Ordered_container *catalog);
void save_all(struct Ordered_container * name_lib, struct Ordered_container * catalog);
void save_data(FILE* outfile, struct Ordered_container *name_lib, struct Ordered_container *catalog);
/*add and remove Objects*/
void remove_Record(struct Ordered_container * name_lib, struct Ordered_container * ID_lib, struct Ordered_container * catalog);
void remove_collection(struct Ordered_container * catalog);
void remove_Record_from_Collection(struct Ordered_container * ID_lib, struct Ordered_container * catalog, char * name_buf);
void add_Record_to_Collection(struct Ordered_container * ID_lib, struct Ordered_container * catalog, char * name_buf);
void check_and_delete_Records(struct Ordered_container * name_lib, struct Ordered_container * ID_lib, struct Ordered_container * catalog);
void clear_containers(struct Ordered_container *catalog, struct Ordered_container * name_lib, struct Ordered_container * id_lib);
void clear_lib(struct Ordered_container *name_lib, struct Ordered_container *id_lib);
void clear_catalog(struct Ordered_container *catalog);
/*Read and Get Objects*/
FILE * read_check_file(FILE * fstream, char * file_buf, const char * mode);
struct Collection * read_get_Collection(struct Ordered_container *catalog, char *name_buf);
struct Record * read_ID_get_Record(struct Ordered_container *ID_lib);
struct Record * read_title_get_Record(struct Ordered_container * name_lib, char * title_buf);
void * data_from_arg(struct Ordered_container * container, void * arg, OC_find_item_arg_fp_t fafp);
/*read and check input*/
void read_check_rating(struct Record * rec_ptr, int * rating);
int read_int_value(int *num);
int get_title(char *title_buf);
/*simple checks*/
int Collection_not_empty(const struct Collection * coll_ptr);
int are_Collections_empty(struct Ordered_container *catalog);
int check_Collections(struct Ordered_container *catalog, struct Record *rec);
void check_empty_print(struct Ordered_container *oc, OC_apply_fp_t print, const char *empty, const char * not_empty);
/*Utility and miscellaneous commands*/
void quit(struct Ordered_container * catalog, struct Ordered_container * name_lib, struct Ordered_container * ID_lib);
void discard_input(void);
void invalid_command(void);
void print_allocation(const struct Ordered_container *lib, const struct Ordered_container *catalog);
int compare_Collection_names(const struct Collection * data_ptr1, const struct Collection * data_ptr2);
int comp_Collection_to_name(char *arg, const struct Collection * data_ptr);
int compare_Record_ID(const struct Record * data_ptr1, const struct Record * data_ptr2);
int comp_Record_to_ID(int *ID, struct Record * data_ptr);

int main(void)
{
	struct Ordered_container *library_by_name;
	struct Ordered_container *library_by_ID;
	struct Ordered_container *catalog;
	struct Record *rec_ptr;
	struct Collection *coll_ptr;
	char command1, command2;
	char record_title_buf[TITLE_ARY_SIZE];
	char collection_name_buf[COLL_ARY_SIZE];
	int rating;

	coll_ptr = NULL;
	rec_ptr = NULL;
	catalog = library_by_name = library_by_ID = NULL;
	rating = command1 = command2 = 0;

	library_by_name = OC_create_container( (OC_comp_fp_t) compare_Record_titles);
	library_by_ID = OC_create_container( (OC_comp_fp_t) compare_Record_ID );
	catalog = OC_create_container( (OC_comp_fp_t) compare_Collection_names);

	while(1)
	{
		printf(CMD_PROMPT);
		if(scanf(" %c %c", &command1, &command2) == NUM_CMDS)
		{
			switch(command1)
			{
				case 'f':
						switch(command2)
						{
							case 'r':	/*find and print record*/
								if(  (rec_ptr = read_title_get_Record(library_by_name, record_title_buf)) )
									print_Record(rec_ptr);
								break;
							default:
								invalid_command();
								break;
						}
					break;
				case 'p':				/*print*/
						switch(command2)
						{
							case 'r':	/*record*/
								if( (rec_ptr = read_ID_get_Record(library_by_ID)) )
									print_Record(rec_ptr);
								break;
							case 'c':	/*collection*/
								if( (coll_ptr = read_get_Collection(catalog, collection_name_buf)) )
									print_Collection(coll_ptr);
								break;
							case 'L':	/*library*/
								check_empty_print(library_by_name, (OC_apply_fp_t) print_Record, LIB_EMPTY, LIB_CONTAINS );
								break;
							case 'C':	/*catalog*/
								check_empty_print(catalog, (OC_apply_fp_t) print_Collection, CATALOG_EMPTY, CATALOG_CONTAINS);
								break;
							case 'a':	/*memory allocation*/
								print_allocation(library_by_name, catalog);
								break;
							default:
								invalid_command();
								break;
						}
					break;
				case 'm':			/*MODIFY*/
						switch(command2)
						{
							case 'r':		/*rating*/
								if( (rec_ptr = read_ID_get_Record(library_by_ID)) )
									read_check_rating(rec_ptr, &rating);
								break;
							default:
								invalid_command();
								break;
						}
					break;
				case 'a':
						switch(command2)
						{
							case 'r':		/*add record to libary*/
								read_make_Record(library_by_name, library_by_ID);
								break;
							case 'c':		/*add collection to catalog*/
								read_make_Collection(catalog);
								break;
							case 'm':		/*add member to collection*/
								add_Record_to_Collection(library_by_ID, catalog, collection_name_buf);
								break;
							default:
								invalid_command();
								break;
						}
					break;
				case 'd':			/*DELETE*/
						switch(command2)
						{
							case 'r':	/*record*/
								remove_Record(library_by_name, library_by_ID, catalog);
								break;
							case 'c':	/*collection*/
								remove_collection(catalog);
								break;
							case 'm':	/*member from collection*/
								remove_Record_from_Collection(library_by_ID, catalog, collection_name_buf);
								break;
							default:
								invalid_command();
								break;
						}
					break;
				case 'c':			/*CLEAR*/
						switch(command2)
						{
							case 'L':		/*library*/
								check_and_delete_Records(library_by_name, library_by_ID, catalog);
								break;
							case 'C':		/*catalog*/
								clear_catalog(catalog);
								printf(DEL_ALL_COLLECTION);
								break;
							case 'A':		/*all*/
								clear_containers(catalog, library_by_name, library_by_ID);
								break;
							default:
								invalid_command();
								break;
						}
					break;
				case 's':			/*SAVE*/
						switch(command2)
						{
							case 'A':
								save_all(library_by_name, catalog);
								break;
							default:
								invalid_command();
								break;
						}
					break;
				case 'r':			/*RESTORE*/
						switch(command2)
						{
							case 'A':
								restore_all(catalog, library_by_name, library_by_ID);
								break;
							default:
								invalid_command();
								break;
						}
					break;
				case 'q':			/*QUIT*/
							switch(command2)
							{
								case 'q':
									quit(catalog, library_by_name, library_by_ID);
									return 0;
								default:
									invalid_command();
									break;
							}
					break;
				default:			/*INVALID command1*/
					invalid_command();
					break;
			}

		}
		else
			invalid_command();
	}
	return 0;
}

/*read and create objects*/
void read_make_Record(struct Ordered_container * name_lib, struct Ordered_container * ID_lib)
{
	char medium_buf[MED_ARY_SIZE];
	char record_title_buf[TITLE_ARY_SIZE];
	struct Record * rec_ptr;

	rec_ptr = NULL;
	if(scanf(MED_ARY_FMT, medium_buf) == 1)				/*if medium was read from standard input*/
	{
		if( get_title(record_title_buf) )				/*if a title was read*/
		{
			rec_ptr = data_from_arg(name_lib, record_title_buf, (OC_find_item_arg_fp_t) comp_Record_to_title);
			if(!rec_ptr)								/*if no matching record with the title was found*/
			{
				rec_ptr = create_Record(medium_buf, record_title_buf);
				OC_insert(name_lib, rec_ptr);
				OC_insert(ID_lib, rec_ptr);
				printf( ADD_RECORD, get_Record_ID(rec_ptr) );		/*create a record and add it to the library*/
			}
			else
				printf(DUPLICATE_RECORD_LIB);				/*otherwise print an error message*/
		}
	}
	else
		discard_input();						/*clear the current input line*/
	return;
}

void read_make_Collection(struct Ordered_container * catalog)
{
	char collection_name_buf[COLL_ARY_SIZE];
	struct Collection * coll_ptr;

	coll_ptr = NULL;
	if( scanf(COLL_NAME_ARY_FMT, collection_name_buf) == 1 )			/*if a valid collection name was read*/
	{
		coll_ptr = (struct Collection *)data_from_arg(catalog, collection_name_buf, (OC_find_item_arg_fp_t) comp_Collection_to_name);
		if(!coll_ptr)									/*if no collection with the same name was found*/
		{
			coll_ptr = create_Collection(collection_name_buf);
			OC_insert(catalog, coll_ptr);
			printf(COLLECTION_ADDED, get_Collection_name(coll_ptr) );	/*create a collection object and insert it into the catalog*/
		}
		else
			printf(DUPLICATE_COLLECTION);				/*otherwise print an error*/
	}
	else
		discard_input();								/*clear the line*/
	return;
}

/*save and restore*/
void restore_all(struct Ordered_container * catalog, struct Ordered_container * name_lib, struct Ordered_container * ID_lib)
{
	FILE * infile;
	char file_buf[FILE_ARY_SIZE];
	infile = NULL;
	if( (infile = read_check_file(infile, file_buf, "r")) )				/*if file can be opened*/
	{
		clear_catalog(catalog);
		clear_lib(name_lib, ID_lib);									/*clear all data*/
		if( !restore_data(infile, name_lib, ID_lib, catalog) )			/*if invalid data discovered*/
		{
			printf(BAD_FILE_DATA);									/*print error message*/
			clear_catalog(catalog);
			clear_lib(name_lib, ID_lib);							/*clear any data read from the file*/
		}
		fclose(infile);
	}
	return;
}

void save_all(struct Ordered_container * name_lib, struct Ordered_container * catalog)
{
	FILE * outfile;
	char file_buf[FILE_ARY_SIZE];
	outfile = NULL;

	if( (outfile = read_check_file(outfile, file_buf, "w")) )			/*if file can be opened*/
	{
		save_data(outfile, name_lib, catalog);
		fclose(outfile);
	}
	return;
}

void save_data(FILE* outfile, struct Ordered_container *name_lib, struct Ordered_container *catalog)
{
	fprintf(outfile, "%d\n", OC_get_size(name_lib));						/*print number of records*/
	OC_apply_arg(name_lib, (OC_apply_arg_fp_t) save_Record, outfile);		/*save each record*/
	fprintf(outfile, "%d\n", OC_get_size(catalog));							/*print number of collections*/
	OC_apply_arg(catalog, (OC_apply_arg_fp_t) save_Collection, outfile);	/*save each collection*/
	printf(SAVED);
	return;
}

int restore_data(FILE* infile, struct Ordered_container *name_lib, struct Ordered_container *id_lib, struct Ordered_container *catalog)
{
	int num_recs;
	int num_colls;
	struct Record *rec_ptr;
	struct Collection *coll_ptr;

	rec_ptr = NULL;
	coll_ptr = NULL;
	num_recs = num_colls = 0;

	if( fscanf(infile, " %d", &num_recs) != 1 )				/*read the number of records*/
		return 0;
	for(; num_recs > 0; num_recs--)							/*attempt to load each record*/
	{
		rec_ptr = load_Record(infile);
		if(!rec_ptr) return 0;
		OC_insert(name_lib, rec_ptr);
		OC_insert(id_lib, rec_ptr);
	}
	if( fscanf(infile, " %d", &num_colls) != 1 )				/*read the number of collections*/
		return 0;
	for(; num_colls > 0; num_colls--)						/*attempt to load each collections*/
	{
		coll_ptr = load_Collection(infile, name_lib);
		if(!coll_ptr) return 0;
		OC_insert(catalog, coll_ptr);
	}
	printf(LOADED);
	return 1;
}

/*add and delete objects*/
void remove_Record(struct Ordered_container * name_lib, struct Ordered_container * ID_lib, struct Ordered_container * catalog)
{
	struct Record * rec_ptr;
	char record_title_buf[TITLE_ARY_SIZE];

	rec_ptr = NULL;
	if( (rec_ptr = read_title_get_Record(name_lib, record_title_buf)) )			/*if a title was read and a record was found*/
	{
		if( !check_Collections(catalog, rec_ptr) )							/*check if any collections contain the record*/
		{
			OC_delete_item(name_lib, OC_find_item(name_lib, rec_ptr) );
			OC_delete_item(ID_lib, OC_find_item(ID_lib, rec_ptr) );
			printf(DELETE_RECORD, get_Record_ID(rec_ptr), get_Record_title(rec_ptr) );
			destroy_Record(rec_ptr);										/*remove the record from the libraries and delete it*/
		}
		else
			printf(DELETE_RECORD_ERR);										/*otherwise print an error*/
	}
	return;
}

void remove_collection(struct Ordered_container * catalog)
{
	char collection_name_buf[COLL_ARY_SIZE];
	struct Collection * coll_ptr;

	coll_ptr = NULL;
	if( (coll_ptr = read_get_Collection(catalog, collection_name_buf)) )	/*read a name from input and find the collection*/
	{
		OC_delete_item(catalog, OC_find_item(catalog, coll_ptr) );
		printf(DELETE_COLLECTION, get_Collection_name(coll_ptr));
		destroy_Collection(coll_ptr);									/*delete the collection if found*/
	}
	return;
}

void remove_Record_from_Collection(struct Ordered_container * ID_lib, struct Ordered_container * catalog, char * name_buf)
{
	struct Collection * coll_ptr;
	struct Record * rec_ptr;
	/*Read the Collection name from stdin and search for it in the catalog*/
	rec_ptr = NULL;
	coll_ptr = NULL;
	if( (coll_ptr = read_get_Collection(catalog, name_buf)) )
	{
		/*if the collection is found, read the ID from stdin and find the record in the library*/
		if( (rec_ptr = read_ID_get_Record(ID_lib)) )
		{
			/*if the record is found, try to remove the record from the collection*/
			if( !remove_Collection_member(coll_ptr, rec_ptr) )
				printf(DELETE_MEMBER, get_Record_ID(rec_ptr), get_Record_title(rec_ptr) );
			else
				printf(DELETE_MEMBER_ERR);
		}
	}
	return;
}

void add_Record_to_Collection(struct Ordered_container * ID_lib, struct Ordered_container * catalog, char * name_buf)
{
	struct Collection * coll_ptr;
	struct Record * rec_ptr;
	/*Read the Collection name from stdin and search for it in the catalog*/
	if( (coll_ptr = read_get_Collection(catalog, name_buf)) )
	{
		/*if the collection is found, read the ID from stdin and find the record in the library*/
		if( (rec_ptr = read_ID_get_Record(ID_lib)) )
		{
			/*if the record is found, try to add the record to the collection*/
			if( add_Collection_member(coll_ptr, rec_ptr) )
				printf(DUPLICATE_RECORD_COLL);
			else
				printf(MEMBER_ADDED, get_Record_ID(rec_ptr), get_Record_title(rec_ptr) );
		}
	}
	return;
}

void check_and_delete_Records(struct Ordered_container * name_lib, struct Ordered_container * ID_lib, struct Ordered_container * catalog)
{
	if( !are_Collections_empty(catalog) )			/*check if all collections are empty*/
		printf(COLLECTIONS_NOT_EMPTY);
	else
	{
		clear_lib(name_lib, ID_lib);
		printf(DELETE_ALL_RECORDS);					/*if they are, delete all records*/
	}
	return;
}

void clear_containers(struct Ordered_container *catalog, struct Ordered_container * name_lib, struct Ordered_container * id_lib)
{
	clear_catalog(catalog);
	clear_lib(name_lib, id_lib);						/*clear all the data*/
	printf("All data deleted\n");
	return;
}

void clear_lib(struct Ordered_container *name_lib, struct Ordered_container *id_lib)
{
	OC_apply(name_lib, (OC_apply_fp_t) destroy_Record);
	OC_clear(name_lib);
	OC_clear(id_lib);
	reset_Record_ID_counter();						/*destroy all the records, clear the libraries and reset the id counter*/
	return;
}

void clear_catalog(struct Ordered_container *catalog)
{
	OC_apply(catalog, (OC_apply_fp_t) destroy_Collection);
	OC_clear(catalog);								/*destroy all collections and clear the catalog*/
	return;
}

void check_empty_print(struct Ordered_container *oc, OC_apply_fp_t print, const char *empty, const char * not_empty)
{
	if(OC_empty(oc))							/*check if the supplied container is empty*/
		printf(empty);							/*print empty message if it is*/
	else
	{
		printf(not_empty, OC_get_size(oc) );
		OC_apply(oc, (OC_apply_fp_t) print);	/*if not, print each item in the container*/
	}
	return;
}

void read_check_rating(struct Record * rec_ptr, int * rating)
{
	if( read_int_value(rating) )				/*attempt to read an integer from stdin*/
	{
		if(*rating > 0 && *rating <= MAX_RATING )		/*check if rating is in range*/
		{
			set_Record_rating(rec_ptr, *rating);		/*modify the rating if it is*/
			printf(CHANGE_RATING, get_Record_ID(rec_ptr), *rating);
		}
		else
			printf(BAD_RATING);					/*otherwise print an error*/
	}
	return;
}

/*read input and obtain data*/
struct Record * read_ID_get_Record(struct Ordered_container *ID_lib)
{
	int id;
	struct Record * rec_ptr;
	id = 0;
	if( read_int_value(&id) )				/*attempt to read a valid id from stdin*/
	{
		rec_ptr = (struct Record *)data_from_arg(ID_lib, &id, (OC_find_item_arg_fp_t) comp_Record_to_ID);
		if(rec_ptr)
			return rec_ptr;					/*if a record with the id exists, return it*/
		else
			printf(WRONG_ID);				/*otherwise print an error*/
	}
	return NULL;
}

struct Record * read_title_get_Record(struct Ordered_container * name_lib, char * title_buf)
{
	struct Record * rec_ptr;
	if( get_title(title_buf) )				/*attempt to read a title from stdin*/
	{
		rec_ptr = (struct Record *) data_from_arg(name_lib, title_buf, (OC_find_item_arg_fp_t) comp_Record_to_title);
		if(rec_ptr)
			return rec_ptr;					/*if a record exists with the title, return a pointer to it*/
		else
			printf(WRONG_TITLE);			/*otherwise print an error*/
	}
	return NULL;
}

struct Collection * read_get_Collection(struct Ordered_container *catalog, char *name_buf)
{
	struct Collection *coll_ptr;
	if( scanf(COLL_NAME_ARY_FMT, name_buf) == 1 )		/*attempt to read a collection name*/
	{
		coll_ptr = (struct Collection *)data_from_arg(catalog, name_buf, (OC_find_item_arg_fp_t) comp_Collection_to_name);
		if(!coll_ptr)									/*if a collection does not exist with the name*/
		{
			printf(WRONG_NAME);							/*print an error*/
			discard_input();
			return NULL;
		}
		return coll_ptr;
	}
	discard_input();									/*discard the current line*/

	return NULL;
}

void * data_from_arg(struct Ordered_container * container, void *arg, OC_find_item_arg_fp_t fafp)
{
	void *found_item;
	found_item = OC_find_item_arg(container, arg, fafp);		/*find the data that matches arg*/
	if(found_item)
		return OC_get_data_ptr(found_item);						/*if found, return the data pointer*/
	else
		return NULL;
}

/*read and check input*/
int read_int_value(int *num)
{
	if( scanf(" %d", num) == 1)								/*attempt to read an integer*/
		return 1;
	else
	{
		printf(BAD_INT);									/*if unsuccessful, print an error and discard the line*/
		discard_input();
	}
	return 0;
}

int get_title(char *title_buf)
{
	if( !read_Record_title(stdin, title_buf) )				/*attempt to read a title*/
	{
		printf(BAD_TITLE);									/*if unsuccessful, print an error and return*/
		return 0;
	}
	else if(strlen(title_buf) == 0)
	{
		printf(BAD_TITLE);									/*or if the title is blank, do the same*/
		return 0;
	}
	return 1;
}

FILE * read_check_file(FILE * fstream, char * file_buf, const char * mode)
{
	if( scanf(FILE_ARY_FMT, file_buf) == 1)					/*attempt to read a file name*/
	{
		fstream = fopen(file_buf, mode);					/*if successful, open the file and return the pointer*/
		if(fstream)
			return fstream;
	}
	printf(BAD_FILE_NAME);
	discard_input();									/*otherwise print an error and discard the line*/
	return NULL;
}

void quit(struct Ordered_container * catalog, struct Ordered_container * name_lib, struct Ordered_container * ID_lib)
{
	clear_containers(catalog, name_lib, ID_lib);
	OC_destroy_container(catalog);
	OC_destroy_container(name_lib);
	OC_destroy_container(ID_lib);
	printf(END_STR);									/*destroy all the data*/
	return;
}

void print_allocation(const struct Ordered_container *lib, const struct Ordered_container *catalog)
{
	printf("Memory allocations:\n");
	printf("Records: %d\n", OC_get_size(lib));
	printf("Collections: %d\n", OC_get_size(catalog));
	printf("Containers: %d\n", g_Container_count);
	printf("Container items in use: %d\n", g_Container_items_in_use);
	printf("Container items allocated: %d\n", g_Container_items_allocated);
	printf("C-strings: %d bytes total\n", g_string_memory);
	return;											/*print memory information*/
}

void invalid_command(void)
{
	printf(BAD_CMD);
	discard_input();							/*output an error and discard the line*/
	return;
}

int check_Collections(struct Ordered_container *catalog, struct Record *rec )
/*returns non zero if the record is a member of any collections*/
{
	return OC_apply_if_arg(catalog, (OC_apply_if_arg_fp_t) is_Collection_member_present, rec);
}

int Collection_not_empty(const struct Collection * coll_ptr)
/*return non zero if a collection is not empty*/
{
	return !Collection_empty(coll_ptr);
}
int are_Collections_empty(struct Ordered_container *catalog)
{
	if( OC_empty(catalog) )return 1;
	return !OC_apply_if(catalog, (OC_apply_if_fp_t) Collection_not_empty);
	/*Return 0 if there is a Collection that is not empty 1 otherwise*/
}

int compare_Collection_names(const struct Collection * data_ptr1, const struct Collection * data_ptr2)
/*compares collection names*/
{
	return strcmp( get_Collection_name(data_ptr1), get_Collection_name(data_ptr2) );
}

int comp_Collection_to_name(char *arg, const struct Collection * data_ptr)
/*compares a name string to collection names*/
{
	return strcmp( arg, get_Collection_name(data_ptr) );
}

int compare_Record_ID(const struct Record * data_ptr1, const struct Record * data_ptr2)
/*returns negative, 0 or positive if the ID of the firrst record compares less than equal to or greater than the second*/
{
	return get_Record_ID(data_ptr1) - get_Record_ID(data_ptr2);
}

int comp_Record_to_ID(int *ID, struct Record * data_ptr)
{
	assert(ID); assert(data_ptr);
	return *ID - get_Record_ID(data_ptr);
}

void discard_input(void)
/*discards input until a newline is reached*/
{
	char c;
	while( (c = getchar()) != '\n')
		;
	return;
}

