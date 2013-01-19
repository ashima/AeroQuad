/*
  AeroQuad v3.0 - April 2011
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


#ifndef _AEROQUAD_MAGNETOMETER_MPU9150_H_
#define _AEROQUAD_MAGNETOMETER_MPU9150_H_

#include "Arduino.h"

#include "Compass.h"
#include <Platform_MPU9150.h>

void initializeMagnetometer() {
  initializeMPU9150Sensors();
  readMPU9150Sensors();
  measureMagnetometer(0.0, 0.0);  // Assume 1st measurement at 0 degrees roll and 0 degrees pitch
  calculateMagHeading(0.0, 0.0);  // Assume 1st measurement at 0 degrees roll and 0 degrees pitch
}

/* The axes here are determined by comparing the HMC5883L mounted upside down and plugged
 * into the AeroQuad v2.0.6 shield, to the MPU-9150, which is plugged into the gyro port
 * of the AQ shield.
 */
// WARNING: this does not update the MPU9150 sensor values;
//   that must be done via calling readMPU9150Sensors()
void measureMagnetometer(float roll, float pitch) {
  rawMagSum[XAXIS] += (rawMag[YAXIS]=MPU9150.data.mag.y);
  rawMagSum[YAXIS] += -(rawMag[XAXIS]=MPU9150.data.mag.x);
  rawMagSum[ZAXIS] += (rawMag[ZAXIS]=MPU9150.data.mag.z);
  nsamples+=1;
}

void calculateMagHeading(float roll, float pitch) {
  // FIXME: should really give an error
  if (nsamples == 0)
    return;
    
  measuredMagX = (rawMagSum[XAXIS]/nsamples) * magScale[XAXIS] + magBias[XAXIS];
  measuredMagY = (rawMagSum[YAXIS]/nsamples) * magScale[YAXIS] + magBias[YAXIS];
  measuredMagZ = (rawMagSum[ZAXIS]/nsamples) * magScale[ZAXIS] + magBias[ZAXIS];
  
  rawMagSum[XAXIS]=0.0;
  rawMagSum[YAXIS]=0.0;
  rawMagSum[ZAXIS]=0.0;
  nsamples=0;
  
  measuredMag[XAXIS] = measuredMagX;
  measuredMag[YAXIS] = measuredMagY;
  measuredMag[ZAXIS] = measuredMagZ;
  
  const float cosRoll =  cos(roll);
  const float sinRoll =  sin(roll);
  const float cosPitch = cos(pitch);
  const float sinPitch = sin(pitch);

  const float magX = (float)measuredMagX * cosPitch + 
                     (float)measuredMagY * sinRoll * sinPitch + 
                     (float)measuredMagZ * cosRoll * sinPitch;
           
  const float magY = (float)measuredMagY * cosRoll - 
                     (float)measuredMagZ * sinRoll;

  const float tmp  = sqrt(magX * magX + magY * magY);
   
  hdgX = magX / tmp;
  hdgY = -magY / tmp;
}

#endif
