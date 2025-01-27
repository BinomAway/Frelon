/**
  ******************************************************************************
  * @file    audio_application.c
  * @author  SRA
  * @version v1.3.0
  * @date    17-Sep-2021
  * @brief   Audio  application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "audio_application.h"
#include "FIR_IIR.h"
#include <math.h>
arm_rfft_fast_instance_f32 fft_handler;
/** @addtogroup X_CUBE_MEMSMIC1_Applications
  * @{
  */

/** @addtogroup Microphones_Acquisition
  * @{
  */

/** @defgroup AUDIO_APPLICATION
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/** @defgroup AUDIO_APPLICATION_Exported_Variables
  * @{
  */
#define PDM_Buffer_size  ((((AUDIO_IN_CHANNELS * AUDIO_IN_SAMPLING_FREQUENCY) / 1000) * MAX_DECIMATION_FACTOR) / 16)* N_MS
int16_t PDM_Buffer[PDM_Buffer_size];
#define PCM_Buffer_size ((AUDIO_IN_CHANNELS * AUDIO_IN_SAMPLING_FREQUENCY) / 1000)  * N_MS
int16_t PCM_Buffer[PCM_Buffer_size];
#define AMP_MAX 65536  // PCM values are between 0 and 65535, 32768 represents 0.0f

// #define USE_BLOCKER
#ifdef USE_BLOCKER
	#define Blocked_size 96
	int16_t PCM_Buffer_blocked[Blocked_size];
	// make sure USBD_AUDIO_FREQ=48000 in usbdconf.h
	// make sure AUDIO_IN_SAMPLING_FREQUENCY= 8000 in cca02m2_conf.h
#endif

	// if you dont use blocker, for default behavior :
	// make sure USBD_AUDIO_FREQ=48000 in usbdconf.h
	// make sure AUDIO_IN_SAMPLING_FREQUENCY=48000 in cca02m2_conf.h

// #define USE_OLD_ALGO

CCA02M2_AUDIO_Init_t MicParams;
AGC_s agc;

/**
  * @}
  */

/** @defgroup AUDIO_APPLICATION_Private_Variables
  * @{
  */
/* Private variables ---------------------------------------------------------*/
/**
  * @}
  */

/** @defgroup AUDIO_APPLICATION_Exported_Function
  * @{
  */

/**
  * @brief  Half Transfer user callback, called by BSP functions.
  * @param  None
  * @retval None
  */
void CCA02M2_AUDIO_IN_HalfTransfer_CallBack(uint32_t Instance)
{
  UNUSED(Instance);
	#ifndef USE_BLOCKER
	  AudioProcess();
	#endif
	#ifdef USE_BLOCKER
	  //Send_Audio_to_USB((int16_t *)PCM_Buffer, PCM_Buffer_size);
	  //Send_Audio_to_USB((int16_t *)PCM_Buffer_blocked, Blocked_size); //2*48
	#endif
}

/**
  * @brief  Transfer Complete user callback, called by BSP functions.
  * @param  None
  * @retval None
  */


void CCA02M2_AUDIO_IN_TransferComplete_CallBack(uint32_t Instance)
{

  UNUSED(Instance);
	#ifndef USE_BLOCKER
	  AudioProcess();
	#endif
  //Send_Audio_to_USB((int16_t *)PCM_Buffer, PCM_Buffer_size); //2*48

	#ifdef USE_BLOCKER

	// recopie d'une valeur d'un facteur decim pour atteindre la même taille de buffer et match la samplerate de l'USB
	  int i,j;
	  for(i=0; i<8*2; i+=2){
		  int decim = 6;
		  for(j=0; j<decim*2; j+=2){
			 PCM_Buffer_blocked[j+i*decim+0] = PCM_Buffer[i+0];
			 PCM_Buffer_blocked[j+i*decim+1] = PCM_Buffer[i+1];
		  }
	  }
	  Send_Audio_to_USB(PCM_Buffer_blocked, Blocked_size); //2*48
	 #endif

}//CCA02M2_AUDIO_IN_TransferComplete_CallBac

/**
  * @brief  User function that is called when 1 ms of PDM data is available.
  *       In this application only PDM to PCM conversion and USB streaming
  *                  is performed.
  *       User can add his own code here to perform some DSP or audio analysis.
  * @param  none
  * @retval None
  */





