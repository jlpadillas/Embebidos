/*
 * File: practica2b.c
 *
 * Description: Muestra el uso del sensor de color
 *
 * Author:  DSSE
 * Version: 1.0
 *
 */

#include <ev3c.h>
#include <stdio.h>

#define COLOR_SENSOR_NOT_AVAILABLE -1
#define COLOR_SENSOR_PORT 1

#define MAX_ITERATIONS 15
#define SLEEP_DURATION 2  // seconds

typedef enum color_command_enum {
  COL_REFLECT = 0,
  COL_AMBIENT = 1,
  COL_COLOR = 2
} color_command;

void turn_off_leds(void) {
  ev3_set_led(LEFT_LED, GREEN_LED, 0);
  ev3_set_led(LEFT_LED, RED_LED, 0);
  ev3_set_led(RIGHT_LED, GREEN_LED, 0);
  ev3_set_led(RIGHT_LED, RED_LED, 0);
}

int main(void) {
  ev3_sensor_ptr sensors = NULL;  //  List of available sensors
  ev3_sensor_ptr color_sensor = NULL;

  int index, data;
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

  // Set mode
  ev3_mode_sensor(color_sensor, COL_COLOR);
  mode = color_sensor->modes[color_sensor->mode];
  printf("Color mode enabled: %s\n", mode);

  // Initializing the leds
  ev3_init_led();
  turn_off_leds();

  // Print color codes
  printf(" > Color: [\t-\tBLACK\tBLUE\tGREEN\tYELLOW\tRED\tWHITE\tBROWN\t]\n");
  printf(" >  Code: [\t0\t1\t2\t3\t4\t5\t6\t7\t]\n");

  for (index = 0; index < MAX_ITERATIONS; index++) {
    ev3_update_sensor_val(color_sensor);
    data = color_sensor->val_data[0].s32;
    printf("Color value = %d\n", data);

    if (data == 3) {
      // GREEN COLOR
      ev3_set_led(LEFT_LED, GREEN_LED, 255);
      ev3_set_led(RIGHT_LED, GREEN_LED, 255);
    } else if (data == 4) {
      // YELLOW COLOR
      ev3_set_led(LEFT_LED, GREEN_LED, 127);
      ev3_set_led(LEFT_LED, RED_LED, 15);
      ev3_set_led(RIGHT_LED, GREEN_LED, 127);
      ev3_set_led(RIGHT_LED, RED_LED, 15);
    } else if (data == 5) {
      // RED COLOR
      ev3_set_led(LEFT_LED, RED_LED, 255);
      ev3_set_led(RIGHT_LED, RED_LED, 255);
    }
    sleep(SLEEP_DURATION);
    turn_off_leds();
  }

  //  Finish & close devices
  printf("\n*** Finishing color sensor application... OK***\n");
  ev3_delete_sensors(sensors);
  ev3_quit_led();

  return 0;
}
