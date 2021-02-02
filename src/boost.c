
#include <avr/io.h>
#include <avr/interrupt.h>
#include "boost.h"

uint8_t dutyCycle = 0;
uint8_t cycle = 0;
uint16_t adcMovingAverage = 0;
uint16_t avgBuffer[WINDOW_SIZE] = {0};

volatile uint16_t targetVoltage = 512 * WINDOW_SIZE;

ISR(ADC_vect) {
#ifdef MONITOR_ISR    
    PORTB |= (1 << PB0);
#endif 
    // Calculate moving average of ADC readings
    uint16_t adcReading = (((uint16_t)ADCH) << 8) | ADCL;
    adcMovingAverage = adcMovingAverage + (adcReading - avgBuffer[cycle]);
    avgBuffer[cycle] = adcReading;
    
    uint16_t target = targetVoltage;
    if (adcMovingAverage < target && dutyCycle <= PWM_DUTY_LIMIT - PWM_STEP_SIZE) {
        dutyCycle += PWM_STEP_SIZE;
    } else if (adcMovingAverage > target && dutyCycle >= PWM_STEP_SIZE) {
        dutyCycle -= PWM_STEP_SIZE;
    }

    cycle = (cycle+1) % WINDOW_SIZE;

    OCR1A = dutyCycle;
#ifdef MONITOR_ISR 
    PORTB &= ~(1 << PB0);
#endif 
}

int main(void)
{
  PORTB = 0;

#ifdef MONITOR_ISR    
  DDRB = (1 << PB1) | (1 << PB0);
#else
  DDRB = (1 << PB1);
#endif

  // PLL setup
  // =========

  PLLCSR |= (1 << PLLE); // PLL enable

  // PLOCK must be set before asyncrhonous mode can be enabled
  while (!(PLLCSR & (1 << PLOCK))) ;

  PLLCSR |= (1 << PCKE); // Enable asynchronous mode

  // PWM setup
  // =========

  TCCR1 =
      (1 << CTC1)    | // Reset the counter after it reaches OCR1C
      (1 << PWM1A)   | // Set PWM comparator for OCR1A
      (1 << CS10)    | // No prescaler, run at full 64 MHz
      (1 << COM1A1)  | // Clear PB1 after counter reaches OCR1A
      (0 << COM1A0);   // Clear PB1 after counter reaches OCR1A

  OCR1A = 0;
  OCR1C = PWM_MAX_DUTY;

  // ADC setup
  // =========

  ADMUX =
      (0 << ADLAR) | // right shift result
#if defined(USE_2V56_REFERENCE) 
      (1 << REFS2) |
      (1 << REFS1) |
      (0 << REFS0) |
#elif defined(USE_1V1_REFERENCE)
      (0 << REFS2) | 
      (1 << REFS1) |
      (0 << REFS0) |
#endif
      (0 << MUX3)  | // use ADC2 for input (PB4)
      (0 << MUX2)  | // use ADC2 for input (PB4)
      (1 << MUX1)  | // use ADC2 for input (PB4)
      (0 << MUX0);   // use ADC2 for input (PB4)
  ADCSRB =
      (0 << ADTS0) | // ADC free running mode
      (0 << ADTS1) | // ADC free running mode
      (0 << ADTS2);  // ADC free running mode
  ADCSRA = 
      (1 << ADEN)  | 
      (1 << ADATE) | 
      (1 << ADIE)  | 
      (1 << ADPS1) | 
      (1 << ADPS2) |  // prescalar 64
      (1 << ADSC);

  sei();

  // All the magic happens in the ISR.
  for (;;) ;
}