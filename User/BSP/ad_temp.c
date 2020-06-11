/* Includes ------------------------------------------------------------------*/
#include "ad_temp.h"
#include "usart123.h"
#include "AppTask.h"
#include "rtc.h"
#include "Variable.h"
/*********************************************************************
 * MACROS
 */
/*********************************************************************
 * TYPEDEFS
 */

/*********************************************************************
 * CONSTANTS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */

/*********************************************************************
 * LOCAL VARIABLES
 */
ADC_HandleTypeDef             AdcHandle;

/* Variable containing ADC conversions data */
uint16_t   aADCxConvertedData[ADC_CONVERTED_DATA_BUFFER_SIZE];

//uint8_t status = 0;
//static void Error_Handler(void);

/************* NTC B值表 阻值表************/
const uint16_t NTCTAB[141]= 
{
0xF3F,//-40℃
0xF34,//-39℃
0xF29,//-38℃
0xF1D,//-37℃
0xF11,//-36℃
0xF04,//-35℃
0xEF7,//-34℃
0xEE9,//-33℃
0xEDB,//-32℃
0xECC,//-31℃
0xEBC,//-30℃
0xEAC,//-29℃
0xE9B,//-28℃
0xE8A,//-27℃
0xE78,//-26℃
0xE65,//-25℃
0xE52,//-24℃
0xE3E,//-23℃
0xE29,//-22℃
0xE14,//-21℃
0xDFE,//-20℃
0xDE7,//-19℃
0xDD0,//-18℃
0xDB8,//-17℃
0xD9F,//-16℃
0xD86,//-15℃
0xD6C,//-14℃
0xD51,//-13℃
0xD36,//-12℃
0xD1A,//-11℃
0xCFD,//-10℃
0xCDF,//-9℃
0xCC1,//-8℃
0xCA3,//-7℃
0xC83,//-6℃
0xC64,//-5℃
0xC43,//-4℃
0xC22,//-3℃
0xC00,//-2℃
0xBDE,//-1℃
0xBBC,//0℃
0xB98,//1℃
0xB75,//2℃
0xB51,//3℃
0xB2C,//4℃
0xB07,//5℃
0xAE2,//6℃
0xABC,//7℃
0xA97,//8℃
0xA70,//9℃
0xA4A,//10℃
0xA23,//11℃
0x9FC,//12℃
0x9D5,//13℃
0x9AE,//14℃
0x987,//15℃
0x960,//16℃
0x938,//17℃
0x911,//18℃
0x8EA,//19℃
0x8C3,//20℃
0x89B,//21℃
0x874,//22℃
0x84D,//23℃
0x827,//24℃
0x800,//25℃
0x7DA,//26℃
0x7B4,//27℃
0x78E,//28℃
0x768,//29℃
0x743,//30℃
0x71E,//31℃
0x6F9,//32℃
0x6D5,//33℃
0x6B1,//34℃
0x68E,//35℃
0x66B,//36℃
0x649,//37℃
0x626,//38℃
0x605,//39℃
0x5E4,//40℃
0x5C3,//41℃
0x5A3,//42℃
0x583,//43℃
0x564,//44℃
0x545,//45℃
0x527,//46℃
0x509,//47℃
0x4EC,//48℃
0x4D0,//49℃
0x4B3,//50℃
0x498,//51℃
0x47D,//52℃
0x462,//53℃
0x448,//54℃
0x42F,//55℃
0x416,//56℃
0x3FD,//57℃
0x3E5,//58℃
0x3CE,//59℃
0x3B7,//60℃
0x3A0,//61℃
0x38A,//62℃
0x375,//63℃
0x360,//64℃
0x34B,//65℃
0x337,//66℃
0x324,//67℃
0x310,//68℃
0x2FE,//69℃
0x2EB,//70℃
0x2DA,//71℃
0x2C8,//72℃
0x2B7,//73℃
0x2A7,//74℃
0x296,//75℃
0x286,//76℃
0x277,//77℃
0x268,//78℃
0x259,//79℃
0x24B,//80℃
0x23D,//81℃
0x22F,//82℃
0x222,//83℃
0x215,//84℃
0x208,//85℃
0x1FC,//86℃
0x1F0,//87℃
0x1E4,//88℃
0x1D9,//89℃
0x1CE,//90℃
0x1C3,//91℃
0x1B8,//92℃
0x1AE,//93℃
0x1A4,//94℃
0x19A,//95℃
0x191,//96℃
0x187,//97℃
0x17E,//98℃
0x175,//99℃
0x16D,//100℃
};
/*********************************************************************
 * @fn    void ADC_Temp_Vbat_Config( void )
 *
 * @brief  NTC温度和电池
 *          
 * @param  none
 *         
 * @return none
 *                        
 */
