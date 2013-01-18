/*
  AeroQuad v3.0 - May 2011
  www.AeroQuad.com
  Copyright (c) 2011 Ted Carancho.  All rights reserved.
  An Open Source Arduino based multicopter.

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

// parts of the init sequence were taken from AP_InertialSensor_MPU6000.h

#ifndef _AEROQUAD_PLATFORM_MPU9150_H_
#define _AEROQUAD_PLATFORM_MPU9150_H_

#include "Arduino.h"
#include <SensorsStatus.h>
#include <Platform_MPU9150.h>

/* the #defines below are stolen from 
 * https://github.com/jrowberg/i2cdevlib/blob/59a8c950de21e8e6f4fe2a4a7d4db10ea570365b/Arduino/MPU6050/MPU6050.h
 * FIXME: I'm not sure what needs to change about the licensing
 */

// AK-8975 (built into the MPU-9150 as an I2C device)
#define AK8975_ADDRESS_00         0x0C

#define AK8975_RA_WIA             0x00
  #define AK8975_WHO_AM_I         0x48
#define AK8975_RA_HXL             0x03
#define AK8975_RA_HXH             0x04
#define AK8975_RA_HYL             0x05
#define AK8975_RA_HYH             0x06
#define AK8975_RA_HZL             0x07
#define AK8975_RA_HZH             0x08

#define AK8975_RA_CNTL            0x0A
  #define AK8975_MODE_SINGLE        0x1

// MPU-9150
#define MPU9150_I2C_ADDRESS 0x68

#define MPU9150_RA_SMPLRT_DIV       0x19
#define MPU9150_RA_CONFIG           0x1A
  #define MPU9150_CFG_DLPF_CFG_BIT    0
  #define MPU9150_CFG_DLPF_CFG_LENGTH 3
    #define MPU9150_DLPF_BW_256         0x00
    #define MPU9150_DLPF_BW_188         0x01
    #define MPU9150_DLPF_BW_98          0x02
    #define MPU9150_DLPF_BW_42          0x03
    #define MPU9150_DLPF_BW_20          0x04
    #define MPU9150_DLPF_BW_10          0x05
    #define MPU9150_DLPF_BW_5           0x06
#define MPU9150_RA_GYRO_CONFIG      0x1B
  #define MPU9150_GCONFIG_FS_SEL_BIT      3
  #define MPU9150_GCONFIG_FS_SEL_LENGTH   2
    #define MPU9150_GYRO_FS_250         0x00
    #define MPU9150_GYRO_FS_500         0x01
    #define MPU9150_GYRO_FS_1000        0x02
    #define MPU9150_GYRO_FS_2000        0x03
#define MPU9150_RA_ACCEL_CONFIG     0x1C
  #define MPU9150_ACONFIG_AFS_SEL_BIT         3
  #define MPU9150_ACONFIG_AFS_SEL_LENGTH      2
    #define MPU9150_ACCEL_FS_2          0x00
    #define MPU9150_ACCEL_FS_4          0x01
    #define MPU9150_ACCEL_FS_8          0x02
    #define MPU9150_ACCEL_FS_16         0x03

#define MPU9150_RA_I2C_MST_CTRL     0x24
#define MPU9150_RA_I2C_SLV0_ADDR    0x25
  #define MPU9150_I2C_SLV_RW_BIT      7
  #define MPU9150_I2C_SLV_ADDR_BIT    0
  #define MPU9150_I2C_SLV_ADDR_LENGTH 7
#define MPU9150_RA_I2C_SLV0_REG     0x26
#define MPU9150_RA_I2C_SLV0_CTRL    0x27
  #define MPU9150_I2C_SLV_EN_BIT      7
  #define MPU9150_I2C_SLV_BYTE_SW_BIT 6
  #define MPU9150_I2C_SLV_REG_DIS_BIT 5
  #define MPU9150_I2C_SLV_GRP_BIT     4
  #define MPU9150_I2C_SLV_LEN_BIT     0
  #define MPU9150_I2C_SLV_LEN_LENGTH  4
#define MPU9150_RA_I2C_SLV1_ADDR    0x28
#define MPU9150_RA_I2C_SLV1_REG     0x29
#define MPU9150_RA_I2C_SLV1_CTRL    0x2A
#define MPU9150_RA_I2C_MST_STATUS   0x36
#define MPU9150_RA_INT_PIN_CFG      0x37
  #define MPU9150_INTCFG_I2C_BYPASS_EN_BIT    1

