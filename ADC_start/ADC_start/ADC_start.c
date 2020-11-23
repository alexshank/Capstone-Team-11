/******************************************************************************
Alex Shank - 11/07/20

Program to sample an analog input pin using the MSP430's on-board ADC module.
*******************************************************************************/
 
// header files
#include <driverlib/MSP430FR5xx_6xx/driverlib.h>

// constants and variables
#define FFT_SIZE 1024
int ADC_values[FFT_SIZE] = {0};
int ADC_counter = 0;
int ADC_value = 0xFFFF; // initialize to value that should not be obtainable (for debugging)

void main (void)
{
    // initialize board, GPIO, timer modules, and interrupts
    WDT_A_hold(WDT_A_BASE);         // disable watchdog timer
    PMM_unlockLPM5();               // unlock the GPIO power-on default mode
    
    // LED configuration
    P1DIR |= BIT0;      // set as output
    P1OUT |= BIT0;      // set output high
    P1DIR |= BIT1;      // set as output
    P1OUT |= BIT1;      // set output high
    
    // configure ADC on P1.5 (A5)
    ADC12CTL0 |= BIT9 + BIT4;                               // ADC on, sample and hold for 16 ACLK cycles (affects sampling rate?)
    ADC12CTL1 |= BITD + BIT9 + BIT7 + BIT6 + BIT5 + BIT2;   // predivide by 4, source clock is sample timer, divide by 4, ACLK selected
                                                            //  (32768 / 4 / 2 = Fs = 2048 sps), (default single-sample-single-channel)
    ADC12CTL2 |= BIT5;                                      // 12 bit conversion
    ADC12IER0 |= BIT0;                                      // interrupt on MEM0
    ADC12MCTL0 |= BIT2 + BIT0;                              // A5 source
    __enable_interrupt();
    
    // delay for short period to allow voltages to settle
    int k = 0;
    while(k < 500){
        _NOP();
        k++;
    }
    
    // reset ADC interrupts each time a sample is taken
    while(ADC_counter < FFT_SIZE){
        ADC12CTL0 |= BIT1 + BIT0; // enable conversion, start conversion
        _NOP();
    }
    
    // let program run indefinitely
    while(true);
    
}

// ADC ISR for handling each sample
#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void){
    switch(__even_in_range(ADC12IV, ADC12IV_ADC12RDYIFG)){
        case ADC12IV_ADC12IFG0:
            ADC_value = ADC12MEM0;
            ADC_values[ADC_counter] = ADC_value;
            ADC_counter++;
            _NOP();
            P1DIR ^= BIT1;      // toggle output
            break;
        default:
            _NOP();
            break;
    }
}

