// SPDX-License-Identifier: GPL-2.0
/*
 * ROHM BH1710/BH1715/BH1721/BH1750/BH1751 ambient light sensor driver
 *
 *
 * Data sheets:
 *      https://www.alldatasheet.com/view.jsp?Searchword=BH1750
 *
 * 7-bit I2C slave addresses:
 *  0x23 (ADDR pin low)
 *  0x5C (ADDR pin high)
 *
 */

#ifndef __BH1750_H
#define __BH1750_H

#define	BH1750_Write_Address    0x46  // 0x23 << 1 + 0x01
#define	BH1750_Read_Address     0x47

#define BH1750_POWER_ON         0x01
#define BH1750_POWER_DOWN		0x00
#define BH1750_RSET             0x07
#define BH1750_CONTINUE_H_RES_MODE  0x10 // 1lx 120ms
#define BH1750_CONTINUE_H_RES_MODE2 0x11 // 0.5lx 120ms
#define BH1750_CONTINUE_L_RES_MODE  0x13 // 4lx 16ms
#define BH1750_ONE_TIME_H_RES_MODE	0x20 /* auto-mode for BH1721 */
#define BH1750_ONE_TIME_H_RES_MODE2	0x21
#define BH1750_ONE_TIME_L_RES_MODE	0x23
#define BH1750_CHANGE_INT_TIME_H_BIT	0x40
#define BH1750_CHANGE_INT_TIME_L_BIT	0x60

int bh1750_init();
int bh1750_read(unsigned char *buf);
int bh1750_read_lx(float *lx);

#endif // !__BH1750_H