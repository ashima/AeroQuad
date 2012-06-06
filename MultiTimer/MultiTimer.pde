#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
    #include "WProgram.h"
#endif
#include <Wire.h>
#include "wiring_private.h"

#define USB_BAUD 57600
#define USB_SERIAL Serial

class Timer {
private:
  volatile uint8_t *_TCCRnA;
  volatile uint8_t *_TCCRnB;
  volatile uint16_t *_TCNTn;
  volatile uint8_t *_TIMSKn;
  volatile uint8_t *_TIFRn;
public:
	uint16_t pulses_hiword;
	uint32_t last_pulses;
	uint32_t high_time;
	uint32_t last_high_time;
	uint8_t mode;
	enum TimingMode { None, Pulses, Capture };
	
	Timer(volatile uint8_t *TCCRnA, volatile uint8_t *TCCRnB,
        volatile uint16_t *TCNTn, volatile uint8_t *TIMSKn,
        volatile uint8_t *TIFRn) 
        : pulses_hiword(0), mode(None) {
    _TCCRnA = TCCRnA;
    _TCCRnB = TCCRnB;
    _TCNTn = TCNTn;
    _TIMSKn = TIMSKn;
    _TIFRn = TIFRn;
	}
	
	void init_for_pulses() {
    *_TCCRnB = 0;          // disable the timers while they are being configured
    *_TCCRnA = 0;          // WGMn3:0 = 0 means we count from 0 -> 0xFFFF, 0 -> 0xFFFF, ...
    *_TCNTn = 0;           // reset counters
    *_TIMSKn = (1<<TOIE5); // enable pulses_hiword interrupts only
    *_TIFRn = 0;           // clear any pending interrupts
    
    last_pulses = pulses_hiword = 0;
    
    // the Timer now counts up
    // clock on falling edge of external Tn signal if CSn0 = 0, rising edge if CSn0 = 1
    *_TCCRnB = (1<<CS50) | (1<<CS51) | (1<<CS52);
    
    mode = Pulses;
	}
	
	void init_for_capture() {
    *_TCCRnB = 0;          // disable the timers while they are being configured
    *_TCCRnA = 0;          // WGMn3:0 = 0 means we count from 0 -> 0xFFFF, 0 -> 0xFFFF, ...
    *_TIMSKn = (1<<ICIE5); // enable input high_time interrupts only
    *_TIFRn = 0;           // clear any pending interrupts

    high_time = last_high_time = 0;

    // the Timer now counts up
    // clock on falling edge of external Tn signal if CSn0 = 0, rising edge if CSn0 = 1
    *_TCCRnB = (1<<ICES5) | (1<<CS51);  // prescaler of /8
    
    mode = Capture;
	}
	
	void stop() {
	  *_TIMSKn = 0;
	}
};

Timer Timer1 = Timer(&TCCR1A, &TCCR1B, &TCNT1, &TIMSK1, &TIFR1);
ISR(TIMER1_OVF_vect) { Timer1.pulses_hiword++; }
Timer Timer3 = Timer(&TCCR3A, &TCCR3B, &TCNT3, &TIMSK3, &TIFR3);
ISR(TIMER3_OVF_vect) { Timer3.pulses_hiword++; }
Timer Timer4 = Timer(&TCCR4A, &TCCR4B, &TCNT4, &TIMSK4, &TIFR4);
ISR(TIMER4_OVF_vect) { Timer4.pulses_hiword++; }
Timer Timer5 = Timer(&TCCR5A, &TCCR5B, &TCNT5, &TIMSK5, &TIFR5);
ISR(TIMER5_OVF_vect) { Timer5.pulses_hiword++; }

