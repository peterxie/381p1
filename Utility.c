/*
 * Utility.c
 *
 *  Created on: Jan 16, 2014
 *      Author: Peter
 */
#include "Utility.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>

struct Record;
struct Collection;

int compare_Record_titles(const struct Record * data_ptr1, const struct Record * data_ptr2)
/*returns negative, 0 or positive if the titles of the first record compares less than, equal to or greater than the second record*/
{
	return strcmp( get_Record_title(data_ptr1), get_Record_title(data_ptr2) );
}

char *compress_string(char *str)
/*remove leading, extra and trailing whitespace from a string*/
{
	char *result, *iterator;
	int prev_is_space;

	result = iterator = str;
	prev_is_space = 0;

	while( isspace( (int) *iterator) )
		iterator++;

	while(*iterator)
	{
		if( isspace( (int) *iterator) )
		{
			if(prev_is_space)
			{
				iterator++;
				continue;				/*skip consecutive white space*/
			}
			prev_is_space = 1;
		}
		else
		{
			prev_is_space = 0;
		}
		*result++ = *iterator++;		/*copy the character*/
	}
	*result = '\0';		/*terminate with a null byte*/
	remove_trailing_space(str);
	return str;
}

char* remove_trailing_space(char* str)
/*removes trailing whitespace from a string*/
{
	int pos;
	pos = strlen(str) - 1;
	while( isspace( (int)str[pos] ) )
	{
		pos--;
	}
	str[pos+1] = '\0';
	return str;
}

void mem_err_exit(void)
/*function used when there is no more memory to allocate*/
{
	fprintf(stderr, "ERROR: Ran out of Memory\n");
	exit(1);
}
char *read_Record_title(FILE* infile, char *title)
/*read the record title and compress it, then return the compressed version*/
{
	if(!fgets(title, TITLE_ARY_SIZE, infile)) return NULL;
	compress_string(title);
	return title;
}

int comp_Record_to_title(char *title, struct Record * data_ptr)
/*compare the title string to the title of the record*/
{
	assert(title); assert(data_ptr);
	return strcmp(title, get_Record_title(data_ptr) );
}


