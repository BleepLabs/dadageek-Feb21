/*
  Here we control a basic synth with the MIDI inputs

  More info:
  DIN MIDI https://www.pjrc.com/teensy/td_libs_MIDI.html
  USB MIDI https://www.pjrc.com/teensy/td_midi.html

  To use USB MIDI:
  In the Arduino IDE go to tools > USB Type > Serial + MIDI

  Most all MIDI messages are 0-127, 7 bits of data
  Pitch bend is 14 bits, 0 - 16383

*/

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform2;      //xy=175,270
AudioSynthWaveform       waveform1;      //xy=180,217
AudioEffectEnvelope      envelope2;      //xy=344,273
AudioEffectEnvelope      envelope1;      //xy=352,220
AudioMixer4              mixer1;         //xy=553,279
AudioOutputI2S           i2s1;           //xy=718,282
AudioConnection          patchCord1(waveform2, envelope2);
AudioConnection          patchCord2(waveform1, envelope1);
AudioConnection          patchCord3(envelope2, 0, mixer1, 1);
AudioConnection          patchCord4(envelope1, 0, mixer1, 0);
AudioConnection          patchCord5(mixer1, 0, i2s1, 0);
AudioConnection          patchCord6(mixer1, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=611,393
// GUItool: end automatically generated code


const float chromatic[88] = {55.00000728, 58.27047791, 61.73542083, 65.40639999, 69.29566692, 73.4162017, 77.78175623, 82.40690014, 87.30706942, 92.49861792, 97.99887197, 103.8261881, 110.0000146, 116.5409558, 123.4708417, 130.8128, 138.5913338, 146.8324034, 155.5635124, 164.8138003, 174.6141388, 184.9972358, 195.9977439, 207.6523763, 220.0000291, 233.0819116, 246.9416833, 261.6255999, 277.1826676, 293.6648067, 311.1270248, 329.6276005, 349.2282776, 369.9944716, 391.9954878, 415.3047525, 440.0000581, 466.1638231, 493.8833665, 523.2511997, 554.3653352, 587.3296134, 622.2540496, 659.2552009, 698.4565551, 739.9889431, 783.9909755, 830.6095048, 880.0001162, 932.3276461, 987.7667329, 1046.502399, 1108.73067, 1174.659227, 1244.508099, 1318.510402, 1396.91311, 1479.977886, 1567.981951, 1661.219009, 1760.000232, 1864.655292, 1975.533466, 2093.004798, 2217.46134, 2349.318453, 2489.016198, 2637.020803, 2793.82622, 2959.955772, 3135.963901, 3322.438019, 3520.000464, 3729.310584, 3951.066931, 4186.009596, 4434.92268, 4698.636906, 4978.032395, 5274.041605, 5587.652439, 5919.911543, 6271.927802, 6644.876037, 7040.000927, 7458.621167, 7902.133861, 8372.019192};

#include "bleep_base.h" //Then we can add this line that we will still need

//then you can declare any variables you want.
unsigned long current_time;
unsigned long prev_time[8]; //an array of 8 variables all named "prev_time"

//It really doesn't matter but I'm using bytes here as the MIDI data they will store is only 0-127
byte dm_type, dm_note, dm_velocity, dm_channel_recived, dm_data1, dm_data2, dm_cc_num, dm_cc_val;
byte um_type, um_note, um_velocity, um_channel_recived, um_data1, um_data2, um_cc_num, um_cc_val;

byte dm_channel_select = 0; //0 to receive any DIN MIDI channel
byte um_channel_select = 0; //0 to receive any USB MIDI channel

byte prev_cc_pot[8], cc_pot[8]; //two arrays
byte env1_off, env1_on;

#include <MIDI.h>
MIDI_CREATE_INSTANCE(HardwareSerial, Serial1, MIDI); //You can have multiple DIN MIDI systems setup in  the same device using different pins but this is the one on the Bleep Base and most other systems


void setup() {
  start_bleep_base(); //run this first in setup

  MIDI.begin(); //turn on DIN MIDI
  MIDI.turnThruOff(); //if you want the incoming MIDI notes to be copied and sent out remove this line

  //USB MIDI doesn't have a "begin", you just need to enable it in tools > USB Type > Serial + MIDI


  AudioMemory(10);

  sgtl5000_1.enable(); //Turn the adapter board on
  sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN); //Tell it what input we want to use. Not necessary is you're not using the ins
  sgtl5000_1.lineInLevel(10); //The volume of the input. 0-15 with 15 bing more amplifications
  //sgtl5000_1.inputSelect(AUDIO_INPUT_MIC);
  //sgtl5000_1.micGain(13); //0 - 63bd of gain.

  //headphone jack output volume. Goes from 0.0 to 1.0 but a 100% signal will clip over .8 or so.
  // For headphones it's pretty loud at .4
  sgtl5000_1.volume(0.25);

  sgtl5000_1.lineOutLevel(21); //11-32, the smaller the louder. 21 is about 2 Volts peak to peak

  // begin(volume from 0.0-1.0 , frequency , shape of oscillator)
  // See the tool for more info https://www.pjrc.com/teensy/gui/?info=AudioSynthWaveform
  waveform1.begin(1, 220.0, WAVEFORM_SINE);
  waveform2.begin(1, 440.0, WAVEFORM_SINE);

  envelope1.attack(10);
  envelope1.decay(50);
  envelope1.sustain(.25);
  envelope1.release(500);

  envelope2.attack(10);
  envelope2.decay(50);
  envelope2.sustain(.25);
  envelope2.release(500);


  mixer1.gain(0, .5);
  mixer1.gain(1, .5);
  mixer1.gain(2, 0);
  mixer1.gain(3, 0);

} //setup is over

