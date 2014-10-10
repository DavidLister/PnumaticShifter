/*
  Shifter
  
  Controls a soleniod to shift up and down.
  
  October 3, 2014
  by David Lister
  
  Notes:
  
  Gears are 1 N 2 3 4 5 6
            1 0 2 3 4 5 6 When indexed in an array
  
  Numbered gears are reference by their verbose number in variables and symbolic constants.
  
  All time is in ms
  
  TODO:
  
    - Handle negative prior delays for preactivation (maybe)
    - Debounce
    
 */


/***** Symbolic Constants *****/

// Gears
const int ONE = 1;
const int NEUTRAL = 0;
const int TWO = 2;
const int THREE = 3;
const int FOUR = 4;
const int FIVE = 5;
const int SIX = 6;

// Shifting up PRIOR_DELAY (the delay in ms prior to shifting after the pulse is sent)
const unsigned long PRIOR_DELAY_ONE_TO_N = 30;
const unsigned long PRIOR_DELAY_N_TO_TWO = 30;
const unsigned long PRIOR_DELAY_TWO_TO_THREE = 30;
const unsigned long PRIOR_DELAY_THREE_TO_FOUR = 30;
const unsigned long PRIOR_DELAY_FOUR_TO_FIVE = 30;
const unsigned long PRIOR_DELAY_FIVE_TO_SIX = 30;
const unsigned long PRIOR_DELAY_SIX_END = 4000000000; // Prevents shifting higher than six

// To call array, properly, use the gear the car is in before shifting
const unsigned long PRIOR_DELAY_UP_ARRAY[] = {PRIOR_DELAY_N_TO_TWO,
                                              PRIOR_DELAY_ONE_TO_N,
                                              PRIOR_DELAY_TWO_TO_THREE,
                                              PRIOR_DELAY_THREE_TO_FOUR,
                                              PRIOR_DELAY_FOUR_TO_FIVE,
                                              PRIOR_DELAY_FIVE_TO_SIX,
                                              PRIOR_DELAY_SIX_END};

// Shifting down PRIOR_DELAY (the delay in ms prior to shifting after the puslse is sent)
const unsigned long PRIOR_DELAY_ONE_END = 4000000000; // Prevents shifting lower than one
const unsigned long PRIOR_DELAY_N_TO_ONE = 30;
const unsigned long PRIOR_DELAY_TWO_TO_N = 30;
const unsigned long PRIOR_DELAY_THREE_TO_TWO = 30;
const unsigned long PRIOR_DELAY_FOUR_TO_THREE = 30;
const unsigned long PRIOR_DELAY_FIVE_TO_FOUR = 30;
const unsigned long PRIOR_DELAY_SIX_TO_FIVE = 30;

// To call array, properly, use the gear the car is in before shifting
const unsigned long PRIOR_DELAY_DOWN_ARRAY[] = {PRIOR_DELAY_N_TO_ONE,
                                                PRIOR_DELAY_ONE_END,
                                                PRIOR_DELAY_TWO_TO_N,
                                                PRIOR_DELAY_THREE_TO_TWO,
                                                PRIOR_DELAY_FOUR_TO_THREE,
                                                PRIOR_DELAY_FIVE_TO_FOUR,
                                                PRIOR_DELAY_SIX_TO_FIVE};

// Shifting up HOLD_TIME (length of time in ms the solenoid is open)
const unsigned long  HOLD_TIME_ONE_TO_N = 150;
const unsigned long  HOLD_TIME_N_TO_TWO = 150;
const unsigned long  HOLD_TIME_TWO_TO_THREE = 150;
const unsigned long  HOLD_TIME_THREE_TO_FOUR = 150;
const unsigned long  HOLD_TIME_FOUR_TO_FIVE = 150;
const unsigned long  HOLD_TIME_FIVE_TO_SIX = 150;
const unsigned long  HOLD_TIME_SIX_END = 0; // Prevents shifting higher than six


// To call array, properly, use the gear the car is in before shifting
const unsigned long HOLD_TIME_UP_ARRAY[] = {HOLD_TIME_N_TO_TWO,
                                            HOLD_TIME_ONE_TO_N,
                                            HOLD_TIME_TWO_TO_THREE,
                                            HOLD_TIME_THREE_TO_FOUR,
                                            HOLD_TIME_FOUR_TO_FIVE,
                                            HOLD_TIME_FIVE_TO_SIX,
                                            HOLD_TIME_SIX_END};



// Shifting down HOLD_TIME (length of time in ms the solenoid is open)
const unsigned long  HOLD_TIME_ONE_END = 0;
const unsigned long  HOLD_TIME_N_TO_ONE = 150;
const unsigned long  HOLD_TIME_TWO_TO_N = 150;
const unsigned long  HOLD_TIME_THREE_TO_TWO = 150;
const unsigned long  HOLD_TIME_FOUR_TO_THREE = 150;
const unsigned long  HOLD_TIME_FIVE_TO_FOUR = 150;
const unsigned long  HOLD_TIME_SIX_TO_FIVE = 150;

