/* This code uses a means crossing approach to approximate instantaneous frequency of a breathing rate signal
Calculations are performed on integers and then appropriately scaled
Raw BR data is transmitted of bluetooth */

#include <SoftwareSerial.h> 
SoftwareSerial BlueTooth(10, 11); // RX | TX 


const double samplingFrequency = 60; // Sampling freq in Hz
unsigned int sampling_period_ms; // Sampling period in microseconds
unsigned long milliseconds;
float breathing_rate_bpm;

// Mock data from Taoran
// I downsampled the original data sent by 16 times to 60Hz so that it can fit on a buffer on the Arduino
// Also the additionally high sampling rate didn't make it more accurate
const int mock_data_length = 130;
int mock_data_index = 0;
int mock_data[mock_data_length] = {328,329,322,325,323,323,338,341,334,338,337,329,333,336,337,356,357,358,362,361,363,363,367,366,379,384,386,385,386,388,389,387,387,387,395,397,393,396,394,392,391,389,385,397,396,395,391,386,379,374,371,365,383,381,374,372,367,360,354,350,355,369,364,341,346,338,336,334,332,350,351,345,348,345,345,347,349,360,372,375,372,373,375,373,379,377,381,387,389,387,389,387,385,385,385,386,395,398,396,400,400,400,398,399,400,405,408,397,390,389,386,385,381,382,396,390,374,377,368,360,354,349,366,364,358,347,340,331,331,330};

// This boolean holds the current state of the BR graph (is it below or above the mean)
bool below = false;
int reading;

// These variables are used for the calculation of a very long moving average. This is needed to get a sensible threshold for detecting mean crossing.
const int ma_len = 150;
int ma_win[ma_len];
int ma_i = 0;
unsigned long tot = 0;
int avg;

// These variables are used for the calculation of a short moving average
// This is just for smoothing purposes so that there aren't any false intersections cased by high frequency oscillations
const int smoothing_ma_len = 10;
int smoothing_ma_win[smoothing_ma_len];
int smoothing_ma_i = 0;
unsigned long smoothing_tot = 0;
int smoothing_avg;




void setup() {

  // Initialise moving average arrays to all 0
  // Note - the first few readings are inaccurate as it takes time for the long average to stabilise
  for (int i = 0; i < ma_len; i++) {
    ma_win[i]=0;
  }
  for (int i = 0; i < smoothing_ma_len; i++) {
    smoothing_ma_win[i]=0;
  }
  
  sampling_period_ms = round(1000 *(1.0/samplingFrequency));
  BlueTooth.begin(9600); 
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Ready");
  
}


void loop() {
  
  reading = mock_data[mock_data_index];
  mock_data_index++;
  // Roll over mock data index (only needed because mock data instead of sampling from piezo)
  if (mock_data_index >= mock_data_length){
    mock_data_index = 0;
  }

  // If sampling from an actual sensor, replace the code above with reading = getSample

  // Update the long moving average based on last reading
  tot -= ma_win[ma_i];
  ma_win[ma_i] = reading;
  tot += ma_win[ma_i];
  ma_i++;
  // Roll over moving average index
  if (ma_i >= ma_len) {
    ma_i = 0;
  }
  avg = tot/ma_len;

  // Update smoothed data based on last reading
  smoothing_tot -= smoothing_ma_win[smoothing_ma_i];
  smoothing_ma_win[smoothing_ma_i] = reading;
  smoothing_tot += smoothing_ma_win[smoothing_ma_i];
  smoothing_ma_i++;
  // Roll over moving average index
  if (smoothing_ma_i >= smoothing_ma_len) {
    smoothing_ma_i = 0;
  }
  smoothing_avg = smoothing_tot/smoothing_ma_len;

  // Check whether smoothed datapoint is crossing the mean
  if (below) {
    if (smoothing_avg > avg) {
      // Has crossed on an upwards curve
      // Update state
      below = false;
      // Print instantaneous frequency
      breathing_rate_bpm = (1.0/(milliseconds/1000.0))*60.0;
      Serial.println(breathing_rate_bpm);
      // Transmit the time period between beats via bluetooth
      BlueTooth.print(milliseconds);
      BlueTooth.print(",");
      // Reset time
      milliseconds = 0;
    }
  } 
  else {
    if (smoothing_avg <= avg) {
      // Has crossed on a downwards curve
      // Update state
      below = true;
    }
  }

  // Wait required sampling period to take next sample
  milliseconds += sampling_period_ms;
  delay(sampling_period_ms);
  
}
