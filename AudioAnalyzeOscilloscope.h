#ifndef AusioAnalyzeOscilloscope_h_
#define AusioAnalyzeOscilloscope_h_
#include <AudioStream.h>
//#include <ILI9341_t3.h>
#define NO_OF_BLOCKS 2



class AudioAnalyzeOscilloscope : public AudioStream {
  public:
    AudioAnalyzeOscilloscope(void) : AudioStream(1, inputQueueArray) {
    }
    virtual void update(void);
    //void MeasureSetup(int16_t *meas);
    //void Measure(void);
    void AddtoBuffer(int16_t*);
    int16_t buffer[AUDIO_BLOCK_SAMPLES * NO_OF_BLOCKS];

  private:
    audio_block_t *inputQueueArray[1];
    //int16_t *measure;
    //int16_t old_val[AUDIO_BLOCK_SAMPLES];
    uint32_t count = 0;
};
#endif
/*
  void AudioAnalyzeOscilloscope::MeasureSetup(int16_t *meas) {
  __disable_irq();
  measure = meas;
  __enable_irq();
  Serial.println("Init AusioAnalyzeoscilloscope");
  }

  void AudioAnalyzeOscilloscope::Measure() {
  __disable_irq();
  boolean crossing = false;
  int16_t pixel_x = 0;
  uint32_t i = 0;
  do {
    int16_t wave_data = buffer[i];
    if (crossing || wave_data > 0) {
      i++;
      wave_data = buffer[i];
      if (crossing || wave_data <0 ) {
        crossing = true;
        int16_t pixel_y = map(wave_data, 32767, -32768, -127, 127) + 128;
        //display->drawPixel(pixel_x + 15, old_val[pixel_x], 0);//Remove previous pixel
        //display->drawPixel(pixel_x + 15, pixel_y, 0x07B0);//New pixel
        //old_val[pixel_x] = {pixel_y};
        pixel_x++;
      }
    }
    i++;
  } while (i < AUDIO_BLOCK_SAMPLES * NO_OF_BLOCKS);

  __enable_irq();
  }
*/
void AudioAnalyzeOscilloscope::AddtoBuffer(int16_t *audio) {
  const int16_t *begin = audio;
  const int16_t *end = audio + AUDIO_BLOCK_SAMPLES;
  __disable_irq();
  do {
    buffer[count++] = *audio;
    audio++;
  } while (audio < end);
  if (count > (AUDIO_BLOCK_SAMPLES * NO_OF_BLOCKS) - 1) {
    count = 0;
    //Measure();
  }
  __enable_irq();
}

void AudioAnalyzeOscilloscope::update(void) {
  //if (!display) return;
  audio_block_t *block;
  block = receiveReadOnly(0);
  if (block) {
    AddtoBuffer(block->data);
    release(block);
  }
}
