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
AudioSynthWaveform       waveform1;      //xy=95,114
AudioSynthNoisePink      pink1;          //xy=95,307
AudioSynthWaveformSine   sine1;          //xy=100,253
AudioSynthWaveformSineModulated sine_fm1;       //xy=186,184
AudioAnalyzeOscilloscope scope1;     
AudioAnalyzeOscilloscope scope2;     
AudioMixer4              mixer1;         //xy=379,187
AudioEffectEnvelope      envelope1;      //xy=533,247
AudioMixer4              mixer2;         //xy=694,207
AudioOutputI2S           i2s1;           //xy=880,211
AudioConnection          patchCord1(waveform1, 0, mixer1, 0);
AudioConnection          patchCord2(waveform1, sine_fm1);
AudioConnection          patchCord3(pink1, 0, mixer1, 3);
AudioConnection          patchCord4(sine1, 0, mixer1, 2);
AudioConnection          patchCord5(sine_fm1, 0, mixer1, 1);
AudioConnection          patchCord6(mixer1, 0, mixer2, 0);
AudioConnection          patchCord7(mixer1, envelope1);
AudioConnection          patchCord8(envelope1, 0, mixer2, 1);
AudioConnection          patchCord9(mixer2, 0, i2s1, 1);
AudioConnection          patchCord10(mixer2, 0, i2s1, 0);
AudioConnection          patchCord11(mixer2, 0, scope1, 0);
AudioConnection          patchCord12(mixer2, 0, scope2, 0);
AudioControlSGTL5000     sgtl5000_1;     //xy=507,322
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
    y = scope1.buffer[x] >> 9;;
    tft.drawPixel(x+20, y+64, ILI9341_GREEN);
    y = scope2.buffer[x] >> 9;;
    tft.drawPixel(x+20, y+192, ILI9341_GREEN);
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

void plot2ChanOsci(void)
{
  int x;
  int lt,rt;
  int oldlt, oldrt;

  if (scope1.available())
    scope1.update();
  if (scope2.available())
    scope2.update();
  //tft.fillScreen(ILI9341_BLACK);
  for (x = 0; x < 256; x++) {
    if (x % 8 == 0) {
      tft.drawPixel(x + 20, 64, ILI9341_WHITE);
      tft.drawPixel(x + 20, 192, ILI9341_WHITE);
    }
    oldlt = prevBufferLt[cbCount][x];
    tft.drawPixel(x + 20, oldlt + 64, ILI9341_BLACK);
    oldrt = prevBufferRt[cbCount][x];
    tft.drawPixel(x + 20, oldrt + 192, ILI9341_BLACK);

    lt = scope1.buffer[x] >> 8;;
    tft.drawPixel(x + 20, lt + 64, ILI9341_GREEN);
    rt = scope2.buffer[x] >> 8;;
    tft.drawPixel(x + 20, rt + 192, ILI9341_GREEN);
    prevBufferLt[pbCount][x] = lt;
    prevBufferRt[pbCount][x] = rt;
  }
  pbCount++;
  if (pbCount >= NO_OF_BUFFERS) pbCount = 0;
  cbCount++;
  if (cbCount >= NO_OF_BUFFERS) cbCount = 0;

}

void plotGoniometer(void)
{
  int i, l;
  int x = 0;
  int y = 0;
  int newx, newy;
  int oldx, oldy;

  if (scope1.available())
    scope1.update();
  if (scope2.available())
    scope2.update();
  //tft.fillRect(0, 0, 256, 240, ILI9341_BLACK);
  //c1 = scope1.getCount();
  //c2 = scope2.getCount();
  for (i = 0; i < 256; i++) {
    x = (scope1.buffer[i] >> 8);
    y = (scope2.buffer[i] >> 8);
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
  waveform1.begin(WAVEFORM_SAWTOOTH);
  waveform1.amplitude(0.75);
  waveform1.frequency(50);
  waveform1.pulseWidth(0.15);
  sine_fm1.frequency(440);
  sine_fm1.amplitude(0.75);
  sine1.frequency(200);
  sine1.amplitude(0.75);
  pink1.amplitude(0.75);
  envelope1.attack(10);
  envelope1.hold(10);
  envelope1.decay(25);
  envelope1.sustain(0.4);
  envelope1.release(70);
}

int waveform_type = WAVEFORM_SAWTOOTH;
int mixer1_setting = 0;
int mixer2_setting = 0;
elapsedMillis timeout = 0;
bool mixer2_envelope = false;

void loop() {
  button0.update();
  button1.update();
  button2.update();

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

  // middle button switch which source we hear from mixer1
  if (button1.fallingEdge()) {
    if (mixer1_setting == 0) {
      mixer1.gain(0, 0.75);
      mixer1.gain(1, 0.0);
      mixer1.gain(2, 0.0);
      mixer1.gain(3, 0.0);
      Serial.println("Mixer1: Control oscillator");
      mixer1_setting = 1;
    } else if (mixer1_setting == 1) {
      mixer1.gain(0, 0.0);
      mixer1.gain(1, 0.75);
      mixer1.gain(2, 0.0);
      mixer1.gain(3, 0.0);
      Serial.println("Mixer1: Frequency Modulated Oscillator");
      mixer1_setting = 2;
    } else if (mixer1_setting == 2) {
      mixer1.gain(0, 0.0);
      mixer1.gain(1, 0.0);
      mixer1.gain(2, 0.75);
      mixer1.gain(3, 0.0);
      Serial.println("Mixer1: Regular Sine Wave Oscillator");
      mixer1_setting = 3;
    } else if (mixer1_setting == 3) {
      mixer1.gain(0, 0.0);
      mixer1.gain(1, 0.0);
      mixer1.gain(2, 0.0);
      mixer1.gain(3, 0.75);
      Serial.println("Mixer1: Pink Noise");
      mixer1_setting = 0;
    }
  }

  // Right button activates the envelope
  if (button2.fallingEdge()) {
    mixer2.gain(0, 0.0);
    mixer2.gain(1, 1.0);
    mixer2_envelope = true;
    timeout = 0;
    envelope1.noteOn();
  }
  if (button2.risingEdge()) {
    envelope1.noteOff();
    timeout = 0;
  }

  // after 4 seconds of inactivity, go back to
  // steady listening intead of the envelope
  if (mixer2_envelope == true && timeout > 4000) {
    mixer2.gain(0, 0.75);
    mixer2.gain(1, 0.0);
    mixer2_envelope = false;
  }

  // use the knobs to adjust parameters
  float knob1 = (float)analogRead(A1) / 1280.0;
  float knob2 = (float)analogRead(A2) / 1023.0;
  float knob3 = (float)analogRead(A3) / 1023.0;
  sgtl5000_1.volume(knob1);
  Serial.println(knob1);
  waveform1.frequency(360 * knob2 + 0.25);
  sine_fm1.frequency(knob3 * 1500 + 50);
  sine1.frequency(knob3 * 1500 + 50);

  //scope1.update();
  //scope2.update();
  //plotXYDataPoints();

  plot2ChanOsci();
  
}
