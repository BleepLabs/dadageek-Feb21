//This code uses a potentiometer to control the rate of a blinking LED

//The first thing we do is initialize variables.
//Here we name the variables we want to store data in and decide on the data type

//Two slashes means "comment". Anything written after the "//" is ignored by the compiler, the program that turns this code into something the Teensy can use
/*
   This has the same effect
*/

// "int" describes a data type which allows the variable to be a whole number from -32,767 to 32,767 (16 bits)
int led_pin =  12;  // The pin the LED is connected to
int led_state = 0;  // The state of LED.
//"unsigned long" is another data type that's 32 bits, all of them positive. 0 - 4,294,967,295
unsigned long  previous_time = 0;  // will store the last time the LED was updated
unsigned long current_time; //will store how much time has elapsed since the Teensy was reset
// if you don't set it to a numer it will be 0 by default.
unsigned long rate;  //how fast the LED will change
int pot_value;  //store the reading from the potentiometer aka knob
int pot_pin = A0;  //pins that can do analog readings start with A

//This next part is where the code really begins. It uns just once after the Teensy resets.

void setup() { //The setup starts with this currly bracket here
  // Set the LED pin as an output
  pinMode(led_pin, OUTPUT);

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
  rate = (pot_value * 2) + 10; //we can do any math on variables you can imagine.


  //Has "interval1" amount of time past since we last executed this code contained in the curly brackets?
  // If the current time minus the last time is bigger that the interval then this part of the code is run
  // If not then it doesn't and we do the next thing below the curly brackets

  //We're using milliseconds so a value of 1000 would make it turn on for 1 seconds then off for one
  if (current_time - previous_time > rate) {
    previous_time = current_time; //remember what time it is now so we can check how long it's been next loop

    // if the LED is off turn it on and vice versa:
    if (led_state == 0) { // "==" means check if these are the same
      led_state = 1; // "=" means set the first variable equal to the second value
    }
    //if the previous comparison is false do this
    else { 
      led_state = 0;
    }

    // Output the value "led1_state" to "led1_pin.
    //if "led1_state" is low aka "0" the LED will be off
    //if "led1_state" is high aka "1" the LED will be on
    //it's digital so it can only be on or off which you right as 1 or 0. HIGH or LOW can alos be used.
    digitalWrite(led_pin, led_state);

  } // the "if" statement is now over.

} // Loop is over so it starts again with basically no wait
