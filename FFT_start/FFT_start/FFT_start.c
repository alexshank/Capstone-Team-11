/******************************************************************************
Alex Shank - 11/14/20

Program that computes the FFT of an array of 16 fixed-point integers using the
MSP430's LEA module.
*******************************************************************************/
#include <driverlib/MSP430FR5xx_6xx/driverlib.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "DSPLib.h"

#define SAMPLES             16

// input signal and FFT result
DSPLIB_DATA(input,MSP_ALIGN_FFT_Q15(SAMPLES))
_q15 input[SAMPLES];

// benchmark cycle counts
volatile uint32_t cycleCount;

void main(void)
{
    WDT_A_hold(WDT_A_BASE);             // disable watchdog timer
    PMM_unlockLPM5();                   // unlock the GPIO power-on default mode

    // create test input values
    int i;
    for(i = 0; i < SAMPLES; i++){
        input[i] = _Q15(0.002 * i);
    }
    
    msp_status status;

    // initialize the fft parameter structure
    msp_fft_q15_params fftParams;
    fftParams.length = SAMPLES;
    fftParams.bitReverse = true;
    fftParams.twiddleTable = NULL;      // can use NULL pointer when LEA being used

    // perform real FFT with fixed scaling
    msp_benchmarkStart(MSP_BENCHMARK_BASE, 16);
    status = MAP_msp_fft_fixed_q15(&fftParams, input);
    cycleCount = msp_benchmarkStop(MSP_BENCHMARK_BASE);
    msp_checkStatus(status);

    __no_operation();
}
