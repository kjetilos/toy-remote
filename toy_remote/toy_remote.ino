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

static IRsend irsend;
static int idle_count = 0;

#define PIN_UP      9
#define PIN_DOWN   10
#define PIN_LEFT   11
#define PIN_RIGHT  13

void setup()
{
  PCMSK |= _BV(PCINT0)   // Pin 9
         | _BV(PCINT1)   // Pin 10
         | _BV(PCINT2)   // Pin 11
         | _BV(PCINT4);  // Pin 13

  pinMode(PIN_UP, INPUT);    digitalWrite(PIN_UP, HIGH);
  pinMode(PIN_DOWN, INPUT);  digitalWrite(PIN_DOWN, HIGH);
  pinMode(PIN_LEFT, INPUT);  digitalWrite(PIN_LEFT, HIGH);
  pinMode(PIN_RIGHT, INPUT); digitalWrite(PIN_RIGHT, HIGH);
}

void goto_sleep(void)
{
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);  
  sleep_enable();  
  EIFR  |= _BV(PCIF);  // Clear any Pin Change Interrupt flag
  GIMSK |= _BV(PCIE); // Enable Pin Change Interrupt
  sleep_cpu();
  
  // cancel sleep as a precaution
  sleep_disable();
  GIMSK = 0; // Disable Pin Change Interrupt
}

void loop() {
  bool up = digitalRead(PIN_UP) == LOW;
  bool down = digitalRead(PIN_DOWN) == LOW;
  bool left = digitalRead(PIN_LEFT) == LOW;
  bool right = digitalRead(PIN_RIGHT) == LOW;
  
  if (up || down || left || right)
  {
    
  }
  else
  {
    idle_count++;
  }
  
  if (idle_count > 10)
  {
    goto_sleep();
    idle_count = 0;
  }
  
  irsend.enableIROut(38);
  for (int i = 0; i < 1000; i++) {
    irsend.mark(1850);
    irsend.space(750);
  }

  delay(100);  
}
