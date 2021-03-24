// the stadard delay can't have the length cahnged without pops and clicks
// this one works like a stadard tape delay

#include "effect_tape_delay.h" // this needs to be before the other audio code 

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S            i2s2;           //xy=82,298
AudioAmplifier           amp1;           //xy=224,294
AudioMixer4              mixer1;         //xy=321,424
AudioEffectTapeDelay         delay1;         //xy=463,419
AudioMixer4              mixer2;         //xy=623,345
AudioOutputI2S           i2s1;           //xy=804,440
AudioConnection          patchCord1(i2s2, 0, amp1, 0);
AudioConnection          patchCord2(amp1, 0, mixer1, 0);
AudioConnection          patchCord3(amp1, 0, mixer2, 0);
AudioConnection          patchCord4(mixer1, delay1);
AudioConnection          patchCord5(delay1, 0, mixer2, 1);
AudioConnection          patchCord6(delay1, 0, mixer1, 1);
AudioConnection          patchCord7(mixer2, 0, i2s1, 0);
AudioConnection          patchCord8(mixer2, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=575,188
// GUItool: end automatically generated code



//since tape delay isnt in the tool it wont see it
//but you can alwasy use the standard delay module in the tool then swap the name out in here for AudioEffectTapeDelay

//Some effects require memory to be allocated outside of the AudioMemory() function/
// At 44.1 kHz sample rate every 500 integers, 1k byte of memory is 11.3 milliseconds
// On the 4.0 there is 1024k bytes of ram
#define DELAY_SIZE 20000 //size in 2xintegers
int16_t tape_delay_bank[DELAY_SIZE]; //int16_t is a more specific way of saying integer

#include "bleep_base.h" //Then we can add this line that we will still need

//then you can declare any variables you want.
unsigned long current_time;
unsigned long prev_time[8]; //an array of 8 variables all named "prev_time"
float input_volume, feedback_level, wet_level, dry_level;
long delay_time_adjust;


void setup() {
  start_bleep_base(); //run this first in setup

  //the delay is outside audio memory as we decalerd the array for it above
  AudioMemory(10);

  sgtl5000_1.enable(); //Turn the adapter board on
  sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN); //Tell it what input we want to use. Not necessary is you're not using the ins
  sgtl5000_1.lineInLevel(12); //The volume of the input. 0-15 with 15 bing more amplifications
  //sgtl5000_1.inputSelect(AUDIO_INPUT_MIC);
  //sgtl5000_1.micGain(13); //0 - 63bd of gain.

  //headphone jack output volume. Goes from 0.0 to 1.0 but a 100% signal will clip over .8 or so.
  // For headphones it's pretty loud at .4
  sgtl5000_1.volume(0.25);

  //The line out has a seperate level control but it's not meant to be adjusted like the volume function above.
  // If you're not using the line out don't worry about it.
  sgtl5000_1.lineOutLevel(21); //11-32, the smaller the louder. 21 is about 2 Volts peak to peak

  //begin(bank to store data in(integer array), size of that bank (integer), delay length(integer), reduction(integer), interpolation(integer))
  //bank, and size are all defines above, you jsut need to plug them in
  //The bank jsut defines the max size of the delay effect. you can cahge the length here or jsut with "delay1.length()"
  //reduction can make the delay longer at the expense of sample rate. 0 is no reduction 1 doubles the leght but halves the sample rate to 22lHz, 2 is 11k and so on. On the 4.0 we've got a lot of memory so 0 i fine but 1 can have it's uses.
  //interpolation. how fast does does the tape head move to meet the desided length. Larger numbers mean it takes longer to zip to the new length. This is in integers
  //this is a lot but its jsut copy paste really!
  delay1.begin(tape_delay_bank, DELAY_SIZE, DELAY_SIZE / 2, 0, 2);

  mixer1.gain(0, 1); //level from input going into delay. we'll adjsut the other levels in the loop


} //setup is over


void loop() {
  update_controls();
  current_time = millis();

  if (current_time - prev_time[1] > 5) { //we don't need to do all this screamingly fast. Is we slow it down a little there will be less jiggle in the pot readings
    prev_time[1] = current_time;

    input_volume = (1.0 - potRead(0)) * 2.0; //we can amplify or attenuate the signal
    amp1.gain(input_volume);

    feedback_level = (1.0 - potRead(1));
    mixer1.gain(1, feedback_level); //how much of the delay output comes back into the input mixer

    dry_level = potRead(2);
    wet_level = 1.0 - dry_level; //as one goes up the other goes down to control the wet dry mix.
    mixer2.gain(0, dry_level); // signal straigh from input
    mixer2.gain(1, wet_level); //signal from delay output

    delay_time_adjust = potRead(3) * DELAY_SIZE; //delay size is the max so jsut multiply it bu the 0-1.0 pot
    delay1.length(delay_time_adjust);

  }

  //We don't have to do anything in the loop since the audio library will jut keep doing what we told it in the setup
  if (current_time - prev_time[0] > 500) {
    prev_time[0] = current_time;
    Serial.println(feedback_level);
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
