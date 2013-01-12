#include "wiring_private.h" // for sbi and cbi

class Beep
{
  public:
    // if we try to do this in the ctor and put the declaration along with 
    // accel, et al in AeroQuad.pde, TCCR1A and TCCR1B don't get set up properly
    void initialize()
    {
      OCR1B = 0;
      ICR1 = 0;
    
      sbi(TCCR1A, COM1B1); // p160, 0b10 == clear when up-counting, set when down-counting
      cbi(TCCR1A, COM1B0);
      
      sbi(TCCR1B, WGM13); // p148, phase correct 8-bit
      cbi(TCCR1B, WGM12);
      sbi(TCCR1A, WGM11);
      cbi(TCCR1A, WGM10);
      
      cbi(TCCR1B, CS12); // p161, 0b001 == clk/1 (no prescaling)
      cbi(TCCR1B, CS11);
      sbi(TCCR1B, CS10);
    }
    
    void beep(uint16_t freq)
    {
      if (freq != 0)
      {
        ICR1 = freq;
        OCR1B = freq / 2;
      
        sbi(DDRB, 6); // PWM on OC1B
      }
      else
      {
        cbi(DDRB, 6); // PWM on OC1B
      }
    }
};