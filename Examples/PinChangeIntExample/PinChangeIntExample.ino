// PinChangeIntExample
// See the Wiki at http://code.google.com/p/arduino-pinchangeint/wiki for more information.
// for vim editing: :set et ts=2 sts=2 sw=2

// This example demonstrates a configuration of 3 interrupting pins and 2 interrupt functions.
// All interrupts are serviced immediately, but you can then query the values at your leisure.
// This makes loop timing non-critical.
// The interrupt functions are a simple count of the number of activations of the pin (ie, the pin was
// brought low).
// For 2 of the pins, the values are stored and retrieved from an array and they are reset after
// every read. For one of the pins ("MYPIN3"), there is a monotonically increasing count (until the
// uint8_t value rolls over).

// Add more Pins at your leisure.
//-------- define these in your sketch, if applicable ----------------------------------------------------------
//

// You can reduce the memory footprint of this handler by declaring that there will be no pin change interrupts
// on any one or two of the three ports.  If only a single port remains, the handler will be declared inline
// reducing the size and latency of the handler.
//#define NO_PORTB_PINCHANGES // to indicate that port b will not be used for pin change interrupts
//#define NO_PORTC_PINCHANGES // to indicate that port c will not be used for pin change interrupts
//#define NO_PORTD_PINCHANGES // to indicate that port d will not be used for pin change interrupts
// if there is only one PCInt vector in use the code can be inlined reducing latency and code size
// define DISABLE_PCINT_MULTI_SERVICE below to limit the handler to servicing a single interrupt per invocation.
// #define       DISABLE_PCINT_MULTI_SERVICE
//-------- define the above in your sketch, if applicable ------------------------------------------------------
#include <PinChangeInt.h>

// PIN NAMING
// For the Analog Input pins used as digital input pins, you can call them 14, 15, 16, etc.
// or you can use A0, A1, A2, etc. (the Arduino code comes with #define's
// for the Analog Input pins and will properly recognize e.g., pinMode(A0, INPUT);

// For Arduino MEGA (AT2560-based), besides the regular pins and the A (analog) pins,
// you have 4 more pins with defined names:
// static const uint8_t SS   = 53;
// static const uint8_t MOSI = 51;
// static const uint8_t MISO = 50;
// static const uint8_t SCK  = 52;

// NOW CHOOSE PINS
#if defined __AVR_ATmega2560__ || defined __AVR_ATmega1280__ || defined __AVR_ATmega1281__ || defined __AVR_ATmega2561__ || defined __AVR_ATmega640__
	#define FIRST_ANALOG_PIN 54
	#define TOTAL_PINS 69 // But only 18 of them (not including RX0) are PinChangeInt-compatible
                        // Don't use RX0 (Arduino pin 0) in this program- it won't work this is the
                        // pin that Serial.print() uses!
  // See the Arduino and the chip documentation for more details.
  #define MYPIN1 SS
  #define MYPIN2 SCK
  #define MYPIN3 MOSI
#else
	// This only works for ATMega328-compatibles; ie, Leonardo is not covered here.
  // See the Arduino and the chip documentation for more details.
	#define FIRST_ANALOG_PIN 14
	#define TOTAL_PINS 20
  #define MYPIN1 A8
  #define MYPIN2 A9
  #define MYPIN3 A10
#endif

volatile uint8_t latest_interrupted_pin;
volatile uint8_t interrupt_count[TOTAL_PINS]={0}; // possible arduino pins
volatile uint8_t pin3Count=0;

// Do not use any Serial.print() in this function. Serial.print() uses interrupts, and is not compatible
// with an interrupt routine...!
void quicfunc() {
  latest_interrupted_pin=PCintPort::arduinoPin;
  interrupt_count[latest_interrupted_pin]++;
};

// You can assign any number of functions to any number of pins.
// How cool is that?
void pin3func() {
  pin3Count++;
}

void setup() {
  pinMode(MYPIN1, INPUT); digitalWrite(MYPIN1, HIGH);
  PCintPort::attachInterrupt(MYPIN1, &quicfunc, FALLING);  // add more attachInterrupt code as required
  pinMode(MYPIN2, INPUT); digitalWrite(MYPIN2, HIGH);
  PCintPort::attachInterrupt(MYPIN2, &quicfunc, FALLING);
  pinMode(MYPIN3, INPUT); digitalWrite(MYPIN3, HIGH);
  PCintPort::attachInterrupt(MYPIN3, &pin3func, CHANGE);
  Serial.begin(115200);
  Serial.println("---------------------------------------");
}

uint8_t i;
uint8_t currentPIN3Count=0;
void loop() {
  uint8_t count;
  Serial.print(".");
  delay(1000);
  for (i=0; i < TOTAL_PINS; i++) {
    if (interrupt_count[i] != 0) {
      count=interrupt_count[i];
      interrupt_count[i]=0;
      Serial.print("Count for pin ");
#if defined __AVR_ATmega2560__ || defined __AVR_ATmega1280__ || defined __AVR_ATmega1281__ || defined __AVR_ATmega2561__ || defined __AVR_ATmega640__
      if (i == 50) Serial.print("MISO");
      else if (i == 51) { Serial.print("MOSI");
      else if (i == 52) { Serial.print("SCK");
      else if (i == 53) { Serial.print("SS");
      else
#endif
      if (i < FIRST_ANALOG_PIN) {
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
  if (currentPIN3Count != pin3Count) {
      Serial.print("Pin 3 count update: "); Serial.print(pin3Count, DEC); Serial.println();
      currentPIN3Count=pin3Count;
  }
}

