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
 *	Title: dbreadnext.c
 *	Name: Ellen Porter, Andrew Johnson, Kiel Friedt
 *	Date: Feb, 19 2010
 *
 *	Reads the next record.
 */

#include "db.h"

int db_read_next( DB_FILE *db_fp, char *key, void *data )
{
	int r, link, rec_hash;
	int zero = 0;
	long int location;
	int endoffile = lseek(db_fp->file_descriptor, 0, SEEK_END);
	
	if(db_fp->next == -1)
	{
		return -1;
	}
	
	// Save next pointer
	int next = db_fp->next;
	
	r = 0;
	
	// While the key is empty...
	while(r == 0)
	{
		
		// Seek to that value
		location = FILE_INFO + next*db_fp->record_size;
		lseek( db_fp->file_descriptor,location, SEEK_SET);
		
		// Read hash
		read(db_fp->file_descriptor,&rec_hash,sizeof(int));
		
		// Read link
		read(db_fp->file_descriptor, &link, sizeof(int));
		
		// Read key
		read(db_fp->file_descriptor,key,db_fp->key_size);
		
		
		// Checking if the key is empty
		r = strcmp( key, "0" );	
		
		// lseek to link
		next++;	
		// If we reached the end of the file before finding a non empty key.
		if(next > (endoffile-FILE_INFO)/db_fp->record_size)
		{
			key = NULL;	
			data = NULL;
			next = -1;
			return -1;
		}
	}
	
	// Write Data
	read(db_fp->file_descriptor,data,db_fp->data_size);
	db_fp->next = next;
	
	return 0;
}

