/*
 *  tested on JM's hardware and works 10-16-20
*/

#include "mem_sampler.h"
#include "arm_math.h"
#include "utility/dspinst.h"



void MemSampler::update(void)
{
  audio_block_t *ab_left;
  audio_block_t *ab_right;


  if (playing == 0) return;
  ab_left = allocate();
  ab_right = allocate();

  if (ab_left) {

    for (int i = 0; i < AUDIO_BLOCK_SAMPLES; i++) {

      if (playing == 0) {
        ab_left->data[i] = 0;
        ab_right->data[i] = 0;
      }

      if (playing == 1) {

        prev_index1 = index1;
        index1 = accumulator >> (32 - 8);

        if (prev_index1 > 128 && index1 < 128) {
          index2 += 255;
        }

        if (index2 + index1 >= end_mod) {
          index2 = start_pos;
          if (looping == 0) {
            playing = 0;
          }

        }
        uint32_t loc = index2 + index1;
        if (rev == 1) {
          loc = (sample_len - loc);
        }

        ab_left->data[i] = sampleMemory[loc];
        ab_right->data[i] = sampleMemory[loc];
        accumulator += increment;
      }



    }

    transmit(ab_left, 0);
    transmit(ab_right, 1);

    release(ab_left);
    release(ab_right);
  }
}
