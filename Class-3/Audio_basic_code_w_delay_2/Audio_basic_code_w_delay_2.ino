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
AudioSynthWaveform       waveform3;      //xy=113.00566482543945,186.00564193725586
AudioSynthWaveform       waveform4;      //xy=130.0056610107422,244.0056610107422
AudioSynthWaveform       waveform2;      //xy=130.9999713897705,137.99996280670166
AudioSynthWaveform       waveform1;      //xy=153.99996948242188,91.99997901916504
AudioMixer4              mixer1;         //xy=313.9999237060547,165.9999542236328
AudioEffectDelay         delay1;         //xy=474.75568771362305,391.50562286376953
AudioMixer4              mixer2;         //xy=484.5054988861084,262.2555961608887
AudioMixer4              mixer3;         //xy=563.2556304931641,153.75567626953125
AudioEffectDelay         delay2;         //xy=710.0071182250977,238.75708961486816
AudioOutputI2S           i2s1;           //xy=788.7498474121094,90.99997282028198
AudioConnection          patchCord1(waveform3, 0, mixer1, 2);
AudioConnection          patchCord2(waveform4, 0, mixer1, 3);
AudioConnection          patchCord3(waveform2, 0, mixer1, 1);
AudioConnection          patchCord4(waveform1, 0, mixer1, 0);
AudioConnection          patchCord5(mixer1, 0, mixer2, 0);
AudioConnection          patchCord6(mixer1, 0, mixer3, 0);
AudioConnection          patchCord7(delay1, 0, mixer2, 1);
AudioConnection          patchCord8(mixer2, delay1);
AudioConnection          patchCord9(mixer2, 0, mixer3, 1);
AudioConnection          patchCord10(mixer3, 0, i2s1, 0);
AudioConnection          patchCord11(mixer3, delay2);
AudioConnection          patchCord12(delay2, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=181.9999237060547,546.9999809265137
// GUItool: end automatically generated code



#include "bleep_base.h" //Then we can add this line that we will still need

//then you can declare any variables you want.
unsigned long current_time;
unsigned long prev_time[8]; //an array of 8 variables all named "prev_time"
float freq1;
float freq2;
float freq3;
float freq4;
float amp1, amp2, amp3, amp4;
float dtime;

void setup() {
  start_bleep_base(); //run this first in setup

  //there's a lot we need to do in setup now but most of it is just copy paste.
  // This first group should only be done in setup how much RAM to set aside for the audio library to use.
  // The audio library uses blocks of a set size so this is not a percentage or kilobytes, just a kind of arbitrary number.
  // On our Teensy 4.0 we can go up to almost 2000 but that won't leave any RAM for anyone else.
  // It's usually the delay and reverb that hog it.
  AudioMemory(1000);

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
  waveform1.begin(1, 220.0, WAVEFORM_SAWTOOTH);
  waveform2.begin(1, 440.0, WAVEFORM_SQUARE);
  waveform3.begin(1, 220.0, WAVEFORM_SAWTOOTH);
  waveform4.begin(1, 440.0, WAVEFORM_SQUARE);

  //The mixer has four inputs we can change the volume of
  // gain.(channel from 0 to 3, gain from 0.0 to a large number)
  // A gain of 1 means the output is the same as the input.
  // .5 would be half volume and 2 would be double
  // Since we have two oscillators coming in that are already "1" We should take them down by half so we don't clip.
  // If you go over "1" The top or bottom of the wave is just slammed against a wall
  mixer1.gain(0, .25);
  mixer1.gain(1, .25);
  mixer1.gain(2, .25);
  mixer1.gain(3, .25);

  mixer2.gain(0, .5);
  mixer2.gain(1, .5);

  mixer3.gain(0, .5);
  mixer3.gain(1, .5);

  delay1.delay(0, 1000);
  delay2.delay(0, 50);

} //setup is over


void loop() {
  update_controls();
  current_time = millis();

  freq1 = (1.0 - potRead(0)) * 1000.0;
  //freq1=potRead(0)*1000.0;
  waveform1.frequency(freq1);

  freq2 = freq1 * (potRead(1) + 1.0);
  waveform2.frequency(freq2);

  freq3 = freq1 * (potRead(2) + 1.0);
  waveform3.frequency(freq3);

  freq4 = freq1 * (potRead(3) + 1.0);
  waveform4.frequency(freq4);

  amp1 = 1.0 - potRead(4);
  mixer2.gain(1, amp1);

  amp2 = potRead(5);
  amp3 = 1.0 - amp2;

  mixer3.gain(0, amp2);
  mixer3.gain(1, amp3);

  dtime = potRead(6) * 1000.0; 
  // delay1.delay(0, dtime);  //sounds glitchy


  /*
    amp1 = potRead(4) * .25;
    mixer1.gain(0, amp1);

    amp2 = potRead(5) * .25;
    mixer1.gain(1, amp2);

    amp3 = potRead(6) * .25;
    mixer1.gain(2, amp3);

    amp4 = potRead(7) * .25;
    mixer1.gain(3, amp4);
  */

  //We don't have to do anything in the loop since the audio library will jut keep doing what we told it in the setup
  if (current_time - prev_time[0] > 100) {
    prev_time[0] = current_time;
    Serial.println(amp2);
    Serial.println(amp3);

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
