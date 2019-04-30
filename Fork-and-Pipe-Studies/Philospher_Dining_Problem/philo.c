#include <errno.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
/**
 * Assignment 6
 * By: Tam Nguyen
 * Class: CS 360
 * March 2019
 * General idea: Solve the Dining Philosopher problem with C using threads/semaphor (this program uses arrays of semaphors).
 * The randomGaussian() was provided to get the sleep time for eat/think.
 */
void simulation(struct sembuf chopstickUP[], struct sembuf chopstickDOWN[], int chopstickSemID);
int randomGaussian(int mean, int stddev) { // Function provided by instructors
	double mu = 0.5 + (double) mean;
	double sigma = fabs((double) stddev);
	double f1 = sqrt(-2.0 * log((double) rand() / (double) RAND_MAX));
	double f2 = 2.0 * 3.14159265359 * (double) rand() / (double) RAND_MAX;
	if (rand() & (1 << 5)) 
		return (int) floor(mu + sigma * cos(f2) * f1);
	else            
		return (int) floor(mu + sigma * sin(f2) * f1);
}


int main()
{
    int chopstickSemID;
	// Create semaphore with private ID, initialize with 1, and set a flag (idk what the flags means)
	if ((chopstickSemID = semget(IPC_PRIVATE, 5, IPC_CREAT | IPC_EXCL | 0600)) == -1) {
        fprintf(stderr, "Error (%d): %s (semget error).\n", errno, strerror(errno));
        return (-1);
    }
	
	// variable how to handle the semaphores
	struct sembuf chopstickUP[5]; // only decrement the semaphor value and don't alter anything else
	struct sembuf chopstickDOWN[5]; // only increment the value
	// each argument (I think) is corresponse to the semget

    for(int i = 0; i < 5; i++)
    {
        struct sembuf tmp1 = {i, 1, 0}; // make the chopstick available
        struct sembuf tmp2 = {i, -1, 0}; // make it inuse
        chopstickUP[i] = tmp2;
        chopstickDOWN[i] = tmp1;
        //semop(chopstickSemID, &chopstickDOWN[i],1); // only put down the left at the start, other philo can worry about there own stick
        semctl(chopstickSemID, i, SETVAL, 1);
    }
    simulation(chopstickUP, chopstickDOWN, chopstickSemID);
    
    return 0;
}

void simulation(struct sembuf chopstickUP[], struct sembuf chopstickDOWN[], int chopstickSemID) {
    pid_t pid[5]; // child pids
    int i;
    for (i = 0; i < 5; ++i) {
        if ((pid[i] = fork()) < 0) {
            fprintf(stderr, "Error (%d): %s (Fail to create child %d).\n", errno, strerror(errno), i);
            for(int n = i - 1; n >= 0; n--) { //If any of the child couldn't be created, kill all others and close
                kill (pid[n], SIGTERM); // it's either all of none
                fprintf(stderr, ">Terminating signal sent to child %d.\n", n);
            }
            abort();
        } 
        else if (pid[i] == 0) { //child process
            srand(getpid()); // this guarantee all 5 child doesn't start with same think/eat time (placing it here)
            int cycles = 0;
            int totalThinkTime = 0, totalEatTime = 0;
            int eatTime = randomGaussian(9, 3); // same as below comment
            int thinkTime = randomGaussian(11,7); // 11 and 7 are predefine by assignment
            while (totalEatTime < 100) {
                eatTime = (eatTime < 0)? 0:eatTime;
                thinkTime = (thinkTime < 0)? 0:thinkTime;
                /*----------think---------*/
                printf("Philospher %d is thinking for %d seconds (%d)\n", i+1, thinkTime, totalThinkTime);
                sleep(thinkTime);
                totalThinkTime += thinkTime;
                /*----------Pick up Chopstick---------*/
                semop(chopstickSemID, &chopstickUP[i],1); // pick up left
                semop(chopstickSemID, &chopstickUP[(i+1)%5],1); // pick up right
                /*----------Eat---------*/
                printf("Philospher %d is eating for %d seconds (%d)\n", i+1, eatTime, totalEatTime);
                sleep(eatTime);
                totalEatTime += eatTime;
                
                thinkTime = randomGaussian(11,7); // 11 and 7 are predefine by assignment
                eatTime = randomGaussian(9, 3);
                cycles++;
                /*---------Drop Chopstick---------*/
                semop(chopstickSemID, &chopstickDOWN[(i+1)%5],1); // put right chopstick down
                semop(chopstickSemID, &chopstickDOWN[i],1); // put left down
            }
            printf("Philosopher %d ate for %d seconds and thought for %d seconds, over %d cycles.\n", i+1, totalEatTime, totalThinkTime, cycles);
            exit(0); // prevent child from making their child
        }// parent process just keeps making child
    }

    int status;
    pid_t tmppid;
    while (i > 0) {
        tmppid = wait(&status); // make sure all the child are dead
        --i;
        pid[i] = 0; // not necessary but what ever
    }

}