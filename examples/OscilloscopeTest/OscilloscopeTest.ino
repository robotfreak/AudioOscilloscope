#include <ILI9341_t3.h>
#include <font_Arial.h> // from ILI9341_t3
#include <SPI.h>
#include "AudioAnalyzeOscilloscope.h"

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

AudioPlaySdWav           playSdWav1;
AudioAnalyzeOscilloscope scope1;
AudioAnalyzeOscilloscope scope2;
AudioOutputI2S           i2s1;
AudioConnection          patchCord1(playSdWav1, 0, i2s1, 0);
AudioConnection          patchCord2(playSdWav1, 1, i2s1, 1);
AudioConnection          patchCord3(playSdWav1, 0, scope1, 0);
AudioConnection          patchCord4(playSdWav1, 1, scope2, 0);
AudioControlSGTL5000     sgtl5000_1;

// Use these with the Teensy Audio Shield
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14

// For optimized ILI9341_t3 library
#define TFT_DC      5
#define TFT_CS      4
#define TFT_RST     255  // 255 = unused, connect to 3.3V

//ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK, TFT_MISO);
ILI9341_t3 tft = ILI9341_t3(TFT_CS, TFT_DC);
/*
  void enableScope(boolean enable) {
  enable ? scope.ScreenSetup(&tft) : scope.ScreenSetup(NULL);
  }
*/

void plotDataPoints(void)
{
  int x;
  int y = 0;

  tft.fillScreen(ILI9341_BLACK);
  for (x = 0; x < 256; x++) {
    y = scope1.buffer[x] >> 9;;
    tft.drawPixel(x + 20, y + 128, ILI9341_GREEN);
  }
}

void plotXYDataPoints(void)
{
  int i;
  int x = 0;
  int y = 0;

  tft.fillScreen(ILI9341_BLACK);
  for(i=0;i<256; i++) { 
    x = scope1.buffer[i] >> 9;;
    //tft.drawPixel(x+20, y+64, ILI9341_GREEN);
    y = scope2.buffer[i] >> 9;;
    tft.drawPixel(x+128, y+128, ILI9341_GREEN);
  }
}

void setup() {
  Serial.begin(115200);
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(Arial_24);
  //tft.setTextSize(3);
  //tft.setCursor(40, 8);
  //tft.println("Oscillosope");

  AudioMemory(8);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);
  //enableScope(true);

  //SPI.setMOSI(SDCARD_MOSI_PIN);
  //SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }
  delay(1000);

}

void loop() {
  if (playSdWav1.isPlaying() == false) {
    Serial.println("Start playing");
    playSdWav1.play("SDTEST1.WAV");
    delay(10); // wait for library to parse WAV info
  }
  scope1.update();
  scope2.update();
  plotXYDataPoints();
}
