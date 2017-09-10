/*

 * Utility.h
 *
 *  Created on: Jan 16, 2014
 *      Author: Peter
 */

#ifndef UTILITY_H
#define UTILITY_H

#include "Record.h"
#include <stdio.h>

#define TITLE_ARY_SIZE 64
#define MED_ARY_SIZE 8
#define COLL_ARY_SIZE 16
#define FILE_ARY_SIZE 32
#define MED_ARY_FMT "%7s"
#define COLL_NAME_ARY_FMT "%15s"
#define FILE_ARY_FMT "%31s"
#define NDEBUG

int compare_Record_titles(const struct Record * data_ptr1, const struct Record * data_ptr2);

char *compress_string(char *str);

char* remove_trailing_space(char* str);

void mem_err_exit(void);

char *read_Record_title(FILE* infile, char *title);

int comp_Record_to_title(char *title, struct Record * data_ptr);

int comp_Record_to_ID(int *ID, struct Record * data_ptr);

#endif /* UTILITY_H_ */
