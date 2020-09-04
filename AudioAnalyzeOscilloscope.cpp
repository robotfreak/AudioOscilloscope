
#include <Arduino.h>
#include <Audio.h>
#include <AudioStream.h>
#include "AudioAnalyzeOscilloscope.h"

static void copyBuffer(void *destination, const void *source)
{
	const uint16_t *src = (const uint16_t *)source;
	uint32_t *dst = (uint32_t *)destination;

	for (int i=0; i < AUDIO_BLOCK_SAMPLES; i++) {
		*dst++ = *src++;  // real sample plus a zero for imaginary
	}
}

void AudioAnalyzeOscilloscope::update(void) {
  audio_block_t *block;
  block = receiveReadOnly(0);
  if (block) {
    copyBuffer(buffer, block->data);
	outputflag = true;
    release(block);
  }
}
