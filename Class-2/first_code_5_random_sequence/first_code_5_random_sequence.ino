//bleep base basic controls

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

  rate1 = potRead(2) * 1000.0;
  //rate1 = pcellRead() * 1000.0;


  if (current_time - prev_time[2] > 10) {
    prev_time[2] = current_time;
    Serial.print(array_counter);
    Serial.print("  ");
    Serial.print(array1[array_counter]);
    Serial.println();
  }


  if (current_time - prev_time[1] > rate1) {
    prev_time[1] = current_time;

    int r1 = random(0,100);
    if (r1>60){
      led_state=1;
    }
    else{
      led_state=0;
    }
    
  }

  if (current_time - prev_time[0] > 30) {
    prev_time[0] = current_time;
    h1 = potRead(0);
    b1 = potRead(1) * led_state;

    set_LED(0, h1, 1.0, b1);
    set_LED(1, h2, 1.0, b2);
    LEDs.show();
  }

  int latch1 = 0;
  for (int j = 0; j < 8; j++) {
    if (buttonRead(j) == 0) {
      h2 = j * (.1);
      latch1 = 1;
    }
  }

  if (latch1 == 1) {
    b2 = 1.0;
  }
  else {
    b2 = 0;
  }


  /*
    if (buttonRead(0) == 0) {
      h2 = .2;
      b2 = 1.0;
    }
    else {
      b2 = 0;
    }
    if (buttonRead(1) == 0) {
      h2 = .4;
      b2 = 1.0;
    }
    else {
      b2 = 0;
    }
    if (buttonRead(2) == 0) {
      h2 = .6;
      b2 = 1.0;
    }
    else {
      b2 = 0;
    }
  */





}// loop is over
