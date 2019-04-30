#include <errno.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>

/**
 * Assignment 7
 * By: Tam Nguyen
 * Class: CS 360
 * March 2019
 * General idea: Solve the Dining Philosopher problem with C using threads (this program uses arrays of semaphors).
 * The randomGaussian() was provided to get the sleep time for eat/think.
 */

// IGNORE:For debugging purposes
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

static void *philoTask(void *argv);
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

static pthread_mutex_t server = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t sticksUpdated = PTHREAD_COND_INITIALIZER; 
static int chopstick [5] = {1};

int main()
{
	pthread_t philo[5];
    for(int i = 0; i < 5; i++)
        chopstick[i] = 1;

    for(int i = 0; i < 5; i++) { // Make our threads
        if (pthread_create(&philo[i], NULL, philoTask, &i) != 0) {
            fprintf(stderr, "Error (%d): %s (Error creating thread %d).", errno, strerror(errno), i+1);
            return -1;
        }
    }
    
    for(int i = 0; i < 5; i++) { // merge out threads
        if (pthread_join(philo[i], NULL) != 0)
            fprintf(stderr, "Error (%d): %s (Error joining thread %d).", errno, strerror(errno), i+1);
    }
    
    return 0;
}

static void *philoTask(void *argv) {
    int id = (*(int *) argv)-1;
    int left = id, right = (id+1)%5;
    srand(id + time(NULL) * id); // this guarantee all 5 child doesn't start with same think/eat time (placing it here)
    int cycles = 0;
    int totalThinkTime = 0, totalEatTime = 0;
    int eatTime = randomGaussian(9, 3); // same as below comment
    int thinkTime = randomGaussian(11,7); // 11 and 7 are predefine by assignment
    while (totalEatTime < 100) {
        
        eatTime = (eatTime < 0)? 0:eatTime;
        thinkTime = (thinkTime < 0)? 0:thinkTime;
        /*----------think---------*/
        printf("Philospher %d is thinking for %d seconds (%d)\n", id+1, thinkTime, totalThinkTime);
        sleep(thinkTime);
        totalThinkTime += thinkTime;
        /*----------Pick up Chopstick---------*/
        pthread_mutex_lock(&server);
        while (!(chopstick[left] && chopstick[right])) // if both are not available
            pthread_cond_wait(&sticksUpdated, &server);

        chopstick[left] = chopstick[right] = 0;
        pthread_mutex_unlock(&server);
        /*----------Eat---------*/
        printf("Philospher %d is eating for %d seconds (%d)\n", id+1, eatTime, totalEatTime);
        sleep(eatTime);
        totalEatTime += eatTime;
        thinkTime = randomGaussian(11,7); // 11 and 7 are predefine by assignment
        eatTime = randomGaussian(9, 3);
        cycles++;
        /*---------Drop Chopstick---------*/
        chopstick[left] = chopstick[right] = 1;
        pthread_cond_signal(&sticksUpdated);
        
    }
    printf("Philosopher %d ate for %d seconds and thought for %d seconds, over %d cycles.\n", id+1, totalEatTime, totalThinkTime, cycles);
}