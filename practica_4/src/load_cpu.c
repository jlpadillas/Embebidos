/*
 * File: load_cpu.c
 *
 * Description: Ejecuta una carga simulada en el sistema durante el tiempo indicado
 *              y a la prioridad elegida
 *
 * Author:  DSSE
 * Version: 1.0
 *
*/

#include <stdio.h>
#include <stdlib.h>  // atoi function
#include <pthread.h>
#include <sched.h>

#include "addons/ev3c-addons.h"

int main (int argc, char *argv[]) {
    int load, seconds = 0;
    struct sched_param param;

    if (argc != 4) {
        perror ("Usage: sudo load_cpu <priority> <load> <seconds>\n");
        return EXIT_FAILURE;
    }

    //  Process args
    param.sched_priority = atoi (argv[1]);
    load = atoi (argv[2]);                  // Load specified as percentage
    seconds = atoi (argv[3]);

    //  Set priority and policy
    if (sched_setscheduler (0, SCHED_FIFO, &param) != 0) {
        perror ("Error in sched_setscheduler");
        return EXIT_FAILURE;
    }
    // Percentage symbol expressed with double %
    printf ("Running %d%% load in CPU for %d seconds\n", load, seconds);
    int policy = SCHED_FIFO;
    if (pthread_getschedparam (pthread_self(), &policy, &param) != 0) {
        perror ("Error in pthread_getschedparam");
        return EXIT_FAILURE;
    }
    printf ("Thread priority = %d\n", param.sched_priority);

    // Load CPU
    do_some_job (load, seconds);

    return EXIT_SUCCESS;
}
