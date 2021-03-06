
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include "config.h"

uint8_t cycle = 0;
uint8_t pwmCycle = 0;
int32_t dutyCycle = 0;
int32_t adcMovingAverage = 0;
uint16_t avgBuffer[WINDOW_SIZE] = {0};

volatile int32_t targetVoltage = 512 * WINDOW_SIZE;

#define _PWM_STEP_SIZE ((int32_t)(PWM_STEP_SIZE * PWM_DUTY_RESOLUTION))
#define _PWM_DUTY_UPPER_LIMIT ((int32_t)(PWM_DUTY_UPPER_LIMIT * PWM_DUTY_RESOLUTION))
#define _PWM_DUTY_LOWER_LIMIT ((int32_t)(PWM_DUTY_LOWER_LIMIT * PWM_DUTY_RESOLUTION))

ISR(ADC_vect) {
#ifdef MONITOR_ISR
   PORTB |= (1 << PB0);
#endif
    // Calculate moving average of ADC readings
    uint8_t adcLow = ADCL;
    uint16_t adcReading = ADCH << 8 | adcLow;
#if WINDOW_SIZE == 1
    #define _ADC_VALUE adcReading
#else
    adcMovingAverage += adcReading;
    adcMovingAverage -= avgBuffer[cycle];
    avgBuffer[cycle] = adcReading;
    #define _ADC_VALUE adcMovingAverage
#endif

    int32_t target = targetVoltage;
    if (_ADC_VALUE < target - (HYSTERESIS * WINDOW_SIZE) && dutyCycle <= _PWM_DUTY_UPPER_LIMIT - _PWM_STEP_SIZE)
    {
        dutyCycle += _PWM_STEP_SIZE;
    }
    else if (_ADC_VALUE > target + (HYSTERESIS * WINDOW_SIZE) && dutyCycle >= _PWM_DUTY_LOWER_LIMIT + _PWM_STEP_SIZE)
    {
        dutyCycle -= _PWM_STEP_SIZE;
    }
#ifdef ENABLE_DITHERING
    // Dither the PWM duty cycle to allow for higher resolution
    OCR1A = (uint8_t)(dutyCycle / PWM_DUTY_RESOLUTION) 
        + (dutyCycle % PWM_DUTY_RESOLUTION < pwmCycle ? 1 : 0);
#else
    OCR1A = (uint8_t)((dutyCycle + (PWM_DUTY_RESOLUTION/2)) / PWM_DUTY_RESOLUTION);
#endif

#if WINDOW_SIZE != 1
    cycle = (cycle + 1) % WINDOW_SIZE;
#endif    
    pwmCycle = (pwmCycle + 1) % PWM_DUTY_RESOLUTION;

#ifdef MONITOR_ISR
    PORTB &= ~(1 << PB0);
#endif
}

int main(void) {
    PORTB = 0;

#ifdef MONITOR_ISR
    DDRB = (1 << PB1) | (1 << PB0);
#else
    DDRB = (1 << PB1);
#endif

    // PLL setup
    // =========

    PLLCSR |= (1 << PLLE); // PLL enable

    // PLOCK must be set before asynchronous mode can be enabled
    while (!(PLLCSR & (1 << PLOCK)))
        ;

    PLLCSR |= (1 << PCKE); // Enable asynchronous mode

    // PWM setup
    // =========

    TCCR1 =
        (1 << CTC1) |   // Reset the counter after it reaches OCR1C
        (1 << PWM1A) |  // Set PWM comparator for OCR1A
        ((PWM_PRESCALER & 0b1111) << CS10) |   // No prescaler, run at full 64 MHz
#ifdef PWM_INVERT
        (1 << COM1A1) | // Set PB1 after counter reaches OCR1A
        (1 << COM1A0);  // Set PB1 after counter reaches OCR1A      
#else
        (1 << COM1A1) | // Clear PB1 after counter reaches OCR1A
        (0 << COM1A0);  // Clear PB1 after counter reaches OCR1A
#endif

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
        (0 << MUX3) | // use ADC2 for input (PB4)
        (0 << MUX2) | // use ADC2 for input (PB4)
        (1 << MUX1) | // use ADC2 for input (PB4)
        (0 << MUX0);  // use ADC2 for input (PB4)
    ADCSRB =
        (0 << ADTS0) | // ADC free running mode
        (0 << ADTS1) | // ADC free running mode
        (0 << ADTS2);  // ADC free running mode
    ADCSRA =
        (1 << ADEN) |
        (1 << ADATE) |
        (1 << ADIE) |
        (1 << ADPS1) |
        (1 << ADPS2) | // ADC prescaler is 64
        (1 << ADSC);

    sei();

    // All the magic happens in the ISR.
    for (;;) {
        set_sleep_mode(SLEEP_MODE_IDLE);
        sleep_mode();
    }
}