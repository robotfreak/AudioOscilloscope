#ifndef AusioAnalyzeOscilloscope_h_
#define AusioAnalyzeOscilloscope_h_
#include <AudioStream.h>
#define NO_OF_BLOCKS 8



class AudioAnalyzeOscilloscope : public AudioStream {
  public:
    AudioAnalyzeOscilloscope(void) : AudioStream(1, inputQueueArray) {
    }
    virtual void update(void);

   	bool available() {
	  	if (outputflag == true) {
		 	 outputflag = false;
			 return true;
		  }
		 return false;
	  }

 
    int16_t buffer[AUDIO_BLOCK_SAMPLES * NO_OF_BLOCKS] __attribute__ ((aligned (4)));

  private:
    audio_block_t *inputQueueArray[1];
  	volatile bool outputflag;
};
#endif
