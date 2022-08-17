/*
 * File: yield.c
 *
 * Description: Medida del tiempo de contexto en la plataforma
 *
 * Author:  DSSE
 * Version: 1.0
 *
*/

#include <stdio.h>
#include <time.h>
#include <pthread.h>
#include <sched.h>
#include <stdbool.h>
#include <stdlib.h>

#include <misc/timespec_operations.h>

#define MEASURES        500000

bool volatile flag  = false;            // Synchronization flag
int volatile  num   = 0;                // Loop counter

struct timespec end_time, begin_time;   // Temp variables
struct timespec list_time[MEASURES];    // Buffer to store measures

// Prototypes
/**
 * @brief Process the data stored in the buffer.
 * It outputs the min, max and avg values.
 */
void show_results (void);

/**
 * @brief Code to set the start time and yield the cpu
 * @param arg: none
 * @returns pthread_exit
 */
void *high_prio_thread_begin (void *arg);

/**
 * @brief Code to set the end time and yield the cpu
 * @param arg: none
 * @returns pthread_exit
 */
void *high_prio_thread_end (void *arg);

// Main program that creates two threads
int main() {
    pthread_t t1, t2;
    struct sched_param sch_param;
    pthread_attr_t attr;

    // Set the priority of the main program to max_prio
    sch_param.sched_priority = (sched_get_priority_max (SCHED_FIFO));
    if (pthread_setschedparam (pthread_self(), SCHED_FIFO, &sch_param) != 0) {
        printf ("Error while setting main thread's priority\n");
        return EXIT_FAILURE;
    }

    // Create the thread attributes object
    if (pthread_attr_init (&attr) != 0)
    {
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

    // Set the priority of thread 2 to max
    sch_param.sched_priority = (sched_get_priority_max (SCHED_FIFO));
    if (pthread_attr_setschedparam (&attr, &sch_param) != 0) {
        printf ("Error in schedparam attribute\n");
        return EXIT_FAILURE;
    }

    // create thread 2 with the attributes specified in attr
    if (pthread_create (&t2, &attr, high_prio_thread_end, NULL) != 0) {
        printf ("Error when creating thread 2\n");
    }

    // Set the priority of thread 1 to max_prio
    // the other attributes remain the same for both threads
    sch_param.sched_priority = (sched_get_priority_max (SCHED_FIFO));
    if (pthread_attr_setschedparam (&attr, &sch_param) != 0) {
        printf ("Error in schedparam attribute\n");
        return EXIT_FAILURE;
    }

    // create thread 1 with the attributes specified in attr
    if (pthread_create (&t1, &attr, high_prio_thread_begin, NULL) != 0) {
        printf ("Error when creating thread 1\n");
        return EXIT_FAILURE;
    }

    // wait until high prio threads finish
    pthread_join (t1, NULL);
    pthread_join (t2, NULL);
    return EXIT_SUCCESS;
}

void show_results (void) {
    int i;
    double acc = 0;
    double min = 1E9;
    double max = 0;
    double current_measure;

    for (i = 1; i < num; i++) {                 // Discard first measure
        current_measure = t2d (list_time[i]);   // timespec to double conversion
        acc += current_measure;
        if (current_measure > max) {
            max = current_measure;
        }
        if (current_measure < min) {
            min = current_measure;
        }
    }
    printf ("Context Switch Metrics\n \tMin: %gus \tMax: %gus \tAvg: %gus \n",
            min * 1E6, max * 1E6, (acc / (num - 1)) * 1E6);
}

void *high_prio_thread_begin (void *arg) {

    sched_yield();                      // Initial synchronization
    while (num < MEASURES) {
        flag = true;
        // read the time
        clock_gettime (CLOCK_MONOTONIC, &begin_time);
        sched_yield();
        num++;
    }
    pthread_exit (NULL);
}

void *high_prio_thread_end (void *arg) {

    while (!flag) {                     // Initial synchronization
        sched_yield();
    }

    while (num < MEASURES) {
        if (!flag) {
            sched_yield();
        }
        clock_gettime (CLOCK_MONOTONIC, &list_time[num]);
        // calculate difference
        decr_timespec (&list_time[num], &begin_time);
        flag = false;
    }

    // Process stored data
    show_results();
    pthread_exit (NULL);
}
