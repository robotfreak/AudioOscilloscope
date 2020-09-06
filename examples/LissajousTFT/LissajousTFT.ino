/* LissajousTFT: shows lissajous figures on a TFT screen
* two waveforms are generated with different parameters.
* Frequency can be adjusted with two potentiometers (A2,A3)
* A button on Pin 0 is used to switch between waveforms
* The two audio streams are used as source for the Goniometer.
* Uses two AudioAnalyzeOscilloscope objects with 1024 audio sample depth
*
* Example based on: */
// Advanced Microcontroller-based Audio Workshop
//
// http://www.pjrc.com/store/audio_tutorial_kit.html
// https://hackaday.io/project/8292-microcontroller-audio-workshop-had-supercon-2015
// 
// Part 2-8: Oscillators

#include <ILI9341_t3.h>
#include <font_Arial.h> // from ILI9341_t3
#include <Bounce.h>
#include "AudioAnalyzeOscilloscope.h"

Bounce button0 = Bounce(0, 15);
Bounce button1 = Bounce(1, 15);  // 15 = 15 ms debounce time
Bounce button2 = Bounce(2, 15);

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
AudioSynthWaveform       waveform2; //xy=899,642
AudioSynthWaveform       waveform1; //xy=905,540
AudioAnalyzeOscilloscope scope1;     
AudioAnalyzeOscilloscope scope2;     
AudioMixer4              mixer1; //xy=1119,558
AudioMixer4              mixer2; //xy=1119,660
AudioOutputI2S           i2s1; //xy=1393,578
AudioConnection          patchCord1(waveform2, 0, mixer2, 0);
AudioConnection          patchCord2(waveform1, 0, mixer1, 0);
AudioConnection          patchCord3(mixer1, 0, i2s1, 0);
AudioConnection          patchCord4(mixer2, 0, i2s1, 1);
AudioConnection          patchCord5(mixer1, 0, scope1, 0);
AudioConnection          patchCord6(mixer2, 0, scope2, 0);
AudioControlSGTL5000     sgtl5000_1; //xy=1238,726
// GUItool: end automatically generated code

#define NO_OF_BUFFERS 8
int pbCount = 0;
int cbCount = 1;
int16_t prevBufferLt[NO_OF_BUFFERS][AUDIO_BLOCK_SAMPLES * NO_OF_BLOCKS];
int16_t prevBufferRt[NO_OF_BUFFERS][AUDIO_BLOCK_SAMPLES * NO_OF_BLOCKS];

void initPrevBuffer(void) {
  memset(&prevBufferLt[0][0], 0, NO_OF_BUFFERS*AUDIO_BLOCK_SAMPLES*NO_OF_BLOCKS*2);  
  memset(&prevBufferRt[0][0], 0, NO_OF_BUFFERS*AUDIO_BLOCK_SAMPLES*NO_OF_BLOCKS*2); 
  pbCount = 0; 
  cbCount = 1;
}

void plotDataPoints(void)
{
  int x;
  int y = 0;

  tft.fillScreen(ILI9341_BLACK);
  for(x=0;x<256; x++) { 
    y = scope1.buffer[x] >> 8;;
    tft.drawPixel(x+20, y+64, ILI9341_GREEN);
    y = scope2.buffer[x] >> 8;;
    tft.drawPixel(x+20, y+192, ILI9341_GREEN);
  }
}

void plotXYDataPoints(void)
{
  int i;
  int x = 0;
  int y = 0;
  int newx, newy;

  tft.fillScreen(ILI9341_BLACK);
  for(i=0;i<256; i++) { 
    x = (scope1.buffer[i] >> 8);
    //tft.drawPixel(x+20, y+64, ILI9341_GREEN);
    y = (scope2.buffer[i] >> 8);
    newx = x*29/41 - y*29/41;
    newy = y*29/41 + x*29/41;
    tft.drawPixel(newx+128, newy+128, ILI9341_GREEN);
  }
}

