//Switch statement variables
int state;
int nextState;

// Flow values
const int MIN_FLOW = 10;  //Need value!!

// Calculation values
const int MAX_RESIST_RED;   //10k?
const int MAX_RESIST_GREEN; //10k?
const int MAX_RESIST_BLUE;  //10k?
double percentRed, percentGreen, percentBlue;
double appRed, appGreen, appBlue;

// INPUT PINS
const int LASER_SWITCH = 2;

const int RED_VALUE = 18;
const int GREEN_VALUE = 19;
const int BLUE_VALUE = 20;

const int RED_SENSOR_PIN = 21;
const int GREEN_SENSOR_PIN = 22;
const int BLUE_SENSOR_PIN = 23;

const int MANUAL_APP_SWITCH = 3; 

// OUTPUT PINS
const int FAN_SIGNAL = 1;

const int RED_CURRENT = 6;
const int GREEN_CURRENT = 9;
const int BLUE_CURRENT = 10;

// FLOW SENSORA VALUES
byte redSensorInterrupt = 0; // ?? 
byte greenSensorInterrupt = 0; // ??
byte blueSensorInterrupt = 0; // ??

float calibrationFactor = 4.5;

volatile byte redPulseCount;
volatile byte greenPulseCount;
volatile byte bluePulseCount;

float redFlowRate;
float greenFlowRate;
float blueFlowRate;

unsigned int redFlowMilliLitres;
unsigned long redTotalMilliLitres;
unsigned long redOldTime;

unsigned int greenFlowMilliLitres;
unsigned long greenTotalMilliLitres;
unsigned long greenOldTime;

unsigned int blueFlowMilliLitres;
unsigned long blueTotalMilliLitres;
unsigned long blueOldTime;

void setup() {
   //initializes input pins
   pinMode(LASER_SWITCH, INPUT);
   pinMode(RED_VALUE, INPUT);
   pinMode(GREEN_VALUE, INPUT);
   pinMode(BLUE_VALUE, INPUT);
   pinMode(MANUAL_APP_SWITCH, INPUT);

   //initialize output pins
   pinMode(FAN_SIGNAL, OUTPUT);
   pinMode(RED_CURRENT, OUTPUT);
   pinMode(GREEN_CURRENT, OUTPUT);
   pinMode(BLUE_CURRENT, OUTPUT);

   //setup state machine
   state = 0;
   nextState = 0; 

   //setup flow sensors
   pinMode(RED_SENSOR_PIN, INPUT);
   pinMode(GREEN_SENSOR_PIN, INPUT);
   pinMode(BLUE_SENSOR_PIN, INPUT);

   digitalWrite(RED_SENSOR_PIN, HIGH);
   digitalWrite(GREEN_SENSOR_PIN, HIGH);
   digitalWrite(BLUE_SENSOR_PIN, HIGH);
   
   redPulseCount = 0;
   redFlowRate = 0.0;
   redFlowMilliLitres = 0;
   redTotalMilliLitres = 0;
   redOldTime = 0;

   greenPulseCount = 0;
   greenFlowRate = 0.0;
   greenFlowMilliLitres = 0;
   greenTotalMilliLitres = 0;
   greenOldTime = 0;

   bluePulseCount = 0;
   blueFlowRate = 0.0;
   blueFlowMilliLitres = 0;
   blueTotalMilliLitres = 0;
   blueOldTime = 0;

   attachInterrupt(redSensorInterrupt, redPulseCounter, FALLING);
   attachInterrupt(greenSensorInterrupt, bluePulseCounter, FALLING);
   attachInterrupt(blueSensorInterrupt, greenPulseCounter, FALLING);
   
   //Initialize serial port
   Serial.begin(9600);
}

void loop() 
{
  switch (state)
  {
    case 1: 
      checkFlowRate();
      break;
    case 2:
      calcValues();
      break;
    case 3:
      sendValues();
      break;
    case 4:
      lasersOff();
      break;
  } 
}

