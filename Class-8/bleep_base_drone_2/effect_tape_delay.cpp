
#include "effect_tape_delay.h"
#include "arm_math.h"
#include "utility/dspinst.h"

void AudioEffectTapeDelay::begin(short *delayline, uint32_t max_len, uint32_t dly_len, short redux, short lerp)
{
  max_dly_len = max_len - 1;

  desired_delay_length = dly_len;
  if (desired_delay_length > max_dly_len) {
    desired_delay_length = max_dly_len;
  }
  l_delayline = delayline;
  write_head = 0;

  rate_redux = redux;
  lerp_len = lerp;
}

void AudioEffectTapeDelay::sampleRate(short redux)
{
  rate_redux = redux;
}

uint32_t AudioEffectTapeDelay::length(uint32_t dly_len)
{
  desired_delay_length = dly_len;
  if (desired_delay_length > max_dly_len) {
    desired_delay_length = max_dly_len;
  }
  return delay_length;
}

uint32_t AudioEffectTapeDelay::length_no_lerp(uint32_t dly_len)
{
  delay_length = dly_len;
  desired_delay_length=dly_len;
  if (delay_length > max_dly_len) {
    delay_length = max_dly_len;
  }
  return delay_length;
}

void AudioEffectTapeDelay::update(void)
{
  audio_block_t *block;
  short *bp;
  //static uint32_t preva;
  static short tick, tock;
  int input1;
static byte mm;
  if (l_delayline == NULL)return;
  uint32_t max_dly_len_m = max_dly_len;
  block = receiveWritable(0);
  if (block) {
    bp = block->data;

    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {

      tick++;
      if (tick > lerp_len) {

        if (delay_length < desired_delay_length - 1) {
          delay_length++;
        }

        if (delay_length > desired_delay_length + 1) {
          delay_length--;
        }
        tick = 0;
      }


      tock++;
      if (tock > rate_redux) {
        tock = 0;
        write_head++;
      }
      if (write_head >= max_dly_len_m) {
        write_head = 0;
      }

      read_head = ((write_head) + delay_length);

      if (read_head > (max_dly_len_m - 1)) {
        read_head -= (max_dly_len_m);
      }

      l_delayline[write_head] = *bp ;
      *bp++ = l_delayline[read_head];
    }

    transmit(block);
    release(block);
  }
}
