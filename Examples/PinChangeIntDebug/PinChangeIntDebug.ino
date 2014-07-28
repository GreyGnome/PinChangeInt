// PinChangeIntDebug
// version 1.0 Wed Jul  9 16:20:56 CDT 2014
// Lean project for debugging. Don't expect a lot of commentary in here. This is for hacking.
// This code taken from Examples/PinChangeIntTest, so refer there for more information and commentary.

#define PINMODE
#define FLASH
#include <ByteBuffer.h>
#include <MemoryFree.h>
#include <PinChangeInt.h>

// This example demonstrates a configuration of 6 interrupting pins and 3 interrupt functions.
// A variety of interrupting pins have been chosen, so as to test all PORTs on the Arduino.
// The pins are as follows:
#define INTERRUPT_PIN1 2  // port D
#define INTERRUPT_PIN2 3
#define INTERRUPT_PIN3 11 // Port B
#define INTERRUPT_PIN4 12
#define INTERRUPT_PIN5 A3 // Port C, also can be given as "17"
#define INTERRUPT_PIN6 A4

uint8_t pins[6]={ INTERRUPT_PIN1, INTERRUPT_PIN2, INTERRUPT_PIN3, INTERRUPT_PIN4, INTERRUPT_PIN5, INTERRUPT_PIN6 };
uint8_t ports[6]={ 0, 0, 0, 0, 0, 0 };

uint8_t latest_interrupted_pin;
uint8_t interrupt_count[20]={0}; // 20 possible arduino pins
uint8_t port;
uint8_t mode;

ByteBuffer printBuffer(200);
char charArray[16];
char numBuffer[5] = { 0, 0, 0, 0, 0 };
uint8_t printFull=0;

volatile boolean start=0;
volatile boolean initial=true;
long begintime=0;
long now=0;

void smallIntToString(char *outString, int number) {
	uint8_t thousands=0;
  uint8_t hundreds=0;
  uint8_t tens=0;
  uint8_t ones=0;

	if (number > 9999) {
		outString[0]='S'; outString[1]='I'; outString[2]='Z'; outString[3]='E'; outString[4]=0;
		return;
	}
  while (number >= 1000 ) {
    thousands++;
    number-=1000;
  }
  while (number >= 100 ) {
    hundreds++;
    number-=100;
  }
  while (number >= 10 ) {
    tens++;
    number-=10;
  }
  ones=number;
  ones+=48;
  if (thousands > 0) {
		thousands+=48; hundreds+=48; tens+=48;
		outString[0]=thousands; outString[1]=hundreds; outString[2]=tens;
		outString[3]=ones; outString[4]=0;
	}
  else if (hundreds > 0) {
		hundreds+=48; tens+=48;
		outString[0]=hundreds; outString[1]=tens; outString[2]=ones; outString[3]=0;
	}
  else if (tens > 0) {
		tens+=48;
		outString[0]=tens; outString[1]=ones; outString[2]=0;
	}
  else { outString[0]=ones; outString[1]=0; };
}

void showMode() {
  switch (mode) {
  case FALLING:
    printBuffer.putString((char *) "-F-");
  break;
  case RISING:
    printBuffer.putString((char *) "+R+");
  break;
  case CHANGE:
    printBuffer.putString((char *) "*C*");
  break;
  }
}

void quicfunc0() {
  latest_interrupted_pin=PCintPort::arduinoPin;
  mode=PCintPort::pinmode;
  showMode();
  if (start==1) {
    interrupt_count[latest_interrupted_pin]++;
  }
  smallIntToString(numBuffer, latest_interrupted_pin);
  printBuffer.putString((char *) "f0p"); printBuffer.putString(numBuffer); printBuffer.putString((char *) "-P");
  smallIntToString(numBuffer, digitalPinToPort(latest_interrupted_pin));
  printBuffer.putString(numBuffer);
  printBuffer.putString((char *) "\n");
};

#define MAXPINCOUNT 6
void attachInterrupts() {
	uint8_t i;
  for (i=0; i < MAXPINCOUNT; i++) {
    pinMode(pins[i], INPUT); digitalWrite(pins[i], HIGH);
    ports[i]=digitalPinToPort(pins[i]);
    PCintPort::attachInterrupt(pins[i], &quicfunc0, CHANGE);
  }
}

void detachInterrupts() {
	uint8_t i;
  for (i=0; i < MAXPINCOUNT; i++) {
		PCintPort::detachInterrupt(pins[i]);
  }
}

uint8_t i;
bool interrupts_are_attached=false;
void setup() {
  Serial.begin(115200);
  delay(250);
  Serial.println("Test");
  delay(250);
  Serial.print("*---*");
  begintime=millis();
	attachInterrupts(); interrupts_are_attached=true;
	Serial.println("NOTICE: Interrupts ATTACHED.");
}

void loop() {
  #define LOOPDELAY 2000
  now=millis();
  uint8_t count;
  char outChar;
  uint8_t pinState;
  while ((outChar=(char)printBuffer.get()) != 0) Serial.print(outChar);
  if ((now - begintime) > LOOPDELAY) {
    Serial.print(".");
    pinState=digitalRead(INTERRUPT_PIN1);
    if (pinState == HIGH){
      Serial.print("H");
    }
    else { Serial.print("L");
    }
    if (printBuffer.checkError()) {
      Serial.println("NOTICE: Some output lost due to filled buffer.");
    }
    for (i=0; i < 20; i++) {
      if (interrupt_count[i] != 0) {
        count=interrupt_count[i];
        interrupt_count[i]=0;
        Serial.print("Count for pin ");
        if (i < 14) {
          Serial.print("D");
          Serial.print(i, DEC);
        } else {
          Serial.print("A");
          Serial.print(i-14, DEC);
        }
        Serial.print(" is ");
        Serial.println(count, DEC);
      }
    }
    begintime=millis();
		if (interrupts_are_attached) {
			detachInterrupts(); interrupts_are_attached=false;
			Serial.print("NOTICE: Interrupts DETACHED. Memory: ");
			Serial.print(freeMemory(), DEC);
			Serial.println(" bytes");
		}
		else {
			Serial.print("NOTICE: ATTACHING Interrupts. Memory: ");
			Serial.print(freeMemory(), DEC);
			Serial.println(" bytes");
			attachInterrupts(); interrupts_are_attached=true;
		}
	}
}

