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
float freq1, temp1, temp2, expo1;
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


  //freq1 = (1-potRead(0)) * 500;
  temp1 = potRead(0);
  if (temp1 < .5) {
    freq1 = (temp1 * 2.0 * 200.0) + 50.0;
  }
  if (temp1 >= .5) {
    freq1 = (((temp1 - .5) * 2.0) * 1000.0) + 250.0;
  }

  temp2 = (1 - potRead(0)) * 2000.0;
  if (temp2 < 500) {
    freq1 = map(temp2, 0, 500, 200, 800); //map is only for integers
  }
  if (temp2 >= 500) {
    freq1 = map(temp2, 500, 1000, 800, 1000);
  }

  //expo1 = powf((1 - potRead(0)) * 1000.0, 2) / powf(1000.0, 1);
  expo1 = expo_converter(potRead(0), 2000, 3.5);

  waveform1.frequency(expo1 + 220);



  if (current_time - prev_time[1] > 10) {
    prev_time[1] = current_time;
    lfo1 += .02;
    if (lfo1 >= 1.0) {
      lfo1 = 0;
    }

    if (peak1.available()) {
      lfo2 = peak1.read();
    }
    if (peak2.available()) {
      lfo3 = peak2.read();
    }


    for (int ledj = 2; ledj < 8; ledj++) {
      float hue1 = (ledj - 2) * .15;
      float hue2 = hue1 + lfo1;
      if (hue2 >= 1.0) {
        hue2 -= 1.0;
      }
      set_LED(ledj, hue2, lfo2, lfo3);
    }


    LEDs.show();

  }


  //We don't have to do anything in the loop since the audio library will jut keep doing what we told it in the setup
  if (current_time - prev_time[0] > 20) {
    prev_time[0] = current_time;

    Serial.print(temp2);
    Serial.print(" ");
    Serial.println(expo1);

  }

}// loop is over

//input 0-1, returns 0-max in expo curve
//max is 0 to whatever
//if curve is 1 its linear, <1 log >1 expo
float expo_converter(float input, float max1, float curve) {
  float ex1 = powf((1.0 - input) * max1, curve) / powf(max1, curve - 1.0);
  return ex1;
}
