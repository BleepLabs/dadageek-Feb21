//If we want a second LED we need to have separate variables for some things

//Variables will not have anything inherently in common because of their names. We could name them anything.
// The variable names are only there to help us. The compiler gets rid of the names

// "int" describes a data type which allows the variable to be a whole number from -32,767 to 32,767 (16 bits)
int led1_pin =  10;  // The pin for LED1
int led1_state = 0;  // The state of LED1

int led2_pin =  9;   // The LED2 pin
int led2_state = 0;  // The LED2 state

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

int button_reading;
int button_pin = 5;

//This next part is where the code really begins. It uns just once after the Teensy resets.

void setup() { //The setup starts with this currly bracket here
  // Set the LED pin as an output
  // set both LED pins as outputs
  pinMode(led1_pin, OUTPUT);
  pinMode(led2_pin, OUTPUT);
  pinMode(button_pin, INPUT_PULLUP); //must be done for button pins. Make the defualt state for the pin HIGH and 1

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
  rate1 = (pot_value * 2) + 10; //we can do any math on variables you can imagine.

  button_reading = digitalRead(button_pin); //read the status of button pin. Is it 0V or 3.3V.
  if (button_reading == 0) { //0 is pressed, 1 is not
    rate1 = rate1 / 8; // mae it blink 8 times faster if the button is down. 
  }

  //Has "interval1" amount of time past since we last executed this code contained in the curly brackets?
  // If the current time minus the last time is bigger that the interval then this part of the code is run
  // If not then it doesn't and we do the next thing below the curly brackets
  //We're using milliseconds so a value of 1000 would make it turn on for 1 seconds then off for one

  if (current_time - previous_time1 > rate1) { //has "interval1" amount of time past since we last executed this code?
    previous_time1 = current_time; //if the statement is true remember the current time

    // if the LED is off turn it on and vice versa:
    if (led1_state == LOW) {
      led1_state = HIGH;
    }
    else {
      led1_state = LOW;
    }
    /*
        //  This wouldn't work. Why not?
        if (led1_state == LOW) {
          led1_state = HIGH;
        }
        if (led1_state == HIGH) {
          led1_state = LOW;
        }
    */
    digitalWrite(led1_pin, led1_state);   // Write the value "led1_state" to the pin "led1_pin".
  }


  if (current_time - previous_time2 > rate2) { //has "interval2" amount of time past since we last executed this code?
    previous_time2 = current_time;

    // if  LED2 is off turn it on and vice versa:
    if (led2_state == LOW) { //you might see examples wtieen this way. it's exacrly the saem as 0 and 1
      led2_state = HIGH;
    }
    else {
      led2_state = LOW;
    }

    digitalWrite(led2_pin, led2_state);   // Write the value "led2_state" to "led2_pin.

  }

} // Loop is over so it starts again with basically no wait
