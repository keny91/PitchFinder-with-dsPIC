/**************************************************************************************************
* @file		Projects\AudioAdjuster\src\main.c
*
* Summary:
*         	main function for the audio adjuster project
*
* ToDo:
*     		none
*
* Originator:
*     		Andy Watt
*
* History:
* 			Version 1.00	20/04/2013	Andy Watt	Initial Version copied from Microchip Demo Project and modified 
*      		Version 1.01    28/04/2013	Andy Watt	Added filter and modulate function calls 
*      		Version 1.02    01/05/2013	Andy Watt	Added mode switching and low pass filter function calls 
*      		Version 1.03	07/05/2013	Andy Watt	Added transform function calls
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

//#define __DEBUG_OVERRIDE_INPUT
//#define __DEBUG_FILTERS
//#define __DEBUG_SHIFTERS
//#define __DEBUG_TRANSFORMS

#define FRAME_SIZE 				128
//#define UPPER_CARRIER_FREQ 		625	
//#define LOWER_CARRIER_FREQ 		62.5
//#define CARRIER_INC				62.5
//#define CARRIER_DEC				62.5
//#define PRESSED					1
//#define UNPRESSED				0
//
//Modes are used to change the way the device does things, pressing switch 1 changes the mode
//#define MODE_DO_NOTHING			0 //the device passes the audio straight through to the output
//#define MODE_BAND_PASS_FILTER	1 //the device uses the band pass filter to remove negative audio frequencies
//#define MODE_BAND_PASS_SHIFT	3 //the device band pass filters and shifts the audio frequencies
//#define MODE_LOW_PASS_FILTER	2 //the device uses the shifted low pass filter to remove negative audio frequencies
//#define MODE_LOW_PASS_SHIFT		4 //the device uses shifted low pass filters and shifts the audio frequencies
//#define MODE_FREQ_DOMAIN		5 //the device works on the audio signal in the frequency domain
//#define MODE_TOTAL				6

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






//int iMode = MODE_DO_NOTHING;
	/*	int iSwitch1Pressed = UNPRESSED;
	int iSwitch2Pressed = UNPRESSED;*/
//	int iShiftAmount = 1;
//	
//	float fCarrierFrequency = 1;
//	createComplexSignal(fCarrierFrequency,FRAME_SIZE,compCarrierSignal);
//	
//	#ifdef __DEBUG_OVERRIDE_INPUT
//		float debugFrequency;
//	#endif
//
/*	#ifdef __DEBUG_FILTERS//if in filter debug mode create a test signal
		debugFrequency = 0;
		createSimpleSignal(debugFrequency,FRAME_SIZE,frctAudioIn);
	#endif

	#ifdef __DEBUG_SHIFTERS//if in shifter debug mode create a constant test signal
		debugFrequency = 1250;
		createSimpleSignal(debugFrequency,FRAME_SIZE,frctAudioIn);		
	#endif

	#ifdef __DEBUG_TRANSFORMS//if in transform debug mode create a constant test signal
		debugFrequency = 1250;
		createSimpleSignal(debugFrequency,FRAME_SIZE,frctAudioIn);		
	#endif
	*/
/*	initFilter();*/

	ex_sask_init( );

	//Initialise Audio input and output function
	ADCChannelInit	(pADCChannelHandle,adcBuffer);			
	OCPWMInit		(pOCPWMHandle,ocPWMBuffer);			

	//Start Audio input and output function
	ADCChannelStart	(pADCChannelHandle);
	OCPWMStart		(pOCPWMHandle);	
	
//	short int led_color;
  //unsigned short int led_state;
//	static unsigned short int const LED_ON =SASK_LED_ON;
//	static unsigned short int const LED_OFF = SASK_LED_OFF;

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


		fourierTransform(FRAME_SIZE,compX,frctAudioIn);
	//	filterNegativeFreq(FRAME_SIZE,compXfiltered,compX);


		// should they be ints or 
		MaxFreqaValue = 0;
		MaxValue = 0;		
		i=0;
		module = 0;
		//fractional a;
		//int sizesss  = FRAME_SIZE;
	
		for(i = 1 ; i<64;i++){
			module = pow(compX[i].real,2) + pow( compX[i].imag,2);
			//module = compXfiltered[w].real;
			if(module > MaxValue){
				MaxFreqaValue = i;
				MaxValue = module;
			}
		}

		//LowInterval = (FRAME_SIZE/2)*(30/100); 
		//HighInterval = (FRAME_SIZE/2)*(66/100);
		//LowInterval = round(LowInterval);
			LowInterval = 20;
			HighInterval = 40;

// interval 1:
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
		else{
			YELLOW_LED = 1;
	        RED_LED = 1;
	        GREEN_LED = 1;
		}

/*
		while(OCPWMIsBusy(pOCPWMHandle));	
		//Write the real part of the frequency shifted complex audio signal to the output
		OCPWMWrite (pOCPWMHandle,frctAudioOut,FRAME_SIZE);*/
		
	}
}
