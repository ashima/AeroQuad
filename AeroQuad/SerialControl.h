void readByte(char * data)
{
  byte index = 0;
  byte timeout = 0;
  byte data = 0;

  do {
    if (SERIAL_AVAILABLE() == 0) {
      delay(1);
      timeout++;
    } else {
      data = SERIAL_READ();
      timeout = 0;
    }
  } while(timeout < 10);

	return data;
}

void readValueSerial(char *data, byte size) {
  byte index = 0;
  byte timeout = 0;
  data[0] = '\0';

  do {
    if (SERIAL_AVAILABLE() == 0) {
      delay(1);
      timeout++;
    } else {
      data[index] = SERIAL_READ();
      timeout = 0;
      index++;
    }
  } while ((index == 0 || data[index-1] != ';') && (timeout < 10) && (index < size-1));

  data[index] = '\0';
}
char* readPacket(void){
	/* Here we define the packet
		START_BYTE = 0x02 already received
		CONTROL_BYTE = 0-127 = command, 128-255 = control
		command (B) [64=ParityBit,32=,16=,8=,4=,2=HeightHold,1=UserInControl]
		C1 command(X)
		C2 command(Y)
		C3 command(Z)
		C4 command(T)
		
		control [64=ParityBit,32=,16=,8=,4=,2=,1=]
		C1 control(control)
		
		STOP_BYTE = 0x03
	 */
	char packet[6];
	byte timeout = 0;
	byte index = 0;
	
	//read the first byte
	while(SERIAL_AVAILABLE() != 0 && timeout < 10){
		delay(1);
		timeout++;
	}
	
	
	
	size = 3; //default size is 3, if the control byte & 0x40 == True, then size=6;
	do {
    if (SERIAL_AVAILABLE() == 0) {
      delay(1);
      timeout++;
    } else {
      data[index] = SERIAL_READ();
      timeout = 0;
      index++;
    }
    //Check the byte
    if (index>0 && packet[0]) == 
  } while ((index == 0 || data[index-1] != ';') && (timeout < 10) && (index < size-1));

	
}