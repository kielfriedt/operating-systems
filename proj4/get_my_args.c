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
 *
 * Send in an array of strings. This function allocates the memory for this parameter,
 * the caller does not.
 * This function will get input from the user using getline().
 * This function will print out an error message, free up the mem, and return NULL if:
 *   too many characters
 *   or
 *   too many tokens (arguments separated by spaces or tabs)
 * This function will return a pointer to the malloced memory if no such problems. The 
 * calling function should free this memory when finished with it.
 * This function puts pointers to the tokens in the parameter array of strings.
 * If user enters:  (extra spaces are intentional)
 * "    ls     -l  -a"
 * buff[0] points to the l in ls
 * buff[1] points to the - in -l
 * buff[2] points to the - in -a
 * buff[3] = NULL
 * 
 *
 * The array of strings buff is ready to send into execvp (if there are no &,<,>, or |)
 *
 * If the user just hits Enter
 * buff[0] = NULL
 * In this case the caller will need to free up the memory.
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <ctype.h>
 
void *get_my_args(char *buff[]) {
   int bytes_read;
   size_t MAX_BYTES = 100;
   size_t nbytes = MAX_BYTES+2;
   int MAX_TOKENS = 20;
   char *my_string;
   int i=0;
   int token=0;

   // malloc enough space for a MAX_BYTES character line
   my_string = (char *)malloc(nbytes + 1);
   if(my_string == NULL) { // did malloc fail?
       return NULL;
   }
   
   bytes_read = getline (&my_string, &nbytes, stdin);
   
   if(bytes_read == -1) {
       free(my_string); // free up the space -- no memory leaks!
       fprintf(stderr, "getline failed\n");
       return NULL;
   }
   if(bytes_read > MAX_BYTES+1) {
      free(my_string); // free up the space
      fprintf(stderr, "Too many characters in command\n");
      return NULL;
   }
   
   do {
      // eat up space and tab characters
      while(my_string[i]==' ' || my_string[i]=='\t') {
          my_string[i]='\0';
          i++;
      }

      // OK, we have a char that isn't a space or a tab--is it a newline? 
      if(my_string[i]=='\n'){ 
          if(token > MAX_TOKENS) {
		      free(my_string);
              fprintf(stderr, "Too many arguments in command\n");
              return NULL;
          }
          //replace \n with \0
          my_string[i]='\0';
		  
          //end token string with NULL so that execvp will work
		  buff[token]=NULL;
		  
		  // return the pointer to the malloced memory so that we can free it
          return my_string;
      }
	  
      //char not a newline. It must be the start of new token.
      buff[token]=&my_string[i];
	  token++;
	  i++;
      
      // eat up the rest of the characters in this token 
      // isgraph determines if a character is printable and a non-space
	  while( isgraph(my_string[i]) && my_string[i] != '\n' ) {
          i++;
      }
	  
  }    
  while(1);
      
}
