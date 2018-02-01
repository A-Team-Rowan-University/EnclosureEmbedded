# Lab 6: Open Loop Systems
#### Joshua Gould
##### Created - 11-25-2017
##### Updated - 11-26-2017

## Library Used
msp430.h - default library

## Dependencies
* MSP430
* Power Resistor
* 5V Regulator
* PWM Fan
* Phototransistor
* Multimeter and oscilloscope

## Set Requirements
"Focus on system modeling getting a system to a desirable state. For this lab, you will be attempting to keep a voltage regulator within a specific temperature range using a DC fan which you will have control over. For this part to be a success, you need to figure out what is the minimum fan speed you need to cool off the regulator so that is stays operational. You will need to take a 5V regulator from the back of the lab and drop the output across a 100 ohm power resistor (1 watt should do the trick). The input to the voltage regulator will be between 15-20V. It would be really useful to see what the temperature of your system is so you can determine the performance of your system. This can be done either by displaying the current temperature over a display, passing the information over UART, or other ways as well. Remember that UART is Asynchronous, meaning that you can send information whenever you would like from your controller back to a PC, it doesn't have to be reactionary."

### README
"Your README needs to contain schematics of your system, the plot of the plot of the temperature and input voltages at the 5C steps, and a brief talk about why you chose the processor you did along with the control technique for the fan. As always, you should include a brief description of the code. You need to also include information on how to use your control software, including what inputs it is expecting and what range of values you are expecting."

### System Modeling
"For starters, you need to figure out with your fan at MAX what the temperature that the voltage regulator reaches. Your thermistors/PTATs/Whatever else you want to use to measure temperature may not be calibrated, so your results may be consistently off with reality, but for now that is ok. After figuring this out, in increments of about 5C, see what fan speed you need to have to maintain that temperature. Do this until your regulator gets down to about 30C-40C, keeping a record of what your applied Duty Cycles or voltages were. Then using this information, attempt to find a transfer function between the applied input and the resulting temperature to model the system behavior. A simple way to do this is in MATLAB or Excel to plot your applied input on the x-axis, and your steady state temperature on your y-axis and attempt a line fit."

### Open Loop Control System
"You then need to use this information to make a final open loop control system where a user can state what temperature they want the regulator to be maintained at, and the microcontroller will basically calculate/look up what it needs to set the fan to. Do not over complicate this and make it some elaborate system. All this needs to do is some math and set a duty cycle or voltage, and display in some capacity the current temperature of the system as you are measuring it."

#### Tasks
* [x] Create circuit for heating/cooling
* [x] Read tempeture of system over UART using ADC
* [x] System temperature plot (5C steps)
* [x] Reason for processor choice
* [x] Control technique for fan
* [x] Code description
* [x] Model max temperature
* [x] Plot System
* [x] User can control temperature

## Circuit 
This system contains a few parts for it to work. Firstly, the MSP430FR5994 needs the code provided below to handle PWM with temperature readings over UART. When heating up the LM35, this circuit requires 15-20V running to a 5V regulator. In our regulator circuit we attached a 33 Ohm power resistor to increase heating speeds. As for the PWM, the negative lead of the fan connected to the PWM pin of the MSP430 through the gate of a MOSFET. 12 Volts ran across the MOSFET to current limit the pin. In this configuration we were able to completely turn off the fan and run the fan at 100% PWM over UART.

