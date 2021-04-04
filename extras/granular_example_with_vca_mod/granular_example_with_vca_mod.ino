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
AudioSynthWaveform       waveform1;      //xy=118,571
AudioSynthWaveformDc     dc1;            //xy=120,637
AudioInputI2S            i2s2;           //xy=215,409
AudioMixer4              mixer2;         //xy=272,608
AudioEffectMultiply      multiply1;      //xy=374,501
AudioEffectGranular      granular1;      //xy=432,403
AudioMixer4              mixer1;         //xy=648,421
AudioOutputI2S           i2s1;           //xy=809,360
AudioConnection          patchCord1(waveform1, 0, mixer2, 0);
AudioConnection          patchCord2(dc1, 0, mixer2, 1);
AudioConnection          patchCord3(i2s2, 0, granular1, 0);
AudioConnection          patchCord4(i2s2, 0, multiply1, 0);
AudioConnection          patchCord5(mixer2, 0, multiply1, 1);
AudioConnection          patchCord6(multiply1, 0, mixer1, 1);
AudioConnection          patchCord7(granular1, 0, mixer1, 0);
AudioConnection          patchCord8(mixer1, 0, i2s1, 0);
AudioConnection          patchCord9(mixer1, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=631,559
// GUItool: end automatically generated code



#include "bleep_base.h" //Then we can add this line that we will still need

//then you can declare any variables you want.
unsigned long current_time;
unsigned long prev_time[8]; //an array of 8 variables all named "prev_time"

#define GRANULAR_MEMORY_SIZE 12800  // enough for 290 ms at 44.1 kHz
int16_t granularMemory[GRANULAR_MEMORY_SIZE];


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

  waveform1.begin(1, 400, WAVEFORM_SQUARE);
  mixer2.gain(0, .5);
  mixer2.gain(1, 1);

  dc1.amplitude(.5);

  granular1.begin(granularMemory, GRANULAR_MEMORY_SIZE);

  mixer1.gain(0, 0);
  mixer1.gain(1, 1);
  mixer1.gain(2, 0);
  mixer1.gain(3, 0);

} //setup is over

void loop() {
  update_controls();
  current_time = millis();

  if (buttonState(0) == FELL) {
    float msec = potRead(0) * 200.0;
    granular1.beginFreeze(msec);
  }
  if (buttonState(0) == ROSE) {
    granular1.stop();
  }

  if (buttonState(1) == FELL) {
    float msec = potRead(0) * 200.0;
    granular1.beginPitchShift(msec);
  }
  if (buttonState(1) == ROSE) {
    granular1.stop();
  }

  granular1.setSpeed(potRead(1) * 4.0);

  //We don't have to do anything in the loop since the audio library will jut keep doing what we told it in the setup
  if (current_time - prev_time[0] > 500) {
    prev_time[0] = current_time;

    //Here we print out the usage of the audio library
    // If we go over 90% processor usage or get near the value of memory blocks we set aside in the setup we'll have issues or crash.
    // If you're using too many block, jut increas the number up top untill you're over it by a few
    Serial.print("processor: ");
    Serial.print(AudioProcessorUsageMax());
    Serial.print("%    Memory: ");
    Serial.print(AudioMemoryUsageMax());
    Serial.println();
    AudioProcessorUsageMaxReset(); //We need to reset these values so we get a real idea of what the audio code is doing rather than just peaking in every half a second
    AudioMemoryUsageMaxReset();
  }

}// loop is over
