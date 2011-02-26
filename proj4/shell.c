/*
*    Copyright (C) 2011  Kiel Friedt
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
*    along with this program.  If not, see &lt;http://www.gnu.org/licenses/&gt;.
*/

/*
* This code implements a simple shell program
* At this time it supports just simple commands with
* any number of args.
*/

#include &lt;stdio.h&gt;
#include &lt;unistd.h&gt;
#include &lt;sys/types.h&gt;
#include &lt;errno.h&gt;
#include &lt;signal.h&gt;
#include &lt;sys/wait.h&gt;
#include &lt;fcntl.h&gt;
#include &lt;sys/stat.h&gt;
#include &lt;stdlib.h&gt;

void *get_my_args(char *buff[]);
int specialchar(char *command);
int do_command(char **args);
int outputRedirection(char *fileName);
int inputRedirection(char *fileName);
void piper2(char **buff, int pipeflag);

struct sigaction original_act, new_act;

int magic[2];    //pipe variable

/*
* The main shell function
*/
main() {
    char *buff[20];
    char *inputString;
    
    //SIGNALS
    sigset_t set;
    sigfillset( &set );
    
    while(1) {
        int status;
        //checking for background processes
        pid_t background;
        while((background=waitpid(-1, &status, WNOHANG)) &gt; 1)
        {
            if(WIFEXITED(status) && !WEXITSTATUS(status))
            {
                printf("Child %d terminated normally\n", background);
            }
        }
        
        // Print out the prompt and get the input
        fprintf(stdout, ": ");
        
        //Make the program ignore Ctrl-C command:
        new_act.sa_handler = SIG_IGN;
        sigaction(SIGINT, &new_act, &original_act);
        
        //  Get the input from the user
        if( (inputString = get_my_args(buff)) == NULL) continue;
        
        // Check for blank line
        if (buff[0] == NULL)
        {
            free(inputString);
            continue;
        }
        
        // Ignore the line if its a comment
        if(strcmp(buff[0], "#") == 0)
        {
            free(inputString);
            continue;
        }
        
        //cd command
        if(strcmp(buff[0], "cd") == 0)
        {
            if(buff[1] != NULL)
            {
                if(chdir(buff[1]) &lt; 0)
                {
                    printf("No such file or directory\n");
                }
            }
            else
            chdir(getenv("HOME")); // home directory
            free(inputString);
            continue;
        }
        
        // Exit the shell if the user requests
        if (!strcmp(buff[0], "exit")) break;
        do_command(buff);
        //free up the malloced memory
        free(inputString);
    }// end of while(1)
}

