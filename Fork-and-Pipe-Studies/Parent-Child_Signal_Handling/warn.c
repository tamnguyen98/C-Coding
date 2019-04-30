#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

///////////////////////////////
//Assignment 5: Fork,Pipe, and signals
//Class: CS 360
//By: Tam Nguyen
//Description: Write a program (in C) that forks where the parents accepts user's input and child printing the inputs (passed from parent) periodically. 
//The child and parents must communicate with each other via Pipe and signals.
/////////////////////////////////

#define buffSize 512
int parseParentInput(int readFD, char *buff, int *printPtr);

int childPid;
int flag = 0, canPrint = 1; // Yea I know "Flag" is not a good name, but it's being use by both child and parent for different task, so it can't have a proper name

void parentHandler(int sig) {
    if (sig == 2) // SIGINT
        flag = 1;
    else if (sig == 8) // SIGFPE
        flag = 2;
    else if (sig == EINVAL) {
        fprintf(stderr, "Error (%d): %s (signal error in parent).\n", errno, strerror(errno));
    }
}
void childHandler(int sig) {
    if (sig == 14) // SIGALARM
        alarm(0); // reset all alarm
    else if (sig == 8) { //SIGFPE
        flag = 1;
        alarm(0);
    } else if(sig == 2) { // SIGINT
        canPrint = 0;
    } else if (sig == EINVAL) {
        fprintf(stderr, "Error (%d): %s (signal error in child).\n", errno, strerror(errno));
    }
    
}

int main(int argc, char const *argv[])
{
    int fd[2]; // Passing stuff to child
    pipe(fd); // Open coms connection
	childPid = fork();
	if(childPid){
        char buff[buffSize] = {'\0'}; // Inside because we want to make sure it'll nulled out each time
		close(fd[0]); // close the reading (child) end
        signal(SIGINT, parentHandler);
        signal(SIGFPE, parentHandler);
        while (1) {
            pause(); // stall the program until we get a signal (any signal will work since we're not ignoring any signals)
            if (flag == 1) {
                printf("Enter new message: ");
                if (fgets(buff, buffSize, stdin)) {
                    if(!strchr(buff, '\n')){ // if the buffer doesn't contain \n (when we enter more than buffSize)
                        scanf("%*[^\n]");scanf("%*c");//clear upto newline
                        buff[buffSize-1] = '\n';
                    }
                }
                if (strlen(buff) > 1) { // check to make sure it's not empty
                    write(fd[1], buff, buffSize); // write to the the pipe (start of the conveyer belt)
                    flag = 0;
                    kill(childPid, SIGFPE);
                }
                else
                    fprintf(stderr, "Error: Empty input.\n");
                
            } else if (flag == 2) {
                break;
            }
        }
        close(fd[1]); // close it
        wait(NULL); // wait for child to finish before exiting (will fix onscreen display glitches)
	}else{
        char buff[buffSize] = {'\0'};
        close(fd[1]); // close the write end (begining) since we're reading from it (end of the coveyer)
        signal(SIGALRM, childHandler); // tell how to hand alarm signal
        signal(SIGINT, childHandler);
        signal(SIGFPE, childHandler);
        pause();
        int printAt = 0;
        int timer = 5;
		while (1) {
            if (flag) {
                printAt = 0;
                timer = parseParentInput(fd[0], buff, &printAt);
                flag = 0;
                if (strcmp(&(buff [printAt]), "exit\n") == 0 || timer == 0) {
                    printf("Child exiting...\n");
                    kill(getppid(), SIGFPE);
                    break;
                }
                
                canPrint = 1;
                ualarm(5000, 0); // Send and alarm in x microsec. I just want it to print the first message really fast.
			    pause(); // suspen program.
            }
            else if (canPrint) {
                fflush(NULL); // flush stdout
                printf("\r%s", &(buff[printAt])); // Needs the \r because it'll print weird char from buff overflow
                alarm(timer); // Send and alarm in x sec
			    pause(); // suspen program.
            }
		}
        close(fd[0]);// close our read end
        exit(0);
	}
	return 0;
}

int parseParentInput(int readFD, char *buff, int *printPtr) {
    read(readFD, buff, buffSize);
    if (strcmp(buff, "exit\n") == 0) {
        return 0;
    }

    int len = strlen(buff);
    int i = 0, actualStart;
    if (buff[0] == ' ') {
        for(i = 0; i < len && buff[i] == ' '; i++) {} //skip begining space if any
    }
    actualStart = i; // start if we remove prefixed spaces
    int mix = 0; //is the first token mix with letters?
    int dashOccur; // flag
    for(dashOccur = 0; i < len; i++) // just to check the first token if it's a valid timer value
    {
        char c = buff[i];
        if (c != ' ') { // while we're on the first token
            if (c == '-') {
                if (dashOccur == 0) { // check to see if this is the first occurance
                    dashOccur++;
                } else { // multiple dash, so we're gonna treat it like a word
                    mix = 1;
                    break;
                }
            }
            else {
                if (!(c >= '0' && c <= '9')) {
                mix = 1;
                break;
                }
                else if (i == strlen(buff)-1) {
                    mix = 1; // Assume the user just want to print the number
                    fprintf(stderr, "WARNING: only a number is detected with no following messages. Proceeding to print number with 5 sec interval.\n");
                }
            }
        }
        else // exit the token
            break;
        
    }
    for( ;i < len && buff[i] == ' '; i++) {} // remove any extra spaces after the first token
    
    int timer;
    int containExit = strcmp("exit\n", &buff[i]);
    if (!mix) { // if it's a valid timer
        char *ptr;
        timer = strtol(buff, &ptr, 10); // try to conver it to base 10
        if (errno == EINVAL || errno == ERANGE)
            fprintf(stderr, "Error (%d): %s\n", errno, strerror(errno));
        if (timer < 0) { // if it's a negative value
            timer = timer*-1;
            if (containExit)
                fprintf(stderr, "Error: First token (timer) is negative; taking the absolute value (%d).\n", timer);
        }
        else if (timer == 0) {
            if (containExit) // if it's not a exit
                fprintf(stderr, "Error: Can't have a timer of 0. Setting timer to 5 second.\n");
            timer = 5;
        }
        *printPtr = i; // we want to print what ever is after the timer
    } else {
        *printPtr = actualStart; // ignore prefix spaces.
        fprintf(stderr, "Notice: Unable to get delay timer (make sure first token is an integer); using default value (5).\n");
        timer = 5;
    }
    
    return timer;
}