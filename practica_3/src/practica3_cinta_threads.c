/*
 * File: practica3.c
 *
 * Description: Muestra el uso de los motores
 *
 * Author:  DSSE
 * Version: 1.0
 *
*/

#define _USE_MATH_DEFINES // for C

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "misc/error_checks.h"
#include "ev3c.h"

#define BELT_MOTOR_PORT 'A'          // Motor ports are named using characters
#define FULL_SPEED_LARGE_MOTOR 900   // units: deg/seg
#define FULL_SPEED_MEDIUM_MOTOR 1200 // units: deg/seg
#define SUSPENSION_TIME 2000         // units: usecs
#define DIAMETER_WHEEL 4             // units: cms
#define MAX_SIZE_BYTES 24
#define TOUCH_SENSOR_PORT 1
#define TOUCH_SENSOR_NOT_AVAILABLE -1
#define NUM_STOPS 3 // number of stops

typedef enum stop_mode_enum
{
    COAST,
    BRAKE,
    HOLD
} stop_mode;
static char *STOP_MODE_STRING[] = {"coast", "brake", "hold"};

typedef enum commands_enum
{
    RUN_ABS_POS,
    RUN_REL_POS,
    RUN_TIMED,
    STOP
} commands;
static char *COMMANDS_STRING[] = {"run-to-abs-pos", "run-to-rel-pos",
                                  "run-timed", "stop"};

typedef enum state_button_enum
{
    NOT_PRESSED,
    PRESSED
} state_button;

int main(void)
{
    //  Regular variables
    int index = 1;                 // Loop variable
    ev3_motor_ptr motors = NULL;   // List of motors
    ev3_sensor_ptr sensors = NULL; // List of available sensors
    ev3_sensor_ptr push_sensor = NULL;
    double stop_distances[NUM_STOPS] = {7.5, 17.0, 24.0}; // List of distances to stop
    double circumference = DIAMETER_WHEEL * M_PI;

    // Loading all motors
    motors = ev3_load_motors();
    if (motors == NULL)
    {
        printf("Error on ev3_load_motors\n");
        return EXIT_FAILURE;
    }
    // Get the target motor
    ev3_motor_ptr belt_motor = ev3_search_motor_by_port(motors, BELT_MOTOR_PORT);
    if (belt_motor == NULL)
    {
        printf("Error on ev3_search_motor_by_port\n");
        return EXIT_FAILURE;
    }

    // Loading all sensors
    sensors = ev3_load_sensors();
    if (sensors == NULL)
    {
        printf("Error on ev3_load_sensors\n");
        return TOUCH_SENSOR_NOT_AVAILABLE;
    }
    // Get touch sensor by port
    push_sensor = ev3_search_sensor_by_port(sensors, TOUCH_SENSOR_PORT);
    if (push_sensor == NULL)
    {
        printf("Error on ev3_search_sensor_by_port\n");
        return TOUCH_SENSOR_NOT_AVAILABLE;
    }
    // Init sensor
    push_sensor = ev3_open_sensor(push_sensor);
    if (push_sensor == NULL)
    {
        printf("Error on ev3_open_sensor\n");
        return TOUCH_SENSOR_NOT_AVAILABLE;
    }

    // Init motor
    ev3_reset_motor(belt_motor);
    ev3_open_motor(belt_motor);

    printf("*** Practica 3: Cinta transportadora ***\n");

    // The main thread loops until the push button is pressed
    while (1)
    {
        ev3_update_sensor_val(push_sensor);
        if (push_sensor->val_data[0].s32 == PRESSED)
        {
            break;
        }
        usleep(SUSPENSION_TIME * 25); // 50 msecs
    }

    // Start the movement
    // Motor configuration
    // ev3_stop_command_motor(belt_motor, BRAKE);
    ev3_stop_command_motor_by_name (belt_motor, STOP_MODE_STRING[BRAKE]);
    ev3_set_duty_cycle_sp(belt_motor, 25); // Power to the engine
    ev3_set_position(belt_motor, 0);
    printf("Start position %d (degrees)\n", ev3_get_position(belt_motor));

    double distance_traveled = 0.0;
    double distance_to_move = 0.0;
    // RUN_ABS_POS
    for (index = 0; index < NUM_STOPS; index++)
    {
        distance_to_move = 360 * stop_distances[index] / circumference;
        distance_traveled += stop_distances[index];

        int round_distance_to_move = round(distance_to_move);

        printf("\tDistance traveled: %.2f cms\tGrades to move: %.4fº (%dº)\n",
               distance_traveled, distance_to_move, round_distance_to_move);

        ev3_set_position_sp(belt_motor, round_distance_to_move);
        ev3_command_motor_by_name(belt_motor, COMMANDS_STRING[RUN_ABS_POS]); // Action!
        usleep(SUSPENSION_TIME);
        while ((ev3_motor_state(belt_motor) & MOTOR_RUNNING))
            ;
        sleep(2);
        printf("Position[%d]: %d (degrees)\n", index, ev3_get_position(belt_motor));
    }

    // Let's reset and close the motors
    ev3_set_position_sp(belt_motor, 0);
    ev3_command_motor_by_name(belt_motor, COMMANDS_STRING[RUN_ABS_POS]);
    usleep(SUSPENSION_TIME);
    while ((ev3_motor_state(belt_motor) & MOTOR_RUNNING))
        ;
    printf("Final position %d (degrees)\n", ev3_get_position(belt_motor));
    // sleep(2);

    // ! Finish & close devices
    ev3_reset_motor(belt_motor);
    ev3_delete_motors(motors);
    ev3_delete_sensors(sensors);
    return EXIT_SUCCESS;
}
