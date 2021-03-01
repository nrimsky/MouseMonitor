const double samplingFrequency = 20; // Sampling freq in Hz
unsigned int sampling_period_ms; // Sampling period in microseconds
unsigned long milliseconds;

int mock_data_length = 100;
int mock_data_index = 0;
float mock_data[100] = {1.78,2.06,3.13,3.27,2.11,2.36,1.86,1.97,2.06,1.72,1.67,1.81,2.05,1.94,2.15,1.98,2.16,3.15,3.3,2.02,2.24,1.84,2.08,1.96,1.69,1.68,1.87,1.98,1.97,1.99,1.8,2.18,3.26,2.98,2.03,2.31,1.8,2.04,1.91,1.67,1.67,1.86,1.98,1.9,1.92,1.84,1.86,2.94,3.44,2.23,2.29,2.05,1.7,1.97,1.74,1.66,1.76,2.0,1.94,1.83,1.92,1.83,2.16,3.19,3.27,2.19,2.21,1.85,1.91,1.95,1.7,1.65,1.75,1.95,1.93,1.87,1.96,1.81,2.36,3.36,2.92,2.09,2.21,1.71,2.06,1.87,1.71,1.7,2.02,2.02,2.07,1.93,1.87,2.09,3.19,3.16,2.05,2.25,1.81,2.03};


bool below = false;
int num_crossings = 0;
float freq_bpm;
float reading;

int ma_len = 150;
// Moving average is needed to get a suitable threshold
float ma_win[150];
int ma_i = 0;
float tot = 0;
float avg;





void setup() {

  for (int i = 0; i < ma_len; i++) {
    ma_win[i]=0;
  }
  
  sampling_period_ms = round(1000 *(1.0/samplingFrequency));
  Serial.begin(9600);
  while(!Serial);
  Serial.println("Ready");
  
}


void loop() {

  if (num_crossings == 20) {
    // 20 crossings is 10 wavelengths
    freq_bpm = (10.0/(milliseconds/1000.0))*60.0;
    Serial.print(freq_bpm,3);
    Serial.println();
    // Restart counting crossings once num_crossings reaches 20
    milliseconds = 0;
    num_crossings = 0;
  }

  reading = mock_data[mock_data_index];
  mock_data_index++;
  // Roll over mock data index (only needed because mock data instead of sampling from piezo)
  if (mock_data_index >= mock_data_length){
    mock_data_index = 0;
  }

  
  tot -= ma_win[ma_i];
  ma_win[ma_i] = reading;
  tot += ma_win[ma_i];
  ma_i++;
  // Roll over moving average index
  if (ma_i >= ma_len) {
    ma_i = 0;
  }

  avg = tot/ma_len;

  // Check whether reading is crossing threshold
  if (below) {
    if (reading > avg*1.4) {
      below = false;
      num_crossings++;
    }
  } else {
    if (reading < avg*1.4) {
      below = true;
      num_crossings++;
    }
  }

  milliseconds += sampling_period_ms;
  delay(sampling_period_ms);
}
