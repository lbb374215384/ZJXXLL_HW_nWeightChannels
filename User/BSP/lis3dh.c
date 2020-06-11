/* Includes ------------------------------------------------------------------*/
#include "lis3dh.h"
#include "lis3dh_reg.h"
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
lis3dh_ctx_t dev_ctx;
lis3dh_int1_cfg_t _6d_cfg;
lis3dh_ctrl_reg3_t ctrl_reg3;
uint8_t ReadID;

  /*
   *  Initialize mems driver interface
   */
  //lis3dh_ctx_t dev_ctx;
  //lis3dh_ctrl_reg3_t ctrl_reg3;
  lis3dh_click_cfg_t click_cfg;
/*********************************************************************
 * EXTERNAL VARIABLES
 */

/*********************************************************************
 * EXTERNAL FUNCTIONS
 */



/*********************************************************************
 * LOCAL VARIABLES
 */
I2C_HandleTypeDef I2cHandle;

/*********************************************************************
 * LOCAL FUNCTIONS
 */
 
 
static int32_t I2C_WriteData(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len);
static int32_t I2C_ReadData(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len);

/*********************************************************************
 * @fn     uint8_t LIS3DH_Init( void )
 *
 * @brief  三轴加速度传感器
 *          
 * @param  none
 *         
 * @return none
 *                        
 */
void LIS3DH_Init( void )
{
  /*##-1- Configure the I2C peripheral ######################################*/
  I2cHandle.Instance             = I2Cx;
  I2cHandle.Init.Timing          = I2C_TIMING;
  I2cHandle.Init.OwnAddress1     = LIS3DH_I2C_ADD_H;
  I2cHandle.Init.AddressingMode  = I2C_ADDRESSINGMODE_7BIT;
  I2cHandle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  I2cHandle.Init.OwnAddress2     = 0xFF;
  I2cHandle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  I2cHandle.Init.NoStretchMode   = I2C_NOSTRETCH_DISABLE;
  
  if(HAL_I2C_Init(&I2cHandle) != HAL_OK)
  {
    /* Initialization Error */
    Error_Handler();
  }
  if (HAL_I2CEx_ConfigAnalogFilter(&I2cHandle, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_I2CEx_ConfigDigitalFilter(&I2cHandle, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /**********加速度传感器配置**********/
  dev_ctx.write_reg = I2C_WriteData;
  dev_ctx.read_reg  = I2C_ReadData;
  dev_ctx.handle    = &I2cHandle;
	
  lis3dh_device_id_get(&dev_ctx, &ReadID);// Check device ID
  if (ReadID != LIS3DH_ID)
  {
    Error_Handler();
  }
  

  /*
   * Set Output Data Rate
   * The recommended accelerometer ODR for single and
   * double-click recognition is 400 Hz or higher
   */
  lis3dh_data_rate_set(&dev_ctx, LIS3DH_ODR_400Hz);

  /*
   * Set full scale to 2 g
   */
  lis3dh_full_scale_set(&dev_ctx, LIS3DH_16g);

  /*
   * Set click threshold to 12h  -> 0.281 g
   * 1 LSB = full scale/128
   * Set TIME_LIMIT to 33h -> 127 ms.
   * 1 LSB = 1/ODR
   */
  lis3dh_tap_threshold_set(&dev_ctx, 0x12);
  lis3dh_shock_dur_set(&dev_ctx, 0x33);

  /*
   * Enable Click interrupt on INT pin 1
   */
  lis3dh_pin_int1_config_get(&dev_ctx, &ctrl_reg3);
  ctrl_reg3.i1_click = PROPERTY_ENABLE;
  lis3dh_pin_int1_config_set(&dev_ctx, &ctrl_reg3);
  lis3dh_int1_gen_duration_set(&dev_ctx, 0);

  /*
   * Enable single click on all axis
   */
  lis3dh_tap_conf_get(&dev_ctx, &click_cfg);
  click_cfg.xs = PROPERTY_ENABLE;
  click_cfg.ys = PROPERTY_ENABLE;
  click_cfg.zs = PROPERTY_ENABLE;
  lis3dh_tap_conf_set(&dev_ctx, &click_cfg);

  /*
   * Set device in HR mode
   */
  lis3dh_operating_mode_set(&dev_ctx, LIS3DH_HR_12bit);
		
}



/*
 * @brief  Write generic device register (platform dependent)
 *
 * @param  handle    customizable argument. In this examples is used in
 *                   order to select the correct sensor bus handler.
 * @param  reg       register to write
 * @param  bufp      pointer to data to write in register reg
 * @param  len       number of consecutive register to write
 *
 */
static int32_t I2C_WriteData(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len)
{
  if (handle == &I2cHandle)
  {
    /* Write multiple command */
    reg |= 0x80;
    HAL_I2C_Mem_Write(handle, LIS3DH_I2C_ADD_H, reg, I2C_MEMADD_SIZE_8BIT, bufp, len, 1000);
  }
  return 0;
}

/*
 * @brief  Read generic device register (platform dependent)
 *
 * @param  handle    customizable argument. In this examples is used in
 *                   order to select the correct sensor bus handler.
 * @param  reg       register to read
 * @param  bufp      pointer to buffer that store the data read
 * @param  len       number of consecutive register to read
 *
 */
static int32_t I2C_ReadData(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len)
{
  if (handle == &I2cHandle)
  {
    /* Read multiple command */
    reg |= 0x80;
    HAL_I2C_Mem_Read(handle, LIS3DH_I2C_ADD_H, reg, I2C_MEMADD_SIZE_8BIT, bufp, len, 1000);
  }
  return 0;
}
