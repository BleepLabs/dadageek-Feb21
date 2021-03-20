/* 
 *  tested on JM's hardware and works 10-16-20
*/

#ifndef synth_sampler_h_
#define synth_sampler_h_

#include "Arduino.h"
#include "AudioStream.h"
#include "arm_math.h"
#include <SerialFlash.h>
#include "spi_interrupt.h"


#define AUDIO_SAMPLE_RATE_ROUNDED (44118)


class AudioSampler :
  public AudioStream
{

  public:
    AudioSampler(void) :
      AudioStream(0, NULL), tone_amp(0), tone_freq(0),
      tone_width(0.25), tone_incr(0),
      looping(0), playing(0)
    {
    }

    void frequency(float t_freq) {
      t_freq = norm * t_freq;
      if (t_freq < 0.0) t_freq = 0.0;
      else if (t_freq > AUDIO_SAMPLE_RATE_EXACT / 2) t_freq = AUDIO_SAMPLE_RATE_EXACT / 2;
      tone_incr = (t_freq * (0x80000000LL / AUDIO_SAMPLE_RATE_EXACT)) + 0.5;
    }

    void amplitude(float n) {        // 0 to 1.0
      if (n < 0) n = 0;
      else if (n > 1.0) n = 1.0;
      if ((tone_amp == 0) && n) {
        // reset the phase when the amplitude was zero
        // and has now been increased.
        tone_phase = 0;
      }
      // set new magnitude
      tone_amp = n * 32767.0;
    }

    void begin(float t_amp, float t_freq, uint32_t s_pos, uint32_t len) {
      amplitude(t_amp);
      frequency(t_freq);
      sample_len = len;
      start_pos = s_pos;
      end_mod = len;
      mod_start_pos = 0;
      get = 0;
    }

    void sample_loop(int bb) {
      looping = bb;
    }

    void sample_play_loc(uint32_t s_pos, uint32_t len) {

      if (rev == 0) {
        fpos = 0;
        sample_len = len;
        start_pos = s_pos; //44 BYTES IN WAVE HEADER
        end_mod = len;
        index = 0;
        tone_phase = 0;
        get = 0;
        bankshift = 0;
        bankoff = 0;


        AudioStartUsingSPI();
        SerialFlash.read(start_pos + (fpos * 4), in8b, buffrl);
        AudioStopUsingSPI();

        js = 0;
        for (uint16_t  j = 0; j < buffrl - 4; j += 4) {
          int16_t new_left = (in8b[j + 1] << 8) | (in8b[j]);
          int16_t new_right = (in8b[j + 3] << 8) | (in8b[j + 2]);
          sample_left_A[js] = new_left;
          sample_right_A[js] = new_right;
          js++;
        }

        playing = 1;

      }

      if (rev == 1) {
        sample_len = len;
        rev_start = s_pos ;
        fpos = len;
        rev_end = s_pos;
        rev_mod_start_pos = rev_start;
        index = 0;
        tone_phase = 0;
        get = 0;
        bankshift = 3;
        bankoff = bankshift << 8;
        // Serial.print("rev_start "); Serial.println(rev_start);
        // Serial.print("rev_end "); Serial.println(rev_end);
        // Serial.println();
        playing = 1;

      }

    }



    void sample_stop() {
      playing = 0;
    }

    uint16_t sample_status() {
      return playing;
    }

    void sample_reverse(byte bb) {
      rev = bb;
      if (rev != prev_rev) {
        get = 1;
      }
      prev_rev = bb;
      //fpos = sample_len;

    }

    uint32_t sample_start_pos(float pd) {
      mod_start_pos = pd * (sample_len - 256);
      return rev_mod_start_pos;
    }

    uint32_t sample_play_len(float pd) {
      //pd=(pd*-1.00)+1.00;
      if (pd > 0) {
        end_mod_en = 1;
        end_mod = (pd * sample_len) + 256;
      }
      if (pd == 0) {
        end_mod_en = 0;
      }
      return end_mod;
    }

    uint32_t sample_loc(void) {
      uint32_t a = (uint32_t)index;
      return sample_address + a;
    }

    uint32_t revstart() {
      return rev_start;
    }

    uint32_t revend() {
      return rev_end;
    }

    byte loop_trig() {
      return loop_trig1;
    }

    uint32_t transfer_time() {
      return gfd;
    }

    virtual void update(void);

  private:
#define SPI_SIZE (256) // how many 16b samples in each channel 
    //2048 35ms single sample
    uint32_t gfcm, gfd;
    float norm = 86.1679 * 2; //normal playback speed

    short    tone_amp;
    short    tone_freq;
    uint32_t tone_phase;
    uint32_t index_assume;
    int32_t rev_start, rev_end;
    uint32_t tone_width;
    short sample;
    // volatile prevents the compiler optimizing out the frequency function
    volatile uint32_t tone_incr;
    short    tone_type;
    const int16_t *arbdata;
    //int16_t *sfsample;
    //int16_t *sfsample16;
    int16_t js;
    uint32_t sf_pos;
    uint32_t sample_address;
    uint32_t sample_len, start_pos, mod_start_pos, end_mod, rev_mod_start_pos;
    char in8[(SPI_SIZE * 4) + 8];
    char in8b[(SPI_SIZE * 4) + 8];

    int16_t sample_left_A[(SPI_SIZE) + 2];
    int16_t sample_right_A[(SPI_SIZE) + 2];

    uint16_t get, end_flag;
    const uint16_t buffrl = (SPI_SIZE * 4) + 8;
    uint16_t pindex;
    uint32_t index, scale, end_mod_en;
    int32_t revindex = 256;
    int32_t fpos, blocknum;
    uint16_t looping, playing, rev, bankoff;
    int16_t bankshift, bankswitch;
    byte loop_trig1;
    byte prev_rev;
};


#endif
