/*
 *	Copyright (C) 2011  Kiel Friedt
 *
 *    This program is free software: you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation, either version 3 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License
 *    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
/*
 *	Title: db.h
 *	Name: Ellen Porter, Andrew Johnson, Kiel Friedt
 *	Date: Feb, 19 2010
 *
 *	Header file.  Defines the struct and functions
 */

#define DB_PATH_MAX 128
#define DB_KEY_MAX 128 
#define LOCK_EXT 5
#define FILE_INFO 12

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/unistd.h>
#include <unistd.h>
#include <assert.h>


/* Structure Definition */
typedef struct 
{ 
	int file_descriptor;
	char *file_name; 
	int key_size;
	int data_size;
	int hash_size;
	int record_size;
	int next;
} DB_FILE; 

/* Function Declarations */
int db_create( char *file_name, int key_size, int data_size, int hash_size );
DB_FILE *db_open( char *file_name );
int db_close( DB_FILE *db_fp );
int db_key_size( DB_FILE *db_fp );
int db_data_size( DB_FILE *db_fp );
int db_hash_size( DB_FILE *db_fp );
int db_read( DB_FILE *db_fp, char *key, void *data );
int db_write( DB_FILE *db_fp, char *key, void *data );
int db_begin_scan( DB_FILE *db_fp );
int db_read_next( DB_FILE *db_fp, char *key, void *data );

