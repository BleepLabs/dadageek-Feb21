// recieve and play 4 note midi poly
// can be easily expanded



#include <MIDI.h>
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI); //You can have multiple DIN MIDI systems setup in  the same device using different pins but this is the one on the Bleep Base and most other systems



#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform[4];      //xy=154,486
AudioEffectEnvelope      envelope[4];      //xy=364,366
AudioMixer4              mixer1;         //xy=505,414
AudioOutputI2S           i2s1;           //xy=701,388
AudioConnection          patchCord1(waveform[0], envelope[0]);
AudioConnection          patchCord2(envelope[0], 0, mixer1, 0);
AudioConnection          patchCord3(waveform[1], envelope[1]);
AudioConnection          patchCord4(envelope[1], 0, mixer1, 1);
AudioConnection          patchCord5(waveform[2], envelope[2]);
AudioConnection          patchCord6(envelope[2], 0, mixer1, 2);
AudioConnection          patchCord7(waveform[3], envelope[3]);
AudioConnection          patchCord8(envelope[3], 0, mixer1, 3);


AudioConnection          patchCord322(mixer1, 0, i2s1, 0);
AudioConnection          patchCord411(mixer1, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=672,507
// GUItool: end automatically generated code


#include "bleep_base.h" //Then we can add this line that we will still need


//starts at midi note 12, C0
const float chromatic[121] = {16.3516, 17.32391673, 18.35405043, 19.44543906, 20.60172504, 21.82676736, 23.12465449, 24.499718, 25.95654704, 27.50000365, 29.13523896, 30.86771042, 32.7032, 34.64783346, 36.70810085, 38.89087812, 41.20345007, 43.65353471, 46.24930897, 48.99943599, 51.91309407, 55.00000728, 58.27047791, 61.73542083, 65.40639999, 69.29566692, 73.4162017, 77.78175623, 82.40690014, 87.30706942, 92.49861792, 97.99887197, 103.8261881, 110.0000146, 116.5409558, 123.4708417, 130.8128, 138.5913338, 146.8324034, 155.5635124, 164.8138003, 174.6141388, 184.9972358, 195.9977439, 207.6523763, 220.0000291, 233.0819116, 246.9416833, 261.6255999, 277.1826676, 293.6648067, 311.1270248, 329.6276005, 349.2282776, 369.9944716, 391.9954878, 415.3047525, 440.0000581, 466.1638231, 493.8833665, 523.2511997, 554.3653352, 587.3296134, 622.2540496, 659.2552009, 698.4565551, 739.9889431, 783.9909755, 830.6095048, 880.0001162, 932.3276461, 987.7667329, 1046.502399, 1108.73067, 1174.659227, 1244.508099, 1318.510402, 1396.91311, 1479.977886, 1567.981951, 1661.219009, 1760.000232, 1864.655292, 1975.533466, 2093.004798, 2217.46134, 2349.318453, 2489.016198, 2637.020803, 2793.82622, 2959.955772, 3135.963901, 3322.438019, 3520.000464, 3729.310584, 3951.066931, 4186.009596, 4434.92268, 4698.636906, 4978.032395, 5274.041605, 5587.652439, 5919.911543, 6271.927802, 6644.876037, 7040.000927, 7458.621167, 7902.133861, 8372.019192, 8869.845359, 9397.273811, 9956.06479, 10548.08321, 11175.30488, 11839.82309, 12543.8556, 13289.75207, 14080.00185, 14917.24233, 15804.26772, 16744.03838};


unsigned long current_time;
unsigned long prev_time[8];
float sequence_timer_rate;
int seq_step, note_division;
int seq_length = 31; //32 steps
float bpm;
float hue0, hue1, bright0, bright1;
byte random_pool[8];
int random_pool_count;
float sampler_freq[4];
int prev_bpm;
int record_mode, play_mode;
int trig[4];
float amp[4];
float out_level;
int clipping;

uint16_t mtick;
uint16_t ma_loc;
int16_t poly_count;
int16_t voice_sel;
byte poly_bank[8] = {255, 255, 255, 255, 255, 255, 255, 255};

float prev_tempo, tempo;
int array_sel, prev_array_sel;
float sel_random;
byte um_channel_select = 1;
byte um_type, um_note, um_velocity, um_channel_recived, um_data1, um_data2, um_cc_num, um_cc_val, um_note_state;

void setup() {
  start_bleep_base(); //run this first in setup

  AudioMemory(100);

  sgtl5000_1.enable(); //Turn the adapter board on
  sgtl5000_1.volume(0.25);
  sgtl5000_1.lineOutLevel(21); //11-32, the smaller the louder. 21 is about 2 Volts peak to peak

  for (byte m = 0; m < 4; m++) {
    waveform[m].begin(1, 220.0, WAVEFORM_SINE);
    envelope[m].attack(10);
    envelope[m].decay(10);
    envelope[m].sustain(.4);
    envelope[m].release(10);
    mixer1.gain(m, .25);
  }
} //setup is over

void midicheck() {

  if (usbMIDI.read()) {   // Is there a USB MIDI message incoming ?
    um_note_state = 2; //noting
    um_type = usbMIDI.getType();
    um_channel_recived = usbMIDI.getChannel();

    if (um_channel_recived == um_channel_select || um_channel_select == 0) {
      if (um_type == midi::NoteOn) {
        um_note = usbMIDI.getData1();
        um_velocity = usbMIDI.getData2();
        if (um_velocity > 0) {
          Serial.print("USB note On:");
          Serial.print(um_note);
          Serial.print("  velocity: ");
          Serial.println(um_velocity);
          um_note_state = 1;  //note on

        }
        if (um_velocity == 0) { //some systems send velocity 0 as note off
          Serial.print("USB note Off: ");
          Serial.println(um_note);
          um_note_state = 0;//note off 
        }
      }

      else if (um_type == midi::NoteOff) { //else if can be used so we only have one outcome
        um_note = usbMIDI.getData1();
        Serial.print("USB note Off: ");
        Serial.println(um_note);
        um_note_state = 0;//note off 
      }

      else if (um_type == midi::ControlChange) {
        um_cc_num = usbMIDI.getData1();
        um_cc_val = usbMIDI.getData2();
        Serial.print("USB CC#: ");
        Serial.print(um_cc_num);
        Serial.print("  value: ");
        Serial.println(um_cc_val);
      }

      //If it's not a note on or off or cc do this.
      // If there were just "ifs" being used above there could be a final else that would catch all the other message types
      // the other types are :
      /*
        usbMIDI::AfterTouchPoly
        usbMIDI::ProgramChange
        usbMIDI::AfterTouchChannel
        usbMIDI::PitchBend
        usbMIDI::SystemExclusive
      */

      else {
        um_data1 = usbMIDI.getData1();
        um_data2 = usbMIDI.getData2();
      }
    }

    //wer're still inside "if (usbMIDI.read()) {"

    byte in_vel = um_velocity;
    byte in_note = um_note;

    if (um_note_state == 0) {
      byte off_sel = 0;
      for (byte j = 0; j < 4; j++) {
        if (in_note == poly_bank[j]) {  //if the note is alreay in there bank and wer'e turing it off
          off_sel = j;
          break;  //we did the thing so scanning the rest is unecessary. this exits the for
        }
      }
      envelope[off_sel].noteOff();
      poly_bank[off_sel] = 255;
    }

    if (um_note_state == 1) {
      byte on_sel = 0;
      for (byte j = 0; j < 4; j++) {   //scan the bank to find a blank spot. Using 255 as midi is 0-127 so we can keep it a byte
        if (poly_bank[j] == 255) {
          on_sel = j;
          break;
        }
      }
      poly_bank[on_sel] = in_note;

      waveform[on_sel].frequency(chromatic[in_note]);
      waveform[on_sel].amplitude(in_vel / 127.0);
      envelope[on_sel].noteOn();
    }

    if (1) {
      Serial.print(in_vel); Serial.print(" ");
      Serial.print(in_note); Serial.print(" ");
      Serial.println();
      for (byte j = 0; j < 4; j++) {
        Serial.print(poly_bank[j]); Serial.print(" ");
      }
      Serial.println(); Serial.println();
    }
  }  //if midi read is over

}


void loop() {
  update_controls();
  current_time = millis();

  midicheck();




  if (current_time - prev_time[0] > 100 && 0) {
    prev_time[0] = current_time;


    Serial.print(AudioProcessorUsageMax());
    Serial.print(" %  ");
    Serial.print(AudioMemoryUsageMax());
    Serial.println();
    AudioProcessorUsageMaxReset(); //We need to reset these values so we get a real idea of what the audio code is doing rather than just peaking in every half a second
    AudioMemoryUsageMaxReset();
  }

}// loop is over
