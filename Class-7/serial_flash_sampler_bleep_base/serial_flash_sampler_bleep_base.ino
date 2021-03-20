/*
  "simple" serial flash sampler
  Find out more here https://github.com/BleepLabs/dadageek-Feb21/wiki/Memory-types-and-sampling-to-serial-flash
  
  In this example one button is dedicated to erasing, one for recording and one playing.
  The bank they are all address is controlled by the pot.
  A second sampler is started but unused

  The samplers really just play back sections of memory. You can have multiple ones lookign at the same part of memory ie the saem sample

  Since "AudioSampler" is not in the regualr library use "playSdWav" as it has the same outputs
  Then just "AudioPlaySdWav" with "AudioSampler" and "playSdWavX" with "samperX"

  queue_left and queue_right are AudioRecordQueue object and are what take the audio data
   and get it into the sampler. Don't change thier names

  Sampler functions:
  
  sample_loop(0 or 1); Loop this sampler
  sample_reverse(0 or 1); play it backwards
  


*/

//include these before the rest
#include "sampler2.h"
#include <EEPROM.h>
#include <Bounce2.h>
////////////////////////////////////////

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSampler           sampler1;     //xy=81,443
AudioSampler           sampler0;     //xy=82,484
AudioInputI2S            line_in;           //xy=156,246
AudioMixer4              sampler_mixer_right;         //xy=289,514
AudioMixer4              sampler_mixer_left;         //xy=291,430
AudioAmplifier           amp_in_left;           //xy=338,216
AudioAmplifier           amp_in_right;           //xy=341,262
AudioRecordQueue         queue_left;         //xy=484,188
AudioRecordQueue         queue_right;         //xy=498,241
AudioMixer4              final_mixer_right;         //xy=505,470
AudioMixer4              final_mixer_left;         //xy=518,380
AudioOutputI2S           i2s2;           //xy=682,409
AudioConnection          patchCord1(sampler1, 0, sampler_mixer_left, 0);
AudioConnection          patchCord2(sampler1, 1, sampler_mixer_right, 0);
AudioConnection          patchCord3(sampler0, 0, sampler_mixer_left, 1);
AudioConnection          patchCord4(sampler0, 1, sampler_mixer_right, 1);
AudioConnection          patchCord5(line_in, 0, amp_in_left, 0);
AudioConnection          patchCord6(line_in, 1, amp_in_right, 0);
AudioConnection          patchCord7(sampler_mixer_right, 0, final_mixer_right, 0);
AudioConnection          patchCord8(sampler_mixer_left, 0, final_mixer_left, 0);
AudioConnection          patchCord9(amp_in_left, 0, final_mixer_left, 1);
AudioConnection          patchCord10(amp_in_left, queue_left);
AudioConnection          patchCord11(amp_in_right, 0, final_mixer_right, 1);
AudioConnection          patchCord12(amp_in_right, queue_right);
AudioConnection          patchCord13(final_mixer_right, 0, i2s2, 1);
AudioConnection          patchCord14(final_mixer_left, 0, i2s2, 0);
AudioControlSGTL5000     sgtl5000_1;
// GUItool: end automatically generated code

#include "bleep_base.h"

#define FlashChipSelect 6 //The flash chip uses SPI for communication. SPI pins can be shared but each decvice needs its onw selet pin

//each block is 256kilobyte
//In 44.1kHz stereo that's just under 1.5 seconds
//on the s25fl127 there are 64 blocks
#define sfblocks 8
#define number_of_banks 8 // 64/sfblocks

//leave these two alone plz
#define getsize AUDIO_BLOCK_SAMPLES*2
uint32_t rec_size = (sfblocks * 0x10000);

int16_t rec_target, foffset, mode_timer_latch, rec_mode, mode, prev_bank_sel;
uint32_t bankstart[number_of_banks];
uint32_t samplelen[number_of_banks];
byte bank_status[number_of_banks];
uint32_t sfaddress, address;
uint32_t mode_cm, mode_timer;
unsigned long current_time;
unsigned long prev_time[12]; //an array of 12 variables all named "prev_time"
int bank_sel;
float freq[4];
float fstep1, fstep2;
float vol;
float vol_rec_mon;
byte tick;

//this must be included at the end of declarations I know its annoying
#include "sampler_helpers2.h"


