#include <assert.h>
#include <errno.h>
#include <math.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

///////////////////////////////
//Assignment 4: basically remaking pipe.
//Class: CS 360
//By: Tam Nguyen
//Description: Write a program (in C) 
//called connect.c targeted at the Linux 
//platform that performs like a shell pipeline. 
/////////////////////////////////

char **argParser(int argc, char **argv, int *colonLoc); // Function to get us a section of argv array

int main (int argc, char *argv[]) {
    if (argc  <= 1 || (argv[1][0] == ':')) {
        fprintf(stderr, "Error: need at least one shell command.\n");
        return -1; // Don't do anything if there are no args or there is no left side cmd
    }
    else if (argc == 2) execvp(argv[1], argv+1); // if there's only one argument, just execute it.

    int fd[2]; // Our fd for pipe
    int rfd = fd[0], wfd = fd[1]; // Out read/write fd's
    int colonLoc = 0; // Location of colon
    int childStatus;
    
    char **arg1 = argParser(argc, argv, &colonLoc);
    assert(arg1 != NULL); // We want to crash since pipe crash if you dont give it a left cmd
    char **arg2 = argParser(argc, argv, &colonLoc); // we don't care if there is a second cmd
    int singleArg = (arg2 == NULL)? 0:1;

    assert(pipe(fd) >= 0);
    rfd = fd[0]; wfd = fd[1];
    if (fork()) { // Parent process
        wait(&childStatus); // Wait for child to finish with it's exec
        assert (childStatus == 0);
        close(wfd);
        dup2(rfd, STDIN_FILENO);

        if (singleArg) {
            execvp(arg2[0], arg2); // execVP will look for the path for us.
        }
        else { // Just print us the file
            char buff='\0';
            while (read(rfd, &buff, 1)) {
                printf("%c", buff);
            }
        }
        close(rfd);
        free(arg1);
        free(arg2);
    }
    else { // Child process
        close(rfd); // So that we can write to it
        dup2(wfd, STDOUT_FILENO);
        execvp(arg1[0], arg1); // Can not just do this statement because it won't execute the second command correctly
        free(arg1);
        free(arg2);

        close(wfd);
        exit(0);
    }
    
    return 0;
}

char **argParser(int argc, char **argv, int *colonLoc) {
    int i = *colonLoc;

    if (i == argc-1 ||i == argc)    return NULL; // check to see if we're at the end of the argv array
    i++;
    while ( i < argc && argv[i][0] != ':') i++;

    int newsize = i-*colonLoc;
    char **strArr = malloc(newsize * sizeof(char*)); // Want to include null
    for (int n = 0, n2 = *colonLoc+1; n < newsize && n2 < i && n2 < argc; n++) {
        strArr[n] = argv[n2]; 
        n2++;
    }
    printf("\r"); // Program crashes without this statement
    *colonLoc = i;
    strArr[i] = (char*)0; // Set null for exec
    return strArr;
}
