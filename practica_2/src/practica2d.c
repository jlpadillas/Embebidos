/*
 * File: practica2d.c
 *
 * Description: Muestra el uso del sensor de color
 * Concurrente y maneja datos compartidos.
 *
 * Thread principal:
 *   - Identifica los botones pulsados:
 *     + Botón BUTTON_BACK -> Salida
 *   - Una vez pulsado un botón, el thread principal espera a que el
 *     resto de threads terminen.
 *   - Muestra mensaje de finalización
 *
 * 2nd thread:
 *   - Lee datos del sensor y aplica el filtro de paso bajo.
 *   - Almacena el valor en una variable compartida (uso de mutex).
 *
 * 3rd thread (reportero):
 *   - Muestra por pantalla:
 *     + Botón pulsado o no pulsado
 *     + Valor del sensor de color
 *     + Modo de funcionamiento del sensor de color
 *   - Lee el valor de la variable compartida (uso de mutex).
 *
 * Author:  DSSE
 * Version: 1.0
 *
 */
// ! ----------------------- INCLUDES ----------------------- ! //
#include <ev3c.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>

// ! ------------------------ PREPROCESSOR VALUES ------------------------ ! //
#define COLOR_SENSOR_NOT_AVAILABLE -1
#define COLOR_SENSOR_PORT 1
#define SLEEP_DURATION_READER 50000     // microseconds = 50 ms
#define SLEEP_DURATION_MAIN 500000      // microseconds = 500 ms
#define SLEEP_DURATION_REPORTER 850000  // microseconds = 850 ms
#define THREADS_NUMBER 2
#define ALPHA 0.3
#define MAX_LENGTH_MSG 100

// ! ----------------------- GLOBAL VARIABLES ----------------------- ! //
ev3_sensor_ptr color_sensor = NULL;
int first_time = 1;  // 1 -> True && 0 -> False
char *mode;

const char *BUTTON[] = {"NO_BUTTON",    "BUTTON_LEFT", "BUTTON_UP",
                        "BUTTON_RIGHT", "BUTTON_DOWN", "BUTTON_CENTER",
                        "BUTTON_BACK"};

typedef enum color_command_enum {
  COL_REFLECT = 0,
  COL_AMBIENT = 1,
  COL_COLOR = 2
} color_command;

typedef struct {
  pthread_mutex_t mutex;
  int button_pressed;
  int filtered_value;
  int end_program;  // 0 -> False && 1 -> True
} shared_data;

// ! ----------------------- FUNCTIONS ----------------------- ! //
void read_and_filter_data(void *arg);

