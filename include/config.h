// PWM_PRESCALER determines the carrier frequency of the PWM signal. The PLL runs at 64 MHz, 
// and the prescaler divides it down: f_carrier = 64 MHz / (2^(PWM_PRESCALER - 1))
// Min: 1
// Max: 15
#define PWM_PRESCALER 1

// PWM_MAX_DUTY determines the resolution of the PWM signal, which in turn determines the
// output frequency. f_output = f_carrier / PWM_MAX_DUTY
// Min: 2
// Max: 255
#define PWM_MAX_DUTY 255

// PWM duty resolution allows for duty cycle to be adjusted in increments smaller than
// 1. If the duty cycle PWM is not a whole number. 
// Min: 1
// Max: 100
#define PWM_DUTY_RESOLUTION 100

// By default, if PWM duty cycle is not a whole number it will be rounded to the nearest
// whole number. If dithering is enabled, it will interpolate between adjecent whole numbers
// according in accordance with the fractional part.
// #define ENABLE_DITHERING

// The PWM signal will bottom out at PWM_DUTY_LOWER_LIMIT even if the feedback voltage is above desired
// point.
// Min: 0
// Max: PWM_MAX_DUTY - 1
#define PWM_DUTY_LOWER_LIMIT 0

// The PWM signal will max out at PWM_DUTY_UPPER_LIMIT even if the feedback voltage is below desired
// point. This is a crude way to limit the load current.
// Min: 1
// Max: PWM_MAX_DUTY
#define PWM_DUTY_UPPER_LIMIT 220

// PWM duty will be adjusted by the step size on every new reading.
// Min: 1 / PWM_DUTY_RESOLUTION
// Max: 255
#define PWM_STEP_SIZE 0.05

// Whether to invert the outgoing PWM singal
//#define PWM_INVERT 

// The window size for the moving average of ADC readings for feedback voltage. Larger window allows
// for more resolution at the expense of adjustment lag.
// Min: 1
// Max: 64
#define WINDOW_SIZE 1

// PWM duty cycle will not change if the feedback reading is within the hysteresis range of the desired voltage. 
// The unit is 1/1024 of the reference voltage selected. 
// Min: 0
// Max: 1024
#define HYSTERESIS 0

// If MONITOR_ISR is set, PB0 will be brought high during the interrupt handler, 
// allowing the ADC timings to be measured.
#define MONITOR_ISR

// Choose between internal 2.56V and internal 1.1V reference voltage for the ADC. Note that internal
// 2.56V reference requires VCC of at least 3V. If none are defined, VCC is used.
//#define USE_2V56_REFERENCE
#define USE_1V1_REFERENCE