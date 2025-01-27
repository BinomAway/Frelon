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
#include "string.h"
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
int16_t PDM_Buffer[((((AUDIO_IN_CHANNELS * AUDIO_IN_SAMPLING_FREQUENCY) / 1000) * MAX_DECIMATION_FACTOR) / 16)* N_MS ];

int16_t PCM_Buffer[((AUDIO_IN_CHANNELS * AUDIO_IN_SAMPLING_FREQUENCY) / 1000)  * N_MS ];
int16_t PCM_Buffer_A[((AUDIO_IN_CHANNELS * AUDIO_IN_SAMPLING_FREQUENCY) / 1000)  * N_MS ];
int16_t PCM_Buffer_B[((AUDIO_IN_CHANNELS * AUDIO_IN_SAMPLING_FREQUENCY) / 1000)  * N_MS ];

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
  AudioProcess();
}

/**
  * @brief  Transfer Complete user callback, called by BSP functions.
  * @param  None
  * @retval None
  */
void CCA02M2_AUDIO_IN_TransferComplete_CallBack(uint32_t Instance)
{
  UNUSED(Instance);
  AudioProcess();
}

/**
  * @brief  User function that is called when 1 ms of PDM data is available.
  *       In this application only PDM to PCM conversion and USB streaming
  *                  is performed.
  *       User can add his own code here to perform some DSP or audio analysis.
  * @param  none
  * @retval None
  */
int ardui_cnt = 0;

void AudioProcess(void)
{
	static float32_t buff[AUDIO_SIZE];
	static int cpt = 0;
	int i = 0,j=0;
	static float32_t out_agc[AUDIO_SIZE];
	static float32_t out_fft[AUDIO_SIZE];
	float32_t amplitude[LENGTHYIN];
	float frequencies[LENGTHYIN];
	int iPk[200];
	int nbPeaks = 0;
	int detect=-1;


  if (CCA02M2_AUDIO_IN_PDMToPCM(CCA02M2_AUDIO_INSTANCE, (uint16_t *)PDM_Buffer, PCM_Buffer) != BSP_ERROR_NONE)
  {
    Error_Handler();
  }

  ardui_cnt++;
  if ( ardui_cnt & 1 )
  {
    memcpy(PCM_Buffer_A,PCM_Buffer,((AUDIO_IN_CHANNELS * AUDIO_IN_SAMPLING_FREQUENCY) / 1000)  * N_MS);
  } 
  else {
    memcpy(PCM_Buffer_B,PCM_Buffer,((AUDIO_IN_CHANNELS * AUDIO_IN_SAMPLING_FREQUENCY) / 1000)  * N_MS);
  }


//Tant qu'on a pas rÃ©cupÃ©rÃ© un Ã©chantillon complet
  if(cpt < AUDIO_SIZE-1)
    {
  	  for(i=cpt; i < cpt+((AUDIO_IN_CHANNELS * AUDIO_IN_SAMPLING_FREQUENCY) / 1000)  * N_MS; i++)
  	  {
        if ( ardui_cnt & 1 )
        {
  		    buff[i] = PCM_Buffer_B[j];
        } 
        else {
  		   buff[i] = PCM_Buffer_A[j];
        }


  		  j++;
  	  }
  	  cpt+=((AUDIO_IN_CHANNELS * AUDIO_IN_SAMPLING_FREQUENCY) / 1000)  * N_MS;

    }
    else
    {
		//On fait passer notre Ã©chantillon dans l'agc 
  	  	AGC(&agc, buff, out_agc,10, AUDIO_SIZE);		
		//On applique la fft sur la sortie de l'agc et on rÃ©cupÃ¨re le spectre d'amplitude
  	  	arm_rfft_fast_f32(&fft_handler,out_agc,out_fft,1); 
  		arm_cmplx_mag_f32(out_fft,amplitude,LENGTHYIN);


  		for(i = 0; i < LENGTHYIN; i++) {
  			  frequencies[i] = (float)i/2048*8000 ;	//2048 et 8000 car on ne peut pas multiplier les variables des defines de AUDIO_SIZE et AUDIO_IN_SAMPLING_FREQUENCY
  		}
  		if(agc.gain<8){		// Si c'est pas du bruit
  		if((nbPeaks = findpeaks(amplitude,iPk))>0)
  			{
  					  // Ok il y a des pics donc faire la dÃƒÂ©tection
  					  detect=detection(nbPeaks,iPk,frequencies,amplitude);
  					  if(detect==1)HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET);//allumer LED si un frelon est dÃ©tectÃ©
  					  else HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET);
  			}
  		}
  		cpt = 0;
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
  arm_rfft_fast_init_f32(&fft_handler,AUDIO_SIZE);


  	  	  // Initialiser l'AGC avec une valeur cible de 1.0 et un coefficient alpha de 0.01
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