#define MPU9150_RA_ACCEL_XOUT_H     0x3B
#define MPU9150_RA_ACCEL_XOUT_L     0x3C
#define MPU9150_RA_ACCEL_YOUT_H     0x3D
#define MPU9150_RA_ACCEL_YOUT_L     0x3E
#define MPU9150_RA_ACCEL_ZOUT_H     0x3F
#define MPU9150_RA_ACCEL_ZOUT_L     0x40
#define MPU9150_RA_TEMP_OUT_H       0x41
#define MPU9150_RA_TEMP_OUT_L       0x42
#define MPU9150_RA_GYRO_XOUT_H      0x43
#define MPU9150_RA_GYRO_XOUT_L      0x44
#define MPU9150_RA_GYRO_YOUT_H      0x45
#define MPU9150_RA_GYRO_YOUT_L      0x46
#define MPU9150_RA_GYRO_ZOUT_H      0x47
#define MPU9150_RA_GYRO_ZOUT_L      0x48
#define MPU9150_RA_EXT_SENS_DATA_00 0x49
#define MPU9150_RA_EXT_SENS_DATA_01 0x4A
#define MPU9150_RA_EXT_SENS_DATA_02 0x4B
#define MPU9150_RA_EXT_SENS_DATA_03 0x4C
#define MPU9150_RA_EXT_SENS_DATA_04 0x4D
#define MPU9150_RA_EXT_SENS_DATA_05 0x4E

#define MPU9150_RA_I2C_SLV0_DO      0x63
#define MPU9150_RA_I2C_SLV1_DO      0x64

#define MPU9150_RA_I2C_MST_DELAY_CTRL   0x67

#define MPU9150_RA_USER_CTRL        0x6A
  #define MPU9150_USERCTRL_I2C_MST_EN_BIT         5
#define MPU9150_RA_PWR_MGMT_1       0x6B
  #define MPU9150_PWR1_DEVICE_RESET_BIT   7
  #define MPU9150_PWR1_SLEEP_BIT          6
  #define MPU9150_PWR1_CLKSEL_BIT         0
  #define MPU9150_PWR1_CLKSEL_LENGTH      3
    #define MPU9150_CLOCK_INTERNAL          0x00
    #define MPU9150_CLOCK_PLL_XGYRO         0x01
    #define MPU9150_CLOCK_PLL_YGYRO         0x02
    #define MPU9150_CLOCK_PLL_ZGYRO         0x03
    #define MPU9150_CLOCK_PLL_EXT32K        0x04
    #define MPU9150_CLOCK_PLL_EXT19M        0x05
    #define MPU9150_CLOCK_KEEP_RESET        0x07
#define MPU9150_RA_PWR_MGMT_2       0x6C

#define MPU9150_RA_WHO_AM_I         0x75
  #define MPU9150_WHO_AM_I_BIT        1
  #define MPU9150_WHO_AM_I_LENGTH     6


typedef struct {
  short x;
  short y;
  short z;
} tAxis;

union uMPU9150 {
  unsigned char rawByte[];
  unsigned short rawWord[];
  struct {
    tAxis accel;
    short temperature;
    tAxis gyro;
    tAxis mag;
  } data;
} MPU9150;

void AK8975_WriteReg(int addr, byte data) {
  updateRegisterI2C(AK8975_ADDRESS_00, addr, data);
}

byte AK8975_ReadReg(int addr) {
  sendByteI2C(AK8975_ADDRESS_00, addr);
  return readByteI2C(AK8975_ADDRESS_00);
}

void MPU9150_WriteReg(int addr, byte data) {
  updateRegisterI2C(MPU9150_I2C_ADDRESS, addr, data);
}

byte MPU9150_ReadReg(int addr) {
  sendByteI2C(MPU9150_I2C_ADDRESS, addr);
  return readByteI2C(MPU9150_I2C_ADDRESS);
}