void ADC_Temp_Vbat_Config( void )
{
	
	/* ADC channel configuration structure declaration */
  ADC_ChannelConfTypeDef        sConfig;
	
	 /* ### - 1 - Initialize ADC peripheral #################################### */
  
  AdcHandle.Instance          = ADC1;
  HAL_ADC_DeInit(&AdcHandle);

  AdcHandle.Init.ClockPrescaler        = ADC_CLOCK_SYNC_PCLK_DIV2;      /* Synchronous clock mode, input ADC clock with prscaler 2 */
  AdcHandle.Init.Resolution            = ADC_RESOLUTION_12B;            /* 12-bit resolution for converted data */
  AdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;           /* Right-alignment for converted data */
  AdcHandle.Init.ScanConvMode          = ADC_SCAN_DIRECTION_FORWARD;    /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
  AdcHandle.Init.EOCSelection          = ADC_EOC_SINGLE_CONV;           /* EOC flag picked-up to indicate conversion end */
  AdcHandle.Init.LowPowerAutoPowerOff  = DISABLE;
  AdcHandle.Init.LowPowerAutoWait      = DISABLE;                       /* Auto-delayed conversion feature disabled */
  AdcHandle.Init.ContinuousConvMode    = ENABLE;                        /* Continuous mode enabled (automatic conversion restart after each conversion) */
  AdcHandle.Init.DiscontinuousConvMode = DISABLE;                       /* Parameter discarded because sequencer is disabled */
  AdcHandle.Init.ExternalTrigConv      = ADC_SOFTWARE_START;            /* Software start to trig the 1st conversion manually, without external event */
  AdcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE; /* Parameter discarded because software trigger chosen */
  AdcHandle.Init.DMAContinuousRequests = ENABLE;                        /* ADC DMA continuous request to match with DMA circular mode */
  AdcHandle.Init.Overrun               = ADC_OVR_DATA_OVERWRITTEN;      /* DR register is overwritten with the last conversion result in case of overrun */
  AdcHandle.Init.SamplingTimeCommon    = ADC_SAMPLETIME_239CYCLES_5;

  /* Initialize ADC peripheral according to the passed parameters */
	
	
  if( HAL_ADC_Init(&AdcHandle)!= HAL_OK)
	{
		printf("HAL_ADC_Init Err\r\n");
	}

  /* ### - 2 - Start calibration ############################################ */
  HAL_ADCEx_Calibration_Start(&AdcHandle);

  
  /* ### - 3 - Channel configuration ######################################## */
  sConfig.Channel      = ADC_CHANNEL_0;               /* Channel to be converted */
  sConfig.Rank         = ADC_RANK_CHANNEL_NUMBER;
  HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);

	sConfig.Channel      = ADC_CHANNEL_1;               /* Channel to be converted */
  sConfig.Rank         = ADC_RANK_CHANNEL_NUMBER;
	HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
	
	sConfig.Channel      = ADC_CHANNEL_2;               /* Channel to be converted */
  sConfig.Rank         = ADC_RANK_CHANNEL_NUMBER;
  HAL_ADC_ConfigChannel(&AdcHandle, &sConfig);
  
  /* ### - 4 - Start conversion in DMA mode ################################# */
  HAL_ADC_Start_DMA(&AdcHandle,(uint32_t *)aADCxConvertedData, 30 );
							 
}

