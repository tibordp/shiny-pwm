// PWM_MAX_DUTY determines the resolution of the PWM signal, which in turn determines the
// frequency. Since the PLL runs at 64 MHz, full 8 bit resolution gives a frequency of ~ 250 kHz
// (64 MHz / 255).
#define PWM_MAX_DUTY 255

// The PWM signal will max out at PWM_DUTY_LIMIT even if the feedback voltage is below desired
// point. This is a crude way to limit the load current.
#define PWM_DUTY_LIMIT 90

// PWM duty will be adjusted by the step size on every new reading.
#define PWM_STEP_SIZE 1

// The window size for the moving average of ADC readings for feedback voltage. Larger window allows
// for more resolution. (min=1, max=64)
#define WINDOW_SIZE 10

// If MONITOR_ISR is set, PB0 will be brought high during the interrupt handler, 
// allowing the ADC timings to be measured.
#define MONITOR_ISR

// Choose between internal 2.56V and internal 1.1V reference voltage for the ADC. Note that internal
// 2.56V reference requires VCC of at least 3V. If none are defined, VCC is used.
#define USE_2V56_REFERENCE
//#define USE_1V1_REFERENCE