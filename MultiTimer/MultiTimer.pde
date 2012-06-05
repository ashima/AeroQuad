#ifdef ARDUINO && ARDUINO >= 100
#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

	#include <Wire.h>
#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define PWM_FREQUENCY 300   // in Hz
#define PWM_PRESCALER 8
#define PWM_COUNTER_PERIOD (F_CPU/PWM_PRESCALER/PWM_FREQUENCY)


#define LEDPIN 13
#define LED2PIN 4
#define LED3PIN 31

#define USB_BAUD 57600
#define USB_SERIAL Serial

// #define TWI

uint32_t _t3;
uint32_t _t4;
uint32_t _t5;

void resetTimers()
{
  // disable the timers while they are being configured
  TCCR3B = TCCR4B = TCCR5B = 0;
  // WGMn3:0 = 0, which means the counter counts from 0 -> 0xFFFF, 0 -> 0xFFFF, ...
  TCCR3A = TCCR4A = TCCR5A = 0; // WGMn1:0 = 0
  // reset counters
  TCNT3 = TCNT4 = TCNT5 = 0;
  // enable overflow interrupts, disable output compare interrupts
  TIMSK3 = TIMSK4 = TIMSK5 = (1<<TOIE5);
  // clear any pending interrupts
  TIFR5 = TIFR4 = TIFR3 = 0;

  _t3 = _t4 = _t5 = 0;

  // clock on falling edge of external Tn signal if CSn0 = 0, rising edge if CSn0 = 1
  TCCR3B = TCCR4B = TCCR5B = (1<<CS52) | (1<<CS51) | (1<<CS50);
}

enum ADC_Instruments { Thermocouple = 0, Current = 1, Voltage = 2 };
// use pins ADC0, 1, ..., ADC{ADC_Count}
const char ADC_Count = 3;
// how much of each successive measurement should be included
const float ADC_exponential_filter_factor = 0.01;
volatile float _adc[ADC_Count];
volatile char _curADC = 0;


void setup()
{
  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, HIGH);
  pinMode(LED2PIN, OUTPUT);
  digitalWrite(LED2PIN, LOW);
  pinMode(LED3PIN, OUTPUT);
  digitalWrite(LED3PIN, LOW);

  // debugging
  PORTA = 0;
  DDRA = 0xFF;

  USB_SERIAL.begin(USB_BAUD);
  USB_SERIAL.println("MultiTimer");

  sbi(DDRB, PB5);
  sbi(DDRB, PB6);
  TCCR1A = (1<<WGM11)|(1<<COM1A1)|(1<<COM1B1)|(1<<COM1C1);  // Clear OCnA/OCnB/OCnC on compare match, set OCnA/OCnB/OCnC at BOTTOM (non-inverting mode)
  TCCR1B = (1<<WGM13)|(1<<WGM12)|(1<<CS11);                 // Prescaler set to 8, that gives us a resolution of 0.5us
  ICR1 = PWM_COUNTER_PERIOD;                               // Clock_speed / ( Prescaler * desired_PWM_Frequency) #defined above.
  OCR1A = 1000 * 2;
  OCR1B = 1000 * 2;
  
  Wire.begin();
  // Recommendation from Mgros to increase I2C speed to 400kHz
  // http://aeroquad.com/showthread.php?991-AeroQuad-Flight-Software-v2.0&p=11262&viewfull=1#post11262
  TWBR = 12;

  // SDA/SCL pullups
  sbi(PORTD, PD0);
  sbi(PORTD, PD1);
        
  resetTimers();  
}

ISR(TIMER3_OVF_vect) { _t3++; }
ISR(TIMER4_OVF_vect) { _t4++; }
ISR(TIMER5_OVF_vect) { _t5++; }


uint32_t _last_n3 = 0;
uint32_t _last_n4 = 0;
uint32_t _last_n5 = 0;

float _last_n3_long = 0;
float _last_n4_long = 0;
float _last_n5_long = 0;

float _last_n3_std = 0;
float _last_n4_std = 0;
float _last_n5_std = 0;

uint32_t _last_n3_delta = 0;
uint32_t _last_n4_delta = 0;

