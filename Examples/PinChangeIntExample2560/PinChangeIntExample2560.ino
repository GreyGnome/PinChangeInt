// PinChangeIntExample2560 
// This only works for ATMega2560-based boards.
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

// PIN NAMING
// For the Analog Input pins used as digital input pins, you can call them 14, 15, 16, etc.
// or you can use A0, A1, A2, etc. (the Arduino code will properly recognize the symbolic names,
// for example, pinMode(A0, INPUT_PULLUP);

// For Arduino MEGA (AT2560-based), besides the regular pins and the A (analog) pins,
// you have 4 more pins with defined names:
// SS   = 53
// MOSI = 51
// MISO = 50
// SCK  = 52

// NOW CHOOSE PINS
#if ! ( defined __AVR_ATmega2560__ || defined __AVR_ATmega1280__ || defined __AVR_ATmega1281__ || defined __AVR_ATmega2561__ || defined __AVR_ATmega640__ )
#error "This sketch only works on chips in the ATmega2560 family."
#endif

#define FIRST_ANALOG_PIN 54
#define TOTAL_PINS 69 // But only 18 of them (not including RX0) are PinChangeInt-compatible
                      // Don't use RX0 (Arduino pin 0) in this program- it won't work this is the
                      // pin that Serial.print() uses!
// See the Arduino and the chip documentation for more details.
#define MYPIN1 SS
#define MYPIN2 SCK
#define MYPIN3 MOSI
#define PIN3TEXT "MOSI" // This will say what MYPIN3 is, on the serial monitor

volatile uint8_t latest_interrupted_pin;
volatile uint8_t interrupt_count[TOTAL_PINS]={0}; // possible arduino pins
volatile uint8_t pin3Count=0;

// Do not use any Serial.print() in this function. Serial.print() uses interrupts, and is not compatible
// with an interrupt routine...!
void quicfunc() {
  latest_interrupted_pin=PCintPort::arduinoPin;
  interrupt_count[latest_interrupted_pin]++;
};

// You can assign any number of functions to different pins. How cool is that?
void pin3func() {
  pin3Count++;
}

void setup() {
  pinMode(MYPIN1, INPUT_PULLUP);
  attachPinChangeInterrupt(MYPIN1, quicfunc, FALLING);  // add more attachInterrupt code as required
  pinMode(MYPIN2, INPUT_PULLUP);
  attachPinChangeInterrupt(MYPIN2, quicfunc, FALLING);
  pinMode(MYPIN3, INPUT_PULLUP);
  attachPinChangeInterrupt(MYPIN3, pin3func, CHANGE);
  Serial.begin(115200);
  Serial.println("---------------------------------------");
}

uint8_t i;
uint8_t currentPIN3Count=0;
void loop() {
  uint8_t count;
  Serial.print(".");
  delay(1000);                                // every second,
  for (i=0; i < TOTAL_PINS; i++) {
    if (interrupt_count[i] != 0) {            // look at all the interrupted pins
      count=interrupt_count[i];               // store its count since the last iteration
      interrupt_count[i]=0;                   // and reset it to 0.
      Serial.print("Count for pin ");
      if (i == 50) { Serial.print("MISO"); }  // then tell the user what it was, in a friendly way.
      else if (i == 51) { Serial.print("MOSI"); }
      else if (i == 52) { Serial.print("SCK"); }
      else if (i == 53) { Serial.print("SS"); }
      else if (i < FIRST_ANALOG_PIN) {
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
  if (currentPIN3Count != pin3Count) {        // Print our monotonically increasing counter (no reset to 0)
      Serial.print(PIN3TEXT);
      Serial.print(" count update: "); Serial.print(pin3Count, DEC); Serial.println();
      currentPIN3Count=pin3Count;
  }
}

