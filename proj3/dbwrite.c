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
 *	Title: dbwrite.c
 *	Name: Ellen Porter, Andrew Johnson, Kiel Friedt
 *	Date: Feb, 19 2010
 *
 *	Writes record to hash location.
 */

#include "db.h"

int findNextEmpty(DB_FILE *db_fp, int start);
int findEndLink(DB_FILE *db_fp, int start);

int db_write( DB_FILE *db_fp, char *key, void *data )
{
	
	char * lockName = (char *) malloc(strlen(db_fp->file_name) +LOCK_EXT);
	strcpy(lockName, db_fp->file_name);
	lockName = strcat(lockName, ".LCK\0");
	
	int pfd;
	while((pfd =  open(lockName, O_CREAT | O_EXCL | O_RDWR, 0600)) == -1 )
	{
		if( errno==EEXIST || errno == EINTR  )
		{
			sleep(1);
		}
		else 
	   	{
			return -1;
		}
	} // End LOCK while
	
	if(strlen(key)+1 > db_fp->key_size)
	{
		close(pfd);
		remove(lockName);// remove the lock
		return -1;
	}
	
	//printf("%s has the lock\n", db_fp->lock_file);
	int r, link, rec_hash;
	int endofList, emptySpot;
	int zero = 0;
	char rec_key[db_fp->key_size];
	long int location;
	
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
		close(pfd);
		remove(lockName);// remove the lock
		return -1;
	}	
	
	// Read link
	read(db_fp->file_descriptor, &link, sizeof(int));
	
	// Read key
	read(db_fp->file_descriptor,&rec_key,db_fp->key_size);	
	int prev_link = link;
	
	/******* Check if we're updating a record ******/
	
	int rec_start = rec_hash;
	
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
	
	if(pfd >= 0)
	{
		// We're not updating a record
		if(r != 0)
		{
			// Returns the hash of the last empty file in the linked list
			emptySpot = findNextEmpty(db_fp, rec_start);
			// if rec_start == emptySpot then,	
			// record was already empty, skip linking	
			if(rec_start != emptySpot)
			{	
				// /******* Start Linking! ******/
				
				// Returns the hash of the last file in the linked list
				
				endofList = findEndLink(db_fp, rec_start);
	  			emptySpot = lseek(db_fp->file_descriptor, 0, SEEK_END);
				emptySpot = (emptySpot-FILE_INFO)/db_fp->record_size;
				//emptySpot = findNextEmpty(db_fp, endofList);
				
				// lseek to the end of list
				location = FILE_INFO + endofList*db_fp->record_size;
				lseek( db_fp->file_descriptor,location, SEEK_SET);
				// Rewrite hash
				write(db_fp->file_descriptor, &endofList, sizeof(int));
				//printf("Updating link at %d to %d\n", endofList, emptySpot);
				write(db_fp->file_descriptor, &emptySpot, sizeof(int));
				// set rec_hash to the empty record.
				rec_hash = emptySpot;
			}
		}
		
		
		// rec_hash should now be an empty record slot or spot we're updating
		location = FILE_INFO + rec_hash*db_fp->record_size;
		lseek( db_fp->file_descriptor,location, SEEK_SET);
		
		//printf("Writing the following information at: %d\n", rec_hash);
		// Write hash to hash (should already be there)
		write(db_fp->file_descriptor, &rec_hash, sizeof(int));
		//printf("  Link: %d\n", link);
		// Rewrite link
		write(db_fp->file_descriptor, &link, sizeof(int));
		//printf("  Key: %s\n", key);
		// Write the key
		write(db_fp->file_descriptor, key, db_fp->key_size);
		//printf("  Data: %s\n", data); 
		// Write Data
		write(db_fp->file_descriptor, data, db_fp->data_size );
		
	}
	<<<<<<< .mine
	
	
	// rec_hash should now be an empty record slot.
	location = FILE_INFO + rec_hash*db_fp->record_size;
	lseek( db_fp->file_descriptor,location, SEEK_SET);
	
	printf("Storing record at: \n");
	printf("  Hash: %d\n", rec_hash);
	
	// Write hash to hash (should already be there)
	write(db_fp->file_descriptor, &rec_hash, sizeof(int));
	
	// Write zero to link
	write(db_fp->file_descriptor, &zero, sizeof(int));
	
	// Write the key
	write(db_fp->file_descriptor, key, DB_KEY_MAX);
	
	// Write Data
	write(db_fp->file_descriptor, data, db_fp->data_size );
	
	remove(str3); // remove the lock
	free(str3);
	=======
	// Remove the lock
	close(pfd);
	remove(lockName);
	>>>>>>> .r38
	return 0;
}

/**
 *  Follows the links to the end, returns that records hash. (The record at the end of the list!!!!!)  Not an empty record!
 */
int findEndLink(DB_FILE *db_fp, int start)
{
	int link, hash;
	long int location;
	<<<<<<< .mine
	link = 1;
	while(link != 0)
		=======
		int test = 1;
	while(test != 0)
		>>>>>>> .r38
	{
		// lseek to start of record
		location = FILE_INFO + start*db_fp->record_size;
		lseek(db_fp->file_descriptor,location, SEEK_SET);
		
		// Read hash
		read(db_fp->file_descriptor,&hash,sizeof(int));
		
		// Read link
		read(db_fp->file_descriptor, &link, sizeof(int));
		
		// Read key
		char key[db_fp->key_size];
		read(db_fp->file_descriptor,&key,db_fp->key_size);
		// once we find a record with an empty key, return its hash
		if(link == 0)
		{ 
			test = 0;
		}
		// Try next link
		start = link;
	}
	return hash;
}
<<<<<<< .mine

int findNextEmpty(int db, int rec_size, int start)
=======
int findNextEmpty(DB_FILE *db_fp, int start)
>>>>>>> .r38
{
	long int location;
	int r = 1;
	int hash, link;
	int quit = 0;
	while(r != 0 && quit != 2)
	{	
		// lseek to start of record
		location = FILE_INFO + start*db_fp->record_size;
		lseek( db_fp->file_descriptor,location, SEEK_SET);		
		// Read hash
		read(db_fp->file_descriptor,&hash,sizeof(int));
		
		// Read link
		read(db_fp->file_descriptor, &link, sizeof(int));
		
		// Read key
		char key[db_fp->key_size];
		read(db_fp->file_descriptor,&key,db_fp->key_size);
		
		
		if( start >= db_fp->hash_size )
		{
			start = -1;
			quit++;
			
		}
		// Turn the key into an int, check if it's 0
		r =  strcmp(key, "0");//atoi(key);	
		// Start looking at next hash
		start++;
		
	}
	
	// Once we found an empty record, return the hash.
	return hash;
}


