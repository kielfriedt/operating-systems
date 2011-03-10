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
/**
 *  hash.c 
 *
 *  Hash takes a key string and a hash_size integer and returns a
 *  hash value for that key, a integer from 0 to hash_size-1.
 */

int hash(char *key, int hash_size)
{
    // Locals
    int i;
    unsigned int hash = 0;
    unsigned char *p;
	
    // Hash calc
    for(i=0, p=(unsigned char *)key; *p != '\0'; i++,p++) 
    {
        hash ^= ((unsigned int)*p) << (8*(i%sizeof( int )));
    }
    
    // Return
    return(hash_size == 0 ? hash : hash % hash_size);
}


