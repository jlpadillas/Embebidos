#include <stdio.h>

#include "../include/ev3c.h"
#include "../include/ev3c_lcd.h"

int main(int argc, char* argv[]) {
  int32_t x = 0;
  int32_t y = 0;
  const char* text = "Hello World!";

  ev3_init_lcd();                  // Initialize the LCD
  ev3_clear_lcd();                 // Clear the LCD
  ev3_text_lcd_large(x, y, text);  // Draw the text
  ev3_quit_lcd();                  // Quit the LCD

  return 0;
}