/**
  * @brief  Conversion complete callback in non blocking mode 
  * @param  hadc ADC handle
  * @retval None
  */
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	 uint16_t tempad,batad;
	
	 HAL_ADC_Stop_DMA(hadc);
	
	 RT0_Temp =  AD_Filter( aADCxConvertedData, 0, 3, 10 );//柜温
	 RT1_Temp  = AD_Filter( aADCxConvertedData, 1, 3, 10 );//除霜
	
	 batad = AD_Filter( aADCxConvertedData, 2, 3, 10 );//电池电量
	
	
	 RT0_Var = look_up_table( (uint16_t*)NTCTAB, 141, RT0_Temp );//温度值+40
	 RT1_Var = look_up_table( (uint16_t*)NTCTAB, 141, RT0_Temp );//温度值+40
	
   //ADC0_Task();
	 //ADC1_Task();
	
	
//	 temp = look_up_table( (uint16_t*)NTCTAB,136, tempad )-30;
//	 vbat = (float)batad/4096*3.303*268/200;

	 Timer_Event_Set( &timer_10msDB[NTC_TASK], 1010, ENABLE, 0, ADC_DMA_Start_Evt );//
}

/*********************************************************************
 * @fn    void ADC2_Task( void )
 *
 * @brief  化霜传感器检测 defrosting NTC
 *          
 * @param  none
 *         
 * @return none
 *                        
 */
void ADC1_Task( void )//10ms
{
//	 if( Flag2.OneBit.Ad1FirstDect == 1)
//	 {
//		  Flag2.OneBit.Ad1FirstDect = 0;
//		 	LastRT1_Var = RT1_Var;
//		 DefrostTemp = (RT1_Var-50)*10 + cfgPara.defrosteCompensation;
//	 }
	 
	 	/*********防止温度变化凸显*********/
		if(RT1_Var != LastRT1_Var)
		{ 
			Temp1_Change++;
			if(Temp1_Change > 9 )
			{ 
				LastRT1_Var = RT1_Var;
				Temp1_Change=0;
				
//				DefrostTemp = (RT1_Var-40)*10 + cfgPara.defrosteCompensation;
			}
			else 
				RT1_Var = LastRT1_Var;
		}
		else 
		{ 
			Temp1_Change = 0;
		}		

	  /*****传感器开路故障*****/
    if( RT1_Temp > 4000 )    //传感器故障5s延时确认
    { 
			if( SensorErrorCount1 < 20 )
				SensorErrorCount1++;
			else
			{
				if( ErrorState.OneBit.SensorError1 ==0 )
				  ErrorState.OneBit.SensorError1 = 1;
			}
    }
    else                              
    { 
			if( ErrorState.OneBit.SensorError1 )   //传感器故障解除3s延时确认
			{ 
				SensorErrorCount1--;
				if(SensorErrorCount1==0)
				{ 
					ErrorState.OneBit.SensorError1=0;
				}
			}
			else
			{ 
				ErrorState.OneBit.SensorError1=0;
				SensorErrorCount1=0;
			}
		}
    /*****传感器短路故障*****/
		if( RT1_Temp < 40 )
		{ 
			if(SensorShortCount1 < 20 )
				SensorShortCount1++;
			else
			{ 
				if( ErrorState.OneBit.SensorShortError1 ==0 )
				  ErrorState.OneBit.SensorShortError1=1;
			}
		}
		else
		{ 
			if(ErrorState.OneBit.SensorShortError1)
			{ 
				SensorShortCount1--;
				if(SensorShortCount1==0)
				{  
					ErrorState.OneBit.SensorShortError1=0;
				}
			}
			else
			{ 
				ErrorState.OneBit.SensorShortError1=0;
				SensorShortCount1=1;
			}
		}

//		if( ErrorState.OneBit.SensorShortError1==0 && ErrorState.OneBit.SensorError1==0)
//		{
//			if( DefrostTemp < cfgPara.AL*10 )
//			{ 
//				if(ALCount < 20 )
//					 ALCount++;
//				else
//				{ 
//					if( ErrorState.OneBit.AL == 0 )
//						Set_Event( &timer_10msDB[SymbolRefurbish_TASK], CompressorStateSymbolRefurbish );
//					 ErrorState.OneBit.AL=1;
//				}
//			}
//			else
//			{ 
//				if( ErrorState.OneBit.AL)
//				{ 
//					 ALCount--;
//					if(ALCount==0)
//					{  
//						 ErrorState.OneBit.AL=0;
//					}
//				}
//				else
//				{ 
//					ErrorState.OneBit.AL=0;
//					ALCount=0;
//				}
//			}
//		}
//		else
//		{
//			ErrorState.OneBit.AL =0;
//			ALCount=0;
//		}
}

