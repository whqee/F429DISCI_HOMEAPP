#include "bh1750.h"
#include "i2c.h"

#define i2c hi2c3

/** read 2 bytes from bh1750 to buf **/
int bh1750_read(unsigned char *buf)
{
    int ret = HAL_I2C_Master_Transmit(&i2c, BH1750_Write_Address, BH1750_CONTINUE_H_RES_MODE,1,100);
    if (ret != HAL_OK)  return ret;
    HAL_Delay(120);
    return HAL_I2C_Master_Receive(&i2c,BH1750_Read_Address,buf,2, 100);
}

int bh1750_read_lx(float *lx)
{
    char buf[2];
    int ret = bh1750_read(buf);
    if (ret != HAL_OK) return ret;
    *lx = (float)( ((uint16_t)buf[0]<<8) + buf[1] )/1.2;
    return ret;
}

int bh1750_init()
{
    int ret = HAL_I2C_Master_Transmit(&i2c, BH1750_Write_Address, BH1750_POWER_ON,1,100);
    if (ret != HAL_OK)  return ret;
    ret = HAL_I2C_Master_Transmit(&i2c, BH1750_Write_Address, BH1750_RSET,1,100);
    if (ret != HAL_OK)  return ret;

    ret = HAL_I2C_Master_Transmit(&i2c, BH1750_Write_Address, 0x42,1,100);
    if (ret != HAL_OK)  return ret;
    ret = HAL_I2C_Master_Transmit(&i2c, BH1750_Write_Address, 0x65,1,100);
    if (ret != HAL_OK)  return ret;
    // 0x42 0x65 -- transmittance 100% for 1750
    // 0x44 0x6a -- transmittance 50%  for 1750
    
    return HAL_I2C_Master_Transmit(&i2c, BH1750_CONTINUE_H_RES_MODE, 0x65,1,100);
}