// To call array, properly, use the gear the car is in before shifting
const unsigned long HOLD_TIME_DOWN_ARRAY[] = {HOLD_TIME_N_TO_ONE,
                                              HOLD_TIME_ONE_END,
                                              HOLD_TIME_TWO_TO_N,
                                              HOLD_TIME_THREE_TO_TWO,
                                              HOLD_TIME_FOUR_TO_THREE,
                                              HOLD_TIME_FIVE_TO_FOUR,
                                              HOLD_TIME_SIX_TO_FIVE};
                                            
// Pins

const int SHIFT_UP = 2;
const int SHIFT_DOWN = 3;
const int SHIFT_UP_INTERRUPT = 0;
const int SHIFT_DOWN_INTERRUPT = 1;
const int SOLENOID_UP = 4;
const int SOLENOID_DOWN = 5;
const int CUT_IGNITION = 6;

// States
const int IN_GEAR = 0;
const int SHIFTING_UP_PRIOR_DELAY = 1;
const int SHIFTING_UP_SOLENOID_ACTIVE = 2;
const int SHIFTING_DOWN_PRIOR_DELAY = 3;
const int SHIFTING_DOWN_SOLENOID_ACTIVE = 4;


/***** Global Variables *****/
boolean shiftUpFlag = 0;
boolean shiftDownFlag = 0;
int currentGear = NEUTRAL;
int state = IN_GEAR;
unsigned long lastOp = millis();

/***** Functions *****/

void shiftUp() {
  if ((state !=  SHIFTING_UP_SOLENOID_ACTIVE) and (state != SHIFTING_DOWN_SOLENOID_ACTIVE)){
    shiftUpFlag = 1;
    shiftDownFlag = 0;
    state = IN_GEAR;
  }
}

void shiftDown() {
  if ((state !=  SHIFTING_UP_SOLENOID_ACTIVE) and (state != SHIFTING_DOWN_SOLENOID_ACTIVE)){
    shiftUpFlag = 0;
    shiftDownFlag = 1;
    state = IN_GEAR;
  }
}

void setup() {

 pinMode(SHIFT_UP, INPUT);
 pinMode(SHIFT_DOWN, INPUT);
 pinMode(SOLENOID_UP, OUTPUT);
 pinMode(SOLENOID_DOWN, OUTPUT);
 pinMode(CUT_IGNITION, OUTPUT);
 
 digitalWrite(SOLENOID_UP, 0);
 digitalWrite(SOLENOID_DOWN, 0);
 digitalWrite(CUT_IGNITION, 0);
 
 attachInterrupt(SHIFT_UP_INTERRUPT, shiftUp, RISING);
 attachInterrupt(SHIFT_DOWN_INTERRUPT, shiftDown, RISING);
 
}

void loop() {
  
  switch (state) {
    
   case IN_GEAR:
     if (shiftUpFlag) {
       state = SHIFTING_UP_PRIOR_DELAY;
       digitalWrite(CUT_IGNITION, 1);
       lastOp = millis();
     }
     else if (shiftDownFlag) {
       state = SHIFTING_DOWN_PRIOR_DELAY;
       digitalWrite(CUT_IGNITION, 1);
       lastOp = millis();
     }
     break;
     
   case SHIFTING_UP_PRIOR_DELAY:
       if (millis() - lastOp >= PRIOR_DELAY_UP_ARRAY[currentGear]) {
         state = SHIFTING_UP_SOLENOID_ACTIVE;
         digitalWrite(SOLENOID_UP, 1);
         lastOp = millis();
       }
     break;
     
   case SHIFTING_UP_SOLENOID_ACTIVE:
       if (millis() - lastOp >= HOLD_TIME_UP_ARRAY[currentGear]) {
         state = IN_GEAR;
         digitalWrite(SOLENOID_UP, 0);
         digitalWrite(CUT_IGNITION, 0);
         currentGear = currentGear + 1;
         lastOp = millis();
       }
     break;
     
   case SHIFTING_DOWN_PRIOR_DELAY:
       if (millis() - lastOp >= PRIOR_DELAY_DOWN_ARRAY[currentGear]) {
         state = SHIFTING_DOWN_SOLENOID_ACTIVE;
         digitalWrite(SOLENOID_DOWN, 1);
         lastOp = millis();
       }
     break;
     
   case SHIFTING_DOWN_SOLENOID_ACTIVE:
     if (millis() - lastOp >= HOLD_TIME_DOWN_ARRAY[currentGear]) {
         state = IN_GEAR;
         digitalWrite(SOLENOID_DOWN, 0);
         digitalWrite(CUT_IGNITION, 0);
         currentGear = currentGear - 1;
         lastOp = millis();
       }
     break;
     
  }
}