/*********************************************************************
 * @fn    void ADC_Task( void )
 *
 * @brief  NTC温度传感器数据采集
 *          
 * @param  none
 *         
 * @return none
 *                        
 */
void ADC0_Task( void )//10ms
{

//	  if( Flag2.OneBit.Ad0FirstDect == 1)
//		{
//		  Flag2.OneBit.Ad0FirstDect	 =0;
//			LastRT0_Var = RT0_Var;
//			TemperatureInside = ( RT0_Var - 40 )*10 + cfgPara.tempCompensation;
//		}

	  /*****传感器开路故障*****/
    if( RT0_Temp > 4000 )    //传感器故障5s延时确认
    { 
			if( SensorErrorCount0 < 20 )
				SensorErrorCount0++;
			else
			{
				if( ErrorState.OneBit.SensorError0 ==0 )
				  ErrorState.OneBit.SensorError0 = 1;
			}
    }
    else                              
    { 
			if( ErrorState.OneBit.SensorError0 )   //传感器故障解除3s延时确认
			{ 
				SensorErrorCount0--;
				if(SensorErrorCount0==0)
				{ 
					ErrorState.OneBit.SensorError0=0;

					
					//压机事件继续
					//Flag2.OneBit.SensorErr = 1;
					//Timer_Event_Set( &timer_10msDB[CompCtr_TASK], 100, ENABLE, 0, CompressorControl );
				}
			}
			else
			{ 
				ErrorState.OneBit.SensorError0=0;
				SensorErrorCount0=0;
			}
		}
    /*****传感器短路故障*****/
		if( RT0_Temp < 40 )
		{ 
			if(SensorShortCount0 < 20 )
				SensorShortCount0++;
			else
			{ 
				if( ErrorState.OneBit.SensorShortError0 == 0 )
				  ErrorState.OneBit.SensorShortError0=1;
			}
		}
		else
		{ 
			if(ErrorState.OneBit.SensorShortError0)
			{ 
				SensorShortCount0--;
				if(SensorShortCount0==0)
				{  
					ErrorState.OneBit.SensorShortError0=0;
					
					//压机事件继续
					//Flag2.OneBit.SensorErr = 1;
					//Timer_Event_Set( &timer_10msDB[CompCtr_TASK], 100, ENABLE, 0, CompressorControl );
				}
			}
			else
			{ 
				ErrorState.OneBit.SensorShortError0=0;
				SensorShortCount0=0;
			}
		}
		/*********防止温度变化凸显*********/
		if(RT0_Var != LastRT0_Var)
		{ 
			Temp0_Change++;
			if(Temp0_Change > 9 )
			{ 
				LastRT0_Var = RT0_Var;
				Temp0_Change=0;
				
				//TemperatureInside = ( RT0_Var - 40 )*10 + cfgPara.tempCompensation;
			}
			else 
				RT0_Var = LastRT0_Var;
		}
		else 
		{ 
			Temp0_Change=0;
		}
		

//		if( ErrorState.OneBit.SensorShortError0==0 && ErrorState.OneBit.SensorError0==0)
//		{
//			if( TemperatureInside > cfgPara.AH*10 )
//			{ 
//				if(AHCount < 20 )
//					 AHCount++;
//				else
//				{ 
//					if( ErrorState.OneBit.AH == 0 )
//						Set_Event( &timer_10msDB[SymbolRefurbish_TASK], CompressorStateSymbolRefurbish );
//					 ErrorState.OneBit.AH=1;
//				}
//			}
//			else
//			{ 
//				if( ErrorState.OneBit.AH )
//				{ 
//					 AHCount--;
//					if( AHCount==0 )
//					{  
//						 ErrorState.OneBit.AH=0;
//					}
//				}
//				else
//				{ 
//					ErrorState.OneBit.AH=0;
//					AHCount=0;
//				}
//			}
//		}
//		else
//		{
//			ErrorState.OneBit.AH =0;
//			AHCount=0;
//		}
//		
//			if( Flag2.OneBit.PretendDisplay !=1 )//正常显示
//			{
//				if( DisplayState == Measure )
//				{
//					if(Flag1.Bits.Measure ==0)
//					{
//						Flag1.Bits.Measure = 1;
//						DisplayTemperature = TemperatureInside;
//					}
//				}
//			}
//			else//假显状态下
//			{
//				if( TemperatureInside <= BeforeDefrosteTemp  )//
//				{
//					Flag2.OneBit.PretendDisplay = 0;//正常显示
//					tPara.OneHourCount=0;
//				}
//			}
}



