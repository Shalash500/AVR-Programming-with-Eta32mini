# AVR Bare-Metal Programming — Eta32mini Ver2

Learning AVR bare-metal C programming using the **Eta32mini Ver2** kit (ATmega32A at 16MHz).  
Every project is written in pure C — no Arduino libraries, no HAL abstractions.  

---

## Board Overview

| Property | Value |
|---|---|
| **MCU** | ATmega32A |
| **Clock** | 16 MHz external crystal |
| **Manufacturer** | FARESPCB |
| **IDE** | Microchip Studio 7 |
| **Programmer** | USBasp via ICSP ribbon cable |
| **Serial** | CH340 USB-to-serial (USB-B connector) |

---

## Board Pin Map

### Output Unit — PORTC (DIP switch block: Output Unit)

| Component | Pin | DIP Switch Label |
|---|---|---|
| Red LED | PC0 | RED |
| Green LED | PC1 | GRN |
| Blue LED | PC2 | BLU |
| Relay 2 | PC3 | REL2 |
| Relay 1 | PC4 | REL1 |
| Buzzer | PC5 | BUZ |

> LEDs and buzzer are **active high**. Enable the corresponding DIP switch before use.

### Keypad / Button Input — PORTD (MODE jumper → DIRECT for simple buttons)

| Component | Pin |
|---|---|
| SW1 / COL1 | PD2 |
| SW2 / COL2 | PD3 |
| SW3 / COL3 | PD4 |
| SW4 / COL4 | PD5 |

> Buttons are **active low** — pressed = LOW (10KΩ pull-up resistors on board).

### Analog Input

| Component | Pin |
|---|---|
| Multi-turn potentiometer | PA0 (ADC0) |

### UART Serial

| Signal | Pin |
|---|---|
| RX | PD0 |
| TX | PD1 |

> Enable PD0 and PD1 DIP switches on the UART Serial Unit block when using serial communication.

---

## Toolchain Setup

### Hardware required
- USBasp programmer
- ICSP 10-pin ribbon cable (connects USBasp to board ICSP header)
- USB-B cable (for CH340 serial communication)

