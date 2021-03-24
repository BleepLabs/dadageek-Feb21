#include "Arduino.h"
#include "AudioStream.h"
#include "arm_math.h"
#include "spi_interrupt.h"

class AudioEffectTapeDelay :
  public AudioStream
{
  public:
    AudioEffectTapeDelay(void):
      AudioStream(1, inputQueueArray) {
    }
    void begin(short *delayline, uint32_t max_len, uint32_t dly_len, short redux, short lerp);
    uint32_t length(uint32_t dly_len);
    uint32_t length_no_lerp(uint32_t dly_len);
    void sampleRate(short redux);
    virtual void update(void);

  private:
    uint32_t dlyd, dlyt;
    audio_block_t *inputQueueArray[1];
    short *l_delayline;
    uint32_t delay_length, desired_delay_length;
    int32_t inv_delay_length;
    uint32_t max_dly_len;
    uint32_t write_head;
    uint32_t delay_depth;
    uint32_t rate_redux;
    uint32_t delay_offset_idx;
    uint32_t   delay_rate_incr;
    uint32_t read_head, feedback;
    short SIMPLE_SMOOTH, lerp_len;
    uint32_t l_delay_rate_index;

    short sync_out_latch;
    short sync_out_count;
};
