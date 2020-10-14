#include <Keyboard.h>

/* Size of the buffer */
#define SPEED_HISTORY_SIZE 5

/* Pins */
#define VELAL_1_PIN_1 4
#define VELAL_1_PIN_2 5
#define VELAL_1_PIN_3 6
#define VELAL_1_PIN_4 7
#define VELAL_2_PIN_1 8
#define VELAL_2_PIN_2 9
#define VELAL_2_PIN_3 10
#define VELAL_2_PIN_4 11

/* Game values */
#define DIR_STEPS 1
#define SPEED_THRESHOLD 5

/* Debug */
#define DEBUG true

/* Struct Velal */
struct Velal {
  // Wheel, speed
  int wheelInput;
  int speedCounter;
  int oldWheelState;
  int speedHistory[SPEED_HISTORY_SIZE];

  // Handlebars, direction
  int handlebarsInput1;
  int handlebarsInput2;
  int oldHandlebarsState;
  int dirCounter;
};

/* Functions declaration */
void initVelal(struct Velal velal, int wheelInput, int handlebarsInput1, int handlebarsInput2);
int dirVelal(struct Velal velal, int steps);
bool speedVelal(struct Velal velal, int threshold);
void debugVelal(struct Velal velal);

/* Clock/time management */
int time;  // In seconds.
int seconds() {
  return millis() / 1000;
}

/* Velals */
struct Velal velal1;
struct Velal velal2;

/* Setup */
void setup() {
  time = 0;

  initVelal(velal1, VELAL_1_PIN_1, VELAL_1_PIN_3, VELAL_1_PIN_4);
  initVelal(velal2, VELAL_2_PIN_1, VELAL_2_PIN_3, VELAL_2_PIN_4);
  
  Keyboard.begin();
  
  Serial.begin(9600);
}

void loop() {
  if(DEBUG) {
    Serial.println("Velal 1\n---------");
    debugVelal(velal1);
    dirVelal(velal1, DIR_STEPS);
    speedVelal(velal1, SPEED_THRESHOLD);
    Serial.println("---------\n");

    Serial.println("Velal 2\n---------");
    debugVelal(velal2);
    dirVelal(velal2, DIR_STEPS);
    speedVelal(velal2, SPEED_THRESHOLD);
    Serial.println("---------\n");
  } else {
    dirVelal(velal1, DIR_STEPS);
    speedVelal(velal1, SPEED_THRESHOLD);
    
    dirVelal(velal2, DIR_STEPS);
    speedVelal(velal2, SPEED_THRESHOLD);
  }
  
  delay(1);
}

/* Velal handling */
void initVelal(struct Velal velal, int wheelInput, int handlebarsInput1, int handlebarsInput2) {
  // Wheel
  velal.wheelInput = wheelInput;
  pinMode(velal.wheelInput, INPUT);
  velal.oldWheelState = digitalRead(velal.wheelInput);
  velal.speedCounter = 0;
  for(int i = 0; i < SPEED_HISTORY_SIZE; ++i) {
    velal.speedHistory[i] = 0;
  }

  // Handlebars
  velal.handlebarsInput1 = handlebarsInput1;
  velal.handlebarsInput2 = handlebarsInput2;
  pinMode(velal.handlebarsInput1, INPUT);
  pinMode(velal.handlebarsInput2, INPUT);
  velal.oldHandlebarsState = digitalRead(velal.handlebarsInput1);
  velal.dirCounter = 0;
}

int dirVelal(struct Velal velal, int steps) {
  // We want at least a step of 1
  if(steps < 1) {
    steps = 1;
  }

  // Update dirCounter
  int newHandlebarsState = digitalRead(velal.handlebarsInput1);
  if(newHandlebarsState != velal.oldHandlebarsState) {
    if(digitalRead(velal.handlebarsInput2) != newHandlebarsState) {
      velal.dirCounter = velal.dirCounter + 1;
    } else {
      velal.dirCounter = velal.dirCounter - 1;
    }
  }
  velal.oldHandlebarsState = newHandlebarsState;

  /* Define dir */
  /* -1 := left, 0 := straight, 1 := right */
  int dir;
  if(velal.dirCounter < -steps) {
    dir = -1;
  } else if(velal.dirCounter > steps) {
    dir = 1;
  } else {
    dir = 0;
  }

  if(DEBUG) {
    Serial.print("dirVelal? ");
    if(dir = -1) {
      Serial.print("left");
    } else if(dir = 1) {
      Serial.print("right");
    } else {
      Serial.print("straight");
    }
      Serial.print(" (dirCounter: ");
      Serial.print(velal.dirCounter);
      Serial.print(", steps: ");
      Serial.print(steps);
      Serial.println(").");
  }

  return dir;
}

bool speedVelal(struct Velal velal, int threshold) {
  // Update speedCounter
  int newWheelState = digitalRead(velal.wheelInput);
  if (newWheelState != velal.oldWheelState) {
    velal.speedCounter = velal.speedCounter + 1;
  }
  velal.oldWheelState = newWheelState;

  // Each second, put the current speedCounter in the speedHistory
  if(seconds() != time) {
    time = seconds();
    velal.speedHistory[time % SPEED_HISTORY_SIZE] = velal.speedCounter;
    velal.speedCounter = 0;
  }

  // Mean speed
  int speed;
  for(int i = 0; i < SPEED_HISTORY_SIZE; ++i) {
    speed += velal.speedHistory[i];
  }
  speed = speed / SPEED_HISTORY_SIZE;

  if(DEBUG) {
    Serial.print("speedVelal? ");
    Serial.print(speed >= threshold);
    Serial.print(" (speed: ");
    Serial.print(speed);
    Serial.print(", threshold: ");
    Serial.print(threshold);
    Serial.println(").");
  }

  return speed >= threshold;
}

void debugVelal(struct Velal velal) {
  Serial.print("wheelInput: Pin ");
  Serial.print(velal.wheelInput);
  Serial.print(" with value ");
  Serial.print(digitalRead(velal.wheelInput));
  Serial.println(".");
  
  Serial.print("handlebarsInput1: Pin ");
  Serial.print(velal.handlebarsInput1);
  Serial.print(" with value ");
  Serial.print(digitalRead(velal.handlebarsInput1));
  Serial.println(".");
  
  Serial.print("handlebarsInput2: Pin ");
  Serial.print(velal.handlebarsInput2);
  Serial.print(" with value ");
  Serial.print(digitalRead(velal.handlebarsInput2));
  Serial.println(".");
}
