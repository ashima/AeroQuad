/*
  AeroQuad v3.0 - Febuary 2012
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

// Led Status Processor controls the LED:s on the shield according to vehicle status


#ifndef _AQ_LedProcessor_H_
#define _AQ_LedProcessor_H_

byte flashingLedState = 0; // this counter increments by one at 10Hz

void processLedStatus() {

  
  //
  // process ready state light in case we use GPS
  //
  #if defined (UseGPS)
    if (haveAGpsLock()) {
      if (isHomeBaseInitialized()) {
        digitalWrite(LED_Green, HIGH);
      }
      else {
        digitalWrite(LED_Green, (flashingLedState & 4));
      }
    }
    else { 
      digitalWrite(LED_Green, (flashingLedState & 2));
    }
  #endif
  
  //
  // process ready state light in case we use Batt monitor
  //
  #if defined (BattMonitor)
    if (batteryAlarm) {
      digitalWrite(LED_Red, flashingLedState & 4);
    } else if (batteryWarning) {
      digitalWrite(LED_Red, (flashingLedState & 15)==0);
    } else if (packetsStarted && packetError) {
      digitalWrite(LED_Red, (flashingLedState & 1)==0);
      beep.beep(packet_error_freq);
    } else if (packetsStarted && bad_packet_count > 5) {
	  digitalWrite(LED_Red, (flashingLedState & 8)==0);
      beep.beep(packet_warning_freq + bad_packet_count*packet_beep_delta);
    } else { 
      digitalWrite(LED_Red, LOW);
    }


  #endif  

  //
  // process mode light
  //
  if (flightMode == ATTITUDE_FLIGHT_MODE) {
    digitalWrite(LED_Yellow, HIGH);
  }
  else {
    digitalWrite(LED_Yellow, LOW);
  }

  flashingLedState++;

}

#endif