void plotGoniometer(void)
{
  int i, l;
  int x = 0;
  int y = 0;
  int newx, newy;
  int oldx, oldy;

  //if (scope1.available())
  //  scope1.update();
  //if (scope2.available())
  //  scope2.update();
  //tft.fillRect(0, 0, 256, 240, ILI9341_BLACK);
  //c1 = scope1.getCount();
  //c2 = scope2.getCount();
  for (i = 0; i < 256; i++) {
    x = (scope1.buffer[i] >> 8);
    y = (scope2.buffer[i] >> 8);
    /* performs an approximated 45 degree pixel shift */
    /* newX = cos(angle)*x - sin(angle)*y */
    /* newY = sin(angle)*x + cos(angle)*y */
    newx = x * 29 / 41 - y * 29 / 41;
    newy = y * 29 / 41 + x * 29 / 41;
    //x = (scope1.buffer[i] >> 8);
    //y = (scope2.buffer][i] >> 8);
    oldx = prevBufferLt[cbCount][i];
    oldy = prevBufferRt[cbCount][i];
    tft.drawPixel(oldx + 128, oldy + 128, ILI9341_BLACK);
    
    tft.drawPixel(newx + 128, newy + 128, ILI9341_GREEN);
    prevBufferLt[pbCount][i] = newx;
    prevBufferRt[pbCount][i] = newy;
   
    if (i % 8 == 0) {
      tft.drawPixel(i, 128, ILI9341_WHITE);
    }
    if (i == 128) {
      for (l = 0; l < 240; l += 8) {
        tft.drawPixel(i, l, ILI9341_WHITE);
      }
    }
  }
  pbCount++;
  if (pbCount >= NO_OF_BUFFERS) pbCount = 0;
  cbCount++;
  if (cbCount >= NO_OF_BUFFERS) cbCount = 0;

}

long previousMillis;
long currentMillis;
int mode = 0;

void setup() {
  Serial.begin(9600);
  tft.begin();
  tft.setRotation(3);
  tft.fillScreen(ILI9341_BLACK);
  tft.setFont(Arial_24);
  AudioMemory(20);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.32);
    initPrevBuffer();
  
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  mixer1.gain(0, 0.75);
  mixer1.gain(1, 0.0);
  mixer1.gain(2, 0.0);
  mixer1.gain(3, 0.0);
  mixer2.gain(0, 0.75);
  mixer2.gain(1, 0.0);
  mixer2.gain(2, 0.0);
  mixer2.gain(3, 0.0);
  waveform1.begin(WAVEFORM_SINE);
  waveform1.amplitude(0.75);
  waveform1.frequency(440);
  waveform1.phase(0.0);
  waveform2.begin(WAVEFORM_SINE);
  waveform2.amplitude(0.75);
  waveform2.frequency(440);
  waveform2.phase(0.5);
  previousMillis = millis();
}

int waveform_type = WAVEFORM_SINE;
int mixer1_setting = 0;
int mixer2_setting = 0;
elapsedMillis timeout = 0;
bool mixer2_envelope = false;

void loop() {
  button0.update();
  button1.update();
  button2.update();
#if 0
  currentMillis = millis();
  if (currentMillis - previousMillis > 2000)
  {
    previousMillis = currentMillis;
    switch (mode) {
      case 0:
        waveform2.frequency(440);
        waveform2.phase(0.0);
        mode++;
      break;
      case 1:
        waveform2.frequency(330);
        mode++;
      break;
      case 2:
        waveform2.frequency(220);
        mode++;
      break;
      case 3:
        waveform2.frequency(440);
        waveform2.phase(0.5);
        mode++;
      break;
      case 4:
        waveform2.frequency(880);
        mode++;
      break;
      default:
        mode =0;
      break;
    }
    
  }
#endif
  // Left changes the type of control waveform
  if (button0.fallingEdge()) {
    Serial.print("Control waveform: ");
    if (waveform_type == WAVEFORM_SAWTOOTH) {
      waveform_type = WAVEFORM_SINE;
      Serial.println("Sine");
    } else if (waveform_type == WAVEFORM_SINE) {
      waveform_type = WAVEFORM_SQUARE;
      Serial.println("Square");
    } else if (waveform_type == WAVEFORM_SQUARE) {
      waveform_type = WAVEFORM_TRIANGLE;
      Serial.println("Triangle");
    } else if (waveform_type == WAVEFORM_TRIANGLE) {
      waveform_type = WAVEFORM_PULSE;
      Serial.println("Pulse");
    } else if (waveform_type == WAVEFORM_PULSE) {
      waveform_type = WAVEFORM_SAWTOOTH;
      Serial.println("Sawtooth");
    }
    waveform1.begin(waveform_type);
  }

  // use the knobs to adjust parameters
  float knob1 = (float)analogRead(A1) / 1280.0;
  float knob2 = (float)analogRead(A2) / 1023.0;
  float knob3 = (float)analogRead(A3) / 1023.0;
  sgtl5000_1.volume(knob1);
  Serial.println(knob1);
  waveform1.frequency(360 * knob2 + 0.25);
  waveform2.frequency(360 * knob3 + 0.25);

//  scope1.update();
//  scope2.update();
//  plotXYDataPoints();
  plotGoniometer();
}