#### Picture
![alt text](https://github.com/RU09342/lab-6taking-control-over-your-embedded-life-gouldj/blob/master/Open%20Loop%20Systems/Open%20Loop%20Ciruit/Circuit%20Pictures/Circuit_top.jpg)
#### Tempeture Sense System
![alt text](https://github.com/RU09342/lab-6taking-control-over-your-embedded-life-gouldj/blob/master/Open%20Loop%20Systems/Open%20Loop%20Ciruit/Circuit%20Pictures/temp-system.jpg)
#### Fan PWM System
![alt text](https://github.com/RU09342/lab-6taking-control-over-your-embedded-life-gouldj/blob/master/Open%20Loop%20Systems/Open%20Loop%20Ciruit/Circuit%20Pictures/pwm-system.jpg)
#### Schematic
![alt text](https://github.com/RU09342/lab-6taking-control-over-your-embedded-life-gouldj/blob/master/Open%20Loop%20Systems/Open%20Loop%20Ciruit/OpenLoopCircuit.PNG)

## System Table

| Duty Cycle	| PWM (%) | TempC |
| :---: |:-------:| :----:|
| 0     | 0	      | >100	|
| 25    | 10	    | 54	  |
| 52    | 20	    | 43	  |
| 77    | 30	    | 39	  |
| 102    | 40	    | 38	  |
| 128   | 50	    | 37	|
| 153   | 60	    | 36	|
| 178   | 70	    | 34	|
| 204   | 80	    | 33	|
| 230   | 90	    | 32	|
| 255   | 100	    | 32	  |

## System Model
Modeling the system of this circuit required a relationship between temperature and PWM. PWM was incremented by 10% as the temperature was read over UART and compared to the LM35 voltage reading from a multimeter. Steady state was taken at each duty cycle

#### PWM (10% decrease) vs Temperature (C)
![Alt Text](https://github.com/RU09342/lab-6taking-control-over-your-embedded-life-gouldj/blob/master/Open%20Loop%20Systems/Open%20Loop%20Ciruit/TempvsPWMchart.PNG)
#### Temperature input 34 to 32
![Alt Text](https://github.com/RU09342/lab-6taking-control-over-your-embedded-life-gouldj/blob/master/Open%20Loop%20Systems/Open%20Loop%20Ciruit/PWM-TEMP-Pics/34%20to%2032.PNG)
#### Temperature input 36 o 34
![Alt Text](https://github.com/RU09342/lab-6taking-control-over-your-embedded-life-gouldj/blob/master/Open%20Loop%20Systems/Open%20Loop%20Ciruit/PWM-TEMP-Pics/36%20to%2034.PNG)
#### Temperature input 39 to 36
![Alt Text](https://github.com/RU09342/lab-6taking-control-over-your-embedded-life-gouldj/blob/master/Open%20Loop%20Systems/Open%20Loop%20Ciruit/PWM-TEMP-Pics/39%20to%2036.PNG)
#### Temperature input 43 to 39
![Alt Text](https://github.com/RU09342/lab-6taking-control-over-your-embedded-life-gouldj/blob/master/Open%20Loop%20Systems/Open%20Loop%20Ciruit/PWM-TEMP-Pics/43%20to%2039.PNG)
#### Temperature input 54 to 43
![Alt Text](https://github.com/RU09342/lab-6taking-control-over-your-embedded-life-gouldj/blob/master/Open%20Loop%20Systems/Open%20Loop%20Ciruit/PWM-TEMP-Pics/54%20to%2043.PNG)
#### Temperature input over 54
![Alt Text](https://github.com/RU09342/lab-6taking-control-over-your-embedded-life-gouldj/blob/master/Open%20Loop%20Systems/Open%20Loop%20Ciruit/PWM-TEMP-Pics/Over54.PNG)

## Code Functionality
This open loop will receive the temperature over UART and convert it to a duty cycle for the PWM of the fan. 
### MSP430FR5994
The MSP430FR5994 was chosen for its 12-Bit Analog-to-Digital Converter (ADC), Six 16-Bit Timers With up to Seven Capture/Compare Registers Each, and its ability to hold up to 20 External Input Channels. The integrated capacitor for offline power is not necessary in this lab, however it is useful for when your laptop dies and you miss some temperature measurements. Additionally, this MSP430 was experimented with in the previous milestone.
### GPIO Setup
Unlike other labs, two pins had to be enabled to handle ADC (P1.4 and P1.5) where each pin GPIO has been defined for each output and input.
```C
void Temp_GPIOInit()
{
	P1OUT &= ~BIT0;        // Clear LED to start
	P1DIR |= BIT0;         // P1.0 output
	P1SEL1 |= BIT5;        // Configure P1.5 for ADC
	P1SEL0 |= BIT5;
}
void Fan_GPIOInit(void)
{
	//For pin 1.4
	P1DIR |= BIT4;		//Pin 1.4
	P1SEL1 &= ~BIT4;	//control which functions will be connected or multiplexed onto the pins.  
	P1SEL0 |= BIT4;		//The higher four bits have as their function to enable JTAG or to disable it.
}
```

### Timers
Timers used in this code was separated for the UART and PWM Fan. Readings set by UART must be made at about once per second. PWM readings are manipulated with the UART interrupt however the PWM timer is required for initialization and mode.
```C
void TimerAInit(void)  //Timer used for UART
{
	TA0CCTL0 = CCIE;			//Disable timer Interrupt
	TA0CCTL1 = OUTMOD_3;			//Set/Reset when the timer counts to the TA0CCR1 value, reset for TA0CCR0
	TA0CCR1 = 256;
	TA0CCR0 = 4096 - 1;			//Set CCR0 for a ~1kHz clock.
	TA0CTL = TASSEL_1 + MC_1 + ID_3;	//Enable Timer A with SMCLK
}
void TimerBInit(void) //PWM Timer
{
	TB0CCTL1 = OUTMOD_3;			//Set OUTMOD_3 (set/reset) for CCR1
									//Set initial values for CCR1 (255 -> 254)
	TB0CCR1 = 0xFF;				//reset and set immediately (May change to slower clock)
	TB0CCR0 = 255 - 1;			//Set CCR0 for a ~1kHz clock.
	TB0CTL = TBSSEL_2 + MC_1;		//Enable Timer B0 with SMCLK and up mode. 1MHz
}
```
### UART Interrupt
The most important part of this lab is the UART interrupt. Here the received value over UART is transmitted to PWM values. Where the input changes from 32 to 54 over UART the temperature changes from 32 to 54. In order to control this, the fan will then change speed depending on the input.
```C
#pragma vector=EUSCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
	switch (__even_in_range(UCA0IV, USCI_UART_UCTXCPTIFG))
	{
	case USCI_NONE: break;
	case USCI_UART_UCRXIFG:
		while (!(UCA0IFG&UCTXIFG));

		tempC_set = UCA0RXBUF;             // send RX to tempC_set

		if (tempC_set <= 32)
		{
			PWM = 0xFF;		//set PWM to 100%
		}
		else if (tempC_set > 32 && tempC_set <= 34)	//change PWM range
		{
			PWM = ((tempC_set - 36.87) / -0.0917)
		}
		else if (tempC_set > 34 && tempC_set <= 36)
		{
			PWM = ((tempC_set - 48.24) / -0.08)
		else if (tempC_set > 36 && tempC_set <= 39)
			{
				PWM = ((tempC_set - 42.03) / -0.0394)
			}
		else if (tempC_set > 39 && tempC_set <= 43)
		{
			PWM = ((tempC_set - 50.85) / -0.1538)
		}
		else if (tempC_set > 43 && tempC_set <= 54)
		{
			PWM = ((tempC_set - 57.90) / -0.2759)
		}
		else if (tempC_set > 54)		//Basically this is the smallest PWM the fan can handle and the temp it sets to at 10%
		{
			PWM = 25;
		}

		TB0CCR1 = PWM;

		break;

	case USCI_UART_UCTXIFG: break;
	case USCI_UART_UCSTTIFG: break;
	case USCI_UART_UCTXCPTIFG: break;

	default:
		break;
	}
}
```
### ADC Interrupt
The ADC interrupt handles the ADC voltage reading from the LM35 and translates the value to a readable hex over UART.
```C
#pragma vector=ADC12_B_VECTOR
__interrupt void ADC12ISR(void)
{
	adc_in = ADC12MEM0;		    //set ADC12MEM to variable
	voltage = adc_in * 0.00029;	    //converts ADC to voltage
	tempC = voltage / 0.01;		     //converts voltage to Temp C
	tempF = ((9 * tempC) / 5) + 32;     //Temp C to Temp F
	while (!(UCA0IFG&UCTXIFG));
	UCA0TXBUF = tempC;		     //change to =tempF to output in Fahrenheit 
}
```
