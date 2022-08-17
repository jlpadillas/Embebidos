/*
 * File: practica2c.c
 *
 * Description: Muestra el uso del sensor de color
 *
 * Author:  DSSE
 * Version: 1.0
 *
 */

#include <ev3c.h>
#include <math.h>
#include <stdio.h>

#define COLOR_SENSOR_NOT_AVAILABLE -1
#define COLOR_SENSOR_PORT 1

#define MAX_ITERATIONS 15
#define SLEEP_DURATION 500000  // microseconds

// #define ALPHA 1
#define ALPHA 0.3

typedef enum color_command_enum {
  COL_REFLECT = 0,
  COL_AMBIENT = 1,
  COL_COLOR = 2
} color_command;

float lowpass_filter(float new_data, float old_data) {
  static int first_time = 1;  // 1 -> True && 0 -> False
  if (first_time == 1) {
    old_data = new_data;
    first_time = 0;
  }
  return old_data + (ALPHA * (new_data - old_data));
}

/**
 * @brief Check whether a button is pressed
 *
 * This function checks whether a button is pressed and returns the
 * corresponding button id
 *
 * @return the button id pressed, -1 otherwise
 */
int is_button_pressed(void);

int main(void) {
  ev3_sensor_ptr sensors = NULL;  //  List of available sensors
  ev3_sensor_ptr color_sensor = NULL;

  int index, data, button_pressed;
  float old_data;
  char *mode;

  // Loading all sensors
  sensors = ev3_load_sensors();
  if (sensors == NULL) {
    printf("Error on ev3_load_sensors\n");
    return COLOR_SENSOR_NOT_AVAILABLE;
  }

  // Get color sensor by port
  color_sensor = ev3_search_sensor_by_port(sensors, COLOR_SENSOR_PORT);
  if (color_sensor == NULL) {
    printf("Error on ev3_search_sensor_by_port\n");
    return COLOR_SENSOR_NOT_AVAILABLE;
  }

  // Init sensor
  color_sensor = ev3_open_sensor(color_sensor);
  if (color_sensor == NULL) {
    printf("Error on ev3_open_sensor\n");
    return COLOR_SENSOR_NOT_AVAILABLE;
  }

  // Init button
  ev3_init_button();

  // Set mode
  ev3_mode_sensor(color_sensor, COL_AMBIENT);
  mode = color_sensor->modes[color_sensor->mode];
  printf("Color mode enabled: %s\n", mode);

  // Read the value for first time
  ev3_update_sensor_val(color_sensor);
  old_data = (float)(color_sensor->val_data[0].s32);

  // Print header
  printf("Ambient light intensity (dark(0) to light(100))\n");

  // Now with the loop
  while ((button_pressed = is_button_pressed()) != BUTTON_BACK) {
    ev3_update_sensor_val(color_sensor);   // Update value
    data = color_sensor->val_data[0].s32;  // Get value
    printf(" > Read = %d\t", data);
    old_data = lowpass_filter((float)data, old_data);
    printf("Filtered = %f\t", old_data);
    data = round(old_data);
    printf("New data = %d\n", data);
    usleep(SLEEP_DURATION);
  }

  //  Finish & close devices
  printf("\n*** Finishing and closing devices...\n");
  ev3_quit_button();
  ev3_delete_sensors(sensors);

  return 0;
}

int is_button_pressed(void) {
  int i, button_pressed = -1;
  int32_t value;

  for (i = BUTTON_LEFT; i <= BUTTON_BACK; i++) {
    value = ev3_button_pressed(i);
    if (value == 1) {
      button_pressed = i;
      break;
    }
  }
  return button_pressed;
}