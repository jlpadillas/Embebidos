/*
 * File: tick.c
 *
 * Description: Medida de la periodicidad del tick del sistema
 *
 * Author:  DSSE
 * Version: 1.0
 *
*/
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#include <misc/timespec_operations.h>

#define MEASURES        100
#define MAX_LOOPS       10000000

// Prototypes
/**
 * @brief Code to check the system tick
 * @param arg: threshold in nanoseconds
 * @returns pthread_exit
 */
void *high_prio_thread (void *arg);

// Main program that creates one high-priority thread
int main (int argc, char *argv[]) {
    pthread_t t1;
    struct sched_param sch_param;
    pthread_attr_t attr;
    int threshold_us = 0;       // Threshold in us; adjust as needed

    if (argc != 2) {
        perror ("Usage: sudo tick <threshold_in_useconds>\n");
        return EXIT_FAILURE;
    }

    //  Process args
    threshold_us = atoi (argv[1]);

    // Set the priority of the main program to max_prio - 1
    sch_param.sched_priority = (sched_get_priority_max (SCHED_FIFO) - 1);
    if (pthread_setschedparam (pthread_self(), SCHED_FIFO, &sch_param) != 0) {
        printf ("Error while setting main thread's priority\n");
        return EXIT_FAILURE;
    }

    // Create the thread attributes object
    if (pthread_attr_init (&attr) != 0) {
        printf ("Error while initializing attributes\n");
        return EXIT_FAILURE;
    }

    // Set thread attributes

    // Never forget the inheritsched attribute
    // Otherwise the scheduling attributes are not used
    if (pthread_attr_setinheritsched (&attr, PTHREAD_EXPLICIT_SCHED) != 0) {
        printf ("Error in inheritsched attribute\n");
        return EXIT_FAILURE;
    }

    // Thread is created joinable
    if (pthread_attr_setdetachstate (&attr, PTHREAD_CREATE_JOINABLE) != 0) {
        printf ("Error in detachstate attribute\n");
        return EXIT_FAILURE;
    }

    // The scheduling policy is fixed-priorities, with
    // FIFO ordering for threads of the same priority
    if (pthread_attr_setschedpolicy (&attr, SCHED_FIFO) != 0) {
        printf ("Error in schedpolicy attribute\n");
        return EXIT_FAILURE;
    }

    // Set the priority of thread 1 to max_prio
    sch_param.sched_priority = (sched_get_priority_max (SCHED_FIFO));
    if (pthread_attr_setschedparam (&attr, &sch_param) != 0) {
        printf ("Error in schedparam attribute\n");
        return EXIT_FAILURE;
    }

    // create thread 1 with the attributes specified in attr
    if (pthread_create (&t1, &attr, high_prio_thread, &threshold_us) != 0) {
        printf ("Error on creating thread 1\n");
        return EXIT_FAILURE;
    }

    // wait until high prio thread finishes
    pthread_join (t1, NULL);

    return EXIT_SUCCESS;
}

void *high_prio_thread (void *arg) {
    int threshold_ns = *(int *) (arg) * 1000;
    struct timespec initial_time, last_time, now, interval;
    struct timespec list_interval[MEASURES];
    struct timespec list_time[MEASURES];
    struct timespec threshold = {0, threshold_ns};
    int num = 0;
    long i = 0;

    // read the initial time
    clock_gettime (CLOCK_MONOTONIC, &initial_time);

    // read the time
    clock_gettime (CLOCK_MONOTONIC, &last_time);
    while (i < MAX_LOOPS && num < MEASURES) {
        // read the time
        clock_gettime (CLOCK_MONOTONIC, &now);
        // calculate difference
        interval = now;
        decr_timespec (&interval, &last_time);
        // if difference is above threshold, store in list
        if (smaller_timespec (&threshold, &interval)) {
            list_interval[num] = interval;
            list_time[num] = now;
            num++;
            clock_gettime (CLOCK_MONOTONIC, &now);
        }
        i++;
        last_time = now;

    }

    // Process data
    for (i = 1; i < num; i++) {                           // Discard first measure
        decr_timespec (&(list_time[i]), &initial_time);  // Make it relative to the initial time
        printf ("Measure over threshold found (%.1f us) at time %.1f ms\n",
                t2d (list_interval[i]) * 1E6, t2d (list_time[i]) * 1E3); // timespec to double conversion (secs)
    }
    pthread_exit (NULL);
}
