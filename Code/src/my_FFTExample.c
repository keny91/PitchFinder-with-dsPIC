/**********************************************************************
* © 2005 Microchip Technology Inc.
*
* FileName:        main_FFTExample.c
* Dependencies:    Header (.h) files if applicable, see below
* Processor:       dsPIC30Fxxxx
* Compiler:        MPLAB® C30 v3.00 or higher
* IDE:             MPLAB® IDE v7.52 or later
* Dev. Board Used: dsPICDEM 1.1 Development Board
* Hardware Dependencies: None
*
* SOFTWARE LICENSE AGREEMENT:
* Microchip Technology Incorporated ("Microchip") retains all ownership and 
* intellectual property rights in the code accompanying this message and in all 
* derivatives hereto.  You may use this code, and any derivatives created by 
* any person or entity by or on your behalf, exclusively with Microchip's
* proprietary products.  Your acceptance and/or use of this code constitutes 
* agreement to the terms and conditions of this notice.
*
* CODE ACCOMPANYING THIS MESSAGE IS SUPPLIED BY MICROCHIP "AS IS".  NO 
* WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT NOT LIMITED 
* TO, IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY AND FITNESS FOR A 
* PARTICULAR PURPOSE APPLY TO THIS CODE, ITS INTERACTION WITH MICROCHIP'S 
* PRODUCTS, COMBINATION WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION. 
*
* YOU ACKNOWLEDGE AND AGREE THAT, IN NO EVENT, SHALL MICROCHIP BE LIABLE, WHETHER 
* IN CONTRACT, WARRANTY, TORT (INCLUDING NEGLIGENCE OR BREACH OF STATUTORY DUTY), 
* STRICT LIABILITY, INDEMNITY, CONTRIBUTION, OR OTHERWISE, FOR ANY INDIRECT, SPECIAL, 
* PUNITIVE, EXEMPLARY, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, FOR COST OR EXPENSE OF 
* ANY KIND WHATSOEVER RELATED TO THE CODE, HOWSOEVER CAUSED, EVEN IF MICROCHIP HAS BEEN 
* ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE.  TO THE FULLEST EXTENT 
* ALLOWABLE BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO 
* THIS CODE, SHALL NOT EXCEED THE PRICE YOU PAID DIRECTLY TO MICROCHIP SPECIFICALLY TO 
* HAVE THIS CODE DEVELOPED.
*
* You agree that you are solely responsible for testing the code and 
* determining its suitability.  Microchip has no obligation to modify, test, 
* certify, or support the code.
*
* REVISION HISTORY:
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* Author            Date      Comments on this revision
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
* HV               09/30/05  First release of source file
*
*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
*
* ADDITIONAL NOTES:
*
*
**********************************************************************/
#include <p33FJ256GP506.h>
#include <dsp.h>
#include "E:\Master\DigitalSignalProcessing\Day6\Source_Tree\Source_Tree\user\apps\FFT\CE018_FFT_DSPlib\h\fft.h"

_FGS(GWRP_OFF & GCP_OFF);
_FOSCSEL(FNOSC_FRC);
_FOSC(FCKSM_CSECMD & OSCIOFNC_ON & POSCMD_NONE);
_FWDT(FWDTEN_OFF);


fractcomplex sigCmpx2[FFT_BLOCK_LENGTH];

fractcomplex twiddleFactors[FFT_BLOCK_LENGTH/2];

int	peakFrequencyBin = 0;				/* Declare post-FFT variables to compute the */
unsigned long peakFrequency = 0;			/* frequency of the largest spectral component */

int main(void)
{
	int i = 0;
	fractional *p_real = &sigCmpx2[0].real ;
	fractcomplex *p_cmpx = &sigCmpx2[0] ;

	TwidFactorInit (LOG2_BLOCK_LENGTH, &twiddleFactors[0], 0);	/* We need to do this only once at start-up */

	for ( i = 0; i < FFT_BLOCK_LENGTH; i++ )/* The FFT function requires input data */
	{					/* to be in the fractional fixed-point range [-0.5, +0.5]*/
		*p_real = *p_real >>1 ;		/* So, we shift all data samples by 1 bit to the right. */
		*p_real++;			/* Should you desire to optimize this process, perform */
	}					/* data scaling when first obtaining the time samples */
						/* Or within the BitReverseComplex function source code */

	p_real = &sigCmpx2[(FFT_BLOCK_LENGTH/2)-1].real ;	/* Set up pointers to convert real array */
	p_cmpx = &sigCmpx2[FFT_BLOCK_LENGTH-1] ; /* to a complex array. The input array initially has all */
						/* the real input samples followed by a series of zeros */


	for ( i = FFT_BLOCK_LENGTH; i > 0; i-- ) /* Convert the Real input sample array */
	{					/* to a Complex input sample array  */
		(*p_cmpx).real = (*p_real--);	/* We will simpy zero out the imaginary  */
		(*p_cmpx--).imag = 0x0000;	/* part of each data sample */
	}

	/* Perform FFT operation */
#ifndef FFTTWIDCOEFFS_IN_PROGMEM
	FFTComplexIP (LOG2_BLOCK_LENGTH, &sigCmpx2[0], &twiddleFactors[0], COEFFS_IN_DATA);
#else
	FFTComplexIP (LOG2_BLOCK_LENGTH, &sigCmpx2[0], (fractcomplex *) __builtin_psvoffset(&twiddleFactors[0]), (int) __builtin_psvpage(&twiddleFactors[0]));
#endif

	/* Store output samples in bit-reversed order of their addresses */
	BitReverseComplex (LOG2_BLOCK_LENGTH, &sigCmpx2[0]);

	/* Compute the square magnitude of the complex FFT output array so we have a Real output vetor */
	SquareMagnitudeCplx(FFT_BLOCK_LENGTH, &sigCmpx2[0], &sigCmpx2[0].real);

	/* Find the frequency Bin ( = index into the sigCmpx2[] array) that has the largest energy*/
	/* i.e., the largest spectral component */
	VectorMax(FFT_BLOCK_LENGTH/2, &sigCmpx2[0].real, &peakFrequencyBin);

	/* Compute the frequency (in Hz) of the largest spectral component */
	peakFrequency = peakFrequencyBin*(SAMPLING_RATE/FFT_BLOCK_LENGTH);

        while (1);	/* Place a breakpoint here and observe the watch window variables */
}
