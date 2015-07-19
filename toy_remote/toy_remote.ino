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

#define PIN_UP      9
#define PIN_DOWN   10
#define PIN_LEFT   11
#define PIN_RIGHT  13
#define PIN_DEBUG   8

static IRsend irsend;
static int idle_count = 0;

/**
 * IR Commands
 */
static const unsigned int cmd_forward       = 0b0101110000011;
static const unsigned int cmd_forward_left  = 0b0101110100001;
static const unsigned int cmd_forward_right = 0b0101111000010;
static const unsigned int cmd_back          = 0b0100010000000;
static const unsigned int cmd_back_left     = 0b0100010100010;
static const unsigned int cmd_back_right    = 0b0100011000001;
static const unsigned int cmd_left          = 0b0100110100011;
static const unsigned int cmd_right         = 0b0100111000000;
static const unsigned int cmd_stop          = 0b0100110000001;

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

  pinMode(PIN_DEBUG, OUTPUT);
  digitalWrite(PIN_DEBUG, LOW);
  
  /* Note that we must use an external crystal oscillator for this function 
   * to work propperly. When using the internal RC oscillator this function
   * setup an output of ~37 kHz */
  irsend.enableIROut(38);  
}

void goto_sleep(void)
{
  set_sleep_mode (SLEEP_MODE_PWR_DOWN);  
  sleep_enable();  
  EIFR  |= _BV(PCIF); // Clear any Pin Change Interrupt flag
  GIMSK |= _BV(PCIE); // Enable Pin Change Interrupt
  sleep_cpu();
  
  // cancel sleep as a precaution
  sleep_disable();
  GIMSK = 0; // Disable Pin Change Interrupt
}

void send_cmd(unsigned int cmd)
{
  for (int i = 12; i >= 0; i--) {
    if (i == 12) {
      irsend.mark(1930);
    } else {
      irsend.mark(800);
    }
    
    if (cmd & (0x1 << i)) {
      irsend.space(991);
    } else {
      irsend.space(300);
    }
  }
  irsend.mark(800);
  irsend.space(1050);
}

void loop() {  
  bool up = digitalRead(PIN_UP) == LOW;
  bool down = digitalRead(PIN_DOWN) == LOW;
  bool left = digitalRead(PIN_LEFT) == LOW;
  bool right = digitalRead(PIN_RIGHT) == LOW;
  
  if (up || down || left || right)
  {
    idle_count = 0;
    digitalWrite(PIN_DEBUG, HIGH);
    if (up) {
      if (left) {
        send_cmd(cmd_forward_left);
      } else if (right) {
        send_cmd(cmd_forward_right);
      } else {
        send_cmd(cmd_forward);
      }
    } else if (down) {
      if (left) {
        send_cmd(cmd_back_left);
      } else if (right) {
        send_cmd(cmd_back_right);
      } else {
        send_cmd(cmd_back);
      }
    } else if (left) {
      send_cmd(cmd_left);      
    } else if (right) {
      send_cmd(cmd_right);
    }
  }
  else
  {
    digitalWrite(PIN_DEBUG, LOW);
    if (idle_count < 10)
    {
      send_cmd(cmd_stop);
      idle_count++;
    }
  }

  
  if (idle_count > 10)
  {
    goto_sleep();
    idle_count = 0;
  }

  // TODO: Figure out what is wrong with the delay function on ATtiny 2313
  delay(2000); // Should be delay(130) instead since we want to wait 130 ms however something is wrong...
}
