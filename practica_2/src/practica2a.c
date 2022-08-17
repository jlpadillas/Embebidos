/*
 * File: practica2a.c
 *
 * Description: Muestra el uso de la botonera
 *
 * Author:  DSSE
 * Version: 1.0
 *
 */

#include <ev3c.h>
#include <stdio.h>
#include <unistd.h>

// #include "ev3c.h"

#define MAX_SIZE_BYTES 24
#define MAX_ITERATIONS 50
#define SLEEP_DURATION 1  // seconds

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
  // Local variables
  int index, button_pressed;
  static const char *BUTTON[] = {"NO_BUTTON",    "BUTTON_LEFT", "BUTTON_UP",
                                 "BUTTON_RIGHT", "BUTTON_DOWN", "BUTTON_CENTER",
                                 "BUTTON_BACK"};
  char button_text[MAX_SIZE_BYTES];  // char array to store the output string

  // Init devices
  ev3_init_button();

  //   // Main loop - 1 second
  //   for (index = 0; index < MAX_ITERATIONS; index++) {
  //       button_pressed = is_button_pressed();
  //       if (button_pressed >= 0) {
  //           sprintf (button_text, "Button: %s", BUTTON [button_pressed + 1]);
  //           printf ("%s\n", button_text);
  //       }
  //       // pseudo-periodic activation
  //       sleep (SLEEP_DURATION);
  //   }

  // // Main loop - 10 us
  // for (index = 0; index < MAX_ITERATIONS * 100; index++) {
  //   button_pressed = is_button_pressed();
  //   if (button_pressed >= 0) {
  //     sprintf(button_text, "Button: %s", BUTTON[button_pressed + 1]);
  //     printf("%s\n", button_text);
  //   }
  //   // pseudo-periodic activation
  //   usleep(SLEEP_DURATION * 50000);
  // }

  // Main loop - 67 ms
  for (index = 0; index < MAX_ITERATIONS * 10; index++) {
    button_pressed = is_button_pressed();
    if (button_pressed >= 0) {
      sprintf(button_text, "Button: %s", BUTTON[button_pressed + 1]);
      printf("%s\n", button_text);
    }
    // pseudo-periodic activation
    usleep(SLEEP_DURATION * 67000);
  }

  //  Finish & close devices
  printf("\n*** Finishing button application... ");
  ev3_quit_button();

  printf("OK***\n");
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
