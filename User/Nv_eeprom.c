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
 * @brief   ��ȡ������
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
 * @brief   Nv�ⲿ���ݴ洢��ַ��ʼ��
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
 * @brief   NV���洢��ʼ������
 *
 * @param   None
 *
 * @return  None
 **************************************************************************************************/
void InitNV_EEpromTask(void )
{
	uint8_t SetDefault = NV_EEPROM_Read;
	
	//SPI_FLASH_BlockErase( STARTUPOPTION_ADDR );
	if( ReadStartupOptions() == 0xFF )//�Ƿ�Ϊ���豸
	{
		 SetDefault = NV_EEPROM_Wirte;
	}
  //UserNV_EEPROMInit(PERIODTIMETASK_ADDR, sizeof(alarm_ctrl_t), (uint8_t *)&AlarmTask, SetDefault ); //��ʱ������Ϣ
  //UserNV_EEPROMInit(DEVICEINFOR_ADDR,    sizeof(device_infor_list), (uint8_t *)&DeviceInfor, SetDefault ); //��ʱ������Ϣ
	
  if(SetDefault == NV_EEPROM_Wirte)
  {
		 SPI_FLASH_SectorErase( STARTUPOPTION_ADDR );
		 SPI_FLASH_BufferWrite((uint8_t *)&SetDefault, STARTUPOPTION_ADDR, 1 );
  }	
}
















