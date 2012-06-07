/*
  AeroQuad v3.0.1 - February 2012
  www.AeroQuad.com
  Copyright (c) 2012 Ted Carancho.  All rights reserved.
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

// FlightControl.pde is responsible for combining sensor measurements and
// transmitter commands into motor commands for the defined flight configuration (X, +, etc.)
//////////////////////////////////////////////////////////////////////////////
/////////////////////////// calculateFlightError /////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#ifndef _AQ_ALTITUDE_CONTROL_PROCESSOR_H_
#define _AQ_ALTITUDE_CONTROL_PROCESSOR_H_


//      float zVelocity = (filteredAccel[ZAXIS] * (1 - accelOneG * invSqrt(isq(filteredAccel[XAXIS]) + isq(filteredAccel[YAXIS]) + isq(filteredAccel[ZAXIS])))) - runTimeAccelBias[ZAXIS];
//      float estimatedSensorAltitude = previousSensorAltitude - zVelocity;
//      float estimatedCurrentAltitude = (estimatedSensorAltitude + currentSensorAltitude) / 2;
//      previousSensorAltitude = currentSensorAltitude;
      // compute throttle z dampening
//      int zDampeningThrottleCorrection = -updatePID(0.0, estimatedZVelocity, &PID[ZDAMPENING_PID_IDX]);
//      zDampeningThrottleCorrection = constrain(zDampeningThrottleCorrection, minThrottleAdjust*0.8, maxThrottleAdjust*0.8);


#if defined AltitudeHoldBaro || defined AltitudeHoldRangeFinder

#define INVALID_THROTTLE_CORRECTION -1000

#define ALTITUDE_BUMP_SPEED 0.01

/**
 * processAltitudeHold
 * 
 * This function is responsible to process the throttle correction 
 * to keep the current altitude if selected by the user 
 */
int altitudeHoldThrottleCorrection = 0;

void processAltitudeHold()
{
  // ****************************** Altitude Adjust *************************
  // Thanks to Honk for his work with altitude hold
  // http://aeroquad.com/showthread.php?792-Problems-with-BMP085-I2C-barometer
  // Thanks to Sherbakov for his work in Z Axis dampening
  // http://aeroquad.com/showthread.php?359-Stable-flight-logic...&p=10325&viewfull=1#post10325
  if (altitudeHoldState == ON) {

    // computer altitude error!
    #if defined AltitudeHoldRangeFinder
      if (isOnRangerRange(rangeFinderRange[ALTITUDE_RANGE_FINDER_INDEX])) {
        if (sonarAltitudeToHoldTarget == INVALID_RANGE) {
          sonarAltitudeToHoldTarget = rangeFinderRange[ALTITUDE_RANGE_FINDER_INDEX];
        }
        altitudeHoldThrottleCorrection = updatePID(sonarAltitudeToHoldTarget, rangeFinderRange[ALTITUDE_RANGE_FINDER_INDEX], &PID[SONAR_ALTITUDE_HOLD_PID_IDX]);
        altitudeHoldThrottleCorrection = constrain(altitudeHoldThrottleCorrection, minThrottleAdjust, maxThrottleAdjust);
      }
    #endif
    #if defined AltitudeHoldBaro
//      if (altitudeHoldThrottleCorrection == INVALID_THROTTLE_CORRECTION) {
//        altitudeHoldThrottleCorrection = updatePID(baroAltitudeToHoldTarget, getBaroAltitude(), &PID[BARO_ALTITUDE_HOLD_PID_IDX]);
//        altitudeHoldThrottleCorrection = constrain(altitudeHoldThrottleCorrection, minThrottleAdjust, maxThrottleAdjust);

		altitudeHoldThrottleCorrection += constrain(100.*(baroAltitudeToHoldTarget - getBaroAltitude()),-5,5);
		altitudeHoldThrottleCorrection = constrain(altitudeHoldThrottleCorrection, -500,500);

//      }
    #endif        
    if (altitudeHoldThrottleCorrection == INVALID_THROTTLE_CORRECTION) {
      throttle = receiverCommand[THROTTLE];
      return;
    }
    
    if (abs(altitudeHoldThrottle - receiverCommand[THROTTLE]) > altitudeHoldPanicStickMovement) {
      altitudeHoldState = ALTPANIC; // too rapid of stick movement so PANIC out of ALTHOLD
    } 
    else {
      
      if (receiverCommand[THROTTLE] > (altitudeHoldThrottle + altitudeHoldBump)) { // AKA changed to use holdThrottle + ALTBUMP - (was MAXCHECK) above 1900
        #if defined AltitudeHoldBaro
          baroAltitudeToHoldTarget += ALTITUDE_BUMP_SPEED;
        #endif
        #if defined AltitudeHoldRangeFinder
          float newalt = sonarAltitudeToHoldTarget + ALTITUDE_BUMP_SPEED;
          if (isOnRangerRange(newalt)) {
            sonarAltitudeToHoldTarget = newalt;
          }
        #endif
      }
      
      if (receiverCommand[THROTTLE] < (altitudeHoldThrottle - altitudeHoldBump)) { // AKA change to use holdThorrle - ALTBUMP - (was MINCHECK) below 1100
        #if defined AltitudeHoldBaro
          baroAltitudeToHoldTarget -= ALTITUDE_BUMP_SPEED;
        #endif
        #if defined AltitudeHoldRangeFinder
          float newalt = sonarAltitudeToHoldTarget - ALTITUDE_BUMP_SPEED;
          if (isOnRangerRange(newalt)) {
            sonarAltitudeToHoldTarget = newalt;
          }
        #endif
      }
    }
    throttle = altitudeHoldThrottle + altitudeHoldThrottleCorrection;// + zDampeningThrottleCorrection;
  }
  else {
	altitudeHoldThrottleCorrection = 0;
    throttle = receiverCommand[THROTTLE];
  }
}

#endif

#endif // _AQ_ALTITUDE_CONTROL_PROCESSOR_H_
