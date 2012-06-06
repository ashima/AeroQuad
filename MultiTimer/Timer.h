#ifndef TIMER_H
#define TIMER_H

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

#endif