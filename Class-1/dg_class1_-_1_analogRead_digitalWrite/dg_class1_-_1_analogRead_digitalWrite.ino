//This code uses a potentiometer to control the rate of a blinking LED

//Two slashes means "comment". Anything written after the "//" is ignored by the compiler, the program that turns this code into something the Teensy can use
/*
   This has the same effect
*/
//Empty lines are irrelevant. You can have as many or few as you'd like.


//This area at the top of our "sketch" is the initialization section
// The first thing we need to do is name the variables we 
// want to store values in and decide on the data type

//"int" stands for integer. It describes a data type which allows the variable 
// to be a whole number from -32,767 to 32,767 (16 bits)
int led_pin =  10;  // The pin the LED is connected to
int led_state = 0;  // The state of LED.
// Pretty much all lines of code end in a semicolon 

//"unsigned long" is another data type that's 32 bits, all of them positive. 0 - 4,294,967,295
// We'll be counting milliseconds with this so the number will get big pretty quickly
unsigned long  previous_time = 0;  // will store the last time the LED was updated
unsigned long current_time; //will store how much time has elapsed since the Teensy was reset
// if you don't set it equal to a number it will be 0 by default.

unsigned long rate;  //how fast the LED will change
int pot_value;  //store the reading from the potentiometer aka knob
int pot_pin = A0;  //pins that start with A can do analog readings 


//This next part is where the code really begins. It runs just once after the Teensy resets.

void setup() { //The setup starts with this curly bracket here
  // Set the LED pin as an output
  pinMode(led_pin, OUTPUT);//This function has two inputs. The pin and the state we want it to be https://www.arduino.cc/reference/en/language/functions/digital-io/pinmode/

  //you don't need to setup the pot pin as an input

} //Setup is over. curly brackets begin and end sections of code.


// This part of the code will start as soon as the setup is done and run over and over again
// as fast as the Teensy can go, executing each line in order, one at a time
void loop()
{
  //Calls the function "millis". https://www.arduino.cc/reference/en/language/functions/time/millis/
  // It returns the time in milliseconds that have elapsed since the device restarted 
  // which we store in the variable "current_time"
  // That means for the rest or this loop "current_time" will be equal to that number
  current_time = millis();

  pot_value = analogRead(A0); //Read the analog voltage at pin A0. Returns 0 for 0 Volts and 1023 for the max voltage (3.3V)
  rate = (pot_value * 2) + 10; //Math! rate is now equal to pot_value times 2 plus 10

  //This next bit of code is something we'll use over and over to control how ofter something happens
  // Has "rate" amount of time past since we last executed the code contained in these curly brackets?
  // "if" the current time minus the previous time is bigger that the rate then the code inside the curly brackets will run
  // If not then it doesn't run it and we continue on to the next thing below the curly brackets

  //We're using milliseconds so a value of 1000 would make it turn on for 1 seconds then off for one
  if (current_time - previous_time > rate) { //if begins
    previous_time = current_time; //remember what time it is so we can check how long it's been since we executed the code inside the curlys

    // if the LED is off turn it on and vice versa:
    if (led_state == 0) { // "==" means check if these values are the same
      led_state = 1; // "=" means set the first variable equal to the second value
    }
    else { //if the previous comparison is not true do this
      led_state = 0;
    }

    //Output the value "led1_state" to "led1_pin.
    // if "led1_state" is low aka "0" the LED will be off
    // if "led1_state" is high aka "1" the LED will be on
    // it's digital so it can only be on or off which you right as 1 or 0. HIGH or LOW can also be used.
    digitalWrite(led_pin, led_state);

  } // the "if" statement is now over.

} // loop is over so we go back to the start of it with basically no wait
