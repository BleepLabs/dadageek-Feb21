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
AudioSynthWaveform       waveform1;      //xy=72,185
AudioSynthWaveform       waveform3;      //xy=95,362
AudioSynthWaveformDc     dc1;            //xy=115,448
AudioSynthWaveform       waveform2;      //xy=140,292
AudioEffectEnvelope      envelope1;      //xy=221,184
AudioEffectEnvelope      envelope3;      //xy=252,357
AudioEffectEnvelope      envelope2;      //xy=278,443
AudioFilterStateVariable filter1;        //xy=392,240
AudioFilterStateVariable filter2;        //xy=399,359
AudioMixer4              mixer1;         //xy=572,280
AudioOutputI2S           i2s1;           //xy=749,288
AudioConnection          patchCord1(waveform1, envelope1);
AudioConnection          patchCord2(waveform3, envelope3);
AudioConnection          patchCord3(dc1, envelope2);
AudioConnection          patchCord4(waveform2, 0, filter1, 1);
AudioConnection          patchCord5(envelope1, 0, filter1, 0);
AudioConnection          patchCord6(envelope3, 0, filter2, 0);
AudioConnection          patchCord7(envelope2, 0, filter2, 1);
AudioConnection          patchCord8(filter1, 0, mixer1, 0);
AudioConnection          patchCord9(filter2, 0, mixer1, 1);
AudioConnection          patchCord10(mixer1, 0, i2s1, 0);
AudioConnection          patchCord11(mixer1, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=611,393
// GUItool: end automatically generated code



#include "bleep_base.h" //Then we can add this line that we will still need

//then you can declare any variables you want.
unsigned long current_time;
unsigned long prev_time[8]; //an array of 8 variables all named "prev_time"
const float chromatic[88] = {55.00000728, 58.27047791, 61.73542083, 65.40639999, 69.29566692, 73.4162017, 77.78175623, 82.40690014, 87.30706942, 92.49861792, 97.99887197, 103.8261881, 110.0000146, 116.5409558, 123.4708417, 130.8128, 138.5913338, 146.8324034, 155.5635124, 164.8138003, 174.6141388, 184.9972358, 195.9977439, 207.6523763, 220.0000291, 233.0819116, 246.9416833, 261.6255999, 277.1826676, 293.6648067, 311.1270248, 329.6276005, 349.2282776, 369.9944716, 391.9954878, 415.3047525, 440.0000581, 466.1638231, 493.8833665, 523.2511997, 554.3653352, 587.3296134, 622.2540496, 659.2552009, 698.4565551, 739.9889431, 783.9909755, 830.6095048, 880.0001162, 932.3276461, 987.7667329, 1046.502399, 1108.73067, 1174.659227, 1244.508099, 1318.510402, 1396.91311, 1479.977886, 1567.981951, 1661.219009, 1760.000232, 1864.655292, 1975.533466, 2093.004798, 2217.46134, 2349.318453, 2489.016198, 2637.020803, 2793.82622, 2959.955772, 3135.963901, 3322.438019, 3520.000464, 3729.310584, 3951.066931, 4186.009596, 4434.92268, 4698.636906, 4978.032395, 5274.041605, 5587.652439, 5919.911543, 6271.927802, 6644.876037, 7040.000927, 7458.621167, 7902.133861, 8372.019192};


void setup() {
  start_bleep_base(); //run this first in setup

  //there's a lot we need to do in setup now but most of it is just copy paste.
  // This first group should only be done in setup how much RAM to set aside for the audio library to use.
  // The audio library uses blocks of a set size so this is not a percentage or kilobytes, just a kind of arbitrary number.
  // On our Teensy 4.0 we can go up to almost 2000 but that won't leave any RAM for anyone else.
  // It's usually the delay and reverb that hog it.
  AudioMemory(20);

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
  waveform1.begin(1, 220.0, WAVEFORM_SQUARE);
  waveform2.begin(1, 440.0, WAVEFORM_SINE);
  waveform3.begin(1, 440.0, WAVEFORM_SQUARE);

  //The mixer has four inputs we can change the volume of
  // gain.(channel from 0 to 3, gain from 0.0 to a large number)
  // A gain of 1 means the output is the same as the input.
  // .5 would be half volume and 2 would be double
  // Since we have two oscillators coming in that are already "1" We should take them down by half so we don't clip.
  // If you go over "1" The top or bottom of the wave is just slammed against a wall
  mixer1.gain(0, .5); //LFO modualted filter
  mixer1.gain(1, .5); //Envelope modualted filter
  mixer1.gain(2, 0);
  mixer1.gain(3, 0);

  dc1.amplitude(1);  //dc1 will jsut put out put a 1 which we will attenuatewith an envelope

} //setup is over

void loop() {
  update_controls();
  current_time = millis();


  if (buttonState(0) == FELL) {
    envelope1.attack(50); // We would only need to cahnge these setting when the envelope is about to be used
    envelope1.decay(50);
    envelope1.sustain(.75);
    envelope1.release(250);
    envelope1.noteOn();
  }

  if (buttonState(0) == ROSE) {
    envelope1.noteOff();
  }

  int note_select = ((1.0 - potRead(0)) * 40.0)+10; //use a pot to select a not in the chromatic array
  float freq1 = chromatic[note_select];
  waveform1.frequency(freq1);

  float freq2 = (1.0 - potRead(1)) * 10.0;
  waveform2.frequency(freq2);
  float filterfreq1 = (1.0 - potRead(2)) * 15000.0;
  filter1.frequency(filterfreq1);
  float depth1 = (1.0 - potRead(3)) * 4.0; //how much wavefomr 2 will effect the cuttof frequency
  filter1.octaveControl(depth1);
  filter1.resonance(2.0);

  if (buttonState(3) == FELL) {
    //env2 is for filter2
    envelope2.attack(1000); // We would only need to cahnge these setting when the envelope is about to be used
    envelope2.decay(0);
    envelope2.sustain(1);
    envelope2.release(500);
    envelope2.noteOn();

    //env3 is for wavefrom3
    envelope3.attack(100);
    envelope3.decay(50);
    envelope3.sustain(.8);
    envelope3.release(1000);
    envelope3.noteOn();
  }

  if (buttonState(3) == ROSE) {
    envelope2.noteOff();
    envelope3.noteOff();
  }

  int note_select2 = ((1.0 - potRead(4)) * 40.0)+10; //use a pot to select a not in the chromatic array
  float freq3 = chromatic[note_select2];
  waveform3.frequency(freq3);

  float filterfreq2 = (1.0 - potRead(6)) * 15000.0;
  filter2.frequency(filterfreq2);
  float depth2 = (1.0 - potRead(7)) * 4.0; //how much wavefomr 2 will effect the cuttof frequency
  filter2.octaveControl(depth2);
  filter2.resonance(2.0);





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
