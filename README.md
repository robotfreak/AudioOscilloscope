# AudioOscilloscope
A teensy based audio oscilloscope using the teensy audio library and an SPI color TFT. It is based on the Audio library screen oscillospe https://forum.pjrc.com/threads/60470-Teensy-4-0-Audio-Library-screen-oscilloscope

## What is needed?

### Hardware

* A Teensy 3.x or 4.x
* Teensy Audio Shield
* ILI9341 SPI Color TFT 320x240pixel

### Software
* Teensyduino  https://www.pjrc.com/teensy/teensyduino.html
* The Teensy Audio Library (included in Teensyduino)
* ILI9341_t3 Library (included in Teensyduino)

## How it works

This library is an extension for the teensy audio library. It uses the audio sample buffer from the audio codec of the audio shield as source for the oscilloscope. There is no support for external ADC.
Different modes of operation are implemented:
* dual channel oscilloscope mode 
* XY mode for oscillope music https://oscilloscopemusic.com/
* Goniometer (aka Jellyfish) mode (256x256 pixel)
* FFT Fast fourier transformation mode (1024 or 256 FFT points using the builtin audio library function) 
* Peak and RMS Peak Metering mode (using the builtin audio library function)
* Mixed Goniometer / RMS mode

## TODO

At the moment there is no processing of the audio data. Only the raw sample data of the two audio channels are used.
* implement trigger level
* implement x zoom
* implement timebase
