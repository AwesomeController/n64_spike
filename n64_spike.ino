#include "Command.h"

int DATA_PIN = 2;

void setup() {
  attachInterrupt(0, respondToN64, FALLING);
  
  pinMode(DATA_PIN, INPUT);
  digitalWrite(DATA_PIN, HIGH);
}

void loop() {
  
}

void respondToN64() {
  noInterrupts();
  
  Command c = readCommand();
  switch(c)
  {
    case READ_BUTTONS:
      sendButtons();
      break;
    default:
      break;
  }
  
  interrupts();
}

Command readCommand() {
  int currentCommand[9];
  
  for (int i = 0; i < 9; i++) {
    currentCommand[i] << digitalRead(DATA_PIN);
  }
  
  int getButtonsCommand[9] = {0, 0, 0, 0, 0, 0, 0, 1, 1};
  
  if (memcmp(currentCommand, getButtonsCommand, 9) == 0)
  {
    return READ_BUTTONS;
  }
  return UNKNOWN;
}

void sendButtons() {
  pinMode(DATA_PIN, OUTPUT);
  // send the buttons
  pinMode(DATA_PIN, INPUT);
}
