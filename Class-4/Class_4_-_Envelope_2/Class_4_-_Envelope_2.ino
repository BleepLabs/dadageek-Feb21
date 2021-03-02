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
AudioSynthWaveform       waveform2;      //xy=272,247
AudioSynthWaveform       waveform3;      //xy=272,369
AudioSynthWaveform       waveform1;      //xy=274,205
AudioAnalyzePeak         peak1;          //xy=414,352
AudioMixer4              mixer1;         //xy=451,249
AudioFilterStateVariable filter1;        //xy=607,239
AudioEffectEnvelope      envelope1;      //xy=796,257
AudioOutputI2S           i2s1;           //xy=852,330
AudioConnection          patchCord1(waveform2, 0, mixer1, 1);
AudioConnection          patchCord2(waveform3, peak1);
AudioConnection          patchCord3(waveform1, 0, mixer1, 0);
AudioConnection          patchCord4(mixer1, 0, filter1, 0);
AudioConnection          patchCord5(filter1, 0, envelope1, 0);
AudioConnection          patchCord6(envelope1, 0, i2s1, 0);
AudioConnection          patchCord7(envelope1, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=614.4999389648438,388
// GUItool: end automatically generated code

#include "bleep_base.h" //Then we can add this line that we will still need

//then you can declare any variables you want.
unsigned long current_time;
unsigned long prev_time[8]; //an array of 8 variables all named "prev_time"
int current_button_reading[6];
int prev_reading[6];
float attack_adj, sustain_adj, release_adj;

void setup() {
  start_bleep_base(); //run this first in setup

  //How much RAM to set aside for the audio library to use.
  // The audio library uses blocks of a set size so this is not a percentage or kilobytes, just a kind of arbitrary number.
  // On our Teensy 3.2 we can go up to about 200 but that won't leave any RAM for anyone else.
  // It's usually the delay and reverb that hog it.
  AudioMemory(10);

  sgtl5000_1.enable(); //Turn the adapter board on
  sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN); //Tell it what input we want to use. Not necessary yet but good to have
  sgtl5000_1.lineInLevel(13); //The volume of the input. Again we'll get to this later

  //Output volume. Goes from 0.0 to 1.0 but a fully loud signal will clip over .8 or so.
  // For headphones it's pretty loud at .4
  // There are lots of places we can change the final volume level. This one you set once and leave alone.
  sgtl5000_1.volume(0.25);

  //This next group can be done anywhere in the code but we want to start things with these
  // values and change some of them in the loop.

  //Notice we start by writing the object we want, then a period, then the function
  // begin(volume from 0.0-1.0 , frequency , shape of oscillator)
  // See the tool for more info https://www.pjrc.com/teensy/gui/?info=AudioSynthWaveform
  waveform1.begin(1, 220.0, WAVEFORM_SAWTOOTH);
  waveform2.begin(1, 440.0, WAVEFORM_SAWTOOTH);
  waveform3.begin(1, 440.0, WAVEFORM_SINE);

  filter1.frequency(12000);
  filter1.resonance(.7);

  envelope1.sustain(1.0);

  //The mixer has four inputs we can change the volume of
  // gain.(channel from 0 to 3, gain from 0.0 to a large number)
  // A gain of 1 means the output is the same as the input.
  // .5 would be half volume and 2 would be double
  // Since we have two oscillators coming in that are already "1" We should take them down by half so we don't clip.
  // If you go over "1" The top or bottom of the wave is just slammed against a wall
  mixer1.gain(0, .4);
  mixer1.gain(1, .4);
  mixer1.gain(2, 0);
  mixer1.gain(3, 0);

}

void loop() {
  update_controls();
  current_time = millis();

  prev_reading[0] = current_button_reading[0];
  current_button_reading[0] = buttonRead(0);

  if (prev_reading[0] == 1 && current_button_reading[0] == 0) {
    envelope1.noteOn();
    waveform1.frequency(220.0);
    waveform2.frequency(220.0 * 2.0);
  }
  
  if (prev_reading[0] == 0 && current_button_reading[0] == 1) {
    envelope1.noteOff();
  }

  prev_reading[1] = current_button_reading[1];
  current_button_reading[1] = buttonRead(1);

  if (prev_reading[1] == 1 && current_button_reading[1] == 0) {
    envelope1.noteOn();
    waveform1.frequency(233.0819);
    waveform2.frequency(233.0819 * 2.0);
  }
  if (prev_reading[1] == 0 && current_button_reading[1] == 1) {
    envelope1.noteOff();
  }

  attack_adj = (1.0 - potRead(0)) * 1000.0;
  envelope1.attack(attack_adj);

  release_adj = (1.0 - potRead(1)) * 2500.0;
  envelope1.release(release_adj);




  //We don't have to do anything in the loop since the audio library will jut keep doing what we told it in the setup
  if (current_time - prev_time[0] > 50) {
    prev_time[0] = current_time;
    Serial.println(attack_adj);
    Serial.println(release_adj);
    Serial.println();

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
