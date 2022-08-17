/**
 * @file proyecto.c
 * @author Juan Luis Padilla Salome
 * @brief Program that grabs an object (on the left side)
 *     and moves it to the right. Then release.
 * @version 0.1
 * @date 2022-01-15
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "proyecto.h"

// Consideraciones:
// ================
//   Sensores:
//     - Botón de pulsar (puerto 1) que indica el máximo de giro permitido.
//     - Sensor de distancia (pto 3) que indica cuando se llega a la alt. máx.
//
//   Motores:
//     - El de agarre (puerto A). Encargado de coger/soltar el objeto.
//     - El de subir/bajar el brazo (pto B).
//     - El de mover el brazo a izq/dcha (pto C).

// Fases:
// ======
// Son 3 fases principales:

//   0. Se ejecuta el programa.
//        El objeto está listo: a la izquierda del todo.
//        El brazo está en una posición desconocida.
//        La altura se desconoce.
//        El agarre también se desconoce.
//   1. Calibración:
//        1.1. Hay que abrir el agarre.
//        1.2. Hay que subir el brazo.
//        1.3. Hay que mover a la derecha del todo. Luego a la izquierda una
//               determinada distancia que se supone conocida.
//   2. Movimiento:
//        2.1. Se baja el brazo a tope (conocer la distancia de subida/bajada
//               máxima)
//        2.2. Se cierra el agarre.
//        2.3. Se sube el brazo.
//        2.4. Se mueve a la derecha del todo (hasta tocar el botón).
//        2.5. Se baja el brazo.
//        2.6. Se abre el agarre.
//        2.7. Se sube el brazo.
//        2.8. Se mueve a su posición inicial.

// Crear tantos threads como dispositivos externos tengo:
//  - Botón de pulsar.
//  - Sensor de distancia.
//  - Motor de agarre.
//  - Motor de subir/bajar el brazo.
//  - Motor de mover el brazo a izq/dcha.
//  - Reporter.

int main(int argc, char const* argv[]) {
  // ! ------------------- INIT ALL SENSORS AND MOTORS ------------------- ! //
  ev3_sensor_ptr sensors = NULL;  // List of available sensors
  sensors = ev3_load_sensors();   // Loading all sensors
  if (sensors == NULL) {
    printf("Error on ev3_load_sensors\n");
    return EXIT_FAILURE;
  }

  ev3_motor_ptr motors = NULL;  // List of motors
  motors = ev3_load_motors();   // Loading all motors
  if (motors == NULL) {
    printf("Error on ev3_load_motors\n");
    return EXIT_FAILURE;
  }

  ev3_init_lcd();     // Initialize the LCD
  ev3_init_button();  // Initialize the buttons

  // ! ------------------------ INIT COLOR SENSOR ------------------------ ! //
  // Get sensor by port
  color_sensor = ev3_search_sensor_by_port(sensors, COLOR_SENSOR_PORT);
  if (color_sensor == NULL) {
    printf("Error on ev3_search_sensor_by_port: color_sensor\n");
    return EXIT_FAILURE;
  }
  // Open sensor
  color_sensor = ev3_open_sensor(color_sensor);
  if (color_sensor == NULL) {
    printf("Error on ev3_open_sensor: color_sensor\n");
    return EXIT_FAILURE;
  }
  // Set mode
  ev3_mode_sensor(color_sensor, COL_AMBIENT);

  // ! ------------------------ INIT PUSH SENSOR ------------------------- ! //
  // Get sensor by port
  push_sensor = ev3_search_sensor_by_port(sensors, PUSH_SENSOR_PORT);
  if (push_sensor == NULL) {
    printf("Error on ev3_search_sensor_by_port: push_sensor\n");
    return EXIT_FAILURE;
  }
  // Open sensor
  push_sensor = ev3_open_sensor(push_sensor);
  if (push_sensor == NULL) {
    printf("Error on ev3_open_sensor: push_sensor\n");
    return EXIT_FAILURE;
  }

  // ! ---------------------- INIT GRAB-DROP MOTOR ----------------------- ! //
  // Get motor by port
  grab_drop_motor = ev3_search_motor_by_port(motors, GRAB_DROP_MOTOR_PORT);
  if (grab_drop_motor == NULL) {
    printf("Error on ev3_search_motor_by_port\n");
    return EXIT_FAILURE;
  }
  // First, reset and then open the motor
  ev3_reset_motor(grab_drop_motor);
  ev3_open_motor(grab_drop_motor);
  if (grab_drop_motor == NULL) {
    printf("Error on ev3_open_motor\n");
    return EXIT_FAILURE;
  }

  // ! ----------------------- INIT UP-DOWN MOTOR ------------------------ ! //
  up_down_motor = ev3_search_motor_by_port(motors, UP_DOWN_MOTOR_PORT);
  if (up_down_motor == NULL) {
    printf("Error on ev3_search_motor_by_port\n");
    return EXIT_FAILURE;
  }
  // First, reset and then open the motor
  ev3_reset_motor(up_down_motor);
  ev3_open_motor(up_down_motor);
  if (up_down_motor == NULL) {
    printf("Error on ev3_open_motor\n");
    return EXIT_FAILURE;
  }

  // ! ---------------------- INIT LEFT-RIGHT MOTOR ---------------------- ! //
  left_right_motor = ev3_search_motor_by_port(motors, LEFT_RIGHT_MOTOR_PORT);
  if (left_right_motor == NULL) {
    printf("Error on ev3_search_motor_by_port\n");
    return EXIT_FAILURE;
  }
  // First, reset and then open the motor
  ev3_reset_motor(left_right_motor);
  ev3_open_motor(left_right_motor);
  if (left_right_motor == NULL) {
    printf("Error on ev3_open_motor\n");
    return EXIT_FAILURE;
  }

  // ! ---------------------- CALIBRATION ---------------------- ! //
  // calibration();

  // ! ------------------------------ MENU ------------------------------- ! //
  // Limpia el LCD
  // Muestra las tres opciones:
  //  - Pulsar botón para comenzar el movimiento automático.
  //  - Pulsar botón para comenzar el movimiento manual.
  //  - Pulsar botón para salir.
  // print_menu();

  // ! ---------------------- LOOP READING BUTTONS ---------------------- ! //
  // Loop reading buttons
  while (1) {
    // Read the button
    int button_pressed = is_button_pressed();
    if (button_pressed != 0) {
      printf("Button pressed: %d\n", button_pressed);
    }

    // If the button is pressed, break the loop
    if (button_pressed == BUTTON_BACK) {
      break;
    } else if (button_pressed == BUTTON_LEFT) {
      // If the button is pressed, start the manual mode
      printf("Manual mode\n");
      // manual_mode();
    } else if (button_pressed == BUTTON_RIGHT) {
      // If the button is pressed, start the automatic mode
      printf("Automatic mode\n");
      // automatic_mode();
    }
    usleep(500000);
  }

  // Limpia el LCD

  // int i;
  // for (i = 0; i < 100; i++) {
  //   ev3_update_sensor_val(push_sensor);
  //   ev3_update_sensor_val(color_sensor);
  //   printf("[%02d] -> color: %d\tpush: %d\n", i,
  //   color_sensor->val_data[0].s32,
  //          push_sensor->val_data[0].s32);
  //   usleep(500000);
  // }

  // ! Finish & close devices
  ev3_close_motor(grab_drop_motor);
  ev3_close_motor(up_down_motor);
  ev3_close_motor(left_right_motor);
  ev3_close_sensor(color_sensor);
  ev3_close_sensor(push_sensor);
  ev3_delete_motors(motors);
  ev3_delete_sensors(sensors);
  ev3_quit_lcd();
  ev3_quit_button();
  return EXIT_SUCCESS;
}

void checks() {}

void object_drop() {
  // TODO:
}

/**
 * @brief Calibración:
 * 1.1. Hay que abrir el agarre.
 * 1.2. Hay que subir el brazo.
 * 1.3. Hay que mover a la derecha del todo. Luego a la izquierda una
 *        determinada distancia que se supone conocida.
 */
