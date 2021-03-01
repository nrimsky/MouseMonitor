#include "arduinoFFT.h"

arduinoFFT FFT = arduinoFFT(); /* Create FFT object */

#define CHANNEL A0
const uint16_t samples = 128; // This value MUST ALWAYS be a power of 2 (comment from FFT library)
const double samplingFrequency = 3; // Hz, must be less than 10000 due to ADC


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

int mock_data_length = 90;
int mock_data_index = 0;
// Mock data from Xueyan (downsampled to 3Hz)
float mock_data[90] = {0.65,0.52,0.61,0.59,0.53,0.74,0.63,3.6,0.78,0.69,0.64,0.85,0.82,0.68,4.44,0.81,0.69,0.94,0.86,1.73,0.66,1.76,0.52,0.72,0.71,0.58,2.09,0.27,0.2,0.46,0.42,0.0,0.16,3.14,0.12,0.38,0.07,0.07,0.19,0.16,4.11,0.28,0.22,0.22,0.44,0.18,0.24,2.55,0.33,0.41,0.59,0.45,0.09,0.33,4.75,0.26,0.43,0.01,0.15,0.1,0.0,2.82,0.17,1.56,0.12,0.19,0.02,0.17,4.83,0.14,3.42,0.14,0.37,0.22,0.23,0.0,0.23,0.1,0.03,5.0,0.17,0.07,0.1,0.11,0.0,0.19,5.0,0.03,0.26,0.18};

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