void setup() {
  start_bleep_base();
 delay(500); //wait a tiny bit so the chip is ready to go
  //must be done for the flash chip
  SPI.setSCK(14);  // Audio shield has SCK on pin 14
  SPI.setMOSI(7);  // Audio shield has MOSI on pin 7
  SPI.setCS(FlashChipSelect);
  delay(500); //wait a tiny bit so the chip is ready to go

  if (!SerialFlash.begin(FlashChipSelect)) {
    while (1) { //don't continue if the chip isn't there
      Serial.println("Unable to access SPI Flash chip");
      delay(1000);
    }
  }

  AudioMemory(15);

  sgtl5000_1.enable(); //Turn the adapter board on
  sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN); //Tell it what input we want to use. Not necessary yet but good to have
  sgtl5000_1.lineInLevel(13); //The volume of the input. Again we'll get to this later
  sgtl5000_1.volume(0.25);


  load_sample_locations(); //must be done before other sampler stuff. it gets the bankstart[n] samplelen[n] from eeprom
  //amplitude,frequency,start location,length
  //jsut coply these and cahnge out the bankstart and samplelen array addressing number
  sampler0.begin(1, 1.0, bankstart[0], samplelen[0]);
  sampler1.begin(1, 1.0, bankstart[1], samplelen[1]);

  amp_in_left.gain(1);//can be used to amplify the incoming siganl before the sampler
  amp_in_right.gain(1);

  sampler_mixer_left.gain(0, 1);
  sampler_mixer_left.gain(1, 1);
  sampler_mixer_left.gain(2, 0);
  sampler_mixer_left.gain(3, 0);

  sampler_mixer_right.gain(0, 1);
  sampler_mixer_right.gain(1, 1);
  sampler_mixer_right.gain(2, 0);
  sampler_mixer_right.gain(3, 0);

  final_mixer_left.gain(0, 1);
  final_mixer_left.gain(1, 0);
  final_mixer_left.gain(2, 0);
  final_mixer_left.gain(3, 0);

  final_mixer_right.gain(0, 1);
  final_mixer_right.gain(1, 0);
  final_mixer_right.gain(2, 0);
  final_mixer_right.gain(3, 0);

  Serial.println("Ready");

}

void loop() {
  update_controls();
  current_time = millis();

  
    //state is a simple way of doing what we were before with the previous and current readings.
    // This is used often with the bounce library you'll see in other examples
    if (buttonState(0) == FELL) {
      erase_bank(bank_sel); //will print out the erase progress
    }

    if (buttonState(1) == FELL) {
      startRecording(bank_sel);  //record must be done in these 3 steps. first start jsut once..
    }

    if (buttonRead(1) == 0) { //we jsut want to know when the button is down now
      continueRecording(); ///...then continue while the button is down...
      //final_mixer_left.gain(1, 1); //listen to the incoming audio
      //final_mixer_right.gain(1, 1);
    }
    if (buttonState(1) == ROSE) {
      stopRecording();//...then stop
      //final_mixer_left.gain(1, 0); //mute incoming audio
      //final_mixer_right.gain(1, 0);
    }

    if (buttonState(2) == FELL) {
      // only do this once like noteOn
      //start location in memory in bytes, length in bytes
      //you can modify these to get some granular type stuff going on.
      sampler0.sample_play_loc(bankstart[bank_sel], samplelen[bank_sel]);
    }

    if (buttonState(2) == ROSE) {
      //if you want the sample to only play when the button is down uncomment this
      // sampler0.sample_stop();
    }
  

  vol = 1.0 - potRead(0);
  vol_rec_mon = 1.0 - potRead(1);
  final_mixer_left.gain(0, vol);
  final_mixer_right.gain(0, vol);
  final_mixer_left.gain(1, vol_rec_mon);
  final_mixer_right.gain(1, vol_rec_mon);


  //the sapler frequency is differnt that a oscilltor frequency. At 1 it plays the sampel back at the recored rate. .5 half as slow, 2, twice as fast
  // lets have 1 be 12:00 on the pot
  float raw_freg_pot = 1.0 - potRead(4);
  if (raw_freg_pot <= .5) { //if the pot is on the left side
    freq[0] = raw_freg_pot * 2.0; //the opt will be goingfrom 0-.5, we want 0-1.0
  }
  if (raw_freg_pot > .5) { //if the pot is on the right side
    //lets break it up. Thsi will allow us to prinyt out each part and jsut make the math easier to get our head around
    fstep1 = (raw_freg_pot + .5); //we start at .5 but we want to start at 1. results in 1.0-1.5
    fstep2 = fstep1 * (4.0 / 1.5); //we want to got to 4 and right now we go to 1.5 so we use those values to scale it up
    freq[0] =  fstep2; //1.0 - 4.0
  }


  //it's easier to jsut use map() but this shows how you ahve to keep everyhting a flaow if you wanna do acurate division
  //bank_sel = int((analogRead(A2) / 4095.0) * float(number_of_banks-1));

  //there are lots of ways to reduce the range of a pot to select "number_of_banks" = 8
  // map works great for integers but not flaots but we can deal with that https://www.arduino.cc/reference/en/language/functions/math/map/

  prev_bank_sel = bank_sel; //so we can only print the bank if the pot cahnges

  bank_sel = (1.0 - potRead(5)) * (number_of_banks - 1);
  sampler0.frequency(freq[0]);

  if (prev_bank_sel != bank_sel) {
    Serial.println(bank_sel);
  }


  if (current_time - prev_time[0] > 1000) {
    prev_time[0] = current_time;
    //Serial.println(freq[0]);

    if (1) {
      Serial.print("proc: ");
      Serial.print(AudioProcessorUsageMax());
      Serial.print("%    Mem: ");
      Serial.print(AudioMemoryUsageMax());
      Serial.println();
      AudioProcessorUsageMaxReset(); //We need to reset these values so we get a real idea of what the audio code is doing rather than just peaking in every half a second
      AudioMemoryUsageMaxReset();
    }
  }

}
