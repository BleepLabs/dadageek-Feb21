// The block we copied from the tool is pasted below
// design tool: https://www.pjrc.com/teensy/gui/

// "#include" means add another file to our code
// So far we've been copy and pasting things in but we can just tell our code to look
// in a library for more functions and data
// These are all necessary to get audio working but we don't need to do anything besides include them

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform1;      //xy=168,208
AudioSynthWaveform       waveform2;      //xy=173,279
AudioAnalyzePeak         peak1;          //xy=365,125
AudioAnalyzePeak         peak2;          //xy=427,178
AudioMixer4              mixer1;         //xy=553,279
AudioOutputI2S           i2s1;           //xy=718,282
AudioConnection          patchCord1(waveform1, 0, mixer1, 0);
AudioConnection          patchCord2(waveform1, peak1);
AudioConnection          patchCord3(waveform2, 0, mixer1, 1);
AudioConnection          patchCord4(waveform2, peak2);
AudioConnection          patchCord5(mixer1, 0, i2s1, 0);
AudioConnection          patchCord6(mixer1, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=611,393
// GUItool: end automatically generated code



#include "bleep_base.h" //Then we can add this line that we will still need

//then you can declare any variables you want.
unsigned long current_time;
unsigned long prev_time[8]; //an array of 8 variables all named "prev_time"
float lfo1, lfo2, lfo3;
float freq1, temp1, temp2;
void setup() {
  start_bleep_base(); //run this first in setup

  //there's a lot we need to do in setup now but most of it is just copy paste.
  // This first group should only be done in setup how much RAM to set aside for the audio library to use.
  // The audio library uses blocks of a set size so this is not a percentage or kilobytes, just a kind of arbitrary number.
  // On our Teensy 4.0 we can go up to almost 2000 but that won't leave any RAM for anyone else.
  // It's usually the delay and reverb that hog it.
  AudioMemory(10);

  sgtl5000_1.enable(); //Turn the adapter board on
  sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN); //Tell it what input we want to use. Not necessary is you're not using the ins
  sgtl5000_1.lineInLevel(10); //The volume of the input. 0-15 with 15 bing more amplifications
  //sgtl5000_1.inputSelect(AUDIO_INPUT_MIC);
  //sgtl5000_1.micGain(13); //0 - 63bd of gain.

  //headphone jack output volume. Goes from 0.0 to 1.0 but a 100% signal will clip over .8 or so.
  // For headphones it's pretty loud at .4
  // There are lots of places we can change the final volume level.
  // For now lets set this one once and leave it alone.
  sgtl5000_1.volume(0.25);

  //The line out has a seperate level control but it's not meant to be adjusted like the volume function above.
  // If you're not using the line out don't worry about it.
  sgtl5000_1.lineOutLevel(21); //11-32, the smaller the louder. 21 is about 2 Volts peak to peak


  //This next group can be done anywhere in the code but we want to start things with these
  // values and change some of them in the loop.

  //Notice we start by writing the object we want, then a period, then the function
  // begin(volume from 0.0-1.0 , frequency , shape of oscillator)
  // See the tool for more info https://www.pjrc.com/teensy/gui/?info=AudioSynthWaveform
  waveform1.begin(1, 0.2, WAVEFORM_SINE);
  waveform2.begin(1, 0.01, WAVEFORM_SINE);

  //The mixer has four inputs we can change the volume of
  // gain.(channel from 0 to 3, gain from 0.0 to a large number)
  // A gain of 1 means the output is the same as the input.
  // .5 would be half volume and 2 would be double
  // Since we have two oscillators coming in that are already "1" We should take them down by half so we don't clip.
  // If you go over "1" The top or bottom of the wave is just slammed against a wall
  mixer1.gain(0, 1);
  mixer1.gain(1, 0);
  mixer1.gain(2, 0);
  mixer1.gain(3, 0);

} //setup is over

void loop() {
  update_controls();
  current_time = millis();


  //this is the confusing way to do it with floats
  temp1 = potRead(0); //0-1.0
  if (temp1 < .5) {//if its on one side do this
    //temp 1 is going from 0-.5 but we want it from 50-250
    // multiply by 2 to get it to 0-1
    // by 200 to get to 0-200
    // add 50 to get 50-250
    freq1 = (temp1 * 2.0 * 200.0) + 50.0; 
  }
  if (temp1 >= .5) {
    //on the other side
    //we want to start at 250 as thats where the other left off so add 250
    //temp is starting at .5 so subtract that and get it to 0-1.0 by multiplying by 2
    //we want it to go to 1000 so multiply by that
    freq1 = (((temp1 - .5) * 2.0) * 1000.0) + 250.0;
  }

  //or we could use map but it only takes integers
  //map(input value, fromLow, fromHigh, toLow, toHigh)

  int low_point = 200
  int middle_point = 800
  int high_point = 1200

  temp2 = potRead(0) * 1000; //0 - 10000
  if (temp2 < 500) {//on one side
    //we start at 0 and go to 500 on this side but want 200-800
    freq1 = map(temp2, 0, 500, low_point, middle_point); 
  }
  if (temp2 >= 500) {
    //now we start at 500 and get to 1000. 800 is where the other left off
    freq1 = map(temp2, 500, 1000, middle_point, high_point);
  }


  //another way to map is with an exponential converter
  // for audio, exponential curves sound more natural. 
  // Pretty much every volume knob you use has an exponential response, with the lower numbers going by more quickly than the higher ones
  // Heres a quick graph. You start and end at the same place but take a different route  http://fooplot.com/#W3sidHlwZSI6MCwiZXEiOiIoeF4yKS8oMTBeMSkiLCJjb2xvciI6IiMwMDAwMDAifSx7InR5cGUiOjAsImVxIjoiKHheMykvKDEwXjIpIiwiY29sb3IiOiIjMzkwOUQ5In0seyJ0eXBlIjowLCJlcSI6Iih4XjQpLygxMF4zKSIsImNvbG9yIjoiI0ZGODAwMCJ9LHsidHlwZSI6MCwiZXEiOiJ4IiwiY29sb3IiOiIjRkYwMDAwIn0seyJ0eXBlIjoxMDAwLCJ3aW5kb3ciOlsiMCIsIjEwIiwiMCIsIjEwIl19XQ--
  
  // to do this I mad a quick function at the bottom, outside the loop
  // expo_converter(input for 0-1.0, high value you want to get to, curve);
  // if curve is 1 its linear, <1 log >1 expo

  expo1 = expo_converter(potRead(0), 1000, 2);


  waveform1.frequency(freq1);

  if (current_time - prev_time[0] > 20) {
    prev_time[0] = current_time;

    //prints the linear and exponential response from the same pot
    Serial.print(temp2);
    Serial.print(" ");
    Serial.println(expo1);

  }



}// loop is over


//input 0-1, returns 0-max in expo curve
//max is 0 to whatever
//if curve is 1 its linear, <1 log >1 expo

float expo_converter(float input, float max1, float curve) {
  //powf is a more effeinct power for floats
  // powf(base, exponent)
  float ex1 = powf((1.0 - input) * max1, curve) / powf(max1, curve - 1.0); 
  return ex1;
}
