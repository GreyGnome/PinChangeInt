// PinChangeIntExample
// This only works for ATMega328-compatibles; ie, Leonardo is not covered here.
// See the Arduino and the chip documentation for more details.
// See the Wiki at http://code.google.com/p/arduino-pinchangeint/wiki for more information.

// for vim editing: :set et ts=2 sts=2 sw=2

// This example demonstrates a configuration of 3 interrupting pins and 2 interrupt functions.
// The functions set the values of some global variables. All interrupts are serviced immediately,
// and the sketch can then query the values at our leisure. This makes loop timing non-critical.

// The interrupt functions are a simple count of the number of times the pin was brought high.
// For 2 of the pins, the values are stored and retrieved from an array and they are reset after
// every read. For one of the pins ("MYPIN3"), there is a monotonically increasing count; that is,
// until the 8-bit value reaches 255. Then it will go back to 0.

// For a more introductory sketch, see the SimpleExample328.ino sketch in the PinChangeInt
// library distribution.

#include <PinChangeInt.h>

// Modify these at your leisure.
#define MYPIN1 A3
#define MYPIN2 A4
#define MYPIN3 A5

// Don't change these.
#define FIRST_ANALOG_PIN 14
#define TOTAL_PINS 19
// Notice that anything that gets modified inside an interrupt, that I wish to access
// outside the interrupt, is marked "volatile". That tells the compiler not to optimize
// them.
volatile uint8_t latest_interrupted_pin;
volatile uint8_t interrupt_count[TOTAL_PINS]={0}; // possible arduino pins
volatile uint8_t pin3Count=0;

// Do not use any Serial.print() in interrupt subroutines. Serial.print() uses interrupts,
// and by default interrupts are off in interrupt subroutines.
// Here we update a counter corresponding to whichever pin interrupted.
void quicfunc0() {
  interrupt_count[MYPIN1]++;
};

void quicfunc1() {
  interrupt_count[MYPIN2]++;
};

// You can assign any number of functions to different pins. How cool is that?
// Here we have a global variable that we increment. We can access this variable outside the interrupt,
// and we know it will be valid because it was declared "volatile"- meaning, the compiler performs
// no optimizations on it.
void pin3func() {
  pin3Count++;
}

// Attach the interrupts in setup()
void setup() {
  pinMode(MYPIN1, INPUT_PULLUP);
  attachPinChangeInterrupt(MYPIN1, quicfunc0, RISING);
  pinMode(MYPIN2, INPUT_PULLUP);
  attachPinChangeInterrupt(MYPIN2, quicfunc1, RISING);
  pinMode(MYPIN3, INPUT_PULLUP);
  attachPinChangeInterrupt(MYPIN3, pin3func, CHANGE); // Any state change will trigger the interrupt.
  Serial.begin(115200);
  Serial.println("---------------------------------------");
}

uint8_t i;
uint8_t currentPIN3Count=0;

void loop() {
  uint8_t count;
  Serial.print(".");
  delay(1000);                                  // every second,
  for (i=0; i < TOTAL_PINS; i++) {
    if (interrupt_count[i] != 0) {              // look at all the interrupted pins
      count=interrupt_count[i];                 // store its count since the last iteration
      interrupt_count[i]=0;                     // and reset it to 0
      Serial.print("Count for pin ");
      if (i < FIRST_ANALOG_PIN) {               // then tell the user what it was, in a friendly way
        Serial.print("D");
        Serial.print(i, DEC);
      } else {
        Serial.print("A");
        Serial.print(i-FIRST_ANALOG_PIN, DEC);
      }
      Serial.print(" is ");
      Serial.println(count, DEC);
    }
  }
  if (currentPIN3Count != pin3Count) {          // Print our monotonically increasing counter (no reset to 0).
      Serial.print("Third pin count update: "); Serial.print(pin3Count, DEC); Serial.println();
      currentPIN3Count=pin3Count;
  }
}

