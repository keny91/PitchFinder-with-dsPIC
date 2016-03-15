/*****f*        user\apps\audio\Audio_Pipe\Audio_Pipe.c
  * Usage:
  *       Execute using MPLAB IDE
  * Summary:
  *         Simple audio pipeline based on DMA in and out of the audio codec
  *
  * Inputs:
  *       Audio codec input
  * Outputs:
  *       Audio codec output

  * Dependencies:
  *      Sask_Init()
  *		__delay32()
  * Notes:
  *		none

  * ToDo:
  *     none

  * Originator:
  *     Chris Bore, BORES Signal processing, chris@bores.com, www.bores.com

  * History:
  *      Version 1.00     13/03/2013
  *****/

#include <p33FJ256GP506.h>
#include <board\h\sask.h>
#include <peripherals\adc\h\ADCChannelDrv.h>
#include <peripherals\pwm\h\OCPWMDrv.h>

#include <board\inc\ex_sask_generic.h>
#include <board\inc\ex_sask_led.h>

#include "..\inc\ex_audio_process.h"

#define FRAME_SIZE 				512

int		adcBuffer		[ADC_CHANNEL_DMA_BUFSIZE] 	__attribute__((space(dma)));
int		ocPWMBuffer		[OCPWM_DMA_BUFSIZE]		__attribute__((space(dma)));

int 		AudioIn	[FRAME_SIZE], AudioWorkSpace[ FRAME_SIZE ], AudioOut [FRAME_SIZE];

int			i;

ADCChannelHandle adcChannelHandle;
OCPWMHandle 	ocPWMHandle;

ADCChannelHandle *pADCChannelHandle 	= &adcChannelHandle;
OCPWMHandle 	*pOCPWMHandle 		= &ocPWMHandle;

int main(void)
{
	ex_sask_init( );

	ADCChannelInit	(pADCChannelHandle,adcBuffer);
	OCPWMInit		(pOCPWMHandle,ocPWMBuffer);

	ADCChannelStart	(pADCChannelHandle);
	OCPWMStart		(pOCPWMHandle);	

	while(1)
	{
		while(ADCChannelIsBusy(pADCChannelHandle));
		ADCChannelRead	(pADCChannelHandle,AudioIn,FRAME_SIZE);
	
		ex_audio_process( FRAME_SIZE, AudioIn, AudioWorkSpace, AudioOut );

		while(OCPWMIsBusy(pOCPWMHandle));	
		OCPWMWrite (pOCPWMHandle,AudioOut,FRAME_SIZE);
	}
}