void AudioProcess(void)
{
	#ifdef USE_OLD_ALGO
		static float32_t buff[AUDIO_SIZE];
		static int cpt = 0;

		static float32_t out_agc[AUDIO_SIZE];
		static float32_t out_fft[AUDIO_SIZE];
		float32_t amplitude[LENGTHYIN];
		float frequencies[LENGTHYIN];
		int iPk[200];
		int nbPeaks = 0;
		int detect = -1;
	#endif
/////////////////////////////////////////////////////////////////////////////////////////////////

	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	int i = 0, j = 0;
	// treating buffer (normally 48 samples for 1 ms
	// sending IN to left channel, and OUT to right channel
	// PCM_Buffer[i] is left channel // PCM_Buffer[i+1] is right channel
	for(i=0; i<PCM_Buffer_size; i+=2){

		//PCM_Buffer[i+1] = FIR_filter(PCM_Buffer[i], 4);
		PCM_Buffer[i+1] = (int16_t) IIR_filter((float) PCM_Buffer[i]);
	}

	// envoi sur USB du buffer PCM_buffer modifié
	Send_Audio_to_USB((int16_t *)PCM_Buffer, PCM_Buffer_size);

	int acc=0; // should rather use a global acc and use a temporal decay
	//buildup time, max saturation value, decay time
	for(i=0; i<PCM_Buffer_size; i+=2){
		acc+=PCM_Buffer[i+1];
	}
	int threshold = 50000;
	if(acc > threshold){ HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET); }

  if (CCA02M2_AUDIO_IN_PDMToPCM(CCA02M2_AUDIO_INSTANCE, (uint16_t *)PDM_Buffer, PCM_Buffer) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }



/////////////////////////////////////////////////////////////////////////////////////////////////
	#ifdef USE_OLD_ALGO

	//Tant qu'on a pas récupéré un échantillon complet

	  if(cpt < AUDIO_SIZE-1)
		{

		  for(i=cpt; i < cpt+PCM_Buffer_size; i++)
		  {
			  buff[i] = PCM_Buffer[j];
			  j++;


		  }
		  cpt += PCM_Buffer_size;

		}
		else
		{
			//On fait passer notre échantillon dans l'agc
			AGC(&agc, buff, out_agc,10, AUDIO_SIZE); // AUDIO_SIZE=2048 cf .h

			//On applique la fft sur la sortie de l'agc et on récupère le spectre d'amplitude
			arm_rfft_fast_f32(&fft_handler,out_agc,out_fft,1);
			arm_cmplx_mag_f32(out_fft, amplitude, LENGTHYIN);


			for(i = 0; i < LENGTHYIN; i++) {
				  frequencies[i] = (float)i/2048*8000 ;	//2048 et 8000 car on ne peut pas multiplier les variables des defines de AUDIO_SIZE et AUDIO_IN_SAMPLING_FREQUENCY

				//AUDIO_SIZE = 2048
				//AUDIO_IN_SAMPLING_FREQUENCY = 8000 // pourquoi vous ne pouvez pas faire de multiplication avec ?? c'est pas un problème de division entière plutôt ?
			}


			if(agc.gain<8){		// Si c'est pas du bruit

			if((nbPeaks = findpeaks(amplitude,iPk))>0)
				{
						// Ok il y a des pics donc faire la dÃ©tection
						detect=detection(nbPeaks, iPk, frequencies, amplitude);


						//if(detect==1) {
						//	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET); //allumer LED si un frelon est détecté
						//} else {
						//	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
						//}

				}
			}
			cpt = 0;
		}
	#endif //USE_OLD_ALGO
}
 





/**
  * @brief  User function that is called when 1 ms of PDM data is available.
  *       In this application only PDM to PCM conversion and USB streaming
  *                  is performed.
  *       User can add his own code here to perform some DSP or audio analysis.
  * @param  none
  * @retval None
  */

void Init_Acquisition_Peripherals(uint32_t AudioFreq, uint32_t ChnlNbrIn, uint32_t ChnlNbrOut)
{
  MicParams.BitsPerSample = 16;
  MicParams.ChannelsNbr = ChnlNbrIn;
  MicParams.Device = AUDIO_IN_DIGITAL_MIC;
  MicParams.SampleRate = AudioFreq;
  MicParams.Volume = AUDIO_VOLUME_INPUT;

  if (CCA02M2_AUDIO_IN_Init(CCA02M2_AUDIO_INSTANCE, &MicParams) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
}






/**
  * @brief  User function that is called when 1 ms of PDM data is available.
  *       In this application only PDM to PCM conversion and USB streaming
  *                  is performed.
  *       User can add his own code here to perform some DSP or audio analysis.
  * @param  none
  * @retval None
  */
void Start_Acquisition(void)
{
  if (CCA02M2_AUDIO_IN_Record(CCA02M2_AUDIO_INSTANCE, (uint8_t *) PDM_Buffer, AUDIO_IN_BUFFER_SIZE) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }
  arm_rfft_fast_init_f32(&fft_handler, AUDIO_SIZE);


  // Initialiser l'AGC avec une valeur cible de 1.0 et un coefficient alpha de 0.01 // TARGET = 1 est une mauvaise valeur
  initAGC(&agc, TARGET, 0.00001f);
}



void Error_Handler(void)
{
  while (1);
}


/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */


