// PinChangeInt SimpleExample sketch
// See the Wiki at http://code.google.com/p/arduino-pinchangeint/wiki for more information.

// for vim editing: :set et ts=2 sts=2 sw=2 et

// This example demonstrates the use of the PinChangeInt library on a single pin of your choice.
// This only works for ATMega328-compatibles; ie, Leonardo is not covered here.
// To use:

// 1. You must be using a fairly recent version of the Arduino IDE software on your PC/Mac,
// that is, version 1.0.1 or later. Check Help->About Arduino in the IDE.

// 2. Wire a simple switch to any Analog or Digital pin (known as ARDUINOPIN, defined below).
// Attach the other end to a GND pin. A "single pole single throw momentary contact"
// pushbutton switch is best for the best interrupting fun.

// 3. When pressed, the switch will connect the pin to ground ("low", or "0") voltage, and interrupt the
// processor. Don't let it confuse you that a switch press means the pin's voltage goes to 0; it
// may seem more intuitive to apply a "1" or high voltage to the pin to represent "pressed".
// But the processor is perfectly happy that we've made "0" equal "Pressed". The reason we've done so
// is because we are using the "internal pullup resistor" feature of the processor... the chip gives
// us a free resistor on every pin!
// See http://arduino.cc/en/Tutorial/DigitalPins for a complete explanation.

// 4. The interrupt is serviced immediately, and the ISR (Interrupt SubRoutine) sets the value of a global
// variable. The sketch can then query the value at its leisure. This makes loop timing non-critical.
// Open Tools->Serial Monitor in the IDE to see the results of your interrupts.

// 5. See PinChangeIntExample328.ino (in the PinChangeInt distribution) for a more elaborate example.

// 6. Create your own sketch using the PinChangeInt library!

#include <PinChangeInt.h>

// Modify this at your leisure.
#define ARDUINOPIN A4

// Notice that values that get modified inside an interrupt, that I wish to access
// outside the interrupt, are marked "volatile". It tells the compiler not to optimize
// the variable.
volatile uint16_t interruptCount=0; // The count will go back to 0 after hitting 65535.

// Do not use any Serial.print() in interrupt subroutines. Serial.print() uses interrupts,
// and by default interrupts are off in interrupt subroutines. Interrupt routines should also
// be as fast as possible. Here we just increment a counter.
void interruptFunction() {
  interruptCount++;
}

// Attach the interrupt in setup()
void setup() {
  pinMode(ARDUINOPIN, INPUT_PULLUP);  // Configure the pin as an input, and turn on the pullup resistor.
                                      // See http://arduino.cc/en/Tutorial/DigitalPins
  attachPinChangeInterrupt(ARDUINOPIN, interruptFunction, FALLING);
  Serial.begin(115200);
  Serial.println("---------------------------------------");
}

// In the loop, we just check to see where the interrupt count is at. The value gets updated by the
// interrupt routine.
void loop() {
  delay(1000);                            // Every second,
  Serial.print("Pin was interrupted: ");
  Serial.print(interruptCount, DEC);      // print the interrupt count.
  Serial.println(" times so far.");
}
