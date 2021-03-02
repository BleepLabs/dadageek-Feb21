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
AudioSynthWaveform       waveform1;      //xy=262,197
AudioSynthWaveform       waveform3;      //xy=274,516
AudioSynthWaveformDc     dc1;            //xy=276,366
AudioSynthWaveform       waveform2;      //xy=278,240
AudioAnalyzePeak         peak1;          //xy=422,554
AudioMixer4              mixer1;         //xy=439,220
AudioEffectEnvelope      envelope2;      //xy=444,369
AudioFilterStateVariable filter1;        //xy=613,273
AudioEffectEnvelope      envelope1;      //xy=639,197
AudioEffectDelay         delay1;         //xy=841,414
AudioMixer4              mixer2;         //xy=845,268
AudioOutputI2S           i2s1;           //xy=1019,215
AudioConnection          patchCord1(waveform1, 0, mixer1, 0);
AudioConnection          patchCord2(waveform3, peak1);
AudioConnection          patchCord3(dc1, envelope2);
AudioConnection          patchCord4(waveform2, 0, mixer1, 1);
AudioConnection          patchCord5(mixer1, 0, filter1, 0);
AudioConnection          patchCord6(envelope2, 0, filter1, 1);
AudioConnection          patchCord7(filter1, 0, envelope1, 0);
AudioConnection          patchCord8(envelope1, 0, mixer2, 0);
AudioConnection          patchCord9(delay1, 0, mixer2, 1);
AudioConnection          patchCord10(mixer2, 0, i2s1, 0);
AudioConnection          patchCord11(mixer2, 0, i2s1, 1);
AudioConnection          patchCord12(mixer2, delay1);
AudioControlSGTL5000     sgtl5000_1;     //xy=167.49993896484375,131
// GUItool: end automatically generated code


#include "bleep_base.h" //Then we can add this line that we will still need

//then you can declare any variables you want.
unsigned long current_time, last_time;
unsigned long prev_time[8]; //an array of 8 variables all named "prev_time"
int current_button_reading[6];
int prev_reading[6];
float attack_adj, sustain_adj, release_adj;
float freq1;
float note_offset;
float fiter_freq;
float lfo_reading;
float lfo_freq;


void setup() {
  start_bleep_base(); //run this first in setup


  AudioMemory(200);

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
  waveform3.begin(1, 0.5, WAVEFORM_SINE);

  filter1.frequency(12000);
  filter1.resonance(4.5);

  envelope1.sustain(1.0);
  dc1.amplitude(1.0);

  envelope2.attack(200);
  envelope2.decay(200);
  envelope2.sustain(0.4);
  envelope2.release(500);




  //The mixer has four inputs we can change the volume of
  // gain.(channel from 0 to 3, gain from 0.0 to a large number)
  // A gain of 1 means the output is the same as the input.
  // .5 would be half volume and 2 would be double
  // Since we have two oscillators coming in that are already "1" We should take them down by half so we don't clip.
  // If you go over "1" The top or bottom of the wave is just slammed against a wall
  mixer1.gain(0, .3);
  mixer1.gain(1, .3);
  mixer1.gain(2, 0);
  mixer1.gain(3, 0);


  delay1.delay(0, 500);

}

void loop() {
  update_controls();
  last_time = current_time;
  current_time = millis();



  freq1 = potRead(4) * 500.0;
  waveform1.frequency((freq1)*note_offset);
  waveform2.frequency((freq1 * 2.001)*note_offset);

  for (int j = 0; j < 6; j++) {
    prev_reading[j] = current_button_reading[j];
    current_button_reading[j] = buttonRead(j);

    if (prev_reading[j] == 1 && current_button_reading[j] == 0) {
      envelope1.noteOn();
      envelope2.noteOn();
      note_offset = (j + 1.0);
    }

    if (prev_reading[j] == 0 && current_button_reading[j] == 1) {
      envelope1.noteOff();
      envelope2.noteOff();
    }
  }

  fiter_freq = (potRead(5) * 10000.0) + (lfo_reading * 200.0); //not great at high frequencies
  //fiter_freq = (potRead(5) * 10000.0) * (1.0 + (lfo_reading * 2.5));
  //fiter_freq = (potRead(5) * 10000.0);
  if (fiter_freq > 15000) {
    fiter_freq = 15000;
  }
  filter1.frequency(fiter_freq); //if we go over 18000 it freaks out

  if (peak1.available()) {
    lfo_reading = peak1.read();
  }

  lfo_freq = potRead(6) * 10.0;
  waveform3.frequency(lfo_freq);

  attack_adj = (1.0 - potRead(0)) * 1000.0;
  envelope1.attack(attack_adj);

  release_adj = (1.0 - potRead(1)) * 2500.0;
  envelope1.release(release_adj);

  mixer2.gain(0, potRead(3) * 2.0);
  mixer2.gain(1, potRead(7) * 2.0);


  //We don't have to do anything in the loop since the audio library will jut keep doing what we told it in the setup
  if (current_time - prev_time[0] > 50) {
    prev_time[0] = current_time;
    //Serial.println(attack_adj);
    // Serial.println(release_adj);
    Serial.println(current_time - last_time);


    if (1) {//cahge this to 1 and it'll print
      Serial.print("processor: ");
      Serial.print(AudioProcessorUsageMax());
      Serial.print("%    Memory: ");
      Serial.print(AudioMemoryUsageMax());
      Serial.println();
      AudioProcessorUsageMaxReset(); //We need to reset these values so we get a real idea of what the audio code is doing rather than just peaking in every half a second
      AudioMemoryUsageMaxReset();
    }
  }

}// loop is over
