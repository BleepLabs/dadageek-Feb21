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
AudioSynthWaveform       waveform6;      //xy=145,362
AudioSynthWaveform       waveform5;      //xy=151,323
AudioSynthWaveform       waveform4;      //xy=158,290
AudioSynthWaveform       waveform7;      //xy=162,397
AudioSynthWaveform       waveform2;      //xy=163,213.00000381469727
AudioSynthWaveform       waveform3;      //xy=163,258
AudioSynthWaveform       waveform1;      //xy=179.00000190734863,166.00000190734863
AudioSynthWaveform       waveform8;      //xy=185,430
AudioMixer4              mixer2;         //xy=350.00000381469727,317.0000042915344
AudioMixer4              mixer1;         //xy=359.00000381469727,217.00000190734863
AudioMixer4              mixer3;         //xy=512.0000076293945,268.00000381469727
AudioOutputI2S           i2s1;           //xy=663.0000076293945,185.00000190734863
AudioConnection          patchCord1(waveform6, 0, mixer2, 1);
AudioConnection          patchCord2(waveform5, 0, mixer2, 0);
AudioConnection          patchCord3(waveform4, 0, mixer1, 3);
AudioConnection          patchCord4(waveform7, 0, mixer2, 2);
AudioConnection          patchCord5(waveform2, 0, mixer1, 1);
AudioConnection          patchCord6(waveform3, 0, mixer1, 2);
AudioConnection          patchCord7(waveform1, 0, mixer1, 0);
AudioConnection          patchCord8(waveform8, 0, mixer2, 3);
AudioConnection          patchCord9(mixer2, 0, mixer3, 1);
AudioConnection          patchCord10(mixer1, 0, mixer3, 0);
AudioConnection          patchCord11(mixer3, 0, i2s1, 0);
AudioConnection          patchCord12(mixer3, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=562,81
// GUItool: end automatically generated code



#include "bleep_base.h" //Then we can add this line that we will still need

//then you can declare any variables you want.
unsigned long current_time;
unsigned long prev_time[8]; //an array of 8 variables all named "prev_time"


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

  // begin(volume from 0.0-1.0 , frequency , shape of oscillator)
  waveform1.begin(1, 100.0, WAVEFORM_SINE); //we set their amplitudes like this to approximate a ramp wave
  waveform2.begin(.9, 100.0, WAVEFORM_SINE);
  waveform3.begin(.8, 100.0, WAVEFORM_SINE);
  waveform4.begin(.7, 100.0, WAVEFORM_SINE);
  waveform5.begin(.6, 100.0, WAVEFORM_SINE);
  waveform6.begin(.5, 100.0, WAVEFORM_SINE);
  waveform7.begin(.4, 100.0, WAVEFORM_SINE);
  waveform8.begin(.3, 100.0, WAVEFORM_SINE);


  //The mixer has four inputs we can change the volume of
  // gain.(channel from 0 to 3, gain from 0.0 to a large number)
  // A gain of 1 means the output is the same as the input.
  // .5 would be half volume and 2 would be double
  // Since we have two oscillators coming in that are already "1" We should take them down by half so we don't clip.
  // If you go over "1" The top or bottom of the wave is just slammed against a wall
  float attenuate = .125;
  mixer1.gain(0, attenuate);
  mixer1.gain(1, attenuate);
  mixer1.gain(2, attenuate);
  mixer1.gain(3, attenuate);
  mixer2.gain(0, attenuate);
  mixer2.gain(1, attenuate);
  mixer2.gain(2, attenuate);
  mixer2.gain(3, attenuate);

  mixer3.gain(0, 1);
  mixer3.gain(1, 1);
  mixer3.gain(2, 1);
  mixer3.gain(3, 1);

} //setup is over

float xfade1, xfade2;  //you can initialize variables here as long as you don't need them in setup. 
float freq[8];

void loop() {
  update_controls();
  current_time = millis();

  freq[1] = potRead(0) * 500.0;
  // it would be nice if we could do this with a "for" and use the increment to select the array of waveforms but the audio objects dont work like that easily.
  waveform1.frequency(freq[1]); 
  waveform2.frequency(freq[1] * 2.0);
  waveform3.frequency(freq[1] * 3.0);
  waveform4.frequency(freq[1] * 4.0);

  waveform5.frequency(freq[1] * 5.0);
  waveform6.frequency(freq[1] * 6.0);
  waveform7.frequency(freq[1] * 7.0);
  waveform8.frequency(freq[1] * 8.0);


  /*
    xfade1 = potRead(0);
    xfade2 = 1.0 - xfade1;

    mixer1.gain(0, xfade1);
    mixer1.gain(1, xfade2);
  */

  if (current_time - prev_time[0] > 50 && 1) {
    prev_time[0] = current_time;
    Serial.print("processor: ");
    Serial.print(AudioProcessorUsageMax());
    Serial.print("%    Memory: ");
    Serial.print(AudioMemoryUsageMax());
    Serial.println();
    AudioProcessorUsageMaxReset(); //We need to reset these values so we get a real idea of what the audio code is doing rather than just peaking in every half a second
    AudioMemoryUsageMaxReset();
  }

}// loop is over
