/*
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
AudioSynthWaveform       waveform2;      //xy=167,267
AudioSynthWaveform       waveform1;      //xy=175,215
AudioMixer4              mixer1;         //xy=383,245
AudioOutputI2S           i2s1;           //xy=591,244
AudioConnection          patchCord1(waveform2, 0, mixer1, 1);
AudioConnection          patchCord2(waveform1, 0, mixer1, 0);
AudioConnection          patchCord3(mixer1, 0, i2s1, 0);
AudioConnection          patchCord4(mixer1, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=484,355
// GUItool: end automatically generated code


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

  mixer1.gain(0, 0);
  mixer1.gain(1, 0);
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
    if (0) { //cahge to 1 to print the channel
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
        }
        if (dm_velocity == 0) { //some systems send velocity 0 as note off
          Serial.print("Note Off: ");
          Serial.println(dm_note);

        }
      }

      else if (dm_type == midi::NoteOff) { //else if can be used so we only have one outcome
        dm_note = MIDI.getData1();
        Serial.print("Note Off: ");
        Serial.println(dm_note);
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
    MIDI.sendControlChange(20, cc_pot, 1);
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
          Serial.print("USB note On:");
          Serial.print(um_note);
          Serial.print("  velocity: ");
          Serial.println(um_velocity);
        }
        if (um_velocity == 0) { //some systems send velocity 0 as note off
          Serial.print("USB note Off: ");
          Serial.println(um_note);

        }
      }

      else if (um_type == midi::NoteOff) { //else if can be used so we only have one outcome
        um_note = usbMIDI.getData1();
        Serial.print("USB note Off: ");
        Serial.println(um_note);
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
    usbMIDI.sendControlChange(21, cc_pot, 1);
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