void calibration() {
  // // 1.1. Hay que abrir el agarre.
  // ev3_set_motor_power(grab_drop_motor, GRAB_DROP_MOTOR_POWER);
  // ev3_run_motor(grab_drop_motor, TIME_GRAB_DROP_MOTOR_OPEN);
  // ev3_stop_motor(grab_drop_motor);
  // // 1.2. Hay que subir el brazo.
  // ev3_set_motor_power(up_down_motor, UP_DOWN_MOTOR_POWER);
  // ev3_run_motor(up_down_motor, TIME_UP_DOWN_MOTOR_UP);
  // ev3_stop_motor(up_down_motor);
  // // 1.3. Hay que mover a la derecha del todo. Luego a la izquierda una
  // //        determinada distancia que se supone conocida.
  // ev3_set_motor_power(left_right_motor, LEFT_RIGHT_MOTOR_POWER);
  // ev3_run_motor(left_right_motor, TIME_LEFT_RIGHT_MOTOR_RIGHT);
  // ev3_stop_motor(left_right_motor);

  printf("Start position %d (degrees)\n", ev3_get_position(up_down_motor));

  if (ev3_get_polarity(up_down_motor) == POL_UP) {
    ev3_set_polarity(up_down_motor, POL_DOWN);
  }
  printf("Polarity %d\n", ev3_get_polarity(up_down_motor));

  // Move the arm up
  // ev3_command_motor_by_name(up_down_motor, "run-forever");
  // ev3_set_duty_cycle_sp(up_down_motor, 5);  // Power to the engine
  // while (1) {
  //   ev3_update_sensor_val(color_sensor);
  //   printf("sensor: %d\n", color_sensor->val_data[0].s32);
  //   if (color_sensor->val_data[0].s32 < 5) {
  //     ev3_command_motor_by_name(up_down_motor, "stop");
  //     printf("Stop\n");
  //     break;
  //   }
  //   usleep(500000);
  // }

  // Elevate arm: set the stop mode to hold
  ev3_stop_command_motor_by_name(up_down_motor, STOP_MODE_STRING[COAST]);

  ev3_set_duty_cycle_sp(up_down_motor, 5);  // Power to the engine
  ev3_set_position(up_down_motor, 0);

  printf("Start position %d (degrees)\n", ev3_get_position(up_down_motor));
  ev3_set_position_sp(up_down_motor, 360);
  ev3_command_motor_by_name(up_down_motor,
                            COMMANDS_STRING[RUN_ABS_POS]);  // Action!
  usleep(SUSPENSION_TIME);
  while ((ev3_motor_state(up_down_motor) & MOTOR_RUNNING & !MOTOR_HOLDING))
    ;  // Wait until motor action is complete
  printf("Current position %d (degrees)\n", ev3_get_position(up_down_motor));
}

