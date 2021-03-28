/*
 drum machine using samples loaded into program memory  

   Controls:
  Pots:
  0 - sample 1 frequency
  1 - sample 2 frequency
  2 -  unused
  3 - output volume
  4 - sample 3 frequency
  5 - sample 4 frequency
  6 -  unused
  7 -  unused

  Large buttons:
  0 - play sample 1
  1 - play sample 2
  2 -  unused
  3 - play sample 3
  4 - play sample 4
  5 -  unused
  
  Small buttons:
  left - hold to record
  right - play / stop

  Left light shows play and record mode, right shows tempo and blinks pinkish if clipping 

 
 */

#include "mem_sampler.h" //include beore the rest
 
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

MemSampler sampler1, sampler2, sampler3, sampler4;

// GUItool: begin automatically generated code
AudioAnalyzePeak         peak1;
AudioMixer4              mixer1;         //xy=383,245
AudioOutputI2S           i2s1;           //xy=591,244
AudioConnection          patchCord1(sampler1, 0, mixer1, 1);
AudioConnection          patchCord2(sampler2, 0, mixer1, 0);
AudioConnection          patchCord11(sampler3, 0, mixer1, 2);
AudioConnection          patchCord21(sampler4, 0, mixer1, 3);
AudioConnection          patchCord3(mixer1, 0, i2s1, 0);
AudioConnection          patchCord4(mixer1, 0, i2s1, 1);
AudioConnection          patchCord41(mixer1, 0, peak1, 0);

AudioControlSGTL5000     sgtl5000_1;     //xy=484,355
// GUItool: end automatically generated code

#include "samples.h"
#include "bleep_base.h" //Then we can add this line that we will still need

//timing "if"s are great but are not accurrate since they jsut in the loop
//sometimes the loop could take longer and they would happen a little late
//If you want acutare timing you can use an interupt timer. This stops everything and rusn the code you need at very precise timing
IntervalTimer sequence_timer; //name the interval timer


//then you can declare any variables you want.
unsigned long current_time;
unsigned long prev_time[8]; //an array of 8 variables all named "prev_time"
float sequence_timer_rate;
int seq_step, note_division;
int seq_length = 31; //32 steps
byte rec_bank[4][256] = {
  {1, 0, 0, 0, 1, 0, 0, 0,  1, 0, 0, 0, 1, 0, 0, 0,  1, 0, 0, 0, 1, 0, 0, 0,  1, 0, 0, 0, 1, 0, 1, 1},
  {0, 0, 0, 0, 0, 0, 0, 0,  1, 0, 1, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0},
  {1, 0, 0, 0, 0, 0, 0, 0,  1, 0, 0, 0, 0, 0, 0, 0,  1, 0, 0, 0, 1, 0, 0, 0,  1, 0, 0, 0, 0, 0, 1, 0},
  {0, 0, 0, 0, 1, 0, 1, 0,  0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,  1, 0, 1, 0, 0, 0, 0, 0},
};
float bpm;
float hue0, hue1, bright0, bright1;
float sampler_freq[4];
int prev_bpm;
int record_mode, play_mode;
int trig[4];
float amp[4];
float out_level;
int clipping;
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


  mixer1.gain(0, .25);
  mixer1.gain(1, .25);
  mixer1.gain(2, .25);
  mixer1.gain(3, .25);

  sampler1.begin(hhc, hhc_length);
  sampler1.sample_loop(0);
  sampler1.frequency(1.0);
  sampler1.sample_reverse(0);

  sampler2.begin(thom_hi, thom_hi_length);
  sampler2.sample_loop(0);
  sampler2.frequency(1.0);
  sampler2.sample_reverse(0);

  sampler3.begin(swirly, swirly_length);
  sampler3.sample_loop(0);
  sampler3.frequency(1.0);
  sampler3.sample_reverse(0);

  sampler4.begin(dmx_kick, dmx_kick_length);
  sampler4.sample_loop(0);
  sampler4.frequency(1.0);
  sampler4.sample_reverse(0);

  //one way to do this is cahnge the rate of the timer or differnt BPMs
  //so we need to convert BPM and smallest step resolution to microseconds
  bpm = 120;
  note_division = 8; //now many steps per quater note

  //I made a function to do this
  // bpm2us(tempo, steps per 1/4 note)
  sequence_timer_rate = bpm2us(bpm, note_division);
  sequence_timer.begin(sequence, sequence_timer_rate); //(name of function to run, rate in microseconds)

} //setup is over

float bpm2us (float in, int steps) {
  float bpm2hz = in / 60.0;
  float step_size = bpm2hz * steps;
  float hz2us = (1.0 / step_size) * 1000000.0;
  return hz2us;
}

