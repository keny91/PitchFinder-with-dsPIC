/**************************************************************************************************
* @file		Projects\PitchFinder\src\main.c
*
* Summary:
*         	main function for the PitchFinder
*
* ToDo:
*     		Find solution to the Flickering Issue
*
* Originator:
*     		Luis Herranz
*
* Version:
* 			Version 1.00	27/03/2016	Luis Herranz	Initial Version, clean
*
***************************************************************************************************/
//#include <p33FJ256GP506.h>
#include <board\h\sask.h>
#include <board\inc\ex_sask_generic.h>
#include <board\inc\ex_sask_led.h>
#include <dsp\h\dsp.h>
#include <peripherals\adc\h\ADCChannelDrv.h>
#include <peripherals\pwm\h\OCPWMDrv.h>
#include "..\inc\filter.h"
#include "..\inc\modulate.h"
#include "..\inc\complexmultiply.h"
#include "..\inc\transform.h"
#include <math.h>


#define FRAME_SIZE 				128

//Allocate memory for input and output buffers
fractional		adcBuffer		[ADC_CHANNEL_DMA_BUFSIZE] 	__attribute__((space(dma)));
fractional		ocPWMBuffer		[OCPWM_DMA_BUFSIZE]		__attribute__((space(dma)));

//variables for FFT
fractcomplex compx[FRAME_SIZE]__attribute__ ((space(ymemory),far));
fractcomplex compX[FRAME_SIZE]__attribute__ ((space(ymemory),far));
fractcomplex compXfiltered[FRAME_SIZE]__attribute__ ((space(ymemory),far));
fractcomplex compXshifted[FRAME_SIZE]__attribute__ ((space(ymemory),far));

//variables for audio processing
fractional		frctAudioIn			[FRAME_SIZE]__attribute__ ((space(xmemory),far));
fractional		frctAudioWorkSpace	[FRAME_SIZE]__attribute__ ((space(ymemory),far));
fractional		frctAudioOut		[FRAME_SIZE]__attribute__ ((space(xmemory),far));
fractcomplex	compAudioOut		[FRAME_SIZE]__attribute__ ((space(xmemory),far));
fractcomplex	compCarrierSignal	[FRAME_SIZE]__attribute__ ((space(ymemory),far));

//Instantiate the drivers
ADCChannelHandle adcChannelHandle;
OCPWMHandle 	ocPWMHandle;

//Create the driver handles
ADCChannelHandle *pADCChannelHandle 	= &adcChannelHandle;
OCPWMHandle 	*pOCPWMHandle 		= &ocPWMHandle;

int main(void)
{



	ex_sask_init( );

	//Initialise Audio input and output function
	ADCChannelInit	(pADCChannelHandle,adcBuffer);			
	OCPWMInit		(pOCPWMHandle,ocPWMBuffer);			

	//Start Audio input and output function
	ADCChannelStart	(pADCChannelHandle);
	OCPWMStart		(pOCPWMHandle);	
	


	int i=0;
	int module;
	int MaxFreqaValue ;
	int MaxValue;
	int LowInterval;
	int HighInterval;

	while(1)
	{		
		
		
	//	#ifndef __DEBUG_OVERRIDE_INPUT//if not in debug mode, read audio in from the ADC
			//Wait till the ADC has a new frame available
			while(ADCChannelIsBusy(pADCChannelHandle));
			//Read in the Audio Samples from the ADC
			ADCChannelRead	(pADCChannelHandle,frctAudioIn,FRAME_SIZE);
	//	#endif

	//	filter the negative part of the Fourier trasform
		fourierTransform(FRAME_SIZE,compX,frctAudioIn);
		filterNegativeFreq(FRAME_SIZE,compXfiltered,compX);


	//	Initialize the values for the Pitch detection
		MaxFreqaValue = 0;
		MaxValue = 0;		
		i=0;
		module = 0;

	//	Extract the highest value in the frequency domain and the index of the belonging step in the interval
		for(i = 1 ; i<64;i++){
			module = pow(compXfiltered[i].real,2) + pow( compXfiltered[i].imag,2);
			if(module > MaxValue){
				MaxFreqaValue = i;
				MaxValue = module;
			}
		}


		//	the frequency spectrum goes from 0 to 4kHz and its represented in a 64 step interval
			LowInterval = 20;
			HighInterval = 40;

		// Find the belonging interval of the Pitch
		if(LowInterval > MaxFreqaValue){
			YELLOW_LED = 1;
	        RED_LED = 1;
	        GREEN_LED = 0;
		}
		else if  (LowInterval <= MaxFreqaValue && HighInterval >= MaxFreqaValue){
			YELLOW_LED = 0;
	        RED_LED = 1;
	        GREEN_LED = 1;
		}
		else if(HighInterval < MaxFreqaValue && 64 >= MaxFreqaValue){
			YELLOW_LED = 1;
	        RED_LED = 0;
	        GREEN_LED = 1;
		}
/*		else{
			YELLOW_LED = 1;
	        RED_LED = 1;
	        GREEN_LED = 1;
		}*/

/*
		while(OCPWMIsBusy(pOCPWMHandle));	
		//Write the real part of the frequency shifted complex audio signal to the output
		OCPWMWrite (pOCPWMHandle,frctAudioOut,FRAME_SIZE);*/
		
	}
}
