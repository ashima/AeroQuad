struct bits_struct {
	unsigned Parity : 1;
	unsigned C64 : 1;
	unsigned C32 : 1;
	unsigned C16 : 1;
	unsigned C8  : 1;
	unsigned C4  : 1;
	unsigned HeightHold : 1;
	unsigned UserInControl : 1;
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
	if (!(type==0x07 && packet[size-1]==0x03)) return false;
	//control packet is 7 B X Y Z T C 3
	if (!(type==0x06 && packet[size-1]==0x02)) return false;

	//check the Parity	
	return true;
}

int ConvertByteToStick(byte val){
	/* Takes the 1 byte value (0-256) and converts it to the range required for the 
	receiverCommand. Normally this is 1000-2000 but we take the defines from Receiver.H
	*/
	
	int scale = (MAXCOMMAND-MINCOMMAND)/255;
	int offset = MINCOMMAND;
	
	return offset + val * scale;
}

void processCommand(char type)
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
	 
	char packet[7];
	byte size;
	if (type==0x06){
		size = 6;
	} else if (type==0x07) {
		size = 7;
	}
	
	readBytes(packet, size);
	
	/* Check the Parity */
	if (PacketisValid(packet, size, type))
	{

	/* Update the bitField Controls*/
		bitField.ch = packet[0];
		if (bitField.field.UserInControl) { /* user is in control, listen to the packet. */
			/* altitude hold state */	
			altitudeHoldState = bitField.field.HeightHold;
			/*update the AUX1 receiver */
			receiverCommand[AUX1] = 1000;
			
			/* Update the Sticks
			The numbers from the packet are [0-256), THe output numbers should be [1000-2000]
			Receiver.h provides  
				MINCOMMAND=1000
				MIDCOMMAND=1500
				MAXCOMMAND=2000
				
			bytes are X,Y,Z,T
			*/
			
			//X
			receiverCommand[XAXIS]    = ConvertByteToStick(packet[1]);
			receiverCommand[YAXIS]    = ConvertByteToStick(packet[2]);			
			receiverCommand[ZAXIS]    = ConvertByteToStick(packet[3]);				
			receiverCommand[THROTTLE] = ConvertByteToStick(packet[4]);					

//			SERIAL_PRINT(receiverCommand[XAXIS]);
//			SERIAL_PRINT(receiverCommand[YAXIS]);
	//		SERIAL_PRINT(receiverCommand[ZAXIS]);
//			SERIAL_PRINT(receiverCommand[THROTTLE]);
		}
	
	}
	
}