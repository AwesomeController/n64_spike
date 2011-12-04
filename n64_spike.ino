#include "Command.h"

#define N64_HIGH DDRD &= ~0x04
#define N64_LOW DDRD |= 0x04
#define N64_QUERY (PIND & 0x04)

int DATA_PIN = 2;
int buttons[33] = {
  0, 0, 0, 1, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0
};

volatile int readCounter = 0;
int currentCommand[9];

void setup() {
  Serial.begin(14400);
  
  attachInterrupt(0, respondToN64, FALLING);
  
  pinMode(DATA_PIN, INPUT);
  N64_HIGH;
}

void loop() {
}

void respondToN64() {
  Command c = readCommand();
  switch(c)
  {
    case READ_BUTTONS:
      delayMicroseconds(100);
      sendButtons();
      break;
    default:
      break;
  }
}

Command readCommand() {
  return READ_BUTTONS;
  
  int commandLength = sizeof(currentCommand) / sizeof(currentCommand[0]);
  
  for (int i = 0; i < commandLength; i++) {
    delayTwoMicroseconds();
    currentCommand[i] = N64_QUERY;
    if (i != 8) {
       if (!finishCycle()) {
         return UNKNOWN;
       }
    }
  }
  
  if (readCounter > 200) {
    Serial.println("Current");    
    for (int i = 0; i < commandLength; i++) {
      Serial.println(currentCommand[i]);
    }
    readCounter = 0;
  } else {
    readCounter++;
  }
  
  int getButtonsCommand[9] = {0, 0, 0, 0, 0, 0, 0, 1, 1};
  
  if (memcmp(currentCommand, getButtonsCommand, 9) == 0)
  {
    return READ_BUTTONS;
  }
  return UNKNOWN;
}

void sendButtons() {
  volatile int length = sizeof(buttons) / sizeof(buttons[0]);
  
  if (readCounter > 200){
    Serial.println(length);
  } else {
    readCounter++;
  }  
  
  
  for (int i = 0; i < length; i++)
  {
    delayMicroseconds(1);
    if (buttons[i]) {
      N64_HIGH;
      delayMicroseconds(3);
    } else {
      N64_LOW;
      delayMicroseconds(2);
      N64_HIGH;
      delayMicroseconds(1);
    }
  }
  N64_HIGH;
}

void delayTwoMicroseconds() {
  asm volatile (
    "nop\nnop\nnop\nnop\nnop\n"
    "nop\nnop\nnop\nnop\nnop\n"
    "nop\nnop\nnop\nnop\nnop\n"
    "nop\nnop\nnop\nnop\nnop\n"
    "nop\nnop\nnop\nnop\nnop\n"
    "nop\nnop\nnop\nnop\nnop\n"
  );
}

boolean finishCycle() {
  waitForHigh() && waitForLow();
}

boolean waitForHigh() {
  int timeout = 63;
  while (!N64_QUERY) {
    if (!--timeout) {
      Serial.println("high");
      return false;
    }
  }
  
  return true;
}

boolean waitForLow() {
  int timeout = 63;
  while (N64_QUERY) {
    if (!--timeout) {
      Serial.println("low");
      return false;
    }
  }
  
  return true;
}
