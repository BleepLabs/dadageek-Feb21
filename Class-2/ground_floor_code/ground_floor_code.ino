//use this as your starting point

//Include means add code from another file. This allows use more functions
#include "bleep_base.h" //must be included at the top

//then you can declare any variables you want.
unsigned long current_time;
unsigned long prev_time[8]; //an array of 8 variables all named "prev_time"
float lfo1 = 1; //float can contain decimals
float lfo2 = 1;
int rate1;
int rate2;
float h1;
float h2;
float b1;
float b2;
float led_state;
int array1[16] = {1, 0, 1, 1, 0, 0, 1, 0, 1, 1, 1, 1, 0, 0, 0, 0, };
int array_counter;

void setup() {
  start_bleep_base(); //run this first in setup
}

void loop() {
  update_controls(); //run this first in the loop
  current_time = millis(); //then check the time

   

}// loop is over
