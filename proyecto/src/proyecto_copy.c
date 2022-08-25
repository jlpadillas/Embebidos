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
//                determinada distancia que se supone conocida.
//   2. Movimiento:
//        2.1. Se baja el brazo a tope (conocer la distancia de subida/bajada
//        máxima) 2.2. Se cierra el agarre. 2.3. Se sube el brazo. 2.4. Se mueve
//        a la derecha del todo (hasta tocar el botón). 2.5. Se baja el brazo.
//        2.6. Se abre el agarre.
//        2.7. Se sube el brazo.
//        2.8. Se mueve a su posición inicial.


int main(int argc, char const *argv[]) {
  ev3_motor_ptr motors = NULL;        // List of motors
  ev3_sensor_ptr sensors = NULL;      // List of available sensors
  ev3_sensor_ptr push_sensor = NULL;  // List of available push sensors
  // double circumference = DIAMETER_WHEEL * M_PI;
  double circumference = DIAMETER_WHEEL * 3.141592653589793;

  // Loading all motors
  // motors = ev3_load_motors();
  // if (motors == NULL) {
  //   printf("Error on ev3_load_motors()\n");
  //   return EXIT_FAILURE;
  // }

  // Get the target motor
  // grab_drop_motor = ev3_search_motor_by_port(motors, GRAB_DROP_MOTOR_PORT);
  // if (grab_drop_motor == NULL) {
  //   printf("Error on ev3_search_motor_by_port(grab_drop_motor)\n");
  //   return EXIT_FAILURE;
  // }

  // Init motor
  ev3_reset_motor(grab_drop_motor);
  ev3_open_motor(grab_drop_motor);

  ev3_stop_command_motor_by_name(grab_drop_motor, STOP_MODE_STRING[BRAKE]);
  ev3_set_duty_cycle_sp(grab_drop_motor, 25);  // Power to the engine

  ev3_set_position(grab_drop_motor, 0);
  printf("Start position %d (degrees)\n", ev3_get_position(grab_drop_motor));

  ev3_set_position_sp(grab_drop_motor, 100);

  ev3_command_motor_by_name(grab_drop_motor,
                            COMMANDS_STRING[RUN_ABS_POS]);  // Action!

  sleep(2);
  printf("Position: %d (degrees)\n", ev3_get_position(grab_drop_motor));

  // ! Finish & close devices
  ev3_reset_motor(grab_drop_motor);
  ev3_delete_motors(motors);
  ev3_delete_sensors(sensors);
  return EXIT_SUCCESS;
}

void checks() {}

void object_drop() {
  // TODO:
}

void calibration() {
  // TODO:
}