/*
* Do the command
*/
int do_command(char **buff)
{
    int status, andflag, flag, x, inflag, outflag, fdin, fdout, pipeflag, total;
    int exitflag = 0;
    
    fdin = 0;
    fdout = 0;
    total = 0;
    x = 0;
    inflag = 0;
    outflag = 0;
    pipeflag = 0;
    flag  = 0;
    
    //pid struct setup
    pid_t child_id, grandchild_id;
    
    // Fork the child process
    child_id = fork();
    
    // Gets the location of special characters and array size
    while(buff[x] != NULL)
    {
        total++;
        if(strcmp(buff[x], "&") == 0)
        andflag = x;
        if(strcmp(buff[x], "&lt;") == 0)
        inflag = x;
        if(strcmp(buff[x], "&gt;") == 0)
        outflag = x;
        if(strcmp(buff[x], "|") == 0)
        pipeflag = x;
        x++;
    }
    
    // get the flag total for special chars
    for(x = 0;x &lt; total; ++x)
    flag += specialchar(buff[x]);
    
    //child enters here
    if(child_id == 0)
    {
        //switch for flag Numbers depending on the combinations
        switch(flag)
        {
            case 0: // No flags were set
            //Ctrl-C allowed for this:
            new_act.sa_handler = SIG_DFL;
            sigaction(SIGINT, &new_act, NULL);
            break;
            case 1: // &
            buff[andflag] = NULL;
            dup2((int)"/dev/null", 1);
            break;
            case 2: // &lt;
            buff[inflag] = NULL;
            exitflag = inputRedirection(buff[inflag+1]);
            buff[inflag+1] = NULL;
            break;
            case 4: //&gt;
            buff[outflag] = NULL;
            exitflag = outputRedirection(buff[outflag+1]);
            buff[outflag+1] = NULL;
            break;
            case 6: //&lt;&gt;
            buff[inflag] = NULL;
            exitflag = inputRedirection(buff[inflag+1]);
            buff[inflag+1] = NULL;
            buff[outflag] = NULL;
            exitflag = outputRedirection(buff[outflag+1]);
            buff[outflag+1] = NULL;
            break;
            case 8:// |
            piper2(buff, pipeflag);
            return;
            break;
            case 10: // &lt;|
            buff[inflag] = NULL;
            exitflag = inputRedirection(buff[inflag+1]);
            buff[inflag+1] = NULL;
            piper2(buff, pipeflag);
            break;
            case 12: // |&gt;
            buff[outflag] = NULL;
            exitflag = outputRedirection(buff[outflag+1]);
            buff[outflag+1] = NULL;
            piper2(buff, pipeflag);
            break;
            case 14: // &lt; | &gt;
            buff[inflag] = NULL;
            exitflag = inputRedirection(buff[inflag+1]);
            buff[inflag+1] = NULL;
            buff[outflag] = NULL;
            exitflag = outputRedirection(buff[outflag+1]);
            buff[outflag+1] = NULL;
            piper2(buff, pipeflag);
            break;
        } // End of switch statement
        
        if(exitflag == 0){
            execvp(buff[0], buff);
            perror("myShell didn't execute the command");
            exit(-1);
        }
        else
        exit(0);
    } // End of child process
    else //beginning of "good" parents
    {
        // Wait for the child process to complete, if necessary
        if(flag == 1)
        {
            waitpid(child_id, &status, WNOHANG);
            printf("Background process %d started\n", child_id);
        }
        else if(flag &lt; 8)
        {
            waitpid(child_id, &status, 0);
            if ( WIFSIGNALED(status) )
            printf("Child %ld terminated due to signal %d\n",(long)child_id, WTERMSIG(status) );
        }
        else
        {
            waitpid(child_id, &status, 0);
            if ( WIFSIGNALED(status) )
            printf("Child %ld terminated due to signal %d\n",(long)child_id, WTERMSIG(status) );
            waitpid(child_id, &status, 0);
            if ( WIFSIGNALED(status) )
            printf("Child %ld terminated due to signal %d\n",(long)child_id, WTERMSIG(status) );
        }
        return;
    } //end of "good" parents
}//end of do_command

/*
* Order: |, &gt;, &lt;, &
* finds flag and returns value
*/
int specialchar(char *command)
{
    int flag = 0;
    
    if(strcmp(command, "&") == 0)
    flag += 1;
    
    if(strcmp(command, "&lt;") == 0)
    flag += 2;
    
    if(strcmp(command,"&gt;") == 0)
    flag += 4;
    
    if(strcmp(command,"|") == 0)
    flag += 8;
    
    return flag;
}

//pipe function
void piper2(char **buff, int pipeflag)
{
    pipe(magic); // pipe before fork
    buff[pipeflag] = NULL;
    pid_t grandchild_id = fork();
    if(grandchild_id == 0)  // Writing out
    {
        dup2(magic[1],1);
        close(magic[0]);
        close(magic[1]);
        execvp(buff[0], buff);
    }
    else            // Reading
    {
        dup2(magic[0],0);
        close(magic[1]);
        close(magic[0]);
        execvp(buff[pipeflag+1], buff+pipeflag+1);
    }
}

//get filename opens,dup2, close and return
int inputRedirection(char *fileName)
{
    int fd;
    if((fd = open(fileName, O_RDONLY)) == -1)
    {
        printf("%s: No such file or directory\n", fileName);
        close(fd);
        return 1;
    }
    dup2(fd,0);
    close(fd);
    return 0;
}

//get filename opens,dup2, close and return
int outputRedirection(char *fileName)
{
    int fd;
    if((fd = open(fileName, O_CREAT|O_WRONLY, 0700)) == -1)
    return 1;
    dup2(fd,1);
    close(fd);
    return 0;
}