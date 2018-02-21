//Switch statement variables
int state;
int nextState;

// Flow values
const int MIN_FLOW = 10;  //Need value!!

// Calculation values
const int MAX_RESIST_RED;
const int MAX_RESIST_GREEN;
const int MAX_RESIST_BLUE;
double percentRed, percentGreen, percentBlue;
double appRed, appGreen, appBlue;

// INPUT PINS
const int LASER_SWITCH = 2;

const int RED_VALUE = 18;
const int GREEN_VALUE = 19;
const int BLUE_VALUE = 20;

const int RED_FLOW = 21;
const int GREEN_FLOW = 22;
const int BLUE_FLOW = 23;

const int MANUAL_APP_SWITCH = 3; 

// OUTPUT PINS
const int FAN_SIGNAL = 1;

const int RED_CURRENT = 6;
const int GREEN_CURRENT = 9;
const int BLUE_CURRENT = 10;
 

void setup() {
   //initializes input pins
   pinMode(LASER_SWITCH, INPUT);
   pinMode(RED_VALUE, INPUT);
   pinMode(GREEN_VALUE, INPUT);
   pinMode(BLUE_VALUE, INPUT);
   pinMode(RED_FLOW, INPUT);
   pinMode(GREEN_FLOW, INPUT);
   pinMode(BLUE_FLOW, INPUT);
   pinMode(MANUAL_APP_SWITCH, INPUT);

   //initialize output pins
   pinMode(FAN_SIGNAL, OUTPUT);
   pinMode(RED_CURRENT, OUTPUT);
   pinMode(GREEN_CURRENT, OUTPUT);
   pinMode(BLUE_CURRENT, OUTPUT);

   //setup state machine
   state = 0;
   nextState = 0; 
   
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
  //Get flow rate for each laser (needs work)
  double redFlow = analogRead(RED_FLOW);
  double greenFlow = analogRead(GREEN_FLOW);
  double blueFlow = analogRead(BLUE_FLOW);
  
   if((redFlow < MIN_FLOW) || (greenFlow < MIN_FLOW) || (blueFlow < MIN_FLOW))
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

