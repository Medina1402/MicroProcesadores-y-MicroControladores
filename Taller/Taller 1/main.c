/* =====================================================================
* @author Abraham Medina Carrillo
* @email abraham.medina.carrillo@uabc.edu.mx
*
* @website https://github.com/Medina1402
* @repository ** private **
* ===================================================================== */

#include <stdio.h>
#include <conio.h>
#include <time.h>

#define CURRENT_TIME ((double)clock()/CLOCKS_PER_SEC)
#define MILISECUNDS(ms) ((double)(ms/CLOCKS_PER_SEC))
#define MAX_CYCLES 1

/*
  ** 0 1 2 3 P ON **
  0 = OFF
  1 = 500ms (ON) -> 500ms (OFF) .... LED
  2 = 250ms (ON) -> 250ms (OFF) .... LED
  3 = 125ms (ON) -> 125ms (OFF) .... LED
  P = (VELms (ON) -> VELms (OFF)<LED>)*MAX_CYCLES -> N*MS (OFF) ... TOGGLE STOP
*/
int main() {
  unsigned char toggle_led = 0, speed = 0, press_stop = 0, stop_counter = 0;
  //unsigned int speeds[4] = {0, 3000, 2000, 1000}; // Prueba perceptible
  unsigned int speeds[4] = {0, 500, 250, 125};
  char formato[] = "0 1 2 3 P LED";
  char options[] = "x            ";
  char key, before_key = '0';
  float timer = 0, current;

  printf("%s\n\r", formato);
  while(1) {
    current = CURRENT_TIME;

    if(current >= timer && key && key!='x' && before_key!='0') {
      
      if(stop_counter < MAX_CYCLES*2) {
        timer = CURRENT_TIME + MILISECUNDS(speeds[speed]);
        options[(before_key - '0') * 2] = 'x';

        if(toggle_led) options[11] = 'O';
        else options[11] = ' ';
        toggle_led = !toggle_led;
        options[0] = ' ';

      } else {
        timer = CURRENT_TIME + MILISECUNDS(1000);
        options[0] = 'x';
        options[speed * 2] = ' ';
        options[11] = '-';
      }

      if(press_stop && before_key!='0') {
        stop_counter++;
        stop_counter%=(MAX_CYCLES*2+1);
        options[8] = 'x';
      } else stop_counter = 0;
    }

    if(kbhit()) {
      key = getche();
      if (key == 'P' || key == 'p') press_stop = 1;
      else press_stop = 0;

      if(
        (key == '0' || key == '1' || key == '2' || key == '3') &&
        before_key != key && !press_stop
      ) {

        // Reajustamos la velocidad de las aspas
        options[0] = ' ';
        options[8] = ' ';
        options[(before_key - '0') * 2] = ' ';
        before_key = key;
        speed = key - '0';
        options[speed * 2] = 'x';

        // Siempre debe parar y apagar el LED en velocidad cero
        if(key == '0') {
          options[11]= ' ';
          toggle_led = 0;
          key = 'x';
        }
      }
    }

    printf("%s\r", &options);
  }

  return 0;
}