unsigned long _last = micros();
unsigned long _print = _last;
bool _doIncrement = false;
unsigned long _increment = _last;
bool _matchBtoA = false;
unsigned long _lastDelta = _last;
unsigned long _delta = 0;
const float _alpha = 25.0 / (400 * 6 * 6 / 60) / 2.0 * 4; 
const int _fullScale = 1024;
const float _vref = 1.1;
const float _resistanceCurrent = 284.5 / _fullScale * _vref / 2 /2; // last /2 is 1/2 coil tap
//const float _voltageDivision = 23.79;
const float _voltageConversion = 0.0251136;
uint8_t _twi = 0;

void loop()
{
  static uint16_t _matchPseudoRPM = 0;
  
  unsigned long t = micros();  

  #ifdef TWI
  if (t - _last < 20000)
    return;

  _last = t;

  const uint8_t addr = (0x50 >> 1) + 1;

  Wire.beginTransmission(addr);
  Wire.send(_twi);
  Wire.send(0);
  Wire.send(0);
  Wire.send(0);
  Wire.send(0);
  Wire.send(0);
  Wire.send(0);
  Wire.send(0);
  Wire.send(0x01);
  Wire.endTransmission();
  uint8_t twi[2];
  Wire.requestFrom(addr, sizeof(twi));
  int i;
  for (i = 0; i < sizeof(twi) && Wire.available(); i++)
    twi[i] = Wire.receive();
  #endif


  if (t - _lastDelta >= 200000)
  {
    _lastDelta += 200000; // 200ms

    cli();
    uint16_t t3 = TCNT3;
    uint16_t t4 = TCNT4;
    sei();
    
    uint32_t n3 = (_t3 << 16) + t3;
    uint32_t n4 = (_t4 << 16) + t4;
    
  
    if (_matchBtoA || _matchPseudoRPM > 0)
    {
      uint32_t d3 = _matchPseudoRPM > 0 
        ? _matchPseudoRPM / 5
        : n3 - _last_n3_delta;
      uint32_t d4 = n4 - _last_n4_delta;
      
      float adj = _alpha * (int16_t)(d3 - d4) * 2;
      
      if (adj > 30)
        adj = 30;
      else if (adj < -30)
        adj = -30;
      else if (0.4 <= adj && adj < 1.0)
        adj = 1;
      else if (-1 < adj && adj <= -0.4)
        adj = -1;
      
      OCR1B += adj;
    }
    
    _last_n3_delta = n3;
    _last_n4_delta = n4;
  }
    

  if (t - _print >= 1000000)
  {
    _print += 1000000; // 1s
    cli();
    uint16_t t3 = TCNT3;
    uint16_t t4 = TCNT4;
    uint16_t t5 = TCNT5;
    sei();
    
    uint32_t n3 = (_t3 << 16) + t3;
    uint32_t n4 = (_t4 << 16) + t4;
    uint32_t n5 = (_t5 << 16) + t5;
    
    
    USB_SERIAL.print(micros());
    USB_SERIAL.print("  ");
//    USB_SERIAL.print(n3 - _last_n3);
//    USB_SERIAL.print(" ");
//    USB_SERIAL.print(n4 - _last_n4);
//    USB_SERIAL.print(" ");
//    USB_SERIAL.print(n5 - _last_n5);
//    USB_SERIAL.print("  ");
    USB_SERIAL.print(_last_n3_long);
    USB_SERIAL.print(" ");
    USB_SERIAL.print(_last_n4_long);
    USB_SERIAL.print(" ");
    USB_SERIAL.print(_last_n5_long);
    USB_SERIAL.print(" ");
    USB_SERIAL.print(_last_n3_std);
    USB_SERIAL.print(" ");
    USB_SERIAL.print(_last_n4_std);
    USB_SERIAL.print(" ");
    USB_SERIAL.print(_last_n5_std);
  
    USB_SERIAL.println();
    
    _last_n3_long = _last_n3_long*0.5+(n3-_last_n3)*0.5;
    _last_n4_long = _last_n4_long*0.5+(n4-_last_n4)*0.5;
    _last_n5_long = _last_n5_long*0.5+(n5-_last_n5)*0.5;

    _last_n3_std = _last_n3_std*0.5+abs((n3-_last_n3) - _last_n3_long)*0.5;
    _last_n4_std = _last_n4_std*0.5+abs((n4-_last_n4) - _last_n4_long)*0.5;
    _last_n5_std = _last_n5_std*0.5+abs((n5-_last_n5) - _last_n5_long)*0.5;

	
    _last_n3 = n3;
    _last_n4 = n4;
    _last_n5 = n5;
    
	
    
  }

}
