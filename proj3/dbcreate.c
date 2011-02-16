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
 *	Title: dbcreate.c
 *	Name: Ellen Porter, Andrew Johnson, Kiel Friedt
 *	Date: Feb, 19 2010
 *
 *	Creates a database with specified paramaters
*/

#include "db.h"

int db_create( char *file_name, int key_size, int data_size, int hash_size )
{

	// If key_size is greater than DB_KEY_MAX, return -1 to indicate an error. 
	if(key_size+1 > DB_KEY_MAX)
	{
		//printf("Key Size to big\n");
		return -1;
	}	
	// If file_name + lock extention is greater than DB_PATH_MAX, return -1 to indicate an error. 
	if(((unsigned int)strlen(file_name) + LOCK_EXT) > DB_PATH_MAX)
	{
		//printf("Path size was to big\n");
	
		return -1;
		
	}
	
	int fd = open( file_name, O_EXCL|O_CREAT|O_RDWR, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP );
	// Removed the errno check to see if file was already found.
	// If any other errors occured
	if(fd < 0)
	{
		return -1;
	}
	
	// Save the key size in the file
	write(fd, &key_size, sizeof(int));
	
	// Save the data size in the file
	write(fd,&data_size, sizeof(int));
	
	// Save the hash size in the file
	write(fd,&hash_size,sizeof(int));
	int i;
	lseek( fd, FILE_INFO, SEEK_SET);
	int z = 0;

	// Make the file 3 times biger than the hash size
	for(i = 0; i < hash_size; i++)
	{
		// Write hash value
		write(fd, &i, sizeof(int));
		// Write empty link
		write(fd, &z, sizeof(int));
		// Write empty key
		write(fd, "0", key_size);
		// Skip data size
		lseek( fd, data_size, SEEK_CUR); 	
	}
	// Close the file
	close(fd);
	return 0;
	
}

