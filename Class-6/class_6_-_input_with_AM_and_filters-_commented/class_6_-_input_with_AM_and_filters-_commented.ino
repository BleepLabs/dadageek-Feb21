/*
  the audio input is sent through a multiply effect, modulated by two oscillators, then the two filters

*/


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
AudioSynthWaveform       waveform2;      //xy=135,209
AudioSynthWaveform       waveform1;      //xy=151,141
AudioInputI2S            i2s2;           //xy=157,368
AudioMixer4              mixer3;         //xy=306,226
AudioEffectMultiply      multiply1;      //xy=340,334
AudioFilterStateVariable filter1;        //xy=475,138
AudioMixer4              mixer1;         //xy=527,291
AudioFilterStateVariable filter2;        //xy=636,127
AudioMixer4              mixer2;         //xy=752,276
AudioOutputI2S           i2s1;           //xy=926,169
AudioConnection          patchCord1(waveform2, 0, mixer3, 1);
AudioConnection          patchCord2(waveform1, 0, mixer3, 0);
AudioConnection          patchCord3(i2s2, 0, multiply1, 1);
AudioConnection          patchCord4(mixer3, 0, multiply1, 0);
AudioConnection          patchCord5(multiply1, 0, mixer1, 0);
AudioConnection          patchCord6(filter1, 0, filter2, 0);
AudioConnection          patchCord7(mixer1, 0, filter1, 0);
AudioConnection          patchCord8(mixer1, 0, mixer2, 1);
AudioConnection          patchCord9(filter2, 2, mixer2, 0);
AudioConnection          patchCord10(mixer2, 0, i2s1, 0);
AudioConnection          patchCord11(mixer2, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=749,430
// GUItool: end automatically generated code


#include "bleep_base.h" //Then we can add this line that we will still need

//then you can declare any variables you want.
unsigned long current_time;
unsigned long prev_time[8]; //an array of 8 variables all named "prev_time"
float freq[2];
//const is constant stored in flash mem aka program space
const float chromatic[88] = {55.00000728, 58.27047791, 61.73542083, 65.40639999, 69.29566692, 73.4162017, 77.78175623, 82.40690014, 87.30706942, 92.49861792, 97.99887197, 103.8261881, 110.0000146, 116.5409558, 123.4708417, 130.8128, 138.5913338, 146.8324034, 155.5635124, 164.8138003, 174.6141388, 184.9972358, 195.9977439, 207.6523763, 220.0000291, 233.0819116, 246.9416833, 261.6255999, 277.1826676, 293.6648067, 311.1270248, 329.6276005, 349.2282776, 369.9944716, 391.9954878, 415.3047525, 440.0000581, 466.1638231, 493.8833665, 523.2511997, 554.3653352, 587.3296134, 622.2540496, 659.2552009, 698.4565551, 739.9889431, 783.9909755, 830.6095048, 880.0001162, 932.3276461, 987.7667329, 1046.502399, 1108.73067, 1174.659227, 1244.508099, 1318.510402, 1396.91311, 1479.977886, 1567.981951, 1661.219009, 1760.000232, 1864.655292, 1975.533466, 2093.004798, 2217.46134, 2349.318453, 2489.016198, 2637.020803, 2793.82622, 2959.955772, 3135.963901, 3322.438019, 3520.000464, 3729.310584, 3951.066931, 4186.009596, 4434.92268, 4698.636906, 4978.032395, 5274.041605, 5587.652439, 5919.911543, 6271.927802, 6644.876037, 7040.000927, 7458.621167, 7902.133861, 8372.019192};
// byte is the smallest 0-255 so i can save memory
const byte major[65] = {1, 3, 5, 6, 8, 10, 12, 13, 15, 17, 18, 20, 22, 24, 25, 27, 29, 30, 32, 34, 36, 37, 39, 41, 42, 44, 46, 48, 49, 51, 53, 54, 56, 58, 60, 61, 63, 65, 66, 68, 70, 72, 73, 75, 77, 78, 80, 82, 84, 85, 87, 89, 90, 92, 94, 96, 97, 99, 101, 102};
int melody1[8] = {15, 22, 12, 14, 22, 15, 17, 30};
int pos_a = 20;
int pos_b = 50;
int inc1;
int modulo_inc1;
int major_inc1, melody_inc1;
int melody_rate;
int direction1, prev_direction1;
int ra1, ra2;
int dice1, dice2, diceses3;
int current_button_reading[6];
int prev_reading[6];
int random_mode, playback_mode;
int final_inc1;
float hue1;
float filter_freq, filter_freq_2;
float lfo;

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
  sgtl5000_1.lineOutLevel(10); //11-32, the smaller the louder. 21 is about 2 Volts peak to peak


  //This next group can be done anywhere in the code but we want to start things with these
  // values and change some of them in the loop.

  //Notice we start by writing the object we want, then a period, then the function
  // begin(volume from 0.0-1.0 , frequency , shape of oscillator)
  // See the tool for more info https://www.pjrc.com/teensy/gui/?info=AudioSynthWaveform
  waveform1.begin(1, 220.0, WAVEFORM_SINE);
  waveform2.begin(1, 440.0, WAVEFORM_SINE);

  //The mixer has four inputs we can change the volume of
  // gain.(channel from 0 to 3, gain from 0.0 to a large number)
  // A gain of 1 means the output is the same as the input.
  // .5 would be half volume and 2 would be double
  // Since we have two oscillators coming in that are already "1" We should take them down by half so we don't clip.
  // If you go over "1" The top or bottom of the wave is just slammed against a wall
  mixer1.gain(0, .8); //before going into the filters, turn down the amplitude a little as the resonace will add to it and maybe clip
  mixer1.gain(1, 0);
  mixer1.gain(2, 0);
  mixer1.gain(3, 0);

  mixer2.gain(0, 0); //these will be set with pots later so it's not really needed here
  mixer2.gain(1, 0);
  mixer2.gain(2, 0);
  mixer2.gain(3, 0);

  mixer3.gain(0, .5); //two oscillators going to the multiply effect
  mixer3.gain(1, .5);
  mixer3.gain(2, 0);
  mixer3.gain(3, 0);

  //resonance, aka feedback or q, makes the harmonics around the cutoff frequency louder
  // over .7 and the resonance will start to come out.
  filter1.resonance(3.0);
  filter2.resonance(3.0);

} //setup is over