### Software required
- [Microchip Studio 7](https://www.microchip.com/en-us/tools-resources/develop/microchip-studio) — IDE and compiler
- [Zadig](https://zadig.akeo.ie) — install libusb-win32 driver for USBasp
- Any serial terminal (Arduino IDE Serial Monitor or CoolTerm) — for UART projects

### Flash command (AVRDUDE in Microchip Studio External Tools)

**Title:** `Flash USBasp`  
**Command:** path to `avrdude.exe`  
**Arguments:**
```
-c usbasp -p m32 -U flash:w:$(ProjectDir)Debug\$(TargetName).hex:i
```

### Critical rule
```c
#define F_CPU 16000000UL   // MUST be the first line — before all includes
#include <avr/io.h>
```
`_delay_ms()` reads F_CPU at the moment the header is included. If F_CPU is not defined first, timing will be completely wrong.

---

## Mini Projects

| # | Project | Topic | Key Registers
|---|---|---|---
| MP-01 | GPIO LED Blink | GPIO output, bit manipulation | DDRC, PORTC 
| MP-02 | Traffic Light | Multiple GPIO outputs, sequencing | DDRC, PORTC
| MP-03 | Button Input | GPIO input, pull-ups, debounce, edge detection | DDRD, PORTD, PIND
| MP-04 | LED + Button State Machine | State machines, multiple inputs/outputs | DDRC, DDRD, PORTC, PORTD, PIND
| MP-05 | Timer CTC Blink | Timer1 CTC mode, ISR, interrupt-driven output | TCCR1A, TCCR1B, OCR1A, TIMSK
| MP-06 | PWM Buzzer | Timer2 CTC tone generation, frequency control | TCCR2, OCR2, TIMSK
| MP-07 | UART Hello World | UART transmit, register-level serial | UBRRH, UBRRL, UCSRB, UCSRC, UDR
| MP-08 | ADC Read Potentiometer | ADC conversion, analog reading, UART output | ADMUX, ADCSRA, ADC

---

## MP-01 — GPIO LED Blink

**What it does:** Blinks the Green LED on PC1 at 500ms on / 500ms off using direct register writes.  
**Key concept:** `DDRC |= (1 << PC1)` sets PC1 as output. `PORTC |=` drives it HIGH. `PORTC &= ~` drives it LOW.  
**DIP switch:** GRN → ON

---

## MP-02 — Traffic Light

**What it does:** Cycles Red → Green → Blue LEDs with 1000ms per colour and 200ms pause between each.  
**Key concept:** Multiple pins set simultaneously with `|` operator. `all_leds_off()` clears all three in one operation.  
**DIP switches:** RED, GRN, BLU → all ON

---

## MP-03 — Button Input

**What it does:** SW1 on PD2 toggles the Green LED on each press. Uses edge detection — responds to the falling edge only, not while held.  
**Key concept:** Buttons are active-low (pressed = LOW). Edge detection tracks previous and current state — ISR fires only on HIGH→LOW transition. Double-check debounce confirms the press is real.  
**DIP switch:** GRN → ON | MODE jumper → DIRECT

---

## MP-04 — LED + Button State Machine

**What it does:** SW1 (PD2) cycles through Red → Green → Blue → Red. SW2 (PD3) resets to Red from any state. Both buttons use edge detection.  
**Key concept:** A `state` variable tracks which LED is active. Incrementing state and wrapping at 2 implements a simple cyclic state machine. Two separate `prev/curr` pairs track each button independently.  
**DIP switches:** RED, GRN, BLU → ON | MODE jumper → DIRECT

---

## MP-05 — Timer CTC Blink

**What it does:** Blinks Green LED at precise frequencies (1Hz, 2Hz, 5Hz) using Timer1 CTC mode and an ISR. SW1 cycles through the three speeds. No `_delay_ms()` in the blink logic.  
**Key concept:** Timer1 counts in the background. When it reaches OCR1A it fires an interrupt and resets to 0. The ISR toggles the LED. `sei()` enables global interrupts — without it the ISR never fires.

**OCR1A formula:**
```
OCR1A = (F_CPU / (2 × Prescaler × Frequency)) - 1
```

| Frequency | Prescaler | OCR1A |
|---|---|---|
| 1 Hz | 1024 | 7811 |
| 2 Hz | 1024 | 3905 |
| 5 Hz | 1024 | 1562 |

**DIP switch:** GRN → ON | MODE jumper → DIRECT

---

## MP-06 — PWM Buzzer

**What it does:** SW1 cycles the buzzer through three audible tones (1000Hz, 2000Hz, 4000Hz) and off. Uses Timer2 CTC mode to generate the frequencies via ISR pin toggling.  
**Key concept:** Timer2 is 8-bit (max OCR2 = 255). Prescaler must be chosen so OCR2 fits inside 8 bits. `buzzer_off()` sets TCCR2 = 0 to fully stop the timer — just disabling the interrupt leaves the pin potentially stuck HIGH.

**OCR2 formula:**
```
OCR2 = (F_CPU / (2 × Prescaler × Frequency)) - 1
```

| Frequency | Prescaler | OCR2 |
|---|---|---|
| 1000 Hz | 64 | 124 |
| 2000 Hz | 64 | 61 |
| 4000 Hz | 64 | 30 |

**DIP switch:** BUZ → ON | MODE jumper → DIRECT

---

## MP-07 — UART Hello World

**What it does:** Sends "Hello from ATmega32!" to the PC terminal every second over UART at 9600 baud.  
**Key concept:** UART baud rate set via UBRR registers. `URSEL` bit must be set when writing UCSRC on ATmega32 (shares address with UBRRH). `uart_send_byte()` waits for the transmit buffer to empty before writing — prevents data corruption.

**Baud rate formula:**
```
UBRR = (F_CPU / (16 × BaudRate)) - 1
```
For 9600 baud: UBRR = 103

**Terminal settings:** COM port (CH340), 9600 baud, 8N1  
**DIP switches:** PD0 (RX) and PD1 (TX) → ON on UART Serial Unit block

---

## MP-08 — ADC Read Potentiometer

**What it does:** Reads the multi-turn potentiometer on PA0 and sends the ADC value (0–1023) to the PC terminal over UART every 500ms.  
**Key concept:** ADC converts analog voltage (0–5V) to a 10-bit digital value (0–1023). `ADSC` bit starts a conversion. Poll until `ADSC` clears — conversion complete. Read result from `ADC` macro (combines ADCL + ADCH automatically).

**Voltage from ADC:**
```
Voltage (V) = (ADC_value / 1023.0) × 5.0
```

**DIP switches:** None needed — potentiometer connects directly to PA0

---

## Key Rules Learned

| Rule | Why |
|---|---|
| `#define F_CPU` must be first line | `_delay_ms()` reads it at include time — wrong order = wrong timing |
| Enable DIP switch for each component | Components are physically disconnected without it |
| ISRs must be short | Long ISRs block other interrupts and miss events |
| `sei()` is required for any interrupt | Global interrupt flag must be enabled or no ISR ever fires |
| Active-low buttons: pressed = LOW | Pull-up resistors hold the pin HIGH when button is open |
| Timer2 is 8-bit, Timer1 is 16-bit | OCR2 max = 255, OCR1A max = 65535 — choose prescaler accordingly |
| `TCNT1 = 0` on speed change | Prevents one irregular period when OCR1A changes mid-count |
| `TCCR2 = 0` to stop buzzer | Fully stops the timer — disabling interrupt alone leaves pin state undefined |
