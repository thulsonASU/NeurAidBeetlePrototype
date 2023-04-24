//DEFINE DIGITIAL PINS
int pushBtn1 = 10;
int pushBtn2 = 11;
int motorPin = 9;

// Define the pin connections for ANALOG in
const int micPin1 = A0; // connect the amplifier output to analog pin 0
const int micPin2 = A1; // connect the amplifier output to analog pin 2

// DEFINE MICROPHONE VARS
const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
const int filterValue = 100; // Filter value for high-pass filter
unsigned int sample;
unsigned int sample2;

// BUTTON VARS
int btn1State = HIGH;         // current state of the button
int lastBtn1State = HIGH;     // previous state of the button
int btn2State = HIGH;         // current state of the button
int lastBtn2State = HIGH;     // previous state of the button
unsigned long lastDebounceTime = 0;  // last time the button was debounced
unsigned long debounceDelay = 10;    // debounce time in milliseconds

// MOTOR VARS
float intensityMultiplier = 1;
int motordebugFlag = 0;

// DEFINE FUNCTIONS
float rmse(float signal1, float signal2) 
{ 
  // Root Mean Square Error
  float diff = signal1 - signal2; // Calculate difference between signals
  return sqrt(diff * diff); // Return RMSE of difference signal
}

void setup() {
   pinMode(pushBtn1, INPUT_PULLUP);
   pinMode(pushBtn2, INPUT_PULLUP);
   pinMode(motorPin, OUTPUT); // initialize the motor pin as an output
   Serial.begin(9600);
}

