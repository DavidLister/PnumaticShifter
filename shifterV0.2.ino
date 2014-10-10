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
 
 Version Histoty:
 
 v0.1: Initial
 
 Initil Roughing out of the code.
 
 v0.2: Removed interupts, added 7 segment display
 
 - The inputs are now polled instead of interupt based.
 - Added 7 segment display functionality
 - Made function to switch gears properly (change number in correct sequence)
 
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
const unsigned long PRIOR_DELAY_UP_ARRAY[] = {
  PRIOR_DELAY_N_TO_TWO,
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
const unsigned long PRIOR_DELAY_DOWN_ARRAY[] = {
  PRIOR_DELAY_N_TO_ONE,
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
const unsigned long HOLD_TIME_UP_ARRAY[] = {
  HOLD_TIME_N_TO_TWO,
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
const unsigned long HOLD_TIME_DOWN_ARRAY[] = {
  HOLD_TIME_N_TO_ONE,
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

const int DISP_A = 7;
const int DISP_B = 8;
const int DISP_C = 9;
const int DISP_D = 10;
const int DISP_E = 11;
const int DISP_F = 12;
const int DISP_G = 13;

// States
const int IN_GEAR = 0;
const int SHIFTING_UP_PRIOR_DELAY = 1;
const int SHIFTING_UP_SOLENOID_ACTIVE = 2;
const int SHIFTING_DOWN_PRIOR_DELAY = 3;
const int SHIFTING_DOWN_SOLENOID_ACTIVE = 4;

// Misc
const unsigned long SHIFT_INTERVAL = 1000; // Least allowable time between shifting in ms
const int UP = 1;
const int DOWN = -1;

/***** Global Variables *****/
boolean shiftUpFlag = 0;
boolean shiftDownFlag = 0;
int currentGear = NEUTRAL;
int state = IN_GEAR;
unsigned long lastOp = millis();
unsigned long lastShift = millis();

/***** Functions *****/


int changeGear(int gear, int dir) {
  /*
  if dir is positive, changes up
   if dir is negative, changes down
   if dir is 0 it returns original gear
   
   Gears are 1 N 2 3 4 5 6
   1 0 2 3 4 5 6 When indexed in an array
   */
  if (dir > 0) {  // Shifting up
    if (gear == ONE) {
      return NEUTRAL;
    }
    else if (gear == NEUTRAL) {
      return TWO;
    }
    else if (gear == SIX) {
      return SIX;
    }
    else {
      return gear + 1;
    }
  }
  else if (dir < 0) {
    if (gear == TWO) {
      return NEUTRAL;
    }
    else if (gear == NEUTRAL) {
      return ONE;
    }
    else if (gear == ONE) {
      return ONE;
    }
    else {
      return gear - 1;
    }
  }
  else {
    return gear;
  }
}

void displayGear(int gear) {
  if (gear == ONE) {
    digitalWrite(DISP_A, 0);
    digitalWrite(DISP_B, 1);
    digitalWrite(DISP_C, 1);
    digitalWrite(DISP_D, 0);
    digitalWrite(DISP_E, 0);
    digitalWrite(DISP_F, 0);
    digitalWrite(DISP_G, 0);

  }
  else if (gear == NEUTRAL) {
    digitalWrite(DISP_A, 0);
    digitalWrite(DISP_B, 0);
    digitalWrite(DISP_C, 1);
    digitalWrite(DISP_D, 0);
    digitalWrite(DISP_E, 1);
    digitalWrite(DISP_F, 0);
    digitalWrite(DISP_G, 1);    
  }
  else if (gear == TWO) {
    digitalWrite(DISP_A, 1);
    digitalWrite(DISP_B, 1);
    digitalWrite(DISP_C, 0);
    digitalWrite(DISP_D, 1);
    digitalWrite(DISP_E, 1);
    digitalWrite(DISP_F, 0);
    digitalWrite(DISP_G, 1);    
  }
  else if (gear == THREE) {
    digitalWrite(DISP_A, 1);
    digitalWrite(DISP_B, 1);
    digitalWrite(DISP_C, 1);
    digitalWrite(DISP_D, 1);
    digitalWrite(DISP_E, 0);
    digitalWrite(DISP_F, 0);
    digitalWrite(DISP_G, 1);    
  }
  else if (gear == FOUR) {
    digitalWrite(DISP_A, 0);
    digitalWrite(DISP_B, 1);
    digitalWrite(DISP_C, 1);
    digitalWrite(DISP_D, 0);
    digitalWrite(DISP_E, 0);
    digitalWrite(DISP_F, 1);
    digitalWrite(DISP_G, 1);    
  }
  else if (gear == FIVE) {
    digitalWrite(DISP_A, 1);
    digitalWrite(DISP_B, 0);
    digitalWrite(DISP_C, 1);
    digitalWrite(DISP_D, 1);
    digitalWrite(DISP_E, 0);
    digitalWrite(DISP_F, 1);
    digitalWrite(DISP_G, 1);    
  }
  else if (gear == SIX) {
    digitalWrite(DISP_A, 1);
    digitalWrite(DISP_B, 0);
    digitalWrite(DISP_C, 1);
    digitalWrite(DISP_D, 1);
    digitalWrite(DISP_E, 1);
    digitalWrite(DISP_F, 1);
    digitalWrite(DISP_G, 1);    
  }
  else{    
    digitalWrite(DISP_A, 0);
    digitalWrite(DISP_B, 0);
    digitalWrite(DISP_C, 0);
    digitalWrite(DISP_D, 0);
    digitalWrite(DISP_E, 0);
    digitalWrite(DISP_F, 0);
    digitalWrite(DISP_G, 0);
  }
}

void setup() {

  pinMode(SHIFT_UP, INPUT);
  pinMode(SHIFT_DOWN, INPUT);
  pinMode(SOLENOID_UP, OUTPUT);
  pinMode(SOLENOID_DOWN, OUTPUT);
  pinMode(CUT_IGNITION, OUTPUT);

  pinMode(DISP_A, OUTPUT);
  pinMode(DISP_B, OUTPUT);
  pinMode(DISP_C, OUTPUT);
  pinMode(DISP_D, OUTPUT);
  pinMode(DISP_E, OUTPUT);
  pinMode(DISP_F, OUTPUT);
  pinMode(DISP_G, OUTPUT);

  digitalWrite(SOLENOID_UP, 0);
  digitalWrite(SOLENOID_DOWN, 0);
  digitalWrite(CUT_IGNITION, 0);


  //attachInterrupt(SHIFT_UP_INTERRUPT, shiftUp, RISING);
  //attachInterrupt(SHIFT_DOWN_INTERRUPT, shiftDown, RISING);

  displayGear(currentGear);

}

void loop() {

  switch (state) {

  case IN_GEAR:
    if (digitalRead(SHIFT_UP) == 0) {
      shiftUpFlag = 0;
    }

    if (digitalRead(SHIFT_DOWN) == 0) {
      shiftDownFlag = 0;
    }

    if (millis() - lastShift >= SHIFT_INTERVAL) {

      if (digitalRead(SHIFT_UP)) {
        if (currentGear != SIX) {
          digitalWrite(CUT_IGNITION, 1);
          shiftUpFlag = 1;
          lastOp = millis();
          state = SHIFTING_UP_PRIOR_DELAY;
        }
      }
      else if (digitalRead(SHIFT_DOWN)) {
        if (currentGear != ONE) {
          digitalWrite(CUT_IGNITION, 1);
          shiftDownFlag = 1;
          lastOp = millis();
          state = SHIFTING_DOWN_PRIOR_DELAY;
        }
      }

    }

    else {

      if (shiftUpFlag == 0) {
        if (digitalRead(SHIFT_UP)) {
          if (currentGear != SIX) {
            digitalWrite(CUT_IGNITION, 1);
            shiftUpFlag = 1;
            lastOp = millis();
            state = SHIFTING_UP_PRIOR_DELAY;
          }
        }       
      }

      if (shiftDownFlag == 0) {
        if (digitalRead(SHIFT_DOWN)) {
          if (currentGear != ONE) {
            digitalWrite(CUT_IGNITION, 1);
            shiftDownFlag = 1;
            lastOp = millis();
            state = SHIFTING_DOWN_PRIOR_DELAY;
          }
        }
      }
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
      currentGear = changeGear(currentGear, UP);
      displayGear(currentGear);
      lastShift = millis();
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
      currentGear = changeGear(currentGear, DOWN);
      displayGear(currentGear);
      lastShift = millis();
      lastOp = millis();
    }
    break;

  }
}

