/*void ADC10Init(void);*/

//unsigned volatile int		adc_in 		= 0;


/*void ADC10Init(void)
{
	ADC10CTL0 = ADC10IE + ADC10SHT0 + ADC10ON + MSC;        // Set sample time for the ADC10 control register 0, turn ADC10 on
	ADC10CTL1 = ADC10SHT_0 + INCH3 + ADC10SSEL_0 + CONSEQ_2;  // Enable sample timer for the ADC10 control register 1
	ADC10AE0 |= BIT3; 
}*/

/*
//ADC ISR
#pragma vector=ADC10_VECTOR
__interrupt void ADC10ISR(void)
{
	adc_in = ADC10MEM;		    // Pull the result of the ADC conversation from the memory register
	
	voltage = adc_in * 0.00029;	    	//converts ADC to voltage
	tempC = voltage / 0.01;		     	//converts voltage to Temp C
	tempF = ((9 * tempC) / 5) + 32;     //Temp C to Temp F
	
	//check to see whether pin 2.0 or 2.1 is on
	// the purpose of these if statements is to receive two temperatures on a single ADC channel
	if ((P2OUT && 0x01) == 1) 
	{
		tempO = tempF;	//Store outside temperature from pin 2.0
		P2OUT &= ~BIT0; 
		P2OUT |= BIT1;	// Flip to pin 2.1
	}
	else
	if ((P2OUT && 0x02) == 1)
	{
		tempI = tempF;	//Store outside temperature from pin 2.0
		P2OUT &= ~BIT1;
		P2OUT |= BIT0;	// Flip to pin 2.0
	}
}
*/