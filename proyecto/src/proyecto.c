/**
 * @file proyecto.c
 * @author Padilla (jps55@alumnos.unican.es)
 * @brief
 * @version 0.1
 * @date 2022-08-20
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "proyecto.h"

int main(int argc, char const *argv[]) {
  // Define the pthreads that we're going to use
  pthread_t reporter_th, sensors_th, motors_th;

  // Create the data to share between the threads
  robot_state *data = (robot_state *)malloc(sizeof(robot_state));

  // Init mutex and all data from the robot_state struct
  CHECK(pthread_mutex_init(&(data->mutex), NULL));
  // No need of using mutex, no threads launched yet
  data->buttons_pressed = 0;
  data->state = '\0';

  // Load and initialize all motors, sensors, screen, lights, etc.
  CHECK(setup());

  // Create and launch all the threads
  CHECK(pthread_create(&reporter_th, NULL, (void *)reporter_function,
                       (void *)data));
  CHECK(pthread_create(&sensors_th, NULL, (void *)sensors_function,
                       (void *)data));
  CHECK(
      pthread_create(&motors_th, NULL, (void *)motors_function, (void *)data));

  // Calibrate the arm
  update_status(data, 'C');
  CHECK(calibrate_arm(data));

  // Set waiting mode
  update_status(data, 'W');

  // Wait

  // ! Finish threads
  CHECK(pthread_join(reporter_th, NULL));
  CHECK(pthread_join(sensors_th, NULL));
  CHECK(pthread_join(motors_th, NULL));

  // ! Close devices
  cleanup();

  return EXIT_SUCCESS;
}

int setup() {
  ev3_init_lcd();                // ! Initialize the LCD
  ev3_init_button();             // ! Initialize the buttons
  ev3_init_led();                // ! Initialize the LEDs
  sensors = ev3_load_sensors();  // ! Load all sensors
  motors = ev3_load_motors();    // ! Load all motors

  // Check if the motors and sensors were loaded correctly
  CHECK_NULL(sensors);
  CHECK_NULL(motors);

  // Initialize sensors and motors by port
  color_sensor = ev3_search_sensor_by_port(sensors, COLOR_SENSOR_PORT);
  push_sensor = ev3_search_sensor_by_port(sensors, PUSH_SENSOR_PORT);
  grab_drop_motor = ev3_search_motor_by_port(motors, GRAB_DROP_MOTOR_PORT);
  up_down_motor = ev3_search_motor_by_port(motors, UP_DOWN_MOTOR_PORT);
  left_right_motor = ev3_search_motor_by_port(motors, LEFT_RIGHT_MOTOR_PORT);

  // Check if the motors and sensors were loaded correctly
  CHECK_NULL(color_sensor);
  CHECK_NULL(push_sensor);
  CHECK_NULL(grab_drop_motor);
  CHECK_NULL(up_down_motor);
  CHECK_NULL(left_right_motor);

  // Open sensor
  color_sensor = ev3_open_sensor(color_sensor);
  push_sensor = ev3_open_sensor(push_sensor);

  // Check if the sensors were opened correctly
  CHECK_NULL(color_sensor);
  CHECK_NULL(push_sensor);

  // Set mode of color sensor to detect ambient brightness
  ev3_mode_sensor(color_sensor, COL_AMBIENT);

  // Reset and then open the motors
  ev3_reset_motor(grab_drop_motor);
  ev3_reset_motor(up_down_motor);
  ev3_reset_motor(left_right_motor);
  ev3_open_motor(grab_drop_motor);
  ev3_open_motor(up_down_motor);
  ev3_open_motor(left_right_motor);

  // Check if the motors were opened correctly
  CHECK_NULL(grab_drop_motor);
  CHECK_NULL(up_down_motor);
  CHECK_NULL(left_right_motor);

  // ! OK ! //
  return EXIT_SUCCESS;
}

void cleanup() {
  ev3_quit_lcd();     // Finishes the lcd functionality
  ev3_quit_button();  // Finishes the work with the buttons
  ev3_quit_led();     // Finishes the work with the LEDs

  // Close sensors
  ev3_close_sensor(color_sensor);
  ev3_close_sensor(push_sensor);
  ev3_delete_sensors(sensors);  // Deletes the linked list of available sensors

  // Close motors
  ev3_close_motor(grab_drop_motor);
  ev3_close_motor(up_down_motor);
  ev3_close_motor(left_right_motor);
  ev3_delete_motors(motors);  // Deletes the linked list of available sensors
}

int calibrate_arm(robot_state *data) {
  int push_sensor_value, color_sensor_value;

  // Move the arm to the highest position
  while (1) {
    if ((push_sensor_value == PRESSED) &&
        (color_sensor_value < COLOR_SENSOR_THRESHOLD)) {
      break;
    }
    // Read the sensor values
    pthread_mutex_lock(&data->mutex);             // Get the mutex
    push_sensor_value = data->push_sensor_value;  // Get the push sensor value
    color_sensor_value =
        data->color_sensor_value;        // Get the color sensor value
    pthread_mutex_unlock(&data->mutex);  // Release the mutex

    if (push_sensor_value == NOT_PRESSED) {
      move_right(CALIBRATION_STEP);
    }

    if (color_sensor_value >= COLOR_SENSOR_THRESHOLD) {
      move_up(CALIBRATION_STEP);
    }
    usleep(CALIBRATION_SLEEP);
  }

  // Open the hand at the top
  move_drop_open(MAX_TIME_GRAB_DROP_MOTOR);
  pthread_mutex_lock(&data->mutex);    // Get the mutex
  data->is_hand_open = 1;              // 0 = False && 1 = True
  pthread_mutex_unlock(&data->mutex);  // Release the mutex

  // Define the positions as zero
  ev3_set_position(up_down_motor, 0);
  ev3_set_position(grab_drop_motor, 0);
  ev3_set_position(left_right_motor, 0);

  // Open and close the claw
  grab_drop(data);
  sleep(1);
  grab_drop(data);

  return EXIT_SUCCESS;
}

void reporter_function(void *arg) {
  robot_state *data = (robot_state *)arg;  // Get the data from the argument
  char state = '\0';
  int i;

  while (1) {
    // Read the state of the robot
    pthread_mutex_lock(&data->mutex);    // Get the mutex
    state = data->state;                 // Get the state
    pthread_mutex_unlock(&data->mutex);  // Release the mutex

    if (state == 'M') {
      blink_lights(GREEN, 5);

    } else if (state == 'C') {
      ev3_clear_lcd();  // Clear screen
      ev3_text_lcd_large(5, 55, "Calibrate");
      blink_lights(ORANGE, 5);

    } else if (state == 'S') {
      ev3_clear_lcd();  // Clear screen
      ev3_text_lcd_large(30, 55, "Bye!");
      blink_lights(RED, 15);
      break;

    } else if (state == 'E') {
      ev3_clear_lcd();  // Clear screen
      // Draw an X in the middle of the screen
      print_line_n_times(X_1, Y_1, X_2, Y_2, 1, 20);
      print_line_n_times(X_3, Y_3, X_4, Y_4, 1, 20);
      blink_lights(RED, 15);
      sleep(1);
      update_status(data, 'W');

    } else if (state == 'W') {
      // Clear screen and print header
      ev3_clear_lcd();
      ev3_text_lcd_large(0, 0, "Padilla's");
      ev3_text_lcd_large(15, 20, "project");
      ev3_line_lcd(0, 40, 178, 40, 1);

      // Print options
      ev3_text_lcd_small(0, 55, "Move the arrows to mo-");
      ev3_text_lcd_small(0, 70, "ve the robot's arm.");
      ev3_text_lcd_small(0, 85, "Center to grab/drop an");
      ev3_text_lcd_small(0, 100, "object.");
      ev3_text_lcd_small(0, 115, "Back to exit.");

      // Waiting status = constant green light
      set_color_led(GREEN);

    } else {
      // Don't do nothing in this state, wait for next iteration
    }
    usleep(SLEEP_DURATION_REPORTER);
  }
}

int get_button_pressed(void) {
  int i, button_pressed = 0;
  for (i = 0; i < BUTTON_NUMBER; i++) {
    if (ev3_button_pressed(i)) {
      button_pressed |= (1 << i);
    }
  }
  return button_pressed;
}

void move_left(int steps) { move_left_right_motor(POL_LEFT, steps); }

void move_right(int steps) { move_left_right_motor(POL_RIGHT, steps); }

void move_left_right_motor(int direction, int steps) {
  ev3_set_polarity(left_right_motor, direction);  // Set polarity

  // Define the stop mode
  ev3_stop_command_motor_by_name(left_right_motor, STOP_MODE_STRING[BRAKE]);

  ev3_set_duty_cycle_sp(left_right_motor, 33);  // Power to the engine

  ev3_set_position_sp(left_right_motor, steps);  // Set position to go

  // Action!
  ev3_command_motor_by_name(left_right_motor, COMMANDS_STRING[RUN_REL_POS]);
  usleep(SUSPENSION_TIME);

  while ((ev3_motor_state(left_right_motor) & MOTOR_RUNNING))
    ;
  usleep(SUSPENSION_TIME);
}

void move_up(int steps) { move_up_down_motor(POL_UP, steps); }

void move_down(int steps) { move_up_down_motor(POL_DOWN, steps); }

void move_up_down_motor(int direction, int steps) {
  ev3_set_polarity(up_down_motor, direction);  // Set polarity

  // Define the stop mode
  ev3_stop_command_motor_by_name(up_down_motor, STOP_MODE_STRING[HOLD]);

  ev3_set_duty_cycle_sp(up_down_motor, 33);  // Power to the engine

  ev3_set_position_sp(up_down_motor, steps);  // Set position to go

  // Action!
  ev3_command_motor_by_name(up_down_motor, COMMANDS_STRING[RUN_REL_POS]);
  usleep(SUSPENSION_TIME);

  while (!(ev3_motor_state(up_down_motor) & MOTOR_HOLDING))
    ;  // Wait until motor action is complete
  usleep(SUSPENSION_TIME);
}

void move_grab_close(int steps) { move_grab_drop_motor(POL_GRAB, steps); }

void move_drop_open(int steps) { move_grab_drop_motor(POL_DROP, steps); }

void grab_drop(robot_state *data) {
  int is_hand_open;  // 0 = False && 1 = True

  // Update the status of the hand
  pthread_mutex_lock(&data->mutex);  // Get the mutex
  is_hand_open = data->is_hand_open;
  pthread_mutex_unlock(&data->mutex);  // Release the mutex

  if (is_hand_open == 0) {
    move_grab_close(MAX_TIME_GRAB_DROP_MOTOR);
  } else {
    move_drop_open(MAX_TIME_GRAB_DROP_MOTOR);
    // Define the position as zero
    ev3_set_position(grab_drop_motor, 0);
  }
  pthread_mutex_lock(&data->mutex);
  data->is_hand_open = !is_hand_open;
  pthread_mutex_unlock(&data->mutex);
}

void move_grab_drop_motor(int direction, int time) {
  ev3_set_polarity(grab_drop_motor, direction);  // Set polarity

  // Define the stop mode
  ev3_stop_command_motor_by_name(grab_drop_motor, STOP_MODE_STRING[HOLD]);
  ev3_set_duty_cycle_sp(grab_drop_motor, 25);  // Power to the engine
                                               // Running time (milliseconds)
  ev3_set_time_sp(grab_drop_motor, time);

  // Action!
  ev3_command_motor_by_name(grab_drop_motor, COMMANDS_STRING[RUN_TIMED]);
  usleep(SUSPENSION_TIME);
  while (!(ev3_motor_state(grab_drop_motor) & MOTOR_HOLDING))
    ;  // Wait until motor action is complete
  usleep(SUSPENSION_TIME);
}

void sensors_function(void *arg) {
  robot_state *data = (robot_state *)arg;  // Get the data from the argument
  float old_data, new_data;
  int filtered_value, first_time = 1;  // 0 = false, 1 = true

  while (1) {
    // Check push sensor
    ev3_update_sensor_val(push_sensor);
    int push_sensor_value = push_sensor->val_data[0].s32;

    // Check color sensor
    ev3_update_sensor_val(color_sensor);
    new_data = color_sensor->val_data[0].s32;

    // Calculate filtered value
    if (first_time) {
      old_data = new_data;
      first_time = 0;
    } else {
      old_data = old_data + (ALPHA * (new_data - old_data));
    }
    filtered_value = (int)old_data;

    // Check if thread has to stop
    pthread_mutex_lock(&data->mutex);  // Get the mutex
    if (data->state == 'S') {
      pthread_mutex_unlock(&data->mutex);  // Release the mutex
      break;
    } else {
      // Mutex still locked, update values
      data->push_sensor_value = push_sensor_value;  // Update the value
      data->color_sensor_value = filtered_value;    // Update the value
      pthread_mutex_unlock(&data->mutex);           // Release the mutex
    }

    usleep(SLEEP_DURATION_SENSORS);  // Sleep for a while
  }
}

void motors_function(void *arg) {
  robot_state *data = (robot_state *)arg;  // Get the data from the argument
  int button_pressed;
  int push_sensor_value;  // 0 = False && 1 = True
  int color_sensor_value;
  // Current positions of the motors
  int position_up_down;
  int position_left_right;
  int position_grab_drop;

  // ! Motor's thread will listen buttons and move motors accordingly
  while (1) {
    pthread_mutex_lock(&data->mutex);  // ! Wait and lock the mutex

    // ! Update the buttons pressed
    data->buttons_pressed = get_button_pressed();

    button_pressed = data->buttons_pressed;
    push_sensor_value = data->push_sensor_value;
    color_sensor_value = data->color_sensor_value;
    position_up_down = data->position_up_down;
    position_left_right = data->position_left_right;
    position_grab_drop = data->position_grab_drop;

    pthread_mutex_unlock(&data->mutex);  // ! Unlock the mutex

    // Check bit by bit, applying a mask, if each button is pressed

    if ((button_pressed >> BUTTON_LEFT) & 0x1) {
      if (position_left_right < MAX_DEGREES_LEFT_RIGHT_MOTOR) {
        update_status(data, 'M');
        move_left(MANUAL_STEP);

        pthread_mutex_lock(&data->mutex);
        data->position_left_right += MANUAL_STEP;
        pthread_mutex_unlock(&data->mutex);

        update_status(data, 'W');
      } else {
        update_status(data, 'E');
        sleep(SLEEP_DURATION_ERROR);
      }
    }

    if ((button_pressed >> BUTTON_UP) & 0x1) {
      if (color_sensor_value > COLOR_SENSOR_THRESHOLD ||
          position_up_down > 0) {
        update_status(data, 'M');
        move_up(MANUAL_STEP);

        pthread_mutex_lock(&data->mutex);
        data->position_up_down -= MANUAL_STEP;
        pthread_mutex_unlock(&data->mutex);

        // Movement have been performed, change state to Waiting
        update_status(data, 'W');
      } else {
        // Limit reached, Error status and do nothing
        update_status(data, 'E');
        sleep(SLEEP_DURATION_ERROR);
      }
    }

    if ((button_pressed >> BUTTON_RIGHT) & 0x1) {
      if (push_sensor_value == NOT_PRESSED) {
        update_status(data, 'M');
        move_right(MANUAL_STEP);

        pthread_mutex_lock(&data->mutex);
        data->position_left_right -= MANUAL_STEP;
        pthread_mutex_unlock(&data->mutex);

        // Movement have been performed, change state to Waiting
        update_status(data, 'W');
      } else {
        // Limit reached, Error status and update the position as 0
        update_status(data, 'E');
        ev3_set_position(left_right_motor, 0);

        pthread_mutex_lock(&data->mutex);
        data->position_left_right = 0;
        pthread_mutex_unlock(&data->mutex);

        sleep(SLEEP_DURATION_ERROR);
      }
    }

    if ((button_pressed >> BUTTON_DOWN) & 0x1) {
      if (position_up_down < MAX_DEGREES_UP_DOWN_MOTOR) {
        update_status(data, 'M');
        move_down(MANUAL_STEP);

        pthread_mutex_lock(&data->mutex);
        data->position_up_down += MANUAL_STEP;
        pthread_mutex_unlock(&data->mutex);

        update_status(data, 'W');
      } else {
        update_status(data, 'E');
        sleep(SLEEP_DURATION_ERROR);
      }
    }

    if ((button_pressed >> BUTTON_CENTER) & 0x1) {
      update_status(data, 'M');
      grab_drop(data);
      update_status(data, 'W');
    }

    if ((button_pressed >> BUTTON_BACK) & 0x1) {
      update_status(data, 'S');  // Stop the program

      break;
    }

    usleep(SLEEP_DURATION_BUTTONS);  // Do nothing
  }
}

void update_status(robot_state *data, char status) {
  // Update the status of the robot
  pthread_mutex_lock(&data->mutex);    // Get the mutex
  data->state = status;                // Set the status flag
  pthread_mutex_unlock(&data->mutex);  // Release the mutex
}

void blink_lights(int color, int times) {
  int i;
  for (i = 0; i < times; i++) {
    set_color_led(color);
    set_color_led(OFF);
  }
}

void set_color_led(int color) {
  // Turn off the LEDs
  ev3_set_led(LEFT_LED, GREEN_LED, 0);
  ev3_set_led(LEFT_LED, RED_LED, 0);
  ev3_set_led(RIGHT_LED, GREEN_LED, 0);
  ev3_set_led(RIGHT_LED, RED_LED, 0);

  if (color == GREEN) {
    ev3_set_led(LEFT_LED, GREEN_LED, 255);
    ev3_set_led(RIGHT_LED, GREEN_LED, 255);
  } else if (color == RED) {
    ev3_set_led(LEFT_LED, RED_LED, 255);
    ev3_set_led(RIGHT_LED, RED_LED, 255);
  } else if (color == ORANGE) {
    ev3_set_led(LEFT_LED, GREEN_LED, 127);
    ev3_set_led(LEFT_LED, RED_LED, 15);
    ev3_set_led(RIGHT_LED, GREEN_LED, 127);
    ev3_set_led(RIGHT_LED, RED_LED, 15);
  }
  usleep(BLINK_DURATION);
}

void print_line_n_times(int32_t x0, int32_t y0, int32_t x1, int32_t y1,
                        int32_t bit, int n) {
  int i;
  for (i = 0; i < n; i++) {
    ev3_line_lcd(x0 + i, y0, x1 + i, y1, bit);
  }
}