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

int mock_data_length = 70;
int mock_data_index = 0;
// Mock data from Xueyan (downsampled to 3Hz) = 20
float mock_data[70] = {1.78,2.36,1.67,1.98,2.24,1.68,1.8,2.31,1.67,1.84,2.29,1.66,1.92,2.19,1.7,1.87,2.92,1.87,2.07,3.16,1.99,2.0,3.48,2.03,1.94,3.21,1.94,1.91,3.01,2.02,2.03,3.1,1.92,1.83,2.19,1.87,1.79,1.87,2.33,1.61,1.92,2.21,1.76,1.85,3.5,2.11,1.92,2.75,1.79,1.82,1.81,2.18,1.58,1.96,2.06,1.68,1.98,2.59,1.85,1.92,3.52,1.99,2.31,3.21,1.79,3.57,0.63,1.16,1.78,1.72};

void loop() {
  microseconds = micros();
  
  // This for loop fills up the arrays vReal and vImag with sampled data
  // Only after the arrays are filled is the fft calculated
  for(int i=0; i<samples; i++){

    // This code is just so that the mock data is repeatedly sampled (the index should wrap around to the beginning of the array)
    // The section is not needed if real data is used
    if (mock_data_index >= mock_data_length) {
      mock_data_index = 0;
    }
    mock_data_index++;

    
    // Take a "sample" from the mock data - this line of code could be replaced with taking a sample from the piezo sensor
    vReal[i] = mock_data[mock_data_index];
    vImag[i] = 0;
    // This line of code is just to "wait" until sampling_period_us has elapsed until taking the next sample
    while(micros() - microseconds < sampling_period_us){}
    microseconds += sampling_period_us;
  }

  // FFT calculation
  FFT.Windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);  // Weight the sample array with a hamming window
  FFT.Compute(vReal, vImag, samples, FFT_FORWARD); // Compute FFT of the collected samples
  FFT.ComplexToMagnitude(vReal, vImag, samples); // Convert complex values to magnitudes

  // Print the FFT of that window to the console
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
  Serial.print(x*60, 3); //Print out what frequency is the most dominant (in BPM)
  Serial.print("BPM");
  
  // Wait any amount of time until doing the next window
  delay(1); 
}
