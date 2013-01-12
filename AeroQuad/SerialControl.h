
struct bits_struct {
//endians are fun.
	unsigned UserInControl : 1;
	unsigned HeightHold : 1;
	unsigned C4  : 1;
	unsigned C8  : 1;
	unsigned C16 : 1;
	unsigned C32 : 1;
	unsigned C64 : 1;
	unsigned C128 : 1;
};

union bits_union { 
	char ch;
	struct bits_struct field;
} bitField;

void readBytes(char * data, byte size)
{
  byte index = 0;
  byte timeout = 0;

  do {
    if (SERIAL_AVAILABLE() == 0) {
      delay(1);
      timeout++;
    } else {
      data[index] = SERIAL_READ();
      timeout = 0;
      index++;
    }
  } while((timeout < 10) && (index < size));

}

bool PacketisValid(char * packet, byte size, byte type){

	//Check the last byte
	//control packet is 7 B X Y Z T C 3
	if (type==0x07) if (packet[size-1]!=0x03) return false;
	//control packet is 7 B X Y Z T C 3
	if (type==0x06) if (packet[size-1]!=0x02) return false;

	return true;
}

int ConvertByteToStick(byte val){
	/* Takes the 1 byte value (0-256) and converts it to the range required for the 
	receiverCommand. Normally this is 1000-2000 but we take the defines from Receiver.H
	*/
	
	float scale = (MAXCOMMAND-MINCOMMAND)/255.;
	int offset = MINCOMMAND;
	
	return offset + val * scale;
}

char processCommand(char old_queryType)
{
	/* Here we define the packets
	
	COMM = START_COMM COMM1 COMMX COMMY COMMZ COMMT END_COMM
	CON  = START_CON COMM1 COMMX COMMY COMMZ COMMT CONB END_CON
	
	START_COMM = 0x06
	COMM1 = bitfield [Parity, C64, C32, C16, C8, C4, HeightHold, UserInControl]
	COMMX = 0-255 Roll
	COMMY = 0-255 Pitch
	COMMZ = 0-255 Yaw
	COMMT = 0-255 Throttle
	END_COMM = 0x02
	
	START_CON = 0x07
	COMM1 = bitfield [Parity, C64, C32, C16, C8, C4, HeightHold, UserInControl]
	COMMX = 0-255 Roll
	COMMY = 0-255 Pitch
	COMMZ = 0-255 Yaw
	COMMT = 0-255 Throttle
	CONB = bitfield [B128, B64, B32, B16, B8, B4, B2, B1]
	END_CON = 0x03

	 */

	/* At this point we don't know if a packet was sent. Read the serial and check 
	for a 0x06 or 0x07, if they are read in, we're in the game, if not, it's 
	a normal serial command, return it to the caller.
	*/
	
	char queryType = SERIAL_READ();

	if ((queryType!=0x06) && (queryType!=0x07))
	{
		return queryType;
	}
	char packet[7];
	byte size;
	if (queryType==0x06){
		size = 6;
	} else if (queryType==0x07) {
		size = 7;
	}
//	SERIAL_PRINT("A");	
	readBytes(packet, size);
//	SERIAL_PRINT("B");
//	SERIAL_PRINT(queryType);SERIAL_PRINT(", ");
//	for(int i=0;i<size;i++){SERIAL_PRINT(*(packet+i));SERIAL_PRINT(", ");}

	/* Check the Parity */
	if (PacketisValid(packet, size, queryType))
	{
	bad_packet_count = 0;
	packetWarning=false;
//	SERIAL_PRINT("C");
	/* Update the bitField Controls*/
		bitField.ch = packet[0];
		
/* user is in control, listen to the packet. */
			/* altitude hold state */	
			altitudeHoldState = bitField.field.HeightHold;
			/*update the AUX1 receiver */
			receiverData[MODE] = 2000; /*heading mode*/
			if (altitudeHoldState == 1){
				receiverData[AUX1] = 1000;
			} else {
				receiverData[AUX1] = 2000;
			}
			receiverData[AUX2] = 1000;
			
			/* Update the Sticks
			The numbers from the packet are [0-256), THe output numbers should be [1000-2000]
			Receiver.h provides  
				MINCOMMAND=1000
				MIDCOMMAND=1500
				MAXCOMMAND=2000
				
			bytes are X,Y,Z,T
			*/
			
			//X
			receiverData[XAXIS]    = ConvertByteToStick(packet[1]);
			receiverData[YAXIS]    = ConvertByteToStick(packet[2]);			
			receiverData[ZAXIS]    = ConvertByteToStick(packet[3]);				
			receiverData[THROTTLE] = ConvertByteToStick(packet[4]);					

		//	SERIAL_PRINT(receiverCommand[XAXIS]);
		//	SERIAL_PRINT(receiverCommand[YAXIS]);
		//	SERIAL_PRINT(receiverCommand[ZAXIS]);
		//	SERIAL_PRINTLN(receiverCommand[THROTTLE]);
	
	if (queryType==0x06){
		//return the old query type so that serial commands are continued.
		return old_queryType;
	} else if (queryType==0x07) {
		//update the query type with the control byte from this packet.
		return packet[size-1];
	}
	} else { //invalid packet
	bad_packet_count +=1 ;
	packetWarning=true;
//	beep.beep(8000-bad_packet_count*100);
	
	}
	
	
}