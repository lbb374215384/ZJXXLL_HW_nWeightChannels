/* Includes ------------------------------------------------------------------*/
#include "Nv_eeprom.h"
#include "spi_flash.h"
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


 /**************************************************************************************************
 * @fn      uint8_t ReadStartupOptions( void )
 *
 * @brief   读取配置字
 *
 * @param   None
 *
 * @return  None
 **************************************************************************************************/
uint8_t ReadStartupOptions( void )
{
  // Default to Use Config State 
  uint8_t startupOption = 0;
  SPI_FLASH_BufferRead( &startupOption, STARTUPOPTION_ADDR, 1);
  return ( startupOption );
}

 /**************************************************************************************************
 * @fn     void UserNV_EEPROMInit(uchar EEaddr, uchar len, uchar *buf, uchar setDefault )
 *
 * @brief   Nv外部数据存储地址初始化
 *
 * @param   None
 *
 * @return  None
 **************************************************************************************************/
void UserNV_EEPROMInit(uint32_t EEaddr, uint16_t len, uint8_t *buf, uint8_t setDefault )
{
  if ( setDefault )
  {
    // Write the default value back to EEPROM
		SPI_FLASH_SectorErase( EEaddr );
	  SPI_FLASH_BufferWrite( buf, EEaddr, len);
  }
  else
  {
    // The item exists in EEPROM, read it from EEPROM
		SPI_FLASH_BufferRead( buf, EEaddr, len );
  }
}
 /**************************************************************************************************
 * @fn      void InitNV_EEpromTask(void )
 *
 * @brief   NV区存储初始化任务
 *
 * @param   None
 *
 * @return  None
 **************************************************************************************************/
void InitNV_EEpromTask(void )
{
	uint8_t SetDefault = NV_EEPROM_Read;
	
	//SPI_FLASH_BlockErase( STARTUPOPTION_ADDR );
	if( ReadStartupOptions() == 0xFF )//是否为新设备
	{
		 SetDefault = NV_EEPROM_Wirte;
	}
  //UserNV_EEPROMInit(PERIODTIMETASK_ADDR, sizeof(alarm_ctrl_t), (uint8_t *)&AlarmTask, SetDefault ); //定时任务信息
  //UserNV_EEPROMInit(DEVICEINFOR_ADDR,    sizeof(device_infor_list), (uint8_t *)&DeviceInfor, SetDefault ); //定时任务信息
	
  if(SetDefault == NV_EEPROM_Wirte)
  {
		 SPI_FLASH_SectorErase( STARTUPOPTION_ADDR );
		 SPI_FLASH_BufferWrite((uint8_t *)&SetDefault, STARTUPOPTION_ADDR, 1 );
  }	
}

















