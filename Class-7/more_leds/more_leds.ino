// talking to more LEDs using the header and reviewing for loops
// LED info here https://github.com/BleepLabs/dadageek-Feb21/wiki/Using-LEDs-on-the-Bleep-Base




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

void setup() {
  start_bleep_base(); //run this first in setup
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

  //The line out has a separate level control but it's not meant to be adjusted like the volume function above.
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

  if (current_time - prev_time[1] > 10) {
    prev_time[1] = current_time;
    lfo1 += .02;
    if (lfo1 >= 1.0) {
      lfo1 = 0;
    }

    if (peak1.available()) { //read the level of the audio waveforms and return 0-1.0
      lfo2 = peak1.read();
    }
    if (peak2.available()) {
      lfo3 = peak2.read();
    }


    for (int ledj = 2; ledj < 8; ledj++) { //start at the 2nd led and go to the 7th. The first 2 leds are on the bleep base
      
      //ledj is incrementing so use it to make each LED a different color. 
      // since this is all happening in the for loop the lights will be static though. Nothing outside the loop is changing it 
      float hue1 = (ledj - 2) * .15; 

      float hue2 = hue1 + lfo1; //lfo is going at it's own rate, outside of this for loop so the lights will change
      if (hue2 >= 1.0) {
        //rather than going back to 0, we subtract 1.0
        // that way it cleanly goes around in a loop as thats how the hue rainbow works
        // if it's 1.2, it comes out .2
        hue2 -= 1.0; 
      }
      set_LED(ledj, hue2, lfo2, lfo3); //(led to change, hue, satuuration,brightness)
    }


    LEDs.show();

  }


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
