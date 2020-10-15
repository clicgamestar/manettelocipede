#include <Keyboard.h>

#define SPEED_THRESHOLD 15
#define PLAYER_1_UP 'z'
#define PLAYER_1_LEFT 'q'
#define PLAYER_1_RIGHT 'd'
#define PLAYER_2_UP KEY_UP_ARROW
#define PLAYER_2_LEFT KEY_LEFT_ARROW
#define PLAYER_2_RIGHT KEY_RIGHT_ARROW

/* Vélal A */
int wheelInputA = 2;
int speedCounterA = 0;
int old_wheelStateA;
int leftInputA = 4;
int rightInputA = 5;

/* Vélal B */
int wheelInputB = 6;
int speedCounterB = 0;
int old_wheelStateB;
int leftInputB = 8;
int rightInputB = 9;

/* Time */
int time = 0;
int seconds() {
  return millis() / 1000;
}

/* Function definition */
void gottaGoFast(int wheelInput, int* oldState, int* counter, int threshold, char command) {
  // Update counter
  int newState = digitalRead(wheelInput);
  if (newState != *oldState) {
    *counter = *counter + 1;
  }
  *oldState = newState;

  // Every second, reset timer and keyboard
  if(seconds() != time) {
    time = seconds();
    *counter = 0;
    Keyboard.release(command);
  }

  Serial.print("gottaGoFast? ");
  Serial.print(*counter);
  Serial.print("/");
  Serial.println(threshold);

  if(*counter >= threshold) {
    Keyboard.press(command);
  }
}

void setup() {
  /* Vélal A */
  pinMode(wheelInputA, INPUT);
  old_wheelStateA = digitalRead(wheelInputA);
  pinMode(leftInputA, INPUT);
  pinMode(rightInputA, INPUT);
  
  /* Vélal B */
  pinMode(wheelInputB, INPUT);
  old_wheelStateB = digitalRead(wheelInputB);

  Keyboard.begin();
  
  Serial.begin(9600);
}

void loop() {
  // Vélal A
  gottaGoFast(wheelInputA, &old_wheelStateA, &speedCounterA, SPEED_THRESHOLD, PLAYER_1_UP);

  if(digitalRead(leftInputA) == HIGH && digitalRead(rightInputA) == LOW) {
    Keyboard.press(PLAYER_1_LEFT);
  } else if(digitalRead(leftInputA) == LOW && digitalRead(rightInputA) == HIGH) {
    Keyboard.press(PLAYER_1_RIGHT);
  } else {
    Keyboard.release(PLAYER_1_LEFT);
    Keyboard.release(PLAYER_1_RIGHT);
  }

  // Vélal B
  gottaGoFast(wheelInputB, &old_wheelStateB, &speedCounterB, SPEED_THRESHOLD, PLAYER_2_UP);

  if(digitalRead(leftInputB) == HIGH && digitalRead(rightInputB) == LOW) {
    Keyboard.press(PLAYER_2_LEFT);
  } else if(digitalRead(leftInputB) == LOW && digitalRead(rightInputB) == HIGH) {
    Keyboard.press(PLAYER_2_RIGHT);
  } else {
    Keyboard.release(PLAYER_2_LEFT);
    Keyboard.release(PLAYER_2_RIGHT);
  }
  
  delay(1);
}
