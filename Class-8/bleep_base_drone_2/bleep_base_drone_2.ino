/*
  Bleep base drone machine v2

  Controls:
  Pots:
  0 - digital combine feedback
  1 - filter 1 frequency, lowpass filter
  2 - filter 1 & 2 LFO rate
  3 - output volume
  4 - delay feedback
  5 - filter 1 frequency, bandpass filter
  6 - waveform4 frequency
  7 - delay rate

  Large buttons:
  Plays a set scale

  Small buttons:
  shift this scale up with the right button and and down with the left

  Three oscilltors get thier pitch acording to the buttons.
  They are combined together and then go thoguh a "digital combine effect", a type of distortion
  The combine effetc also has a an oscilltor, waveform 4, or a set level goign thpough it. They are both controlled by pot 6

  the output of the effect funs thoguh filters delays and reverbs to make a nice drone
*/

#include "effect_tape_delay.h" // this needs to be before the other audio code 

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform4;      //xy=285,635
AudioSynthWaveformDc     dc1;            //xy=291,582
AudioSynthWaveform       waveform3;      //xy=329,524
AudioSynthWaveform       waveform2;      //xy=343,482
AudioSynthWaveform       waveform1;      //xy=351,442
AudioAnalyzePeak         peak_mixer4;          //xy=392,754
AudioMixer4              mixer4;         //xy=470,599
AudioMixer4              mixer3;         //xy=522,472
AudioEffectDigitalCombine combine1;       //xy=621,566
AudioMixer4              mixer1;         //xy=667,726
AudioEffectTapeDelay         tapeDelay1;     //xy=670,907
AudioSynthWaveform       lfo2;           //xy=730,359
AudioSynthWaveform       lfo1;           //xy=742,424
AudioFilterStateVariable filter2;        //xy=787,499
AudioFilterStateVariable filter1;        //xy=788,598
AudioAnalyzePeak         peak_lfo2;      //xy=900,331
AudioAnalyzePeak         peak_lfo1;      //xy=915,403
AudioMixer4              mixer2;         //xy=952,772
AudioMixer4              mixer5;         //xy=963,526
AudioAmplifier           amp1;           //xy=1070,857
AudioEffectFreeverb      freeverb1;      //xy=1100,593
AudioOutputI2S           i2s1;           //xy=1214,865
AudioConnection          patchCord1(waveform4, 0, mixer4, 1);
AudioConnection          patchCord2(dc1, 0, mixer4, 0);
AudioConnection          patchCord3(waveform3, 0, mixer3, 2);
AudioConnection          patchCord4(waveform2, 0, mixer3, 1);
AudioConnection          patchCord5(waveform1, 0, mixer3, 0);
AudioConnection          patchCord6(mixer4, 0, combine1, 1);
AudioConnection          patchCord7(mixer4, peak_mixer4);
AudioConnection          patchCord8(mixer3, 0, combine1, 0);
AudioConnection          patchCord9(combine1, 0, filter1, 0);
AudioConnection          patchCord10(combine1, 0, filter2, 0);
AudioConnection          patchCord11(mixer1, tapeDelay1);
AudioConnection          patchCord12(tapeDelay1, 0, mixer1, 3);
AudioConnection          patchCord13(tapeDelay1, 0, mixer2, 1);
AudioConnection          patchCord14(tapeDelay1, 0, mixer4, 3);
AudioConnection          patchCord15(lfo2, peak_lfo2);
AudioConnection          patchCord16(lfo1, peak_lfo1);
AudioConnection          patchCord17(filter2, 1, mixer5, 0);
AudioConnection          patchCord18(filter1, 0, mixer1, 0);
AudioConnection          patchCord19(filter1, 0, mixer5, 1);
AudioConnection          patchCord20(mixer2, amp1);
AudioConnection          patchCord21(mixer5, freeverb1);
AudioConnection          patchCord22(mixer5, 0, mixer2, 0);
AudioConnection          patchCord23(amp1, 0, i2s1, 0);
AudioConnection          patchCord24(amp1, 0, i2s1, 1);
AudioConnection          patchCord25(freeverb1, 0, mixer2, 2);
AudioControlSGTL5000     sgtl5000_1;     //xy=621,278
// GUItool: end automatically generated code



//to import this back into the tool, change AudioEffectTapeDelay to AudioEffectDelay
// then it will show up like the stadnard dealy in there. Cahge it back once you're re imported it

