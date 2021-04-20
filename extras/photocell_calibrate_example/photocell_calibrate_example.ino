//Hold a button to calibrate the photocell to a range and then cahnge it's resposne to  logarithmic
//Based on Stefano's idea

#include "bleep_base.h"
unsigned long current_time;
unsigned long prev_time[8]; //an array of 8 variables all named "prev_time"
int calibrate_mode = 2; //start it at someting other than 0 or 1 so it won't print untill after you calibrate
int maxRead;
int minRead;
float sensorOut;

void setup() {
  start_bleep_base(); //run this first in setup
} //setup is over

void loop() {
  update_controls(); //run this first in the loop
  current_time = millis(); //then check the time

  if (buttonState(0) == ROSE) {
    calibrate_mode = 0;
    Serial.println("...End of Calibration");
    Serial.print(minRead);
    Serial.print(" ");
    Serial.println(maxRead);
  }

  if (buttonState(0) == FELL) {
    calibrate_mode = 1;
    maxRead = 2; //these should only happen once when the calibration begins
    minRead = 1;
    Serial.println("Calibrating ...");

  }
  if (calibrate_mode == 1) {
    //maxRead = 2;  //if these are here they will continue to set max and minRead to 2 and 1
    //minRead = 1;
    //update_controls(); //not needed for analogRead

    //do the calibration
    //this is all good!
    int currentRead = analogRead(A2);
    if (currentRead > maxRead)
    {
      maxRead = currentRead;
    }
    if (currentRead < minRead)
    {
      minRead = currentRead;
    }
  }

  float sensorSteps = 200;
  //I think i get where you're going with the log and its a good idea but fscale will work better
  //sensor steps is a good idea
  // fscale is very similar to map but you can appy an arbitrary curve to it. -1.0 is a very log respose while 1.0 is very exonential. 1.0 is linear

  //sensorOut = map(log(analogRead(A2)), log(minRead), log(maxRead), 0, int(sensorSteps)) / sensorSteps ;
  sensorOut = fscale(analogRead(A2), -.8, minRead, maxRead, 0, sensorSteps) / sensorSteps;

  if (current_time - prev_time[0] > 20 && calibrate_mode == 0) { //don't print when calibrating
    prev_time[0] = current_time;
    Serial.println(sensorOut);
  }

}// loop is over


//from here with a few little cahnges  https://playground.arduino.cc/Main/Fscale/
float fscale(float inputValue, float curve, float originalMin, float originalMax, float newBegin, float newEnd) {

  float OriginalRange = 0;
  float NewRange = 0;
  float zeroRefCurVal = 0;
  float normalizedCurVal = 0;
  float rangedValue = 0;
  boolean invFlag = 0;


  // condition curve parameter
  // limit range

  if (curve > 1) curve = 1;
  if (curve < -1) curve = -1;

  curve = (curve * -1.0) ;
  curve = pow(10, curve); // convert linear scale into lograthimic exponent for other pow function

  // Check for out of range inputValues
  if (inputValue < originalMin) {
    inputValue = originalMin;
  }
  if (inputValue > originalMax) {
    inputValue = originalMax;
  }

  // Zero Refference the values
  OriginalRange = originalMax - originalMin;

  if (newEnd > newBegin) {
    NewRange = newEnd - newBegin;
  }
  else
  {
    NewRange = newBegin - newEnd;
    invFlag = 1;
  }

  zeroRefCurVal = inputValue - originalMin;
  normalizedCurVal  =  zeroRefCurVal / OriginalRange;   // normalize to 0 - 1 float

  // Check for originalMin > originalMax  - the math for all other cases i.e. negative numbers seems to work out fine
  if (originalMin > originalMax ) {
    return 0;
  }

  if (invFlag == 0) {
    rangedValue =  (pow(normalizedCurVal, curve) * NewRange) + newBegin;

  }
  else     // invert the ranges
  {
    rangedValue =  newBegin - (pow(normalizedCurVal, curve) * NewRange);
  }

  return rangedValue;
}
