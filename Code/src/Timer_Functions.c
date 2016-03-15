/*****f*        examples/Timer/Timer_Functions/Timer_Functions.c
  * Usage:
  *       Execute using MPLAB IDE
  * Summary:
  *         Simple loop to time blinking of LED using user timer function
  *
  * Inputs:
  *       none
  * Outputs:
  *       Blinking LED

  * Dependencies:
  *      Sask_Init()
  *		__delay32()
  * Notes:
  *		Demonstrates simple timer using a user timer function.
  *		First of a series of examples that includes Timer_Wait and Timer_Interrupt

  * ToDo:
  *     none

  * Originator:
  *     Chris Bore, BORES Signal processing, chris@bores.com, www.bores.com

  * History:
  *      Version 1.00     13/03/2013
  *****/


#include <p33FJ256GP506.h>
#include <board\h\sask.h>

#include <board\inc\ex_sask_generic.h>
#include <board\inc\ex_sask_led.h>
#include <peripherals\timers\inc\ex_timer.h>

#include <peripherals\adc\h\ADCChannelDrv.h>
#include <peripherals\pwm\h\OCPWMDrv.h>

#include "..\inc\ex_audio_process.h"

#define FRAME_SIZE        512


int   adcBuffer   [ADC_CHANNEL_DMA_BUFSIZE]   __attribute__((space(dma)));
int   ocPWMBuffer   [OCPWM_DMA_BUFSIZE]   __attribute__((space(dma)));
int     AudioIn [FRAME_SIZE], AudioWorkSpace[ FRAME_SIZE ], AudioOut [FRAME_SIZE];






/////////
short int led_color;
  //unsigned short int led_state;
static unsigned short int const LED_ON =SASK_LED_ON;
static unsigned short int const LED_OFF = SASK_LED_OFF;
float instruction_clock_frequency, instruction_cycle_time;
float wait_time;

ADCChannelHandle adcChannelHandle;
OCPWMHandle   ocPWMHandle;

ADCChannelHandle *pADCChannelHandle   = &adcChannelHandle;
OCPWMHandle   *pOCPWMHandle     = &ocPWMHandle;



int main(void)
{


  ADCChannelInit  (pADCChannelHandle,adcBuffer);
  OCPWMInit   (pOCPWMHandle,ocPWMBuffer);

  ADCChannelStart (pADCChannelHandle);
  OCPWMStart    (pOCPWMHandle); 

	
	ex_sask_init( );

	instruction_clock_frequency = 120e6;
	instruction_cycle_time = 1.0 / instruction_clock_frequency;

	ex_timer_init( instruction_cycle_time );
	 
	//led_color = GREEN;
	wait_time = 0.5;
	int i = 0;

	while(1)
	{

    
		//led_switch( led_color );
    if(CheckSwitchS1() == 1){
      if(i == 0)
        i = 2;
      else
        i=i-1; 
    }

    if(CheckSwitchS2() == 1){
      if(i == 2)
        i = 0;
      else
        i++; 
    }
      
    switch(i){
      case 0:
        YELLOW_LED = LED_OFF;
        RED_LED = LED_OFF;
        GREEN_LED = LED_ON;

      break;


      case 1:
        YELLOW_LED = LED_ON;
        RED_LED = LED_OFF;
        GREEN_LED = LED_OFF;
      break;

    
      case 2:

		    YELLOW_LED = LED_OFF;
        RED_LED = LED_ON;
        GREEN_LED = LED_OFF;
      break;
      
      default:
      break;
      }




    while(ADCChannelIsBusy(pADCChannelHandle));
    ADCChannelRead  (pADCChannelHandle,AudioIn,FRAME_SIZE);
  
    ex_audio_process( FRAME_SIZE, AudioIn, AudioWorkSpace, AudioOut );

    while(OCPWMIsBusy(pOCPWMHandle)); 
    OCPWMWrite (pOCPWMHandle,AudioOut,FRAME_SIZE);



    }


		ex_timer_wait( wait_time );
	}			