/***************************************************************************************************
 * @fn      void DataCmdHandler ( char *pBuf )
 *
 * @brief   查表获取温度序号
 *
 * @param   None
 *
 * @return  None
*/
uint8_t look_up_table(uint16_t *a,uint8_t ArrayLong,uint16_t Arraydata)
{    
   uint16_t begin, end, middle;  
    uint16_t i = 0;  
	
    begin = 0 ;  
    end = ArrayLong-1 ;  
	
    if(Arraydata <= a[end])
 			return end;  
    else if(Arraydata >= a[begin]) 
			return begin;  
		
    while( begin < end )  
    {  
			middle = ( begin+end )/2 ;  
			if( Arraydata == a[middle] ) 
				break ;  
			if( Arraydata < a[middle] && Arraydata > a[middle+1]) 
				break ; 
			
			if( Arraydata > a[middle] )  
				end = middle ;                      
			else if( Arraydata < a[middle] )
				begin = middle ;
      
			if( i++ > ArrayLong ) 
				break ;  
    }  
    if( begin > end ) 
			return 0 ; 
		
    return middle ;  
}

/***************************************************************************************************
 * @fn      uint16_t AD_Filter( uint16_t  adArray, uint8_t rowNum, uint8_t  num )
 *
 * @brief   ad数据算法滤波分析（中值数据）
 *
 * @param   adArray  数据结构体  rowNum有效数据列   num 数据个数
 *
 * @return  None
*/
uint16_t AD_Filter( uint16_t *adArray, uint8_t rowNum, uint8_t totalRow, uint8_t  num )
{
	
	uint8_t i = 0, countNum = 0;
	uint16_t admin = 0xFFFF, admax = 0; 
	uint16_t  advalue = 0;
	uint32_t adsum =0;
	
	for(i=0; i< num; i++)
	{
		if( adArray[i*totalRow+rowNum] > admax)
			 admax = adArray[i*totalRow+rowNum];
		
		if( adArray[i*totalRow+rowNum] < admin)
			 admin = adArray[i*totalRow+rowNum];
		
		adsum += adArray[i*totalRow+rowNum];
	}
	countNum = num-2;
	
	advalue = (adsum - admax-admin )/countNum;
	
	return advalue;
}



