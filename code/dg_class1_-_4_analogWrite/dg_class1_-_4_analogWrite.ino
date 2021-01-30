//Have the LED fade rather than just be on or off

//Variables will not have anything inherently in common because of their names. We could name them anything.
// The variable names are only there to help us. The compiler gets rid of the names

// "int" describes a data type which allows the variable to be a whole number from -32,767 to 32,767 (16 bits)
int led1_pin =  9;  //For the examples on Feb 1st I'm using a Teensy 3.2 as I'm all out of 4.0s. On the 3.2 only some pins are PWMable https://www.pjrc.com/teensy/pinout.html
int led1_brightness;
int led2_pin =  10;
int led2_brightness;

//"unsigned long" is another data type that's 32 bits, all of them positive. 0 - 4,294,967,295
unsigned long  previous_time1 = 0;  // will store the last time LED1 was updated
unsigned long  previous_time2 = 0;  // will store the last time LED2 was updated
unsigned long current_time; //will store how much time has elapsed since the Teensy was reset
// if you don't set it to a number it will be 0 by default.

unsigned long rate1;  //how fast the LED1 will change
unsigned long rate2 = 250; //how fast the LED2 will change
//We can set the varible to a value here and not change it in the loop

int pot_value;  //store the reading from the potentiometer aka knob
int pot_pin = A0;  //pins that can do analog readings start with A

//This next part is where the code really begins. It uns just once after the Teensy resets.

void setup() { //The setup starts with this currly bracket here
  // set both LED pins as outputs
  pinMode(led1_pin, OUTPUT);
  pinMode(led2_pin, OUTPUT);

  //you don't need to setup the pot pin as an input

} //Setup is over. curly brackets begin and end sections of code.


// This part of the code will run over and over again as fast as the Teensy can go,
// executing each line in order, one at a time
void loop()
{
  //Calls the function "millis". See info on all the basic functions here https://www.arduino.cc/reference/en/
  // It returns the time in milliseconds that have elapsed since the device restarted which we store in the varible "current_time"
  // That means for the rest or this loop "current_time" will be equal to that number
  current_time = millis();


  pot_value = analogRead(A0); //Read the analog voltage at pin A0. Returns 0 for 0 Volts and 1023 for the max voltage (3.3V)
  rate1 = (pot_value / 100);
  rate2 = rate1 * 3;

  //Has "rate1" amount of time past since we last executed this code contained in the curly brackets?
  // If the current time minus the last time is bigger that the interval then this part of the code is run
  // If not then it doesn't and we do the next thing below the curly brackets
  //We're using milliseconds so a value of 1000 would make it turn on for 1 seconds then off for one

  if (current_time - previous_time1 > rate1) { //has "interval1" amount of time past since we last executed this code?
    previous_time1 = current_time; //if the statement is true remember the current time
    
    led1_brightness++;
    if (led1_brightness > 255) {//PWM is 0-255, 8 bits
      led1_brightness = 0;
    }
    analogWrite(led1_pin, led1_brightness);   // Write the value "led1_brightness" to the pin "led1_pin".
  }


  if (current_time - previous_time2 > rate2) { //has "interval2" amount of time past since we last executed this code?
    previous_time2 = current_time;
    
    led2_brightness++;
    if (led2_brightness > 255) {//PWM is 0-255, 8 bits
      led2_brightness = 0;
    }

    analogWrite(led2_pin, led2_brightness);

  }

} // Loop is over so it starts again with basically no wait