//State 1
void checkFlowRate()
{
  //RED SENSOR
  if((millis() - redOldTime) > 1000) // Only process once per second
  {
    //Disable interrupt while calculating flow rate 
    detachInterrupt(redSensorInterrupt);

    redFlowRate = ((1000.0 / (millis() - redOldTime)) * redPulseCount) / calibrationFactor;

    redOldTime = millis();

    // Converts to mL/sec
    redFlowMilliLitres = (redFlowRate / 60) * 1000;

    redTotalMilliLitres += redFlowMilliLitres;

    redPulseCount = 0;

    attachInterrupt(redSensorInterrupt, redPulseCounter, FALLING);
  }

  //GREEN SENSOR
  if((millis() - greenOldTime) > 1000) // Only process once per second
  {
    //Disable interrupt while calculating flow rate 
    detachInterrupt(greenSensorInterrupt);

    greenFlowRate = ((1000.0 / (millis() - greenOldTime)) * greenPulseCount) / calibrationFactor;

    greenOldTime = millis();

    // Converts to mL/sec
    greenFlowMilliLitres = (greenFlowRate / 60) * 1000;

    greenTotalMilliLitres += greenFlowMilliLitres;

    greenPulseCount = 0;

    attachInterrupt(greenSensorInterrupt, greenPulseCounter, FALLING);
  }

  
  //BLUE SENSOR
  if((millis() - blueOldTime) > 1000) // Only process once per second
  {
    //Disable interrupt while calculating flow rate 
    detachInterrupt(blueSensorInterrupt);

    blueFlowRate = ((1000.0 / (millis() - blueOldTime)) * bluePulseCount) / calibrationFactor;

    blueOldTime = millis();

    // Converts to mL/sec
    blueFlowMilliLitres = (blueFlowRate / 60) * 1000;

    blueTotalMilliLitres += blueFlowMilliLitres;

    bluePulseCount = 0;

    attachInterrupt(blueSensorInterrupt, bluePulseCounter, FALLING);
  }

   if((redFlowMilliLitres < MIN_FLOW) || (greenFlowMilliLitres < MIN_FLOW) || (blueFlowMilliLitres < MIN_FLOW))
   {
      // Turn off LASERs
      state = 4;
      nextState = 2;
   }
   else
   {
      state = nextState;
   }
}

//State 2
void calcValues()
{
  if(digitalRead(MANUAL_APP_SWITCH) == 1)
  {
    // MANUAL VALUES
    
    //red
    double r = analogRead(RED_VALUE) / MAX_RESIST_RED;
    percentRed = r * 0.0915;
  
    //green
    double g = analogRead(GREEN_VALUE) / MAX_RESIST_GREEN;
    percentGreen = g * 0.0976;
    
    //blue
    double b = analogRead(BLUE_VALUE) / MAX_RESIST_BLUE;
    percentBlue = b * 0.732;    
  }
  else
  {
    // App Values
    
    //Parse input string from app (hex?)
    //Convert to decimal
    
    //red
    double r = appRed / MAX_RESIST_RED;
    percentRed = r * 0.0915;
  
    //green
    double g = appGreen / MAX_RESIST_GREEN;
    percentGreen = g * 0.0976;
    
    //blue
    double b = appBlue / MAX_RESIST_BLUE;
    percentBlue = b * 0.732; 
  }

  state = 1;
  nextState = 3;
}

//State 3
void sendValues()
{
  analogWrite(RED_CURRENT, percentRed);
  analogWrite(GREEN_CURRENT, percentGreen);
  analogWrite(BLUE_CURRENT, percentBlue);

  state = 1;
  nextState = 2;
}

//State 4
void lasersOff()
{
  //output high signal to LASER power on current controller
  //OR send 0 percentage for all currents

  state = 1;
  nextState = 2;
}

void laserShow()
{
  int showSelection;
  //laser light show combinations
  switch (showSelection)
  {
    case 1: 
      //set laser show 1 colors
      break;
    case 2:
      //set laser show 2 colors
      break;
    case 3:
      //set laser show 3 colors
      break;
    case 4:
      //set laser show 4 colors
      break;
  } 
}

void redPulseCounter()
{
  redPulseCount++;
}

void greenPulseCounter()
{
  greenPulseCount++;
}

void bluePulseCounter()
{
  bluePulseCount++;
}
