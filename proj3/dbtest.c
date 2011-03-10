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
#include "db.h" 
int main(void) 
{	
	DB_FILE *dbp;
	char buf[100]; 
	char buf2[20]; 
	db_create( "my_data_base", 20, 100, 3 ); 
	dbp = db_open( "my_data_base" ); 
	strcpy( buf, "This is the record for Ms. Smith." );
	db_write( dbp, "SMITH929-1517", buf );
	strcpy( buf, "This is the record for Mr. Johnson." );
	db_write( dbp, "JOHNSON622-9876", buf );
	strcpy( buf, "This is the new record for Ms. Smith." );
	db_write( dbp, "SMITH929-1517", buf );
	db_read( dbp, "SMITH929-1517", buf );
	printf( "This should be the new record for Ms. Smith: %s\n", buf );
	db_read( dbp, "JOHNSON622-9876", buf );
	printf( "This should be the record for Mr. Johnson: %s\n", buf );
	printf( "This read should return 1: %d\n", db_read( dbp, "JUNK", buf ) );
	
	// Start scan
	db_begin_scan( dbp );
	db_read_next( dbp, buf2, buf );		
	printf( "Next record: %s, %s\n", buf2, buf );
	db_read_next( dbp, buf2, buf );		
	printf( "Next next record: %s, %s\n", buf2, buf );
	db_read_next( dbp, buf2, buf );		
	printf( "Next next next record: %s, %s\n", buf2, buf );
	db_read_next( dbp, buf2, buf );		
	printf( "Next next next next record: %s, %s\n", buf2, buf );
	db_close (dbp);									
}
