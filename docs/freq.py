
#!/usr/bin/env python3
"""
Find optimal configuration parameters (clock prescaler and max duty cycle) for
a desired switching frequency.
"""

import math

print("Enter desired frequency in Hz [1000000]: ", end='')
desired = float(input() or "1000000")

frequencies = []

for prescaler in range(1, 16):
    for max_duty in range(2, 256):
        frequency = 64000000. / ((2.0**(prescaler - 1)) * max_duty)
        frequencies.append((prescaler, max_duty, frequency))

frequencies = list(sorted(frequencies, key=lambda x: abs((desired-x[2])/desired)))[:10]
frequencies = list(sorted(frequencies, key=lambda x: -x[1]))

print(f"Frequency\tPrescaler\tMax Duty")
for prescaler, max_duty, frequency in frequencies:
    print(f"{frequency:,.2f} Hz\t{prescaler}\t{max_duty}")