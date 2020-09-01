/*
 * A simple hardware test which receives audio from the audio shield
 * Line-In pins and send it to the Line-Out pins and headphone jack.
 *
 * This example code is in the public domain.
 */
#include <ILI9341_t3.h>
#include <font_Arial.h> // from ILI9341_t3
#include <Bounce.h>
#include "AudioAnalyzeOscilloscope.h"

// For optimized ILI9341_t3 library
#define TFT_DC      5
#define TFT_CS      4
#define TFT_RST     255  // 255 = unused, connect to 3.3V

//ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK, TFT_MISO);
ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC);

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioInputI2S            i2s1;           //xy=200,69
AudioAnalyzeOscilloscope scope1;     
AudioAnalyzeOscilloscope scope2;     
AudioOutputI2S           i2s2;           //xy=365,94
AudioConnection          patchCord1(i2s1, 0, i2s2, 0);
AudioConnection          patchCord2(i2s1, 1, i2s2, 1);
AudioConnection          patchCord3(i2s1, 0, scope1, 0);
AudioConnection          patchCord4(i2s1, 1, scope2, 0);
AudioControlSGTL5000     sgtl5000_1;     //xy=302,184
// GUItool: end automatically generated code


const int myInput = AUDIO_INPUT_LINEIN;
//const int myInput = AUDIO_INPUT_MIC;

void plotXYDataPoints(void)
{
  int i;
  int x = 0;
  int y = 0;

  tft.fillScreen(ILI9341_BLACK);
  for(i=0;i<256; i++) { 
    x = scope1.buffer[i] >> 8;;
    //tft.drawPixel(x+20, y+64, ILI9341_GREEN);
    y = scope2.buffer[i] >> 8;;
    tft.drawPixel(x+128, y+128, ILI9341_GREEN);
  }
}

void setup() {
  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(12);
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(Arial_24);

  // Enable the audio shield, select input, and enable output
  sgtl5000_1.enable();
  sgtl5000_1.inputSelect(myInput);
  sgtl5000_1.volume(0.5);
}

elapsedMillis volmsec=0;

void loop() {
  scope1.update();
  scope2.update();
  plotXYDataPoints();

  // every 50 ms, adjust the volume
  if (volmsec > 50) {
    float vol = analogRead(15);
    vol = vol / 1023.0;
    //audioShield.volume(vol); // <-- uncomment if you have the optional
    volmsec = 0;               //     volume pot on your audio shield
  }
}
