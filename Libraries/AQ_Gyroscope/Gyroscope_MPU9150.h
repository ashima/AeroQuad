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

#ifndef _AEROQUAD_GYROSCOPE_MPU9150_COMMON_H_
#define _AEROQUAD_GYROSCOPE_MPU9150_COMMON_H_

#include <Platform_MPU9150.h>
#include <Gyroscope.h>

void initializeGyro() {
  float range = 2*1000.0;
  gyroScaleFactor = radians(range/65536.0);

  initializeMPU9150Sensors();
}

void gyroUpdateHeading() {
  long int currentTime = micros();
  
  if (gyroRate[ZAXIS] > (float)radians(1.0) || gyroRate[ZAXIS] < (float)radians(-1.0))
    gyroHeading += gyroRate[ZAXIS] * ((currentTime - gyroLastMesuredTime) / 1000000.0);
  
  gyroLastMesuredTime = currentTime;
}

// WARNING: this does not update the MPU9150 sensor values;
//   that must be done via calling readMPU9150Sensors()
void measureGyro() {
  int g[3];
  g[XAXIS] = -(gyroZero[XAXIS] - (gyroRaw[XAXIS]=MPU9150.data.gyro.x));
  g[YAXIS] =   gyroZero[YAXIS] - (gyroRaw[YAXIS]=MPU9150.data.gyro.y);
  g[ZAXIS] =   gyroZero[ZAXIS] - (gyroRaw[ZAXIS]=MPU9150.data.gyro.z);

  for (byte axis = 0; axis <= ZAXIS; axis++)
    gyroRate[axis] = g[axis] * gyroScaleFactor;

  gyroUpdateHeading();
}

// WARNING: this does not update the MPU9150 sensor values;
//   that must be done via calling readMPU9150Sensors()
void measureGyroSum() {
  gyroSample[XAXIS] += (gyroRaw[XAXIS]=MPU9150.data.gyro.x);
  gyroSample[YAXIS] += (gyroRaw[YAXIS]=MPU9150.data.gyro.y);
  gyroSample[ZAXIS] += (gyroRaw[ZAXIS]=MPU9150.data.gyro.z);

  gyroSampleCount++;
}

void evaluateGyroRate() {
  // FIXME: should really give an error
  if (gyroSampleCount == 0)
    return;

  int g[3];
  g[XAXIS] = -(gyroZero[XAXIS] - (gyroSample[XAXIS] / gyroSampleCount));
  g[YAXIS] =   gyroZero[YAXIS] - (gyroSample[YAXIS] / gyroSampleCount);
  g[ZAXIS] =   gyroZero[ZAXIS] - (gyroSample[ZAXIS] / gyroSampleCount);

  gyroSample[XAXIS] = 0;
  gyroSample[YAXIS] = 0;
  gyroSample[ZAXIS] = 0;
  gyroSampleCount = 0;

  for (byte axis = 0; axis <= ZAXIS; axis++)
    gyroRate[axis] = g[axis] * gyroScaleFactor;

  gyroUpdateHeading();
}

void calibrateGyro() {
  int findZero[FINDZERO];

  for (byte axis = 0; axis < 3; axis++) {
    for (int i=0; i<FINDZERO; i++) {
      readMPU9150Sensors();
      
      if (axis == XAXIS)
        findZero[i] = MPU9150.data.gyro.x;
      else if (axis == YAXIS)
        findZero[i] = MPU9150.data.gyro.y;
      else
        findZero[i] = MPU9150.data.gyro.z;
      
      delay(10);
    }
    
    gyroZero[axis] = findMedianInt(findZero, FINDZERO);
  }
}

#endif
