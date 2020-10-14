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
/* https://www.arduino.cc/en/Reference/KeyboardModifiers */
/* Player 1 */
#define PLAYER_1_LEFT 'q'
#define PLAYER_1_RIGHT 'd'
#define PLAYER_1_UP 'z'
#define PLAYER_1_NITRO KEY_LEFT_CTRL
#define PLAYER_1_BACK KEY_ESC
/* Player 2 */
#define PLAYER_2_LEFT KEY_LEFT_ARROW
#define PLAYER_2_RIGHT KEY_RIGHT_ARROW
#define PLAYER_2_UP KEY_UP_ARROW
#define PLAYER_2_NITRO KEY_RIGHT_CTRL
#define PLAYER_2_BACK KEY_BACKSPACE

/* Debug */
#define DEBUG_ON true
#define KEYBOARD_ON false

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
  // Velal 1
  if(DEBUG_ON) {
    Serial.println("Velal 1\n---------");
    debugVelal(velal1);
  }
  
  int dirVelal1 = dirVelal(velal1, DIR_STEPS);
  bool goVelal1 = speedVelal(velal1, SPEED_THRESHOLD);

  if(KEYBOARD_ON) {
    switch(dirVelal1) {
      case 'l':
        Keyboard.press(PLAYER_1_LEFT);
        break;
      case 'r':
        Keyboard.press(PLAYER_1_RIGHT);
        break;
    }

    if(goVelal1) {
      Keyboard.press(PLAYER_1_UP);
    }
  }

  if(DEBUG_ON) {
    Serial.println("---------\n");
  }

  
  // Velal 2
  if(DEBUG_ON) {
    Serial.println("Velal 2\n---------");
    debugVelal(velal1);
  }
  
  int dirVelal2 = dirVelal(velal1, DIR_STEPS);
  bool goVelal2 = speedVelal(velal1, SPEED_THRESHOLD);

  if(KEYBOARD_ON) {
    switch(dirVelal2) {
      case 'l':
        Keyboard.press(PLAYER_2_LEFT);
        break;
      case 'r':
        Keyboard.press(PLAYER_2_RIGHT);
        break;
    }

    if(goVelal2) {
      Keyboard.press(PLAYER_2_UP);
    }
  }

  if(DEBUG_ON) {
    Serial.println("---------\n");
  }

  // Release all keys
  if(KEYBOARD_ON) {
    Keyboard.releaseAll();
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

char dirVelal(struct Velal velal, int steps) {
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
  /* 'l' := left, 's' := straight, 'r' := right */
  int dir;
  if(velal.dirCounter < -steps) {
    dir = 'l';
  } else if(velal.dirCounter > steps) {
    dir = 'r';
  } else {
    dir = 's';
  }

  if(DEBUG_ON) {
    Serial.print("dirVelal? ");
    if(dir = 'l') {
      Serial.print("left");
    } else if(dir = 'r') {
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

  if(DEBUG_ON) {
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
