#ifndef PROYECTO_H
#define PROYECTO_H

// ! ----------------------------- INCLUDES ------------------------------ ! //
#include <math.h>
#include <misc/timespec_operations.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "ev3c.h"

// ! ------------------------ PREPROCESSOR VALUES ------------------------ ! //
#define _USE_MATH_DEFINES  // for C
#define DIAMETER_WHEEL 4   // units: cms
#define ALPHA 0.4

// MOTORS PORTS (Motor ports are named using characters)
#define GRAB_DROP_MOTOR_PORT 'A'
#define UP_DOWN_MOTOR_PORT 'B'
#define LEFT_RIGHT_MOTOR_PORT 'C'

// SENSOR PORTS (Sensor ports are named using numbers)
#define PUSH_SENSOR_PORT 2
#define COLOR_SENSOR_PORT 3

#define OPEN_CLOSE_DISTANCE 3
#define SUSPENSION_TIME 2000  // units: usecs

#define SLEEP_DURATION_BUTTONS 400000   // microseconds = 400 ms
#define SLEEP_DURATION_SENSORS 50000    // microseconds = 50 ms
#define SLEEP_DURATION_REPORTER 500000  // microseconds = 500 ms
#define BLINK_DURATION 40000            // microseconds = 40 ms

#define SLEEP_DURATION_ERROR 750000       // microseconds = 750 ms


#define MANUAL_STEP 20            // units: degrees
#define CALIBRATION_STEP 35       // units: degrees
#define CALIBRATION_SLEEP 250000  // microseconds = 250 ms

#define MAX_TIME_GRAB_DROP_MOTOR 350      // units: milliseconds
#define MAX_DEGREES_UP_DOWN_MOTOR 60      // units: degrees
#define MAX_DEGREES_LEFT_RIGHT_MOTOR 180  // units: degrees

#define COLOR_SENSOR_THRESHOLD \
  3  // max value for the color sensor when reaches the top limit

#define CHECK(x)                                                          \
  do {                                                                    \
    int retval = (x);                                                     \
    if (retval != 0) {                                                    \
      fprintf(stderr, "[ ERROR ]\n\t%s returned %d at %s:%d", #x, retval, \
              __FILE__, __LINE__);                                        \
      exit(EXIT_FAILURE);                                                 \
    }                                                                     \
  } while (0)

#define CHECK_NULL(x)                                                         \
  do {                                                                        \
    void *retval = (x);                                                       \
    if (retval == NULL) {                                                     \
      fprintf(stderr, "[ ERROR ]\n\t%s returned NULL at %s:%d", #x, __FILE__, \
              __LINE__);                                                      \
      exit(EXIT_FAILURE);                                                     \
    }                                                                         \
  } while (0)

#define X_1 (EV3_X_LCD / 4) - 10
#define Y_1 (EV3_Y_LCD / 4)
#define X_2 (EV3_X_LCD * 3 / 4) - 10
#define Y_2 (EV3_Y_LCD * 3 / 4)
#define X_3 X_2  // EV3_X_LCD * 3 / 4
#define Y_3 Y_1  // EV3_Y_LCD / 4
#define X_4 X_1  // EV3_X_LCD / 4
#define Y_4 Y_2  // EV3_Y_LCD * 3 / 4

// ! ---------------------------- DATA TYPES ----------------------------- ! //
typedef enum commands_enum {
  RUN_ABS_POS,
  RUN_REL_POS,
  RUN_TIMED,
  STOP
} commands;

typedef enum color_command_enum {
  COL_REFLECT = 0,
  COL_AMBIENT = 1,
  COL_COLOR = 2
} color_command;

typedef enum color_light_enum {
  OFF = 0,
  GREEN = 1,
  RED = 2,
  ORANGE = 3
} color_light;

typedef enum stop_mode_enum { COAST, BRAKE, HOLD } stop_mode;
typedef enum state_button_enum { NOT_PRESSED, PRESSED } state_button;

typedef enum polarity_code_enum {
  POL_UP = -1,
  POL_DROP = -1,
  POL_LEFT = -1,
  POL_DOWN = 1,
  POL_GRAB = 1,
  POL_RIGHT = 1
} polarity_code;

typedef struct {
  // Control the status of motor
  // C = Calibration; W = Wait; M = Moving; S = Stopping; E = Error
  char state;
  int is_hand_open;  // 0 = False && 1 = True
  int buttons_pressed;

  // Sensors values
  int push_sensor_value;  // 0 = False && 1 = True
  int color_sensor_value;

  // Motor values
  int position_up_down;
  int position_left_right;
  int position_grab_drop;
  pthread_mutex_t mutex;
} robot_state;

// ! ------------------------- GLOBAL VARIABLES -------------------------- ! //
char *COMMANDS_STRING[] = {"run-to-abs-pos", "run-to-rel-pos", "run-timed",
                           "stop"};
char *STOP_MODE_STRING[] = {"coast", "brake", "hold"};
const char *BUTTON[] = {"NO_BUTTON",    "BUTTON_LEFT", "BUTTON_UP",
                        "BUTTON_RIGHT", "BUTTON_DOWN", "BUTTON_CENTER",
                        "BUTTON_BACK"};
const int BUTTON_NUMBER = 6;

// GLOBAL VARIABLES FOR MOTORS
ev3_motor_ptr motors = NULL;  // List of motors
ev3_motor_ptr up_down_motor = NULL;
ev3_motor_ptr grab_drop_motor = NULL;
ev3_motor_ptr left_right_motor = NULL;

// GLOBAL VARIABLES FOR SENSORS
ev3_sensor_ptr sensors = NULL;       // List of available sensors
ev3_sensor_ptr push_sensor = NULL;   // *ptr to the push sensor
ev3_sensor_ptr color_sensor = NULL;  // *ptr to the color sensor

int is_hand_open = 0;  // 0 -> closed, 1 -> open

// ! ----------------------------- FUNCTIONS ----------------------------- ! //
/**
 * @brief Function load all the necessary libraries and initializes the
 * sensors and motors of the robot.
 *
 * @return EXIT_SUCCESS if everything went well
 * @return EXIT_FAILURE otherwise.
 */
int setup();

/**
 * @brief Function that closes all the opened sensors and motors.
 *
 */
void cleanup();

/**
 * @brief Function that calibrates the arm of the robot.
 * It moves the arm to the highest position (until the distance sensor detects
 * something at less than 10 cm).
 * Then, it moves the arm to the right (until the button is pressed).
 *
 * @return EXIT_SUCCESS if everything went well
 * @return EXIT_FAILURE otherwise.
 */
int calibrate_arm(robot_state *data);

void reporter_function(void *arg);

void sensors_function(void *arg);

void lights_function();

void motors_function();

void update_status(robot_state *data, char status);

void blink_lights(int color, int times);
void set_color_led(int color);

void move_up(int steps);
void move_down(int steps);
void move_up_down_motor(int direction, int steps);

void move_left_right_motor(int direction, int steps);
void move_left(int steps);
void move_right(int steps);

void grab_drop(robot_state *data);
void move_grab_close(int steps);
void move_drop_open(int steps);
void move_grab_drop_motor(int direction, int steps);

int get_button_pressed();

void print_cross_in_screen();

void print_line_n_times(int32_t x0, int32_t y0, int32_t x1, int32_t y1,
                        int32_t bit, int n);

#endif