void print_menu() {
  int i, button_pressed = -1, option_selected = 0;
  while (1) {
    button_pressed = is_button_pressed();
    if (button_pressed == BUTTON_UP) {
      option_selected = (option_selected + menu_text_size - 1) % menu_text_size;
    } else if (button_pressed == BUTTON_DOWN) {
      option_selected = (option_selected + 1) % menu_text_size;
    } else if (button_pressed == BUTTON_CENTER) {
      if (option_selected == 0) {
        automatic();
        break;
      } else if (option_selected == 1) {
        manual();
        break;
      } else if (option_selected == 2) {
        // Exit
        printf("Exit\n");
        break;
      }
    }
    // Clear screen and print header
    ev3_clear_lcd();
    ev3_text_lcd_large(0, 0, "Padilla's");
    ev3_text_lcd_large(15, 20, "project");
    ev3_line_lcd(0, 40, 178, 40, 1);
    ev3_text_lcd_small(0, 45, " > MENU");
    ev3_line_lcd(0, 60, 178, 60, 1);
    // Print options
    for (i = 0; i < menu_text_size; i++) {
      if (i == option_selected) {
        ev3_text_lcd_small(0, 70 + i * 15, menu_text[i]);
      } else {
        ev3_text_lcd_normal(0, 70 + i * 15, menu_text[i]);
      }
    }
    usleep(SLEEP_DURATION_BUTTONS);
  }
}

int is_button_pressed(void) {
  int i, button_pressed = 0;

  for (i = BUTTON_BACK; i > BUTTON_LEFT; i--) {
    button_pressed = (button_pressed << 1) | ev3_button_pressed(i);
  }

  if (button_pressed == 0) {
    return -1;
  } else {
    return button_pressed;
  }
}

void manual(void) {
  int i;
  // Clear screen and print header
  ev3_clear_lcd();
  ev3_text_lcd_large(0, 0, "Padilla's");
  ev3_text_lcd_large(15, 20, "project");
  ev3_line_lcd(0, 40, 178, 40, 1);
  ev3_text_lcd_small(0, 45, " > MANUAL MOVEMENT");
  ev3_line_lcd(0, 58, 178, 58, 1);
  // Print options
  ev3_text_lcd_small(0, 70, "Move the arrows to mo-");
  ev3_text_lcd_small(0, 85, "ve the robot's arm.");
  ev3_text_lcd_small(0, 100, "Center to grab/drop an");
  ev3_text_lcd_small(0, 115, "object.");

  // Wait for the user to press a button
  // while (1) {
  //   i = is_button_pressed();
  //   if (i == BUTTON_LEFT) {
  //     ev3_set_motor_power(left_right_motor, LEFT_RIGHT_MOTOR_POWER);
  //     ev3_run_motor(left_right_motor, TIME_LEFT_RIGHT_MOTOR_LEFT);
  //     ev3_stop_motor(left_right_motor);
  //   } else if (i == BUTTON_RIGHT) {
  //     ev3_set_motor_power(left_right_motor, LEFT_RIGHT_MOTOR_POWER);
  //     ev3_run_motor(left_right_motor, TIME_LEFT_RIGHT_MOTOR_RIGHT);
  //     ev3_stop_motor(left_right_motor);
  //   } else if (i == BUTTON_CENTER) {
  //     ev3_set_motor_power(up_down_motor, UP_DOWN_MOTOR_POWER);
  //     ev3_run_motor(up_down_motor, TIME_UP_DOWN_MOTOR_UP);
  //     ev3_stop_motor(up_down_motor);
  //   } else if (i == BUTTON_BACK) {
  //     break;
  //   }
  //   usleep(SLEEP_DURATION_BUTTONS);
  // }
}

void automatic() { printf("Automatic\n"); }

void reporter() {
  printf("\n*** Reporter thread started ***\n");
  while (1) {
    printf("\n*** Reporter thread running ***\n");
    // usleep(SLEEP_DURATION_REPORT);
  }
}