void reporter(void *arg);

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
  int i;

  // Setting up the arrays of threads
  pthread_t threads[THREADS_NUMBER];

  // Creating the attribute of the mutex
  pthread_mutexattr_t mutexattr;
  pthread_mutexattr_init(&mutexattr);

  // Create the data to share between the threads
  // shared_data data;
  shared_data *data = (shared_data *)malloc(sizeof(shared_data));

  // Init data
  if ((pthread_mutex_init(&(data->mutex), &mutexattr)) < 0) {
    fprintf(stderr, "[ERROR] Mutex couldn't be created.\n");
    return EXIT_FAILURE;
  }
  data->end_program = 0;  // 0 -> False && 1 -> True
  data->button_pressed = -1;
  data->filtered_value = 0;

  // Create and set up the attribute for the threads
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  //  List of available sensors
  ev3_sensor_ptr sensors = NULL;
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

  // Init button and sensor
  ev3_init_button();
  color_sensor = ev3_open_sensor(color_sensor);
  if (color_sensor == NULL) {
    printf("Error on ev3_open_sensor\n");
    return COLOR_SENSOR_NOT_AVAILABLE;
  }

  // Set mode
  ev3_mode_sensor(color_sensor, COL_AMBIENT);
  mode = color_sensor->modes[color_sensor->mode];
  // It's constant over all time. There's no need to use it with a mutex.

  // Creating the threads
  // Thread #1 -> Lee datos de color y los filtra
  if ((pthread_create(&threads[0], &attr, (void *)read_and_filter_data,
                      (void *)data)) < 0) {
    fprintf(stderr, "[ERROR] Thread #1 couldn't be created.\n");
    exit(EXIT_FAILURE);
  }
  // Thread #2 -> Imprime toda la información de los dispositivos
  if ((pthread_create(&threads[1], &attr, (void *)reporter, (void *)data)) <
      0) {
    fprintf(stderr, "[ERROR] Thread #2 couldn't be created.\n");
    exit(EXIT_FAILURE);
  }

  // The main thread loops until the chosen button is pressed
  while (1) {
    // Wait and lock the mutex
    pthread_mutex_lock(&data->mutex);
    // Read the button pressed and check if the program should end
    if ((data->button_pressed = is_button_pressed()) == BUTTON_BACK) {
      // The button was pressed, time to notify to the other threads
      data->end_program = 1;               // 0 -> False && 1 -> True
      pthread_mutex_unlock(&data->mutex);  // Unlock the mutex
      break;                               // Exit the loop
    }
    // Unlock the mutex and wait for the next iteration
    pthread_mutex_unlock(&data->mutex);
    // printf("[INFO] [master] Waiting for the next iteration...\n");
    usleep(SLEEP_DURATION_MAIN);
  }

  // Waiting for the threads to end
  for (i = 0; i < THREADS_NUMBER; i++) {
    // if (pthread_join(threads[i], (void **) &resultado) != 0) {
    if (pthread_join(threads[i], NULL) != 0) {
      fprintf(stderr, "[ERROR] Thread #%d return an error with the join.\n", i);
      return EXIT_FAILURE;
    }
  }

  // Free resources, finish & close devices
  pthread_attr_destroy(&attr);
  ev3_quit_button();
  ev3_delete_sensors(sensors);

  printf("\n*** Finishing and closing devices...\n");
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

float lowpass_filter(float new_data, float old_data) {
  if (first_time == 1) {
    old_data = new_data;
    first_time = 0;
  }
  return (old_data + (ALPHA * (new_data - old_data)));
}

void reporter(void *arg) {
  // Cast the param to a shared data
  shared_data *val = (shared_data *)arg;
  // Variable that will store the message to be printed
  char message[MAX_LENGTH_MSG] = {0};

  // Infinite loop until the end_program is modified
  while (1) {
    // Lock the mutex for reading values
    pthread_mutex_lock(&val->mutex);
    // Read variables, create the message and print it
    snprintf(message, MAX_LENGTH_MSG, "Button: %14s. Sensor: %3d. Mode: %s\n",
             BUTTON[val->button_pressed + 1], val->filtered_value, mode);
    printf("%s", message);  // Print the message on the screen
    if (val->end_program == 1) {
      // Unlock the mutex before exiting the loop
      pthread_mutex_unlock(&val->mutex);
      return;  // break
    }
    pthread_mutex_unlock(&val->mutex);  // Unlock the mutex

    usleep(SLEEP_DURATION_REPORTER);  // Wait for the next iteration
  }
}

void read_and_filter_data(void *arg) {
  // Cast the param to a shared data
  shared_data *val = (shared_data *)arg;
  float old_data, new_data;
  int filtered_value;

  // Infinite loop until the end_program is modified
  while (1) {
    // Read sensor and get final value
    ev3_update_sensor_val(color_sensor);
    new_data = color_sensor->val_data[0].s32;
    old_data = lowpass_filter(new_data, old_data);
    filtered_value = round(old_data);
    // Lock the mutex for reading/writing values
    pthread_mutex_lock(&val->mutex);
    if (val->end_program == 1) {
      // Unlock the mutex before exiting the loop
      pthread_mutex_unlock(&val->mutex);
      return;  // break
    }
    // The mutex keeps locked for updating the value
    val->filtered_value = filtered_value;
    pthread_mutex_unlock(&val->mutex);

    usleep(SLEEP_DURATION_READER);  // Wait for the next iteration
  }
}
