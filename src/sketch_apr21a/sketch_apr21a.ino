//DEFINE DIGITIAL PINS
int pushBtn1 = 10;
int pushBtn2 = 11;
int motorPin = 9;

// Define the pin connections for ANALOG in
const int micPin1 = A0; // connect the amplifier output to analog pin 0
const int micPin2 = A1; // connect the amplifier output to analog pin 2

// DEFINE MICROPHONE VARS
const int sampleWindow = 50; // Sample window width in mS (50 mS = 20Hz)
unsigned int sample;
unsigned int sample2;
double previousUserNoiseFiltered = 0; // No noise to start
double previousMax4466Voltage = 0; // no voltage to start
double previousAmbientNoiseFiltered = 0; // no nouse to start
double previousMax9814Voltage = 0; // no voltage to start
int userVoiceCount = 0; // no data
int highCount = 0;
int normalCount = 0;
int lowCount = 0;
int ambHighCount = 0;
int ambNormalCount = 0;
int ambLowCount = 0;
int i = 0;

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
int motorFlag = 1;

// Wires
// Blue to Yellow Red to Green

// DEFINE FUNCTIONS

/*
  rmse func

  ROOT MEAN SQUARE ERROR
  Computes error between two signals

float rmse(float signal1, float signal2) 
{ 
  // Root Mean Square Error
  float diff = signal1 - signal2; // Calculate difference between signals
  return sqrt(diff * diff); // Return RMSE of difference signal
}
*/

/*
  motorFeedback fnc
  
  Function takes the low, normal, and high counts from the data processing section and determines which one is greater. 
  Once it determines which one is greater it provides a feedback to the user by a long pulse or short pulse.
  No feedback if user is in normal range.
*/