#include "bleep_base.h" //Then we can add this line that we will still need

//Some effects require memory to be allocated outside of the AudioMemory() function/
// At 44.1 kHz sample rate every 500 integers, 1k byte of memory is 11.3 milliseconds
// On the 4.0 there is 1024k bytes of ram

#define DELAY_BANK_SIZE 22000
int16_t delay_bank[DELAY_BANK_SIZE];
float freq[12];
unsigned long current_time;
unsigned long prev_time[8];
int rate1 = 30;
int shift = 12;
float hue2;

float level;
float lfo2_output, lfo1_output;

//"const" means to store it in program memory, not RAM
const float chromatic[88] = {55.00000728, 58.27047791, 61.73542083, 65.40639999, 69.29566692, 73.4162017, 77.78175623, 82.40690014, 87.30706942, 92.49861792, 97.99887197, 103.8261881, 110.0000146, 116.5409558, 123.4708417, 130.8128, 138.5913338, 146.8324034, 155.5635124, 164.8138003, 174.6141388, 184.9972358, 195.9977439, 207.6523763, 220.0000291, 233.0819116, 246.9416833, 261.6255999, 277.1826676, 293.6648067, 311.1270248, 329.6276005, 349.2282776, 369.9944716, 391.9954878, 415.3047525, 440.0000581, 466.1638231, 493.8833665, 523.2511997, 554.3653352, 587.3296134, 622.2540496, 659.2552009, 698.4565551, 739.9889431, 783.9909755, 830.6095048, 880.0001162, 932.3276461, 987.7667329, 1046.502399, 1108.73067, 1174.659227, 1244.508099, 1318.510402, 1396.91311, 1479.977886, 1567.981951, 1661.219009, 1760.000232, 1864.655292, 1975.533466, 2093.004798, 2217.46134, 2349.318453, 2489.016198, 2637.020803, 2793.82622, 2959.955772, 3135.963901, 3322.438019, 3520.000464, 3729.310584, 3951.066931, 4186.009596, 4434.92268, 4698.636906, 4978.032395, 5274.041605, 5587.652439, 5919.911543, 6271.927802, 6644.876037, 7040.000927, 7458.621167, 7902.133861, 8372.019192};

void setup() {
  start_bleep_base(); //run this first in setup

  AudioMemory(200);

  sgtl5000_1.enable(); //Turn the adapter board on
  sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN); //Tell it what input we want to use. Not necessary yet but good to have
  //sgtl5000_1.micGain(63);
  sgtl5000_1.lineInLevel(13); //The volume of the input. Again we'll get to this later
  sgtl5000_1.volume(0.75);

  waveform1.begin(1, 220.0, WAVEFORM_SINE);
  waveform2.begin(1, 440.0, WAVEFORM_SINE);
  waveform3.begin(1, 110.0, WAVEFORM_SINE);

  waveform4.begin(.4, 0, WAVEFORM_SINE);

  lfo1.begin(1, 4.2, WAVEFORM_SINE);
  lfo2.begin(1, 4.2, WAVEFORM_SAMPLE_HOLD);

  mixer3.gain(0, .5);
  mixer3.gain(1, .0);
  mixer3.gain(2, .5);
  mixer3.gain(3, 0);

  combine1.setCombineMode(1); //combine is a weird digal distorion I made https://www.pjrc.com/teensy/gui/?info=AudioEffectDigitalCombine

  filter1.frequency(10000);
  filter1.resonance(4.0);

  filter2.frequency(10000);
  filter2.resonance(4.0);

  mixer1.gain(0, .6); //delay input mixer - audio in

  //begin(bank to store data in(integer array), size of that bank (integer), delay length(integer), reduction(integer), interpolation(integer))
  //bank, and size are all defines above, you jsut need to plug them in
  //The bank jsut defines the max size of the delay effect. you can cahge the length here or jsut with "delay1.length()"
  //reduction can make the delay longer at the expense of sample rate. 0 is no reduction 1 doubles the leght but halves the sample rate to 22lHz, 2 is 11k and so on. On the 4.0 we've got a lot of memory so 0 i fine but 1 can have it's uses.
  //interpolation. how fast does does the tape head move to meet the desided length. Larger numbers mean it takes longer to zip to the new length. This is in integers
  //this is a lot but its jsut copy paste really!
  tapeDelay1.begin(delay_bank, DELAY_BANK_SIZE, 0, 0, 1);

  freeverb1.roomsize(.4);
  freeverb1.damping(.9);

  mixer2.gain(0, .5); //dry
  mixer2.gain(1, .3); //delay
  mixer2.gain(2, .3); //reverb

  mixer4.gain(0, 1); //waveform 4...
  mixer4.gain(1, 1); /////and 6 into combine


  mixer5.gain(0, .6); //mix between two filter2
  mixer5.gain(1, -.4); // and filter 1. negative is invert phase

}

