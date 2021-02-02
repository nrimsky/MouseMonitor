#include "arduinoFFT.h"

arduinoFFT FFT = arduinoFFT(); /* Create FFT object */

#define CHANNEL A0
const uint16_t samples = 128; // This value MUST ALWAYS be a power of 2 (comment from FFT library)
const double samplingFrequency = 4; // Hz, must be less than 10000 due to ADC


unsigned int sampling_period_us;
unsigned long microseconds;
/*
These are the input and output vectors
Input vectors receive computed results from FFT
*/
double vReal[samples];
double vImag[samples];

void setup() {
  sampling_period_us = round(1000000*(1.0/samplingFrequency));
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Ready");
}

void loop() {
  microseconds = micros();
  for(int i=0; i<samples; i++){
    vReal[i] = analogRead(CHANNEL);
    vImag[i] = 0;
    while(micros() - microseconds < sampling_period_us){}
    microseconds += sampling_period_us;
  }

  /*Calculate FFT*/
  FFT.Windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);  /* Weigh data */
  FFT.Compute(vReal, vImag, samples, FFT_FORWARD); /* Compute FFT */
  FFT.ComplexToMagnitude(vReal, vImag, samples); /* Compute magnitudes */

  Serial.println("FFT");
  for (int i=0; i<samples; i++) {
    double freq = (i * 1.0 * samplingFrequency) / samples;
    Serial.print(freq);
    Serial.print("\t");
    Serial.print(vReal[i]);
    Serial.println();
  }
  Serial.println("Dominant frequency");
  double x = FFT.MajorPeak(vReal, samples, samplingFrequency);
  Serial.print(x*60, 3); //Print out what frequency is the most dominant.
  Serial.print("BPM");
  delay(1); 
}
