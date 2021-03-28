
#ifndef synth_MemSampler_h_
#define synth_MemSampler_h_

#include "Arduino.h"
#include "AudioStream.h"
#include "arm_math.h"

#define AUDIO_SAMPLE_RATE_ROUNDED (44118)


class MemSampler :
  public AudioStream
{

public:
  MemSampler(void) :
    AudioStream(0, NULL) {
  }

  void frequency(float t_freq) {
    t_freq = norm * t_freq;
    if (t_freq < 0.0) t_freq = 0.0;
    else if (t_freq > AUDIO_SAMPLE_RATE_EXACT / 2) t_freq = AUDIO_SAMPLE_RATE_EXACT / 2;
    increment = (t_freq * (0x80000000LL / AUDIO_SAMPLE_RATE_EXACT)) + 0.5;
  }


  void begin(unsigned int *in_sample, uint32_t len) {
    frequency(norm);
    sampleMemory = in_sample;
    sample_len = len;
    start_pos = 0;
    end_mod = len;
  }

  void start_location(uint32_t starts) {
    start_pos = starts;
    if (start_pos >= sample_len) {
      start_pos = sample_len - 255;
    }
  }

  void play_length(uint32_t plength) {
    end_mod=start_pos+plength;
    if (end_mod >= sample_len) {
      end_mod = sample_len;
    }
  }

  void sample_loop(int bb) {
    looping = bb;
  }

  void play() {
    index2 = start_pos;
    index1 = 0;
    playing = 1;
  }

  void stop() {
    playing = 0;
  }

  void sample_reverse(byte bb) {
    rev = bb;
  }

  virtual void update(void);

private:
// volatile prevents the compiler optimizing out the frequency function
  volatile uint32_t increment;
  uint32_t accumulator;
  float norm = 86.1679 * 2; //normal playback speed
  unsigned int *sampleMemory;
  short    tone_freq;
  int32_t rev_start, rev_end;

  short    tone_type;
  int16_t js;
  uint32_t sf_pos;
  uint32_t sample_address;
  uint32_t sample_len, start_pos, mod_start_pos, end_mod, rev_mod_start_pos;
  uint32_t index1, index2, prev_index1, scale, end_mod_en;
  int32_t fpos, blocknum;
  uint16_t looping, playing, rev, bankoff;
  int16_t bankshift, bankswitch;
  uint32_t print_count;
  byte loop_trig1;
  byte prev_rev;
};


#endif
