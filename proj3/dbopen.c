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
 *	Title: dbopen.c
 *	Name: Ellen Porter, Andrew Johnson, Kiel Friedt
 *	Date: Feb, 19 2010
 *
 *	Opens the specified database.
*/

#include "db.h"

DB_FILE *db_open( char *file_name )
{
	DB_FILE *db = (DB_FILE *) malloc(sizeof(DB_FILE));
	assert(db != 0);
	
	int key_size, data_size, hash_size;
	
	db->file_descriptor = open( file_name, O_RDWR );
	
	// If any errors occured
	if(db->file_descriptor < 0)
	{
		return NULL;
	}
	
	lseek( db->file_descriptor, 0, SEEK_SET);
	
	// Get key_size
	read(db->file_descriptor,&key_size,sizeof(int));

	// Get data_size
	read(db->file_descriptor,&data_size,sizeof(int));

	// Get hash_size
	read(db->file_descriptor,&hash_size,sizeof(int));
	
	
	db->file_name = file_name;
	// Save the key size in DB_FILE
	db->key_size = key_size;
	// Save the data size in DB_FILE
	db->data_size = data_size;
	// Save the hash size in DB_FILE
	db->hash_size = hash_size;
	//svae next to -1
	db->next = -1;
	
	// Calculate record_size
	// Record looks like: Hash - link - key - data
	db->record_size = sizeof(int)*2 + db->key_size + data_size;

	return db;
}

