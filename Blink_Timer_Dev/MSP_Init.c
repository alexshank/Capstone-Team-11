#include <driverlib/MSP430FR5xx_6xx/driverlib.h>

#define ACLK_FREQUENCY           32768       // Hz
#define TIMER_A0_PERIOD          0.5             // seconds
#define TIMER_A0_DUTY_CYCLE      0.5          // percent
#define TIMER_A1_PERIOD          0.0025             // seconds
#define TIMER_A1_DUTY_CYCLE      0.81          // percent (gies > 1.5ms pulse)

/* 
 * setup alternating onboard red and green leds
 */
void initializeRedAndGreenLEDs(){
    // set pin P1.0 to output direction and turn red LED on
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    GPIO_setOutputHighOnPin(GPIO_PORT_P1, GPIO_PIN0);

    // set pin P1.1 to output direction and turn green LED off
    GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN1);
    GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN1); 
}

/* 
 * setup 6 LEDs that represent if string is tuned
 */
void initializeLEDOutputs(){
    // E string
    //GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN7);
    //GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN7);
    
    P5OUT &= ~BIT7;     // set output low to start
    P5DIR |= BIT7;      // set as output pin
    P5SEL0 &= ~BIT7;    // set pin to regular GPIO function
    P5SEL1 &= ~BIT7;
    
    /*
    // A string
    GPIO_setAsOutputPin(GPIO_PORT_P4, GPIO_PIN4);
    GPIO_setOutputHighOnPin(GPIO_PORT_P4, GPIO_PIN4);
    // D string
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN3);
    GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN3);
    // G string
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN0);
    GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN0);
    // B string
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN1);
    GPIO_setOutputHighOnPin(GPIO_PORT_P5, GPIO_PIN1);
    // e string
    GPIO_setAsOutputPin(GPIO_PORT_P8, GPIO_PIN3);
    GPIO_setOutputHighOnPin(GPIO_PORT_P8, GPIO_PIN3);*/
}

/* 
 * setup 6 buttons that initialize tuning the strings
 */
void initializeButtonInputs(){
    // E string button
    /*GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P5, GPIO_PIN5);
    GPIO_enableInterrupt(GPIO_PORT_P5, GPIO_PIN5);
    GPIO_selectInterruptEdge(GPIO_PORT_P5, GPIO_PIN5, GPIO_HIGH_TO_LOW_TRANSITION);
    GPIO_clearInterrupt(GPIO_PORT_P5, GPIO_PIN5);*/
    P3REN |= BIT7;      // will have pullup or pulldown resistor
    P3DIR &= ~BIT7;     // input
    P3OUT &= ~BIT7;      // pulldown resistor
    P3IES &= ~BIT7;      // low to high interrupt
    P3IFG &= ~BIT7;     // clear interrupt flag
    P3IE |= BIT7;       // enable the interrupt
}

/*
 * setup Timer A0 and use period for LED blinking
 */
void initializeTimerA0(){
// set up the TA0 timer in UP mode with ACLK (32768 Hz) and no divide
    TA0CTL |= BIT4 + BIT8;

    // set CCIE bit to enabled interrupt for CCR0
    TA0CCTL0 |= BIT4;

    // set the RESET/SET bits for OUTMOD in TA1CCTL1 register
    TA0CCTL1 |= BIT7 + BIT6 + BIT5;

    // use TA1CCR0 comparator register set PWM period
    TA0CCR0 = (int) (ACLK_FREQUENCY * TIMER_A0_PERIOD);

    // use TA1CCR1 comparator register to set PWM duty cycle
    TA0CCR1 = (int) (ACLK_FREQUENCY * TIMER_A0_PERIOD * TIMER_A0_DUTY_CYCLE);
    
    // do not send the PWM output anywhere
}

/*
 * setup Timer A1 and send PWM output to P1.2
 */
void initializeTimerA1(){
    // set up the TA1 timer in UP mode with ACLK (32768 Hz) and no divide
    TA1CTL |= BIT4 + BIT8;

    // set the RESET/SET bits for OUTMOD in TA1CCTL1 register
    TA1CCTL1 |= BIT7 + BIT6 + BIT5;

    // use TA1CCR0 comparator register set PWM period
    TA1CCR0 = (int) (ACLK_FREQUENCY * TIMER_A1_PERIOD);

    // use TA1CCR1 comparator register to set PWM duty cycle
    TA1CCR1 = (int) (ACLK_FREQUENCY * TIMER_A1_PERIOD * TIMER_A1_DUTY_CYCLE);

    // configure P1.2 to output TA0.1 in primary mode
    P1SEL0 |= BIT2;         					// set pin to use primary module
    P1SEL1 &= ~BIT2;
    P1DIR |= BIT2;          					// set P1.2 as output
}