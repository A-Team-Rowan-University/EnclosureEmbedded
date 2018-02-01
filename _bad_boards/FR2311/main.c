
#include <msp430fr2311.h>

#define CALADC12_15V_30C  *((unsigned int *)0x1A1A)   // Temperature Sensor Calibration-30 C
                                                      //See device datasheet for TLV table memory mapping
#define CALADC12_15V_85C  *((unsigned int *)0x1A1C)   // Temperature Sensor Calibration-85 C

unsigned int temp;
volatile float temperatureDegC;
volatile float temperatureDegF;

 int PWM = 0;

void main(void) {
    WDTCTL = WDTPW | WDTHOLD;               // Stop watchdog timer
    REFCTL0 &= ~REFMSTR;

    //ADC12_A ref control registers
    ADC12CTL0 = ADC12SHT0_8 + ADC12REFON + ADC12ON;

    ADC12CTL1 = ADC12SHP;                     // enable sample timer
    ADC12MCTL0 = ADC12SREF_1 + ADC12INCH_10;  // ADC i/p ch A10 = temp sense i/p
    ADC12IE = 0x001;                          // ADC_IFG upon conv result-ADCMEMO
    __delay_cycles(100);                       // delay to allow Ref to settle
    ADC12CTL0 |= ADC12ENC;

    P3SEL = BIT3 + BIT4;
    P1DIR |= BIT2;
    P1SEL |= BIT2;


    //UART
    UCA0CTL1 |= UCSWRST;
    // SMCLK
    UCA0CTL1 |= UCSSEL_2;
    // 1MHz 9600 baud
    UCA0BR0 = 6;
    // 1MHz 9600
    UCA0BR1 = 0;
    //sets m control register
    UCA0MCTL = UCBRS_0 + UCBRF_13 + UCOS16;
    //sets control register
    UCA0CTL1 &= ~UCSWRST;
    //enable interrupt
    UCA0IE |= UCRXIE;


    TA0CTL |= TASSEL_2 + MC_1 + TACLR; //1 MHz divided by 1
    TA0CCTL1 = OUTMOD_7; //toggle/reset
    TA0CCR0 = 1024; //25kHz

    while(1)
      {
        ADC12CTL0 &= ~ADC12SC;
        ADC12CTL0 |= ADC12SC;                   // Sampling and conversion start

        __bis_SR_register(LPM4_bits + GIE);     // LPM0 with interrupts enabled
        __no_operation();

        // Temperature in Celsius. See the Device Descriptor Table section in the
        // System Resets, Interrupts, and Operating Modes, System Control Module
        // chapter in the device user's guide for background information on the
        // used formula.
        temperatureDegC = (float)(((long)temp - CALADC12_15V_30C) * (85 - 30)) /
                (CALADC12_15V_85C - CALADC12_15V_30C) + 30.0f;

        // Temperature in Fahrenheit Tf = (9/5)*Tc + 32
        temperatureDegF = temperatureDegC * 9.0f / 5.0f + 32.0f;

        __no_operation();                       // SET BREAKPOINT HERE
      }
}

#pragma vector=USCI_A0_VECTOR
__interrupt void USCI_A0_ISR(void)
{
  switch(__even_in_range(UCA0IV,4))
  {
  case 0:break;   // Vector 0 - no interrupt
  case 2:{
      while (!(UCA0IFG&UCTXIFG));  // USCI_A0 TX buffer check


      PWM = UCA0RXBUF;
      // CCR1 PWM duty cycle
      TA0CCR1 = PWM * 4;
      UCA0TXBUF = temp;

      break;

  }

  case 4:break;    // Vector 4 - TXIFG
  default: break;
  }
}

#if defined(__TI_COMPILER_VERSION__) || defined(__IAR_SYSTEMS_ICC__)
#pragma vector=ADC12_VECTOR
__interrupt void ADC12ISR (void)
#elif defined(__GNUC__)
void __attribute__ ((interrupt(ADC12_VECTOR))) ADC12ISR (void)
#else
#error Compiler not supported!
#endif
{
  switch(__even_in_range(ADC12IV,34))
  {
  case  0: break;                           // Vector  0:  No interrupt
  case  2: break;                           // Vector  2:  ADC overflow
  case  4: break;                           // Vector  4:  ADC timing overflow
  case  6:                                  // Vector  6:  ADC12IFG0
    temp = ADC12MEM0;                       // Move results, IFG is cleared
    __bic_SR_register_on_exit(LPM4_bits);   // Exit active CPU
    break;
  case  8: break;                           // Vector  8:  ADC12IFG1
  case 10: break;                           // Vector 10:  ADC12IFG2
  case 12: break;                           // Vector 12:  ADC12IFG3
  case 14: break;                           // Vector 14:  ADC12IFG4
  case 16: break;                           // Vector 16:  ADC12IFG5
  case 18: break;                           // Vector 18:  ADC12IFG6
  case 20: break;                           // Vector 20:  ADC12IFG7
  case 22: break;                           // Vector 22:  ADC12IFG8
  case 24: break;                           // Vector 24:  ADC12IFG9
  case 26: break;                           // Vector 26:  ADC12IFG10
  case 28: break;                           // Vector 28:  ADC12IFG11
  case 30: break;                           // Vector 30:  ADC12IFG12
  case 32: break;                           // Vector 32:  ADC12IFG13
  case 34: break;                           // Vector 34:  ADC12IFG14
  default: break;
  }
}
