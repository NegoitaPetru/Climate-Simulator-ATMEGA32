# **Summary**
The main purpose of the system is to simulate a controller of the temperature in a certain climate. It indicates through 2 LEDs the difference between the environmental temperature (Tm) and the target temperature (Tt). Given their lighting, the temperature would COOL, HEAT or remain the same.

# **Components needed:**
- EvB 5.1 ATMega32 board;
- 2 16x2 LCDs;
- 2 LEDs;
- 2 ADC potentiometers;
- wires.

# **How it works:**
The system detects through potentiometers (ADC) 2 digital signals that indicate *the environmental temperature* **(Tm)** and *the target temperature* **(Tt)**. Once received, the user can interact with 2 buttons that can display on two LCDs (16x2):

1. alternatively, the numeric values of Tm and Tt;
2. the type (state) of the Tm (the system must ... the climate):
	- COOL: Tm > Tt;
	- IDLE: Tm = Tt;
	- HEAT: Tm < Tt.

For **COOL** and **HEAT** the system will send 2 PWM signals on 2 LEDs (blue and red) given the following rules:

**COOL:**
1. if |Tm-Tt| < 5C then:
	- BLUE_LED 25% ED (romana: ED = factor de umplere);
	- RED_LED 75% ED.
2. if 5C <=|Tm-Tt| < 10C then:
	- BLUE_LED 50% ED;
	- RED_LED 50% ED;
3. if -10C <=|Tm-Tt| < 15C then:
	 BLUE_LED 75% ED;
	- RED_LED 25% ED;
4. if |Tm-Tt| >= 15C then:
	- BLUE_LED 100% ED;
	- RED_LED 0% ED;

**HEAT:**
1. if |Tm-Tt| < 5C then:
	- BLUE_LED 75% ED (romana: ED = factor de umplere);
	- RED_LED 25% ED.
2. if 5C <=|Tm-Tt| < 10C then:
	- BLUE_LED 50% ED;
	- RED_LED 50% ED;
3. if 10C <=|Tm-Tt| < 15C then:
	- BLUE_LED 25% ED;
	- RED_LED 75% ED;
4. if |Tm-Tt| >= 15C then:
	- BLUE_LED 0% ED;
	- RED_LED 100% ED;

if |Tm-Tt| < 5C then:
	- BLUE_LED 75% ED (romana: ED = factor de umplere);
	- RED_LED 25% ED.
2. if 5C <=|Tm-Tt| < 10C then:
	- BLUE_LED 50% ED;
	- RED_LED 50% ED;
3. if 10C <=|Tm-Tt| < 15C then:
	- BLUE_LED 25% ED;
	- RED_LED 75% ED;
4. if |Tm-Tt| >= 15C then:
	- BLUE_LED 0% ED;
	- RED_LED 100% ED;
 
 # Functional State Machine
![FSM drawio](https://github.com/user-attachments/assets/c725f2bb-28de-4968-8964-7576d731c193)