void motorFeedback(int lowCount,int normalCount,int highCount,int ambHighCount,int ambLowCount,int ambNormalCount) 
{
  if(lowCount + normalCount + highCount == ambLowCount + ambNormalCount + ambHighCount)
  {
    // Take the difference and find the error between the two signals
    float highError = abs(ambHighCount - highCount);
    float lowError = abs(ambLowCount - lowCount);
    float totalError = abs(lowCount + normalCount + highCount - ambLowCount + ambNormalCount + ambHighCount);

    Serial.println("Error");
    Serial.print("High Error: ");
    Serial.print(highError);
    Serial.print(" ; ");
    Serial.print("totalError: ");
    Serial.print(totalError);
    Serial.print(" ; ");
    Serial.print("Low Error: ");
    Serial.println(lowError);

    //delay(2000); // debug

    if (3*lowError > totalError) 
    {
      // lowCount is the highest
      Serial.println("User is Speaking Softly!");
      if(motorFlag == 1)
      {    
        // provide pulse feedback
        while (i < 5)
        {
          Serial.print("Running Motor: ");
          Serial.println(i);
          // Would be ideal to change the motor pulse based on frequency, but Arduino is wack so I'm using a for loop and delay
          float motorIntensity = 110*intensityMultiplier;
          analogWrite(motorPin,motorIntensity);
          delay(100);
          analogWrite(motorPin,0);
          delay(100);
          i = i + 1;
        }
      }
      i = 0;
      lowCount = 0;
      normalCount = 0;
      highCount = 0;
    } 
    else if (3*highError > totalError) 
    {
      // highCount is the highest
      // highCount is the highest
      Serial.println("User is Speaking Loud!");
      if(motorFlag == 1)
      {    
        // Provide long feedback
        while (i < 2)
        {
          Serial.print("Running Motor: ");
          Serial.println(i);
          // Would be ideal to change the motor pulse based on frequency, but Arduino is wack so I'm using a for loop and delay
          float motorIntensity = 90*intensityMultiplier;
          analogWrite(motorPin,motorIntensity);
          delay(1000);
          analogWrite(motorPin,0);
          delay(1000);
          i = i + 1;
        }
      }
      i = 0;
      lowCount = 0;
      normalCount = 0;
      highCount = 0;
    } 
    else 
    {
      Serial.println("User is Speaking Normally!");
      lowCount = 0;
      normalCount = 0;
      highCount = 0;
      ambLowCount = 0;
      ambNormalCount = 0;
      ambHighCount = 0;
    }
  }
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
  /*
  BUTTON DEBOUNCE SECTION

  Button 1 and Button 2 use two seperate debounce checks as implementing it in a function proved more trouble than it is worth.
  */
  
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
  
  /*
  COLLECT MICROPHONE DATA OVER 50Hz SAMPLE WINDOW

  sample
  sample2

  Variables above are capturing the raw input from the microphones and directly converting them to voltage.
  */

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

  /*
  FILTERING SECTION
  Low pass filter as derived my Nithish Kumar for use with the MAX4466 (User Mic) and MAX9814 (Ambient Mic)
  
  filter for microphone 1 (user mic). Low pass first order filter 200Hz. sampling frequency 7000Hz 
  filter for microphone 2 (ambient mic). Low pass first order filter 500Hz sampling frequency 30000Hz
  */

  float userNoiseFiltered = 0.83526683*previousUserNoiseFiltered + 0.08236658*max4466Voltage + 0.08236658*previousMax4466Voltage;
  previousUserNoiseFiltered = userNoiseFiltered;
  previousMax4466Voltage = max4466Voltage;

  float ambientNoiseFiltered = 0.90049055*previousAmbientNoiseFiltered + 0.04975473*max9814Voltage + 0.04975473*previousMax9814Voltage;
  previousAmbientNoiseFiltered = ambientNoiseFiltered;
  previousMax9814Voltage = max9814Voltage;

  /*
  PRINTING OUTPUTS
  Outputs print to serial monitor for debugging.
  */

  // Print the reading to the serial monitor Bought Board
  Serial.print("User Signal: ");
  Serial.print(userNoiseFiltered);
  // Seperator
  Serial.print(" ; ");
  // Print the reading to the serial monitor Custom Board
  Serial.print("Ambient Signal: ");
  Serial.println(ambientNoiseFiltered);

  /*
  DATA PROCESSING SECTION
  Process the data from the filtered microphone outputs and determine feedback using noise gates and counters.
  */
  
  //float errorThreshold = 1; // Threshold for error
  float userNoiseGate = 0.45;

  if(userNoiseGate <= userNoiseFiltered)
  {
    // PROCESS USER MIC
    userVoiceCount = userVoiceCount + 1;
    if(userVoiceCount >= 2) // filter out microphone spikes and check for a contoninous noise by adding a counter for the user noise
    {
      if(userNoiseFiltered > 1)
      {
        highCount = highCount + 1; // user is speaking at a high volume
      }
      else if (userNoiseFiltered >= 0.7 && userNoiseFiltered <= 1)
      {
        normalCount = normalCount + 1;
      }
      else if (userNoiseFiltered >= 0.05 && userNoiseFiltered < 0.6)
      {
        lowCount = lowCount + 1;
      }
      Serial.println("User Counts");
      Serial.print("User Low count: ");
      Serial.print(lowCount);
      Serial.print(" ; ");
      Serial.print("User Normal count: ");
      Serial.print(normalCount);
      Serial.print(" ; ");
      Serial.print("User High count: ");
      Serial.println(highCount);
    }
  }
  else if (userNoiseGate > userNoiseFiltered)
  {
    // PROCESS AMBIENT MIC
    if(ambientNoiseFiltered > 1.3)
      {
        ambHighCount = ambHighCount + 1; // user is speaking at a high volume
      }
      else if (ambientNoiseFiltered >= 1.21 && ambientNoiseFiltered <= 1.3)
      {
        ambNormalCount = ambNormalCount + 1;
      }
      else if (ambientNoiseFiltered >= 0.09 && ambientNoiseFiltered < 1.2)
      {
        ambLowCount = ambLowCount + 1;
      }
      Serial.println("Ambient Counts");
      Serial.print("Amb Low count: ");
      Serial.print(ambLowCount);
      Serial.print(" ; ");
      Serial.print("Amb Normal count: ");
      Serial.print(ambNormalCount);
      Serial.print(" ; ");
      Serial.print("Amb High count: ");
      Serial.println(ambHighCount);
    }
  
  // MINI DC HAPTIC MOTOR -----------------------------------------------------------------------------------------------------------
  motorFeedback(lowCount,normalCount,highCount,ambHighCount,ambNormalCount,ambLowCount);
  if (ambLowCount + ambNormalCount + ambHighCount >= 20)
  {
    ambLowCount = 0;
    ambNormalCount = 0;
    ambHighCount = 0;
  }
  if (lowCount + normalCount + highCount > ambLowCount + ambNormalCount + ambHighCount)
  {
    // delay(2000); // debug
    lowCount = 0;
    normalCount = 0;
    highCount = 0;
  }  
  
  /*
  if (lowCount + normalCount + highCount >= 20)
  {
    // delay for serial monitor feedback
    delay(2000);
    lowCount = 0;
    normalCount = 0;
    highCount = 0;
    ambLowCount = 0;
    ambNormalCount = 0;
    ambHighCount = 0;
  }
  */
  
  delay(1);

  // MINI DC HAPTIC MOTOR SENSITIVITY TUNING -----------------------------------------------------------------------------------------------------------
  if(motordebugFlag == 1)
  {    
    float motorIntensity = 100*intensityMultiplier;
    Serial.println(motorIntensity);
    analogWrite(motorPin,motorIntensity);
  }
}