ISR(TIMER1_CAPT_vect) {
  static uint16_t up = 0;
  
  // if we are detecting rising edges
  if (TCCR1B & (1<<ICES1)) {
    up = ICR1;
    cbi(TCCR1B, ICES1);
  }
  else {
    int32_t diff = ICR1 - up;
    if (diff < 0)
      diff += 1<<16;
    Timer1.high_time += diff;
    sbi(TCCR1B, ICES1);
  }
}
ISR(TIMER3_CAPT_vect) {
  static uint16_t up = 0;
  
  // if we are detecting rising edges
  if (TCCR3B & (1<<ICES3)) {
    up = ICR3;
    cbi(TCCR3B, ICES3);
  }
  else {
    int32_t diff = ICR3 - up;
    if (diff < 0)
      diff += 1<<16;
    Timer3.high_time += diff;
    sbi(TCCR3B, ICES3);
  }
}
ISR(TIMER4_CAPT_vect) {
  static uint16_t up = 0;
  
  // if we are detecting rising edges
  if (TCCR4B & (1<<ICES4)) {
    up = ICR4;
    cbi(TCCR4B, ICES4);
  }
  else {
    int32_t diff = ICR4 - up;
    if (diff < 0)
      diff += 1<<16;
    Timer4.high_time += diff;
    sbi(TCCR4B, ICES4);
  }
}
ISR(TIMER5_CAPT_vect) {
  static uint16_t up = 0;
  
  // if we are detecting rising edges
  if (TCCR5B & (1<<ICES5)) {
    up = ICR5;
    cbi(TCCR5B, ICES5);
  }
  else {
    int32_t diff = ICR5 - up;
    if (diff < 0)
      diff += 1<<16;
    Timer5.high_time += diff;
    sbi(TCCR5B, ICES5);
  }
}

Timer *timers[] = {&Timer1, &Timer3, &Timer4, &Timer5};

void setup()
{
  sbi(DDRB, PB7);
  sbi(PORTB, PB7);
  
  USB_SERIAL.begin(USB_BAUD);
  USB_SERIAL.println("MultiTimer");
  
  Timer1.init_for_pulses();
  Timer3.init_for_pulses();
  Timer4.init_for_capture();
  Timer5.init_for_capture();
}

char read_b(HardwareSerial &ser)
{
  while (!ser.available())
  {}
  
  return ser.read();
}

bool is_digit(char c) {
  return '0' <= c && c <= '9';
}

void print_timer(Timer &t, uint16_t TCNTn) {
  switch (t.mode) {
  case Timer::Pulses: {
    cli();
    uint16_t pulses_hiword = t.pulses_hiword;
    sei();
    uint32_t pulses = (pulses_hiword << 16) + TCNTn;
    USB_SERIAL.print(pulses - t.last_pulses);
    t.last_pulses = pulses;
    break;
    }
  case Timer::Capture: {
    cli();
    uint32_t high_time = t.high_time;
    sei();
    USB_SERIAL.print((high_time - t.last_high_time) / 2);
    t.last_high_time = high_time;
    break;
    }
  case Timer::None:
    break;
  }
  USB_SERIAL.print(", ");
}

unsigned long _last = micros();
unsigned long _print = _last;
unsigned long _delay = 1000000; // 1s

void loop() {
  if (USB_SERIAL.available()) {
    delay(100);

    char c = USB_SERIAL.read();
    if (is_digit(c)) {
      USB_SERIAL.println("delay");
      _delay = 0;
      do {
        _delay *= 10;
        _delay += c - '0';
        c = USB_SERIAL.read();
      } while (is_digit(c));
    }
    else {
      USB_SERIAL.println("reconfig");
      for (uint8_t i = 0; i < sizeof(timers); i++) {
        switch (c) {
        case 'p':
          timers[i]->init_for_pulses();
          break;
        case 'c':
          timers[i]->init_for_capture();
          break;
        default:
          timers[i]->stop();
        }
        
        c = USB_SERIAL.read();
      }
    }
  }

  unsigned long t = micros();  

  if (t - _print >= _delay)
  {
    _print += _delay;
  
    cli();
    uint16_t t1 = TCNT1;
    uint16_t t3 = TCNT3;
    uint16_t t4 = TCNT4;
    uint16_t t5 = TCNT5;
    sei();
    
    USB_SERIAL.print(micros());
    USB_SERIAL.print(",  ");

    print_timer(Timer1, t1);
    print_timer(Timer3, t3);
    print_timer(Timer4, t4);
    print_timer(Timer5, t5);
    
    USB_SERIAL.println();
  }
}
