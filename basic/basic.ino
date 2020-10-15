#include <Keyboard.h>

#define SPEED_THRESHOLD 15
#define PLAYER_1_UP 'z'
#define PLAYER_2_UP KEY_UP_ARROW

/* Vélal A */
int wheelInputA = 2;
int speedCounterA = 0;
int old_wheelStateA;

/* Vélal B */
int wheelInputB = 6;
int speedCounterB = 0;
int old_wheelStateB;

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
    Keyboard.releaseAll();
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
  
  /* Vélal B */
  pinMode(wheelInputB, INPUT);
  old_wheelStateB = digitalRead(wheelInputB);

  Keyboard.begin();
  
  Serial.begin(9600);
}

void loop() {
  gottaGoFast(wheelInputA, &old_wheelStateA, &speedCounterA, SPEED_THRESHOLD, PLAYER_1_UP);
  gottaGoFast(wheelInputB, &old_wheelStateB, &speedCounterB, SPEED_THRESHOLD, PLAYER_2_UP);
  delay(1);
}