void sequence() { //the funtion we'll run evey "sequence_timer_rate" microseconds

  if (record_mode == 1) {
    if (trig[0] == 1) {  //if a trigger is set to 1 set tath step in the array and then turn the trigger off
      rec_bank[0][seq_step] = 1;
      trig[0] = 0;
    }
    if (trig[1] == 1) {
      rec_bank[1][seq_step] = 1;
      trig[1] = 0;
    }
    if (trig[2] == 1) {
      rec_bank[2][seq_step] = 1;
      trig[2] = 0;
    }
    if (trig[3] == 1) {
      rec_bank[3][seq_step] = 1;
      trig[3] = 0;
    }
  }

  if (play_mode == 1) {
    if (rec_bank[0][seq_step] == 1) {
      sampler1.play();
    }

    if (rec_bank[1][seq_step] == 1) {
      sampler2.play();
    }

    if (rec_bank[2][seq_step] == 1) {
      sampler3.play();
    }

    if (rec_bank[3][seq_step] == 1) {
      sampler4.play();
    }

    seq_step++;  //if we did this first then we wouldn't play seq_step 0 once we hit play

    if (seq_step > seq_length) {
      seq_step = 0;
    }

    if (seq_step % 4 == 0) { //blink every 4 steps
      bright1 = .2;
    }
    else {
      bright1 = 0;
    }
  }

}


void loop() {
  update_controls();
  current_time = millis();

  if (buttonRead(6) == 0) { // if the left small button is down
    record_mode = 1;
    play_mode = 1;
  }
  else {
    record_mode = 0;
  }

  if (buttonState(7) == FELL) {
    play_mode = !play_mode; //! is oppiste of so 0 becomes 1 and 1 becoes 0
    if (play_mode == 0) {
      record_mode = 0;
      seq_step = 0;
      bright1 = 0;
    }
  }

  if (buttonState(0) == FELL) {
    sampler1.play();
    trig[0] = 1;
  }

  if (buttonState(1) == FELL) {
    sampler2.play();
    trig[1] = 1;
  }

  if (buttonState(3) == FELL) {
    sampler3.play();
    trig[2] = 1;
  }

  if (buttonState(4) == FELL) {
    sampler4.play();
    trig[3] = 1;
  }

  if (current_time - prev_time[2] > 5) { //slow all this down a little to reduce jiggly readings
    prev_time[2] = current_time;

    int pot_order[4] = {0, 1, 4, 5}; //what pots we'll be using
    for (int j = 0; j < 4; j++) { //0 1 2 3
      float temp_read = potRead(pot_order[j]); //read the pots we want for the pot_order array
      if (temp_read <= .5) { //if it's on one side
        sampler_freq[j] = temp_read * 2.0; //0-1.0 output
      }
      if (temp_read > .5) { //if it's on the other
        sampler_freq[j] = (((temp_read - .5) * 2.0) * 3.0) + 1.0; //get it to 0-1.0, then 1-3.0
      }
    }

    sampler1.frequency(sampler_freq[0]);
    sampler2.frequency(sampler_freq[1]);
    sampler3.frequency(sampler_freq[2]);
    sampler4.frequency(sampler_freq[3]);

    prev_bpm = bpm;
    bpm = (potRead(2) * 100) + 100; //100-200
    if (bpm != prev_bpm) { //if they arent the same
      sequence_timer_rate = bpm2us(bpm, note_division);
      sequence_timer.update(sequence_timer_rate);
    }

    float out_volume = potRead(3) * 2.0;

    mixer1.gain(0, .5 * out_volume);
    mixer1.gain(1, .7 * out_volume);
    mixer1.gain(2, .5 * out_volume);
    mixer1.gain(3, .6 * out_volume);
    //Yes these add up to over 1 but unlike an osicalttor they arent always at their hignest level.
    // adjsut as needed

    //Let's make a light turn on if it clips
    if (peak1.available()) {
     out_level = peak1.read();
      if (out_level > .8) {
        clipping=1;
        set_LED(1, .4, 1, .5);
      }
      else if (out_level > .9) {
        clipping=1;
        set_LED(1, .35, 1, .7);
      }
      else {
        clipping=0;
      }
    }
  }

  if (current_time - prev_time[3] > 30) {
    prev_time[3] = current_time;
    if (play_mode == 1 && record_mode == 0) {
      hue0 = 0;
      bright0 = .3;
    }
    if (play_mode == 0 ) {
      bright0 = 0;
    }
    if (record_mode == 1 ) {
      hue0 = .33;
      bright0 = .7;
    }
    set_LED(0, hue0, 1, bright0);
    if (clipping==0){
    set_LED(1, 0, 0, bright1);
    }

    LEDs.show();
  }


  if (current_time - prev_time[1] > 50 && 1) {
    prev_time[1] = current_time;
    Serial.println(out_level);

  }

  if (current_time - prev_time[0] > 500 && 0) {
    prev_time[0] = current_time;

    Serial.print("processor: ");
    Serial.print(AudioProcessorUsageMax());
    Serial.print(" %    Memory: ");
    Serial.print(AudioMemoryUsageMax());
    Serial.println();
    AudioProcessorUsageMaxReset(); //We need to reset these values so we get a real idea of what the audio code is doing rather than just peaking in every half a second
    AudioMemoryUsageMaxReset();
  }

}// loop is over