void loop() 
{  
  // PUSH BUTTON CHECKS
  
  // BUTTON 1
  int btn1Reading = digitalRead(pushBtn1); // read the state of the button
  if (btn1Reading != lastBtn1State) {    // if the button state has changed
    lastDebounceTime = millis();       // record the time of the state change
  }
  if ((millis() - lastDebounceTime) > debounceDelay) { // if the debounce delay has passed
    if (btn1Reading != btn1State) {     // if the button state is different from the current state
      btn1State = btn1Reading;          // update the current state
      if (btn1State == LOW) {       // if the button is pressed
        Serial.print("Increase Intensity");
        if(intensityMultiplier <= 2)
        {
          intensityMultiplier = intensityMultiplier + 0.1;
        }
      }
    }
  }
  lastBtn1State = btn1Reading;          // update the previous state

  // BUTTON 2
  int btn2Reading = digitalRead(pushBtn2); // read the state of the button
  if (btn2Reading != lastBtn2State) 
  {    // if the button state has changed
    lastDebounceTime = millis();       // record the time of the state change
  }
  if ((millis() - lastDebounceTime) > debounceDelay) { // if the debounce delay has passed
    if (btn2Reading != btn2State) {     // if the button state is different from the current state
      btn2State = btn2Reading;          // update the current state
      if (btn2State == LOW) {       // if the button is pressed
        Serial.print("Decrease Intensity");
        if(intensityMultiplier >= 0.5)
        {
          intensityMultiplier = intensityMultiplier - 0.1;
        }
      }
    }
  }
  lastBtn2State = btn2Reading;          // update the previous state
  
  // MICROPHONE 1 -----------------------------------------------------------------------------------------------------------
  // Read the analog value from the MAX4466 amplifier output
  unsigned long startMillis = millis();  // Start of sample window
  unsigned int peakToPeak = 0;   // peak-to-peak level

  unsigned int signalMax = 0;
  unsigned int signalMin = 1024;

  // collect data for 50 mS
  while (millis() - startMillis < sampleWindow)
  {
    sample = analogRead(micPin1);
    if (sample < 1024)  // toss out spurious readings
    {
      if (sample > signalMax)
      {
        signalMax = sample;  // save just the max levels
      }
      else if (sample < signalMin)
      {
        signalMin = sample;  // save just the min levels
      }
    }
  }
  peakToPeak = signalMax - signalMin;  // max - min = peak-peak amplitude
  double max4466Voltage = (peakToPeak * 5.0) / 1024;  // convert to volts

  // MICROPHONE 2
  // Read the analog value from the MAX4466 amplifier output
  unsigned long startMillis2 = millis();  // Start of sample window
  unsigned int peakToPeak2 = 0;   // peak-to-peak level

  unsigned int signalMax2 = 0;
  unsigned int signalMin2 = 1024;

  // collect data for 50 mS
  while (millis() - startMillis2 < sampleWindow)
  {
    sample2 = analogRead(micPin2);
    if (sample2 < 1024)  // toss out spurious readings
    {
      if (sample2 > signalMax2)
      {
        signalMax2 = sample2;  // save just the max levels
      }
      else if (sample2 < signalMin2)
      {
        signalMin2 = sample2;  // save just the min levels
      }
    }
  }
  peakToPeak2 = signalMax2 - signalMin2;  // max - min = peak-peak amplitude
  double max9814Voltage = (peakToPeak2 * 5.0) / 1024;  // convert to volts

  // FILTERING
  // USING HIGH PASS FILTER FOR max9814Voltage
  max4466Voltage = max4466Voltage - (max4466Voltage / filterValue); // Apply high-pass filter to remove DC offset
  max4466Voltage = abs(max4466Voltage); // Rectify the AC signal to a positive DC signal
  //float userVoltage = max4466Voltage; // float double and pass voltage to new variable definied as userVoltage
  
  // USING HIGH PASS FILTER FOR max9814Voltage
  max9814Voltage = max9814Voltage - (max9814Voltage / filterValue); // Apply high-pass filter to remove DC offset
  max9814Voltage = 1 - max9814Voltage; // Reduce max9814Voltage signal by 1 to roughly match max4466Voltage
  max9814Voltage = abs(max9814Voltage); // Rectify the AC signal to a positive DC signal
  //float ambientVoltage = max9814Voltage; // float double and pass voltage to new variable defined as ambientVoltage

  // Take the difference and find the error between the two signals
  float error = rmse(max4466Voltage,max9814Voltage);
  
  // Print the reading to the serial monitor Bought Board
  Serial.print("User Signal: ");
  Serial.print(max4466Voltage);
  // Seperator
  Serial.print(" ; ");
  // Print the reading to the serial monitor Custom Board
  Serial.print("Ambient Signal: ");
  Serial.print(max9814Voltage);
  // Seperator
  Serial.print(" ; ");
  Serial.print("Error: ");
  Serial.println(error);

  // While debugging and testing I put the thresholds here for easy access
 float errorThreshold = 0.5; // Threshold for error
 float userNoiseGate = 0.3;
 float ambientNoiseGate = 0.2;
 // float quietThreshold = 0.5; // Threshold for quietness
 // float loudThreshold = 1; // Threshold for loudness

  // ENTER FEEDBACK LOGIC
  if (error > errorThreshold && userNoiseGate < max4466Voltage) // check if error is above threshold and feedback is needed
  {
    // Add counter variable due to microphone spike to check if user is speaking at a sustained level
    if (max4466Voltage > max9814Voltage && ambientNoiseGate < max9814Voltage) // check if user audio is above threshold
    {
      Serial.println("User is too loud!"); // Print message to serial monitor  
    } 
    else if (max4466Voltage < max9814Voltage && ambientNoiseGate < max9814Voltage) // Check if user audio is below threshold
    {
        Serial.println("User is too quiet!"); // Print message to serial monitor
    }
  }
 
  // MINI DC HAPTIC MOTOR -----------------------------------------------------------------------------------------------------------
  if(motordebugFlag == 1)
  {    
    float motorIntensity = 100*intensityMultiplier;
    Serial.println(motorIntensity);
    analogWrite(motorPin,motorIntensity);
  }

  delay(10);
}
