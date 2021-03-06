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
 *	Title: dbread.c
 *	Name: Ellen Porter, Andrew Johnson, Kiel Friedt
 *	Date: Feb, 19 2010
 *
 *	Stores the record key and data into given pointers.
 */

#include "db.h"

int db_read( DB_FILE *db_fp, char *key, void *data )
{
	int r, link, rec_hash;
	long int location;
	char rec_key[db_fp->key_size];
	char rec_data[db_fp->data_size];
	
	
	// Turn key into a hash value
	int hash_value = hash(key, db_fp->hash_size);
	
	// Seek to that value
	location = FILE_INFO + hash_value*db_fp->record_size;
	
	lseek( db_fp->file_descriptor,location, SEEK_SET);
	
	// Read hash
	read(db_fp->file_descriptor,&rec_hash,sizeof(int));
	
	// There was an error reading hash value
	if(hash_value != rec_hash)
	{
		return -1;
	}	
	
	// Read link
	read(db_fp->file_descriptor, &link, sizeof(int));
	
	// Read key
	read(db_fp->file_descriptor,&rec_key,db_fp->key_size);
	
	// Check the inital record
	r = strcmp(rec_key, key);
	while(link != 0 && r != 0)
	{
		// Keep looking if links not empty
		location = FILE_INFO + link*db_fp->record_size;
		lseek(db_fp->file_descriptor,location, SEEK_SET);
		// Read hash
		read(db_fp->file_descriptor,&rec_hash,sizeof(int));
		// Read link
		read(db_fp->file_descriptor, &link, sizeof(int));	
		// Read key
		read(db_fp->file_descriptor,&rec_key,db_fp->key_size);	
		r = strcmp(rec_key, key);
		
	}	
	
	// We found the key before running out of links
	if(r == 0)
	{
		read(db_fp->file_descriptor,data,db_fp->data_size);
		return 0;
	}
	
	return 1;
	
}


