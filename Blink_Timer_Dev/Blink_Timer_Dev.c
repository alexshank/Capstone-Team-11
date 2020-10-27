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
#include <MSP_Init.h>

// function prototypes (need to be moved to other files)
void refreshLEDs();


/*
 * interrupt variables
 */
volatile unsigned int LED_Refresh_Flag = 0;
volatile unsigned int LED_E_HOLD = 0;      // indicates string has been tuned
volatile unsigned int LED_A_HOLD = 0;
volatile unsigned int LED_D_HOLD = 0;
volatile unsigned int LED_G_HOLD = 0;
volatile unsigned int LED_B_HOLD = 0;
volatile unsigned int LED_e_HOLD = 0;


/*
 * program entry point
 */
void main (void)
{
    // initialize board, GPIO, timer modules, and interrupts
    WDT_A_hold(WDT_A_BASE);   // disable watchdog timer
    PMM_unlockLPM5();                           // unlock the GPIO power-on default mode
    initializeRedAndGreenLEDs();
    initializeTimerA0();                        // LED timer
    initializeTimerA1();                        // PWM timer
    initializeLEDOutputs();                     // LEDs indicating string is in tune
    initializeButtonInputs();
    __enable_interrupt();                       // enable all configured interrupts
    
    // continuously check if ISR flags have been set
    while(true){
        if(LED_Refresh_Flag == 1) refreshLEDs();
        
        //P5OUT |= BIT7;
    }
}

void refreshLEDs(){
    GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_toggleOutputOnPin(GPIO_PORT_P1, GPIO_PIN1);
    
    
    //P5OUT ^= BIT7;
    LED_Refresh_Flag = 0;
}

/*
 * Timer A0 interrupt service routine (ISR)
 */
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A0(void){
    LED_Refresh_Flag = 1;
}

/*
 * ISR for button presses
 */
#pragma vector = PORT3_VECTOR
__interrupt void Port_3(void){
    P5OUT ^= BIT7;
    P3IFG &= ~BIT7 ;    // clear interrupt
    _NOP();
}

