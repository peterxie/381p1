/*
 * Record.c
 *
 *  Created on: Jan 15, 2014
 *      Author: Peter
 */
#include "Record.h"
#include "p1_globals.h"
#include "Utility.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

static int ID_counter = 0;
extern int g_string_memory;
const char * RATING_FMT = "%d: %s %d %s\n";
const char * NO_RATING_FMT = "%d: %s u %s\n";
const char * FILE_FMT = "%d %s %d %s\n";
/* a Record contains an int ID, rating, and pointers to C-strings for the title and medium */
struct Record {
	char* title;
	int ID;
	char* medium;
	int rating;
};

static void modify_Record_ID(struct Record * rec, int id);
static void modify_ID_counter(int id);
static void decrement_counter(void);

struct Record* create_Record(const char* medium, const char* title)
{
	struct Record *rec;
	int title_size;
	int medium_size;

	rec = (struct Record*) malloc(sizeof(struct Record));
	if(!rec)
		mem_err_exit();

	title_size = strlen(title) + 1;
	medium_size = strlen(medium) + 1;

	rec->title = (char *) malloc( title_size );
	if(!rec->title)
		mem_err_exit();
	strncpy( rec->title, title, title_size );
	g_string_memory += title_size;

	rec->medium = (char *) malloc( medium_size );
	if(!rec->medium)
		mem_err_exit();
	strncpy( rec->medium, medium, medium_size);
	g_string_memory += medium_size;

	rec->rating = 0;
	rec->ID = ++ID_counter;

	return rec;
}

void destroy_Record(struct Record* record_ptr)
{
	assert(record_ptr);

	g_string_memory -= strlen(record_ptr->title) + 1;
	free(record_ptr->title);

	g_string_memory -= strlen(record_ptr->medium) + 1;
	free(record_ptr->medium);

	free(record_ptr);

	return;
}

int get_Record_ID(const struct Record* record_ptr)
{
	assert(record_ptr);
	return record_ptr->ID;
}

const char* get_Record_title(const struct Record* record_ptr)
{
	assert(record_ptr);
	return record_ptr->title;
}

void set_Record_rating(struct Record* record_ptr, int new_rating)
{
	assert(record_ptr);
	record_ptr->rating = new_rating;
	return;
}

void print_Record(const struct Record* record_ptr)
{
	assert(record_ptr);
	if(record_ptr->rating)
		printf( RATING_FMT, record_ptr->ID, record_ptr->medium, record_ptr->rating, record_ptr->title);
	else
		printf( NO_RATING_FMT, record_ptr->ID, record_ptr->medium, record_ptr->title);
	return;
}

void save_Record(const struct Record* record_ptr, FILE* outfile)
{
	assert(record_ptr); assert(outfile);

	fprintf(outfile, FILE_FMT, record_ptr->ID, record_ptr->medium, record_ptr->rating, record_ptr->title);
	return;
}

struct Record* load_Record(FILE* infile)
{

	int rating, ID;
	char title[TITLE_ARY_SIZE];
	char medium[MED_ARY_SIZE];
	struct Record* rec;

	assert(infile);

	if(!fscanf(infile, "%d", &ID)) return NULL;

	if(!fscanf(infile, MED_ARY_FMT, medium)) return NULL;

	if(!fscanf(infile, "%d", &rating)) return NULL;

	fscanf(infile, " ");
	if( !fgets(title, TITLE_ARY_SIZE, infile) ) return NULL;
	remove_trailing_space(title);

	rec = create_Record(medium, title);
	decrement_counter();					/*keep the counter unmodified*/

	modify_Record_ID(rec, ID);				/*insert correct id*/

	if(ID > ID_counter)
		modify_ID_counter(ID);				/*if the id is the largest seen so far, set the counter equal to it*/

	set_Record_rating(rec, rating);

	return rec;
}

void reset_Record_ID_counter(void)
{
	ID_counter = 0;
	return;
}

static void modify_Record_ID(struct Record * rec, int id)
{
	rec->ID = id;
	return;
}

static void modify_ID_counter(int id)
{
	ID_counter = id;
	return;
}

static void decrement_counter(void)
{
	ID_counter--;
	return;
}
