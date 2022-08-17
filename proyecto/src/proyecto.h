#ifndef PROYECTO_H
#define PROYECTO_H

// ! ----------------------------- INCLUDES ------------------------------ ! //
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "ev3c.h"

// ! ------------------------ PREPROCESSOR VALUES ------------------------ ! //
#define _USE_MATH_DEFINES  // for C
#define DIAMETER_WHEEL 4   // units: cms

// MOTORS PORTS (Motor ports are named using characters)
#define GRAB_DROP_MOTOR_PORT 'A'
#define UP_DOWN_MOTOR_PORT 'B'
#define LEFT_RIGHT_MOTOR_PORT 'C'

// SENSOR PORTS (Sensor ports are named using numbers)
#define PUSH_SENSOR_PORT 1
#define COLOR_SENSOR_PORT 3

#define OPEN_CLOSE_DISTANCE 3
#define SUSPENSION_TIME 2000  // units: usecs

#define SLEEP_DURATION_BUTTONS 450000  // microseconds = 500 ms

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

typedef enum stop_mode_enum { COAST, BRAKE, HOLD } stop_mode;
typedef enum state_button_enum { NOT_PRESSED, PRESSED } state_button;

typedef struct {
  int up;
} motor_up_down_struct;

typedef enum polarity_code_enum { POL_DOWN = -1, POL_UP = 1 } polarity_code;

typedef struct {
  int up;
  char mode;  // 'A' -> AUTOMATIC, 'M' -> MANUAL, 'I' -> IDLE ON MENU
  pthread_mutex_t mutex;
} robot_state;

// typedef struct {
//   char *buttons;
//   int buttons_pressed[BUTTON_NUMBER];  // 0 -> False/Not pushed && 1 -> True/Pushed
//   // pthread_mutex_t mutex;
// } buttons_pressed;

// ! ------------------------- GLOBAL VARIABLES -------------------------- ! //
char *COMMANDS_STRING[] = {"run-to-abs-pos", "run-to-rel-pos", "run-timed",
                           "stop"};

// char *COMMANDS_STRING[] = {"run-forever", "run-to-abs-pos", "run-to-rel-pos",
//                            "run-timed",   "run-direct",     "stop",
//                            "reset"};

char *STOP_MODE_STRING[] = {"coast", "brake", "hold"};

const char *BUTTON[] = {"NO_BUTTON",    "BUTTON_LEFT", "BUTTON_UP",
                        "BUTTON_RIGHT", "BUTTON_DOWN", "BUTTON_CENTER",
                        "BUTTON_BACK"};
const int BUTTON_NUMBER = 6;

// What should be printed on the LCD screen
const char *menu_text[] = {"1. Automatic", "2.    Manual", "3.      Exit"};
const int menu_text_size = 3;

// GLOBAL VARIABLES FOR MOTORS
ev3_motor_ptr grab_drop_motor = NULL;
ev3_motor_ptr up_down_motor = NULL;
ev3_motor_ptr left_right_motor = NULL;

// GLOBAL VARIABLES FOR SENSORS
ev3_sensor_ptr push_sensor = NULL;   // *ptr to the push sensor
ev3_sensor_ptr color_sensor = NULL;  // *ptr to the color sensor

// ! ----------------------------- FUNCTIONS ----------------------------- ! //
void setup();

void checks();
void calibration();

void move_up();
void move_down();
void move_left();
void move_right();

void object_grab();
void object_drop();

void print_menu();
int is_button_pressed();
void manual();
void automatic();

#endif