void initializeMPU9150Sensors() {
  static bool initialized = false;
  if (initialized) 
    return;
  initialized = true;

  unsigned char val;

  val = MPU9150_ReadReg(MPU9150_RA_WHO_AM_I);

  // 0x7E = bits 6:1; bit 7 is R/W, and bit 0 is selected by AD0, but not reflected
  // in the WHO_AM_I register
//  if ( (val & ((1 << MPU9150_WHO_AM_I_LENGTH + 1) - 1) << MPU9150_WHO_AM_I_BIT) ) == MPU9150_I2C_ADDRESS)
  if ((val & 0x7E) == (MPU9150_I2C_ADDRESS & ~1))
    vehicleState |= GYRO_DETECTED | ACCEL_DETECTED;
  else
    return;

  // chip reset
  MPU9150_WriteReg(MPU9150_RA_PWR_MGMT_1, 1 << MPU9150_PWR1_DEVICE_RESET_BIT);
  delay(100);  // Startup time delay

  // wake up device and select GyroZ clock (better performance)
  MPU9150_WriteReg(MPU9150_RA_PWR_MGMT_1, MPU9150_CLOCK_PLL_ZGYRO << MPU9150_PWR1_CLKSEL_BIT);
  MPU9150_WriteReg(MPU9150_RA_PWR_MGMT_2, 0);

  // 100Hz sample, 42Hz low-pass filter, +/- 1000 deg/s, +/- 4g
  MPU9150_WriteReg(MPU9150_RA_SMPLRT_DIV, 9); // Sample rate = 1kHz / (1+9) = 100Hz
  MPU9150_WriteReg(MPU9150_RA_CONFIG, MPU9150_DLPF_BW_42 << MPU9150_CFG_DLPF_CFG_BIT);
  MPU9150_WriteReg(MPU9150_RA_GYRO_CONFIG, MPU9150_GYRO_FS_1000 << MPU9150_GCONFIG_FS_SEL_BIT);
  MPU9150_WriteReg(MPU9150_RA_ACCEL_CONFIG, MPU9150_ACCEL_FS_4 << MPU9150_ACONFIG_AFS_SEL_BIT);

  // disable I2C master, enable I2C bypass -- so we talk to the mag without needing the MPU-9150
  MPU9150_WriteReg(MPU9150_RA_USER_CTRL, 0 << MPU9150_USERCTRL_I2C_MST_EN_BIT);
  MPU9150_WriteReg(MPU9150_RA_INT_PIN_CFG, 1 << MPU9150_INTCFG_I2C_BYPASS_EN_BIT);

  delay(100); // not sure this is needed

  // maybe we should reset the mag?
  if (AK8975_ReadReg(AK8975_RA_WIA) == AK8975_WHO_AM_I)
    vehicleState |= MAG_DETECTED;
  else
    return;
  
  MPU9150_WriteReg(MPU9150_RA_INT_PIN_CFG, 0 << MPU9150_INTCFG_I2C_BYPASS_EN_BIT);

  // read mag x,y,z
  MPU9150_WriteReg(MPU9150_RA_I2C_SLV0_ADDR, (1 << MPU9150_I2C_SLV_RW_BIT) 
                                           | (AK8975_ADDRESS_00 << MPU9150_I2C_SLV_ADDR_BIT));
  MPU9150_WriteReg(MPU9150_RA_I2C_SLV0_REG, AK8975_RA_HXL);
  MPU9150_WriteReg(MPU9150_RA_I2C_SLV0_CTRL, (1 << MPU9150_I2C_SLV_EN_BIT) 
                                           | (1 << MPU9150_I2C_SLV_BYTE_SW_BIT) 
                                           | (1 << MPU9150_I2C_SLV_GRP_BIT) 
                                           | (sizeof(short)*3 << MPU9150_I2C_SLV_LEN_BIT));
  
  // tell the mag to take another sample
  MPU9150_WriteReg(MPU9150_RA_I2C_SLV1_ADDR, (0 << MPU9150_I2C_SLV_RW_BIT) 
                                           | (AK8975_ADDRESS_00 << MPU9150_I2C_SLV_ADDR_BIT));
  MPU9150_WriteReg(MPU9150_RA_I2C_SLV1_REG, AK8975_RA_CNTL);
  MPU9150_WriteReg(MPU9150_RA_I2C_SLV1_DO, AK8975_MODE_SINGLE);
  MPU9150_WriteReg(MPU9150_RA_I2C_SLV1_CTRL, (1 << MPU9150_I2C_SLV_EN_BIT) 
                                           | (sizeof(byte) << MPU9150_I2C_SLV_LEN_BIT));

  // enable the slave I2C devices  
  MPU9150_WriteReg(MPU9150_RA_USER_CTRL, 1 << MPU9150_USERCTRL_I2C_MST_EN_BIT);
}

void readMPU9150Sensors() {
  sendByteI2C(MPU9150_I2C_ADDRESS, MPU9150_RA_ACCEL_XOUT_H);
  Wire.requestFrom(MPU9150_I2C_ADDRESS, sizeof(MPU9150));
  
  // conveniently, all values are words and aligned and proper endian
  for(byte i = 0; i < sizeof(MPU9150) / sizeof(short); i++)
    MPU9150.rawWord[i] = readWordI2C();
}

#endif
