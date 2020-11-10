/*
 * The Ohio State University
 * ECE 4900 - Team 11
 * Alex Shank (.125)
 * 
 * This program implements logic needed to control the Automatic Guitar Tuner
 * created by Capstone Team 11 for ECE 4900.
 */
 
/*
 * header Files
 */
#include <driverlib/MSP430FR5xx_6xx/driverlib.h>

int ADC_values[512] = {0};
int ADC_counter = 0;
const int ADC_COUNT = 512;
int ADC_value = 0x05;


/*
 * program entry point
 */
void main (void)
{
    // initialize board, GPIO, timer modules, and interrupts
    WDT_A_hold(WDT_A_BASE);   // disable watchdog timer
    PMM_unlockLPM5();                           // unlock the GPIO power-on default mode
    
    // LED configuration
    P1DIR |= BIT0;      // set as output
    P1OUT |= BIT0;      // set output high
    P1DIR |= BIT1;      // set as output
    P1OUT |= BIT1;      // set output high
    
    // set P1.5 to A5 module?
    
    // configure reference voltage for ADC
    REFCTL0 |= BIT0;    // reference on, 1.2V by default
    
    // configure ADC on P1.5 (A5)
    ADC12CTL0 |= BIT9 + BIT4;           // ADC on, sample and hold for 16 ACLK cycles (affects sampling rate?)
    ADC12CTL1 |= BITD + BIT9 + BIT6 + BIT5 + BIT2;    // predivide by 4, source clock is sample timer, divide by 4, ACLK selected
                                                        //  (32768 / 4 / 2 = Fs = 2048 sps), (default single-sample-single-channel)
                                                        // TODO may need to be single-channel-repeat sample?
    ADC12CTL2 |= BIT5;           // 12 bit conversion
    ADC12IER0 |= BIT0;                  // interrupt on MEM0
    ADC12MCTL0 |= BIT8 + BIT2 + BIT0;   // 0-1.2V reference select, A5 source
    __enable_interrupt();
    
    // TODO don't think I should have to reset these everytime
    while(ADC_counter < ADC_COUNT){
        ADC12CTL0 |= BIT1 + BIT0; // enable conversion, start conversion
        _NOP();
    }
    
    while(true);
    
}

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

