/*
 * Toy Remote code customized for running on an ATtiny 2313
 *
 * Pin Usage:
 *   9 (PB0) - Up
 *  10 (PB1) - Down
 *  11 (PB2) - Left
 *  12 (PB3) - IR LED Output
 *  13 (PB4) - Right
 *
 * The input buttons are using internal pull-up and are connected to ground so 
 * they are active low.
 * 
 * Kjetil Østerås
 */

#include <IRremote.h>
#include <avr/sleep.h>

IRsend irsend;

#define IR_PIN   8
void setup()
{
  PCMSK |= _BV(PCINT0)   // Pin 9
         | _BV(PCINT1)   // Pin 10
         | _BV(PCINT2)   // Pin 11
         | _BV(PCINT4);  // Pin 13

  pinMode( 9, INPUT); digitalWrite( 9, HIGH);
  pinMode(10, INPUT); digitalWrite(10, HIGH);
  pinMode(11, INPUT); digitalWrite(11, HIGH);
  pinMode(13, INPUT); digitalWrite(13, HIGH);

  
  pinMode(IR_PIN, OUTPUT);
  digitalWrite(IR_PIN, LOW);
}


void goto_sleep(void)
{
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);  
  sleep_enable();  
  EIFR |= _BV(PCIF);  // Clear any Pin Change Interrupt flag
  GIMSK |= _BV(PCIE); // Enable Pin Change Interrupt
  sleep_cpu();
  
  // cancel sleep as a precaution
  sleep_disable();
  GIMSK = 0; // Disable Pin Change Interrupt
}

void loop() {
  digitalWrite(IR_PIN, HIGH);
  delay(500);
  digitalWrite(IR_PIN, LOW);
  delay(500);

  
  irsend.enableIROut(38);
  for (int i = 0; i < 1000; i++) {
    irsend.mark(1850);
    irsend.space(750);
  }
  
  goto_sleep();
}
