/*
   A simple hardware test which receives audio from the audio shield
   Line-In pins and send it to the Line-Out pins and headphone jack.

   This example code is in the public domain.
*/
#include <ILI9341_t3.h>
#include <font_Arial.h> // from ILI9341_t3
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
AudioInputI2S            i2s1;           //xy=985.7142677307129,418.5714530944824
AudioOutputI2S           i2s2;           //xy=1246.4285888671875,417.8571472167969
AudioAnalyzeFFT256       fft256_2;       //xy=1252.8570823669434,480.0000305175781
AudioAnalyzeRMS          rms2;           //xy=1252.857177734375,534.2857437133789
AudioAnalyzeRMS          rms1;           //xy=1257.142807006836,300.00000381469727
AudioAnalyzeFFT256       fft256_1;       //xy=1257.142993927002,355.71428203582764
AudioAnalyzeOscilloscope scope1;
AudioAnalyzeOscilloscope scope2;
AudioConnection          patchCord1(i2s1, 0, i2s2, 0);
AudioConnection          patchCord2(i2s1, 0, rms1, 0);
AudioConnection          patchCord3(i2s1, 0, fft256_1, 0);
AudioConnection          patchCord4(i2s1, 1, i2s2, 1);
AudioConnection          patchCord5(i2s1, 1, fft256_2, 0);
AudioConnection          patchCord6(i2s1, 1, rms2, 0);
AudioConnection          patchCord7(i2s1, 0, scope1, 0);
AudioConnection          patchCord8(i2s1, 1, scope2, 0);
AudioControlSGTL5000     sgtl5000_1;     //xy=1166.2856674194336,594.9999332427979
// GUItool: end automatically generated code


const int myInput = AUDIO_INPUT_LINEIN;
//const int myInput = AUDIO_INPUT_MIC;

void printFFTNumber(int i, float n) {

  if (n >= 0.004) {
    Serial.print(n, 3);
    Serial.print(" ");
    // draw the verticle bars
    int height = n * 256;
    tft.fillRect(i * 20, 220 - height, 16, height, ILI9341_GREEN);
    tft.fillRect(i * 20, 220 - 140, 16, 140 - height, ILI9341_BLACK);
  } else {
    Serial.print("   -  "); // don't print "0.00"
  }

  /*
    if (n > 0.25) {
    Serial.print("***** ");
    } else if (n > 0.18) {
    Serial.print(" ***  ");
    } else if (n > 0.06) {
    Serial.print("  *   ");
    } else if (n > 0.005) {
    Serial.print("  .   ");
    }
  */
}

void plotFFT(void)
{
  // print Fourier Transform data to the Arduino Serial Monitor
  if (fft256_1.available()) {

    Serial.print("FFT: ");
    for (int i = 0; i < 16; i++) { // 0-25  -->  DC to 1.25 kHz
      float n = fft256_1.read(i);
      printFFTNumber(i, n);
    }
    Serial.println();
  }

}

void plot2ChanOsci(void)
{
  int x;
  int y = 0;

  tft.fillScreen(ILI9341_BLACK);
  for (x = 0; x < 256; x++) {
    if (x%8==0) {
      tft.drawPixel(x + 20,64, ILI9341_WHITE);
      tft.drawPixel(x + 20,192, ILI9341_WHITE);
    }
    y = scope1.buffer[x] >> 8;;
    tft.drawPixel(x + 20, y + 64, ILI9341_GREEN);
    y = scope2.buffer[x] >> 8;;
    tft.drawPixel(x + 20, y + 192, ILI9341_GREEN);
  }
}

void plotGoniometer(void)
{
  int i,l;
  int x = 0;
  int y = 0;
  int newx, newy;

  tft.fillRect(0, 0, 256, 240, ILI9341_BLACK);
  for (i = 0; i < 256; i++) {
    x = (scope1.buffer[i] >> 8);
    //tft.drawPixel(x+20, y+64, ILI9341_GREEN);
    y = (scope2.buffer[i] >> 8);
    newx = x * 29 / 41 - y * 29 / 41;
    newy = y * 29 / 41 + x * 29 / 41;
    tft.drawPixel(newx + 128, newy + 128, ILI9341_GREEN);
    if (i%8==0) {
      tft.drawPixel(i,128, ILI9341_WHITE);
    }
    if (i==128) {
      for(l=0;l<240;l+=8) {
        tft.drawPixel(i,l, ILI9341_WHITE);
      }
    }
  }
}

elapsedMillis msecs = 0;

void plotRMS(void)
{
  if (msecs > 15) {
    if (rms1.available() && rms2.available()) {
      msecs = 0;
      float leftNumber = rms1.read();
      float rightNumber = rms2.read();
      Serial.print(leftNumber);
      Serial.print(", ");
      Serial.print(rightNumber);
      Serial.println();

      // draw the verticle bars
      int height = leftNumber * 128;
      tft.fillRect(260, 200 - height, 20, height, ILI9341_GREEN);
      tft.fillRect(260, 200 - 128, 20, 128 - height, ILI9341_BLACK);
      height = rightNumber * 128;
      tft.fillRect(290, 200 - height, 20, height, ILI9341_GREEN);
      tft.fillRect(290, 200 - 128, 20, 128 - height, ILI9341_BLACK);
      // a smarter approach would redraw only the changed portion...

      // draw numbers underneath each bar
      tft.setFont(Arial_10);
      tft.fillRect(260, 220, 30, 16, ILI9341_BLACK);
      tft.setCursor(260, 220);
      tft.print(leftNumber);
      tft.fillRect(290, 220, 30, 16, ILI9341_BLACK);
      tft.setCursor(290, 220);
      tft.print(rightNumber);
    }
  }
}

unsigned long currentMillis, previousMillis;
int mode = 0;

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

  previousMillis = millis();

}


void loop() {
  currentMillis = millis();
  if (currentMillis - previousMillis > 10000)
  {
    previousMillis = currentMillis;
    tft.fillScreen(ILI9341_BLACK);
    mode++;
  }
  switch (mode) {
    case 0:
      scope1.update();
      scope2.update();
      plot2ChanOsci();
      break;
    case 1:
      scope1.update();
      scope2.update();
      plotGoniometer();
      break;
    case 2:
      plotFFT();
      break;
    case 3:
      plotRMS();
      break;
    case 4:
      scope1.update();
      scope2.update();
      plotGoniometer();
      plotRMS();
      break;
    default:
      mode = 0;
      break;
  }

}