void loop() {
  update_controls();
  current_time = millis();

  filter_freq = (potRead(4) * 10000.0) ; //just pot control
  filter1.frequency(filter_freq);
  filter_freq_2 = (potRead(5) * 10000);
  filter2.frequency(filter_freq_2);

  //the filter has 3 outs, low pass, high pass and band pass
  // if you combine the signals you'll get some interesting new harmonics as some frequencies will be increased and decreased.
  // inverting the phase of one by having the mixer negative will change what is phased in our out
  mixer2.gain(0, potRead(6)); //0 to 1 wet signal from filters
  mixer2.gain(1, (potRead(7) * 2.0) - 1.0); //-1.0 to 1.0 dry signal from oscillators

  prev_reading[0] = current_button_reading[0];
  current_button_reading[0] = buttonRead(0);

  if (prev_reading[0] == 1 && current_button_reading[0] == 0) {// 1 is not pressed 0 is pressed
    random_mode = !random_mode; //only works if you are toggling between 0 and 1
  }

  prev_reading[1] = current_button_reading[1];
  current_button_reading[1] = buttonRead(1);

  if (prev_reading[1] == 1 && current_button_reading[1] == 0) {
    playback_mode = playback_mode + 1;
    if (playback_mode > 2) {
      playback_mode = 0;
    }
  }

  melody_rate = 200; //fix the melody rate as we're out of pots and might also just use a static note

  pos_a = potRead(1) * 50.0;
  pos_b = potRead(2) * 50.0;

  freq[0] = potRead(3) * 100.0;
  freq[1] = potRead(4) * 10.0;

  waveform1.frequency(freq[0]); //the two oscillators that will multiply aka AM or ring-mod the audio input
  waveform2.frequency(freq[1]);



  /////////////////////////////////////////////////////////////////////////////////////// LED


  if (current_time - prev_time[3] > 33) { //the leds only need to be set as fast as we can see them. 33ms is about 30Hz
    prev_time[3] = current_time;
    hue1 = playback_mode / 5.0;

    float pot_hue = potRead(4);
    float pot_hue2 = pot_hue + .2;
    if (pot_hue2 > 1.0) {
      pot_hue2 -= 1.0; //you can't do modulo on floats but if it's not wrapping around more than once this works well
    }
    float pot_sat = potRead(5);
    float pot_b = potRead(6);

    //these are used to set the leds but they only change to these values when "LEDs.show();" is executed
    set_LED(0, pot_hue, pot_sat, pot_b); //set_LED(led select,hue,saturation,brightness) H S B are all 0-1.0
    set_LED(1, pot_hue2, pot_sat, pot_b);

    LEDs.show(); //must be done to actually send the data to the LEDs
  }

  ///////////////////////////////////////////////////////////////////////////////////// print town


  if (current_time - prev_time[0] > 500 && 0) { //&& is also. 0 means it wont happen, 1 will
    prev_time[0] = current_time;

    //Here we print out the usage of the audio library
    // If we go over 90% processor usage or get near the value of memory blocks we set aside in the setup we'll have issues or crash.
    // If you're using too many block, jut increase the number up top until you're over it by a few
    Serial.print("processor: ");
    Serial.print(AudioProcessorUsageMax());
    Serial.print("%    Memory: ");
    Serial.print(AudioMemoryUsageMax());
    Serial.println();
    AudioProcessorUsageMaxReset(); //We need to reset these values so we get a real idea of what the audio code is doing rather than just peaking in every half a second
    AudioMemoryUsageMaxReset();
  }

}// loop is over