void loop() {
  update_controls();
  current_time = millis();


  //DIN MIDI input
  if (MIDI.read()) {   // Is there a MIDI message incoming ?
    dm_type = MIDI.getType();
    dm_channel_recived = MIDI.getChannel();
    if (0) { //change to 1 to print the channel
      Serial.print("dm channel recived: ");
      Serial.print(dm_channel_recived);
    }

    //only do all of this if it's the channel we want
    // "||" is "logical or", allowing us to go in when dm_channel_select is 0 aka Omni
    if (dm_channel_recived == dm_channel_select || dm_channel_select == 0) {
      if (dm_type == midi::NoteOn) {
        dm_note = MIDI.getData1();
        dm_velocity = MIDI.getData2();
        if (dm_velocity > 0) {
          Serial.print("Note On:");
          Serial.print(dm_note);
          Serial.print("  velocity: ");
          Serial.println(dm_velocity);
          
          
           //don't do all the work here, just set a "flag"
           // There are several places we will want to turn the envelopes on or off
           // instead of copping that code to each place lets just turn a variable to 1, then do the work later on and turn it back to 0        
          env1_on = 1; 
        

        }

        if (dm_velocity == 0) { //some systems send velocity 0 as note off
          Serial.print("Note Off: ");
          Serial.println(dm_note);
          env1_off = 1;

        }
      }

      else if (dm_type == midi::NoteOff) { //else if can be used so we only have one outcome
        dm_note = MIDI.getData1();
        Serial.print("Note Off: ");
        Serial.println(dm_note);
        env1_off = 1;
      }

      else if (dm_type == midi::ControlChange) {
        dm_cc_num = MIDI.getData1();
        dm_cc_val = MIDI.getData2();
        Serial.print("CC#: ");
        Serial.print(dm_cc_num);
        Serial.print("  value: ");
        Serial.println(dm_cc_val);
      }

      //If it's not a note on or off or cc do this.
      // If there were just "ifs" being used above there could be a final else that would catch all the other message types
      // the other types are :
      /*
        midi::AfterTouchPoly
        midi::ProgramChange
        midi::AfterTouchChannel
        midi::PitchBend
        midi::SystemExclusive
      */

      else {
        dm_data1 = MIDI.getData1();
        dm_data2 = MIDI.getData2();

        Serial.print(dm_type);
        Serial.print("  ");
        Serial.print(dm_data1);
        Serial.print("  ");
        Serial.println(dm_data2);

      }
    }
  }



  //DIN Send
  //There are lots of types of midi messages. Here are the most use ones. All of them are here https://www.pjrc.com/teensy/td_midi.html jsut cahnge it to MIDI instead of usb MIDI
  /*
    MIDI.sendNoteOn(note, velocity, channel);
    MIDI.sendNoteOff(note, velocity, channel);
    MIDI.sendControlChange(control, value, channel);
    MIDI.sendAfterTouch(pressure, channel);
    MIDI.sendPitchBend(value, channel);
  */
  if (buttonState(0) == FELL) {
    MIDI.sendNoteOn(40, 127, 1);
  }
  if (buttonState(0) == ROSE) {
    MIDI.sendNoteOff(40, 0, 1);
  }
  prev_cc_pot[0] = cc_pot[0];
  cc_pot[0] = potRead(0) * 127; //ccs and most all midi messages are only 0-127, 7 bits
  if (prev_cc_pot[0] != cc_pot[0]) { //only send if the readings are different
    MIDI.sendControlChange(20, cc_pot[0], 1);
  }

  ////////////////////

  //USB MIDI works just the same, use change it to usbMIDI.whatever
  //USB recieve
  if (usbMIDI.read()) {   // Is there a USB MIDI message incoming ?
    um_type = usbMIDI.getType();
    um_channel_recived = usbMIDI.getChannel();
    if (0) { //cahge to 1 to print the channel
      Serial.print("um channel recived: ");
      Serial.print(um_channel_recived);
    }

    //only do all of this if it's the channel we want
    // "||" is "logical or", allowing us to go in when um_channel_select is 0 aka Omni channel
    if (um_channel_recived == um_channel_select || um_channel_select == 0) {
      if (um_type == midi::NoteOn) {
        um_note = usbMIDI.getData1();
        um_velocity = usbMIDI.getData2();
        if (um_velocity > 0) {
          env1_on = 1;
          Serial.print("USB note On:");
          Serial.print(um_note);
          Serial.print("  velocity: ");
          Serial.println(um_velocity);
        }
        if (um_velocity == 0) { //some systems send velocity 0 as note off
          Serial.print("USB note Off: ");
          Serial.println(um_note);
          env1_off = 1;
        }
      }

      else if (um_type == midi::NoteOff) { //else if can be used so we only have one outcome
        um_note = usbMIDI.getData1();
        Serial.print("USB note Off: ");
        Serial.println(um_note);
        env1_off = 1;
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
  }

  //USB send. Same as DIN but wit usbMIDI
  if (buttonState(1) == FELL) {
    usbMIDI.sendNoteOn(50, 127, 1);
  }
  if (buttonState(1) == ROSE) {
    usbMIDI.sendNoteOff(50, 0, 1);
  }
  prev_cc_pot[1] = cc_pot[1];
  cc_pot[1] = potRead(1) * 127; //CCs and most all midi messages are only 0-127, 7 bits
  if (prev_cc_pot[1] != cc_pot[1]) { //only send if the readings are different
    usbMIDI.sendControlChange(21, cc_pot[1], 1);
  }



  if (current_time - prev_time[0] > 33) {
    prev_time[0] = current_time;
    LEDs.show();
  }


//after checking the midi , we check to see if we set these to 1
  if (env1_on == 1) {

    //this changes the frequencies but since it's multiplying it has more of an effect on higher notes, messing up the intervals between them
    //float transpose_freq = (potRead(7))*4.0;
    //float keybaorb_note =chromatic[dm_note];
    //float final_freq = keybaorb_note * transpose_freq;

    //here we have three things controlling the note that finally gets played
    // on pot goes from -50 to 50, shifting the notes up or down by one
    // another pot shifts by octaves, shifting up 0,12,24,36,48
    // then we have dm_note from DIN and um_note from USB. If we don't get info from one of them it will just be 0
    // if we get both it would be combined 
    // then we constrain it to the range of the chromatic array. You don't want to go looking outside of the bounds of an array
    // finally we take this combined value and plug it into the chromatic array, resulting in a single note to set the oscillator to
    int transpose_note = (potRead(7) * 100) - 50;
    int octave_shift = (potRead(6) * 4);
    int note_select = dm_note + um_note+ transpose_note + (octave_shift * 12);
    if (note_select >= 87) {
      note_select = 87;
    }
    if (note_select < 0) {
      note_select = 0;
    }
    float final_freq = chromatic[note_select];

    waveform1.frequency(final_freq);
    envelope1.noteOn();
    
    float hue1 = note_select / 87.0; //set the an LED to match the note
    set_LED(0, hue1, 1, 1);

    env1_on = 0;
  }

    if (env1_off == 1) { //the note off is much simpler
    envelope1.noteOff();
    //turn the led off by setting the brightness to 
    //this means light will only be on for however long the "LEDs.show();" update rate as the note on and off are so short
    set_LED(0, 0, 1, 0); 

    env1_off = 0; //close the door behind us so this will only happen once when the note off is received
  }

  //////////////////////////

  if (current_time - prev_time[0] > 500 && 0) {
    prev_time[0] = current_time;

    Serial.print("processor: ");
    Serial.print(AudioProcessorUsageMax());
    Serial.print("%    Memory: ");
    Serial.print(AudioMemoryUsageMax());
    Serial.println();
    AudioProcessorUsageMaxReset(); //We need to reset these values so we get a real idea of what the audio code is doing rather than just peaking in every half a second
    AudioMemoryUsageMaxReset();
  }

}// loop is over
