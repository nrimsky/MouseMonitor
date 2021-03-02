const double samplingFrequency = 20; // Sampling freq in Hz
unsigned int sampling_period_ms; // Sampling period in microseconds
unsigned long milliseconds;

const int mock_data_length = 100;
int mock_data_index = 0;
float mock_data[mock_data_length] = {1.78,2.06,3.13,3.27,2.11,2.36,1.86,1.97,2.06,1.72,1.67,1.81,2.05,1.94,2.15,1.98,2.16,3.15,3.3,2.02,2.24,1.84,2.08,1.96,1.69,1.68,1.87,1.98,1.97,1.99,1.8,2.18,3.26,2.98,2.03,2.31,1.8,2.04,1.91,1.67,1.67,1.86,1.98,1.9,1.92,1.84,1.86,2.94,3.44,2.23,2.29,2.05,1.7,1.97,1.74,1.66,1.76,2.0,1.94,1.83,1.92,1.83,2.16,3.19,3.27,2.19,2.21,1.85,1.91,1.95,1.7,1.65,1.75,1.95,1.93,1.87,1.96,1.81,2.36,3.36,2.92,2.09,2.21,1.71,2.06,1.87,1.71,1.7,2.02,2.02,2.07,1.93,1.87,2.09,3.19,3.16,2.05,2.25,1.81,2.03};

// This boolean holds the current state of the BR graph (is it below or above the mean)
bool below = false;
float reading;

// These variables are used for the calculation of a very long moving average. This is needed to get a sensible threshold for detecting mean crossing.
const int ma_len = 150;
float ma_win[ma_len];
int ma_i = 0;
float tot = 0;
float avg;

// These variables are used for the calculation of a short moving average
// This is just for smoothing purposes so that there aren't any false intersections cased by high frequency oscillations
const int smoothing_ma_len = 5;
float smoothing_ma_win[smoothing_ma_len];
int smoothing_ma_i = 0;
float smoothing_tot = 0;
float smoothing_avg;




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
      Serial.println((1.0/(milliseconds/1000.0))*60.0);
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