void loop() {
  current_time = millis();
  update_controls();

  for (byte j = 0; j < 6; j++) { //make a little keybaord out of the 6 keys
    if (buttonState(j) == FELL) {
      
      if (shift < 4) {
        shift = 4;
      }
      if (shift > 24) {
        shift = 24;
      }
      
      freq[1] = chromatic[(j + shift) * 3];
      freq[2] = freq[1] * 2.01;
      freq[3] = freq[1] / 4.0;
      waveform1.frequency(freq[1]);
      waveform2.frequency(freq[2]);
      waveform3.frequency(freq[3]);
    }
  }

  if (button_state[6] == FELL) { //use the two samll keys to shift the range the 6 keys play
    shift -= 4;
  }
  if (button_state[7] == FELL) {
    shift += 4;
  }


  float fb2 = potRead(0) * -1.0;  //digital combine feedback. Phase is flipped
  mixer4.gain(3, fb2);

  float temp1 = expo_converter(potRead(1), 18000.0, 2.0); //input, new max, curve
  float fliter1_mod = (temp1) * lfo1_output;
  filter1.frequency(fliter1_mod); //frequency is the combination of the pot that has been mapped to exponental and the lfo1

  lfo1.frequency(potRead(2) * 2.0);  //both based on one pot
  lfo2.frequency(potRead(2) * 2.6);

  if (peak_lfo1.available()) { //use two wavefroms as LFOs. Read their outputs with peaks
    lfo1_output = (peak_lfo1.read() * .8) + .2; //so it never goes all the way to 0
  }

  if (peak_lfo2.available()) {
    lfo2_output = ((peak_lfo2.read() * .6) + .4);
  }

  amp1.gain(potRead(3)); //output amp

  float fb1 = potRead(4) * 1.25;
  mixer1.gain(3, fb1); //delay feedback

  float filter2_freq = potRead(5) * 12000.0 * lfo2_output;
  filter2.frequency(filter2_freq);

  if (potRead(6) < .5) { //on one side of the pot
    freq[4] = potRead(6) * 20.0;
    waveform4.frequency(freq[4]);
    waveform4.amplitude(.4);
    dc1.amplitude(0);
    hue2 = .1; //change color
  }
  if (potRead(6) >= .5) {
    dc1.amplitude(((potRead(6) - .5) * 2.0) + .01); //pot is .5-1.0 so subrat by .5 and * by 2 to get 0-1. Then get it to .01-2.01
    waveform4.amplitude(0); //turn off oscillator
    hue2 = .6;//change color
  }

  tapeDelay1.length(potRead(7)*DELAY_BANK_SIZE);

  if (current_time - prev_time[1] > 30) {
    prev_time[1] = current_time;

    set_LED(0, potRead(2) , 1, lfo2_output * .6);

    if (peak_mixer4.available()) {
      float b2 = (peak_mixer4.read());
      set_LED(1, hue2, 1, b2);
    }

    LEDs.show(); // after we've set what we want all the LEDs to be we send the data out through this function
  }

  if (current_time - prev_time[2] > 20 && 1) {
    prev_time[2] = current_time;

    Serial.println(potRead(6), 4); //4 decimals of precision
  }

  if (current_time - prev_time[0] > 500 && 0) {
    prev_time[0] = current_time;

    Serial.print(AudioProcessorUsageMax());
    Serial.print("%  ");
    Serial.print(AudioMemoryUsageMax());
    Serial.println();
    AudioProcessorUsageMaxReset(); //We need to reset these values so we get a real idea of what the audio code is doing rather than just peaking in every half a second
    AudioMemoryUsageMaxReset();
    Serial.println();
  }


}// loop is over

float expo_converter(float input, float max1, float curve) {
  //powf is a more effeinct power for floats
  // powf(base, exponent)
  float ex1 = powf((1.0 - input) * max1, curve) / powf(max1, curve - 1.0);
  return ex1;
}
