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

void setup() {
  start_bleep_base(); //run this first in setup
}

void loop() {
  update_controls(); //run this first in the loop
  current_time = millis(); //then check the time

  rate1 = potRead(4) * 20; //potRead returns 0-1.0
  rate2 = potRead(6) * 20;

  //prev_time[0] is a variable in the first position of the array. in the declaration section we gave it 8 places
  if (current_time - prev_time[0] > rate1) { 
    prev_time[0] = current_time;
    lfo1 *= 1.01;
    if (lfo1 > 1.0) {
      lfo1 = .1;
    }
  }

  if (current_time - prev_time[1] > rate2) {
    prev_time[1] = current_time;
    lfo2 *= 1.01;
    if (lfo2 > 1.0) {
      lfo2 = .1;
    }
  }

  if (current_time - prev_time[2] > 33) { //33 ms is about 30Hz https://www.wolframalpha.com/input/?i=30Hz+in+milliseconds
    prev_time[2] = current_time;

    float h1 = potRead(0); //we can declare new variables but they will only be available to use in inside these {}
    float s1 = potRead(1);
    float h2 = potRead(2);
    float s2 = potRead(3);

    set_LED(0, lfo1, s1, lfo1);  //(LED select, hue, saturation, brightness) H S and B are all 0-1.0
    set_LED(1, lfo2, s2, lfo2);
    LEDs.show(); // after we've set what we want all the LEDs to be we send the data out through this function
  }

  //"for" loops the contents in it's {} as long as the condition is true
  // int j=0   starts with j =0
  // j<8  as long as j is less than 8 keep executing the code inside the "for"
  // j++  each time you get to the bottom of the for loop add one to j. Same as j=j+1
  for (int j = 0; j < 8; j++) {

    //we have to check if the button rose or fell very quickly. If you put this in a timing if you'll miss it
    //FELL is defined as the button was high and now it's low
    if (buttonState(j) == FELL) {  
      //serial print send values from the teesny to your computer and puts them in the serial monitor.
      //You can access it in tools>serial monitor, or just by clicking the icon in the top right of the Arduino program
      Serial.print(" button "); //print the characters " button "
      Serial.print(j); //print the variable j
      Serial.println("FELL "); // print "FELL " then a return. "println" is new line
    }

    if (buttonState(j) == ROSE) {
      Serial.print(" button ");
      Serial.print(j);
      Serial.println("ROSE ");
    }
  } //for is over


  if (current_time - prev_time[3] > 100) {
    prev_time[3] = current_time;
    if (buttonRead(1) == 0) {//print the values of the lfos ever 100 milliseconds when button 1 is pressed
      Serial.print(lfo1);
      Serial.print(" ");
      Serial.println(lfo2);
    }
    if (buttonRead(4) == 0) {//print all the pots when button 1 is pressed
      for (int j = 0; j < 8; j++) {
        Serial.print(potRead(j) * 1000.0); //it's easier to read if it's x 1000
        Serial.print(" ");
      }
      Serial.println();
    }
  }

}// loop is over
