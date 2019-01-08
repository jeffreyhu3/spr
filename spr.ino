/*
  DigitalReadSerial

  Reads a digital input on pin 2, prints the result to the Serial Monitor

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/DigitalReadSerial
*/

// digital pins pushbutton
#define PB1  		2
#define PB2  		3
#define DET_R  		4
#define DET_L  		5
#define PUMP_PB  	9

// digital pins output 
#define M_ROT_DIR  	6
#define M_ROT_PWR  	7
#define PUMP_PWR 	8
#define PUMP_LED 	10
#define M_ROT_LED_R	11
#define M_ROT_LED_L	12

//LED13 for debug
#define LED13  		13

#define SPR_SET 	1
#define SPR_CLR 	0

#define SPR_RIGHT 	1
#define SPR_LEFT 	0

#define SPR_ON 		1
#define SPR_OFF 	0

#define SPR_SENSOR_HELD 		1	//Location sensor pressed and held
#define SPR_SENSOR_RELEASE 		0	//Location sensor release

#define LOCATION_SENSOR_HELD_CNT 	2
#define MAIN_LOOP_CYCLE 			20


/*
************************************************************************************************************************
*                                                   LOCAL VARIABLES
************************************************************************************************************************
*/
int EventPb1;
int EventPb2;
int EventPb1Cmd;
int EventPb2Cmd;
int EventDET_R;	
int EventDET_L;	
int EventPump;
int EventPumpCmd;
unsigned long currentMillis;
unsigned long previousMillis;

/*
************************************************************************************************************************
*                                                    LOCAL FUNCTIONS
************************************************************************************************************************
*/
int readPB1(void);
int readPB2(void);
int readPumpPB(void);
void updateLocationSensor(void);
int readCmd(void);
int setCmd(int EventPb1, int EventPb2, int EventPump);
void processEvent(int EventPb1Temp, int EventPb2Temp, int EventPumpTemp);
void motorRotPwrCtrl(int onOff);
void motorRotDir(int right);
void pumpPwrCtrl(int onOff);
int getEventLocationSensorR(void);
int getEventLocationSensorL(void);
int ParsingCmdToPB1(void);
int ParsingCmdToPB2(void);
int ParsingCmdToPumpPB(void);
void ParsingCmd(int cmd);

// the setup routine runs once when you press reset:
void setup() {
	// initialize serial communication at 9600 bits per second:
	Serial.begin(9600);
	
	// make the pushbutton pins as input and internal pull-up
	pinMode(PB1, INPUT);
	digitalWrite(PB1, HIGH); 
	pinMode(PB2, INPUT);
	digitalWrite(PB2, HIGH); 
	pinMode(DET_R, INPUT);
	digitalWrite(DET_R, HIGH); 
	pinMode(DET_L, INPUT);
	digitalWrite(DET_L, HIGH); 
	pinMode(PUMP_PB, INPUT);
	digitalWrite(PUMP_PB, HIGH);

	// make the pushbutton pins as output for relay on/off control and initial it
	pinMode(M_ROT_DIR, OUTPUT ); 
	digitalWrite(M_ROT_DIR, LOW); 
	pinMode(M_ROT_PWR, OUTPUT ); 
	digitalWrite(M_ROT_PWR, LOW);
	pinMode(PUMP_PWR, OUTPUT ); 
	digitalWrite(PUMP_PWR, LOW);
	
	pinMode(PUMP_LED, OUTPUT ); 
	digitalWrite(PUMP_LED, LOW);
	pinMode(M_ROT_LED_R, OUTPUT ); 
	digitalWrite(M_ROT_LED_R, LOW);
	pinMode(M_ROT_LED_L, OUTPUT ); 
	digitalWrite(M_ROT_LED_L, LOW);
	
	/* LED13 for debug */
	pinMode(LED13, OUTPUT ); 
	digitalWrite(LED13, LOW);	
}

void loop()
{
	int cmd;
	
	currentMillis = millis();	
	if ((currentMillis - previousMillis) > MAIN_LOOP_CYCLE)
	{
		previousMillis = previousMillis + MAIN_LOOP_CYCLE;
		updateLocationSensor();
		processEvent(readPB1() | ParsingCmdToPB1(),readPB2() | ParsingCmdToPB2() ,readPumpPB() | ParsingCmdToPumpPB());
	}
	else
	{
		cmd = readCmd();
		if(cmd != -1)
		{
			Serial.print("cmd=0b");
			Serial.println(cmd, BIN);
			ParsingCmd(cmd);
			/*
			Serial.print("EventPb1Cmd=0b");
			Serial.println(EventPb1Cmd, BIN);
			Serial.print("EventPb2Cmd=0b");
			Serial.println(EventPb2Cmd, BIN);
			Serial.print("EventPumpCmd=0b");
			Serial.println(EventPumpCmd, BIN);
			*/
		}
	}
}

/*
************************************************************************************************************************
*                                        readPB1
*
* Description: This function is called by main loop, read push button status and set event variable
*
* Argument(s): none
* 
*
* Returns    : none
*
* Note(s)    : none
************************************************************************************************************************
*/
int readPB1()
{
	static int curPb1 = SPR_SET;	//current pb1 status
	static int prePb1 = SPR_SET;	//previous value
	int event = SPR_CLR;
	
	curPb1 = digitalRead(PB1);
	
	if(curPb1 != prePb1 && curPb1 == SPR_CLR)	// if pb1 pressed
	{
		event = SPR_SET;
	}
	prePb1 = curPb1;							//save current value to history variable
	
	return event;
}

/*
************************************************************************************************************************
*                                        readPushButton
*
* Description: This function is called by main loop, read push button status and set event variable
*
* Argument(s): none
* 
*
* Returns    : none
*
* Note(s)    : none
************************************************************************************************************************
*/
int readPB2()
{
	static int curPb2 = SPR_SET;	//current pb2 status
	static int prePb2 = SPR_SET;	//previous value
	int event = SPR_CLR;
	
	curPb2 = digitalRead(PB2);
		
	if(curPb2 != prePb2 && curPb2 == SPR_CLR)	// if pb1 pressed
	{
		event = SPR_SET;
	}
	prePb2 = curPb2;							//save current value to history variable
	
	return event;
}

/*
************************************************************************************************************************
*                                        readPumpPB
*
* Description: This function is called by main loop, read pump control button
*
* Argument(s): none
* 
*
* Returns    : none
*
* Note(s)    : none
************************************************************************************************************************
*/
int readPumpPB()
{
	static int curPumpPB = SPR_SET;	//current curPumpPB status
	static int prePumpPB = SPR_SET;	//previous value
	int EventPump = SPR_CLR;
	
	curPumpPB = digitalRead(PUMP_PB);
		
	if(curPumpPB != prePumpPB && curPumpPB == SPR_CLR)	// if curPumpPB pressed
	{
		EventPump = SPR_SET;
	}
	prePumpPB = curPumpPB;								//save current value to history variable
	
	return EventPump;
}

/*
************************************************************************************************************************
*                                        updateLocationSensor
*
* Description: This function is called by main loop, update location limit sensor status for pump motor
*
* Argument(s): none
* 
*
* Returns    : none
*
* Note(s)    : none
************************************************************************************************************************
*/
void updateLocationSensor()
{
	static int curLocationR = SPR_SET;	//current location right status
	static int PreLocationR = SPR_SET;	//previous location right status
	static int curLocationL = SPR_SET;
	static int PreLocationL = SPR_SET;
	
	curLocationR = digitalRead(DET_R);
	curLocationL = digitalRead(DET_L);
	
	if(curLocationR != PreLocationR && curLocationR == SPR_CLR)	// if location Right sensor pressed
	{
		EventDET_R = SPR_SET;									// set the event
	}	
	PreLocationR = curLocationR;								// save current value to previous variable
	
	if(curLocationL != PreLocationL && curLocationL == SPR_CLR)
	{
		EventDET_L = SPR_SET;
	}
	PreLocationL = curLocationL;
}

/*
************************************************************************************************************************
*                                        getEventLocationSensorR
*
* Description: This function is called by application, get the event location sensor rigt and clear it.
*
* Argument(s): none
* 
*
* Returns    : event
*
* Note(s)    : none
************************************************************************************************************************
*/
int getEventLocationSensorR(void)
{
	int EventDET_R_tmp = SPR_CLR;
	
	if(EventDET_R == SPR_SET) 
	{
		EventDET_R_tmp = SPR_SET;
		EventDET_R = SPR_CLR;	
	}
	
	return EventDET_R_tmp;	
}

/*
************************************************************************************************************************
*                                        getStatuLocationSensorR
*
* Description: This function is called by application, get the status current of location sensor right from IO directly
*
* Argument(s): none
* 
*
* Returns    : location sensor right status
*
* Note(s)    : none
************************************************************************************************************************
*/
int getStatuLocationSensorR(void)
{
	return digitalRead(DET_R) == LOW? SPR_SENSOR_HELD:SPR_SENSOR_RELEASE;
}

/*
************************************************************************************************************************
*                                        getEventLocationSensorL
*
* Description: This function is called by application, get the event location sensor left and clear it.
*
* Argument(s): none
* 
*
* Returns    : event
*
* Note(s)    : none
************************************************************************************************************************
*/
int getEventLocationSensorL(void)
{	
	int EventDET_L_tmp = SPR_CLR;
	
	if(EventDET_L == SPR_SET) 
	{
		EventDET_L_tmp = SPR_SET;
		EventDET_L = SPR_CLR;	
	}

	return EventDET_L_tmp;
}

/*
************************************************************************************************************************
*                                        getStatuLocationSensorL
*
* Description: This function is called by application, get the status current of location sensor left from IO directly
*
* Argument(s): none
* 
*
* Returns    : location sensor left status
*
* Note(s)    : none
************************************************************************************************************************
*/
int getStatuLocationSensorL(void)
{
	return digitalRead(DET_L) == LOW? SPR_SENSOR_HELD:SPR_SENSOR_RELEASE;
}

/*
************************************************************************************************************************
*                                        readCmd
*
* Description: This function is called by main loop, read command from serial port such as PC
*
* Argument(s): none
* 
*
* Returns    : cmd, binary format explain in following.
*              Bit2: Set means has a PB1 event
*              Bit1: Set means has a PB2 event
*              Bit0: Set means has a Pump event
*
* Note(s)    : none
************************************************************************************************************************
*/
int readCmd()
{
	String strInput;
	int cmdTmp = 0;
	int cmd = -1;
		
	if (Serial.available() > 0)
	{
		strInput = Serial.readString();
		if(strInput.endsWith("\r"))
		{
			cmdTmp = strInput.toInt();	
			cmd = 0;
			cmd |= (cmdTmp/100 > 0 ? 1 : 0) << 2;
			cmd |= (cmdTmp%100/10 > 0 ? 1 : 0) << 1;
			cmd |= (cmdTmp%100%10 > 0 ? 1 : 0) << 0;
			
			return cmd;
		}
	}
	
	return cmd;
}

/*
************************************************************************************************************************
*                                        setCmd
*
* Description: This function is called application, set event to combine them together.
*
* Argument(s): EventPb1Temp, pb1 event
*              EventPb2Temp, pb2 event
*              EventPumpTemp, pump event
*
* Returns    : cmd, binary format explain in following.
*              Bit2: Set means has a PB1 event
*              Bit1: Set means has a PB2 event
*              Bit0: Set means has a Pump event
*
* Note(s)    : none
************************************************************************************************************************
*/
int setCmd(int EventPb1Temp, int EventPb2Temp, int EventPumpTemp)
{
	int cmd;
	cmd = (EventPb1Temp << 2) | (EventPb2Temp << 1) | (EventPumpTemp << 0);	
	return cmd;
}

/*
************************************************************************************************************************
*                                              processEvent
*
* Description: Process PB1/PB2/Pump control button event. Update all LED status.
*
* Arguments  : none.
*
* Note(s)    : none
************************************************************************************************************************
*/
void processEvent(int EventPb1Temp, int EventPb2Temp, int EventPumpTemp)
{
	static int EventPb1Lock = SPR_CLR;
	static int EventPb2Lock = SPR_CLR;
	static int pumpPwrFlag = SPR_OFF;
	
	if(EventPb1Temp == SPR_SET)			/* if get an event for rotate to right direction */
	{
		EventPb1Lock = SPR_SET;			
		EventPb2Lock = SPR_CLR;	
		if(getStatuLocationSensorR() == SPR_SENSOR_HELD)	//If motor in limit location, ingnore the event.
		{
			EventPb1Lock = SPR_CLR;
			motorRotPwrCtrl(SPR_OFF);
		}
	}
	else if(EventPb2Temp == SPR_SET)	/* if get an event for rotate to left direction */
	{
		EventPb2Lock = SPR_SET;
		EventPb1Lock = SPR_CLR;	
		if(getStatuLocationSensorL() == SPR_SENSOR_HELD)
		{
			EventPb2Lock = SPR_CLR;
			motorRotPwrCtrl(SPR_OFF);
		}
	}
	
	/* Process the motor to rotate until touch the limit sensor will clear the locker */
	if(EventPb1Lock == SPR_SET)
	{
		motorRotPwrCtrl(SPR_ON);
		motorRotDir(SPR_RIGHT);
		if(getEventLocationSensorR() == SPR_SET)
		{
			EventPb1Lock = SPR_CLR;
			motorRotPwrCtrl(SPR_OFF);
		}
	}
	else if(EventPb2Lock == SPR_SET)
	{
		motorRotPwrCtrl(SPR_ON);
		motorRotDir(SPR_LEFT);
		if(getEventLocationSensorL() == SPR_SET)
		{
			EventPb2Lock = SPR_CLR;
			motorRotPwrCtrl(SPR_OFF);
		}
	}
	
	/* Update two leds to indicator location sensor status, tell user what the current location for motor */
	if(getStatuLocationSensorR() == SPR_SENSOR_HELD)
	{
		digitalWrite(M_ROT_LED_R, HIGH);
		digitalWrite(M_ROT_LED_L, LOW);
	}
	else if(getStatuLocationSensorL() == SPR_SENSOR_HELD)
	{
		digitalWrite(M_ROT_LED_R, LOW);
		digitalWrite(M_ROT_LED_L, HIGH);
	}
	else
	{
		digitalWrite(M_ROT_LED_R, LOW);
		digitalWrite(M_ROT_LED_L, LOW);
	}

	/* Process pump on/off and use LED to indicator it status */
	if(EventPumpTemp == SPR_SET)
	{
		if(pumpPwrFlag == SPR_ON)
		{
			pumpPwrFlag = SPR_OFF;
			pumpPwrCtrl(SPR_OFF);
			digitalWrite(PUMP_LED, LOW);	// turn off indicator led 
		}
		else
		{
			pumpPwrFlag = SPR_ON;		
			pumpPwrCtrl(SPR_ON);
			digitalWrite(PUMP_LED, HIGH);	// turn on indicator led
		}
	}
}

/*
************************************************************************************************************************
*                                              motorRotPwrCtrl
*
* Description: control rotate motor power on/off.
*
* Arguments  : onOff, if 1 will turn on motor
*
* Note(s)    : none
************************************************************************************************************************
*/
void motorRotPwrCtrl(int onOff)
{
	if(onOff == SPR_ON)
	{
		digitalWrite(M_ROT_PWR, HIGH);
	}
	else
	{
		digitalWrite(M_ROT_PWR, LOW);
	}
}

/*
************************************************************************************************************************
*                                              motorRotDir
*
* Description: Reverse DC power supply to rotate motor, so that it can contorl mechanical move right/left.
*
* Arguments  : right, if 1 will move to right direction.
*
* Note(s)    : none
************************************************************************************************************************
*/
void motorRotDir(int right)
{
	motorRotPwrCtrl(SPR_ON);
	if(right == SPR_RIGHT)
	{
		digitalWrite(M_ROT_DIR, HIGH);		
	}
	else
	{
		digitalWrite(M_ROT_DIR, LOW);
	}
}

/*
************************************************************************************************************************
*                                              pumpPwrCtrl
*
* Description: control pump motor power on/off.
*
* Arguments  : onOff, if 1 will turn on motor
*
* Note(s)    : none
************************************************************************************************************************
*/
void pumpPwrCtrl(int onOff)
{
	if(onOff == SPR_ON)
	{
		digitalWrite(PUMP_PWR, HIGH);
	}
	else
	{
		digitalWrite(PUMP_PWR, LOW);
	}
}


/*
************************************************************************************************************************
*                                              ParsingCmdToPB1
*
* Description: read command for pump on/off event, then clear it before return.
*
* Arguments  : EventPb1CmdTmp, 1 has a event simulink PB1 prssed
*
* Note(s)    : none
************************************************************************************************************************
*/
int ParsingCmdToPB1(void)
{
	int EventPb1CmdTmp = EventPb1Cmd;
	EventPb1Cmd = 0;
	return EventPb1CmdTmp;
}

/*
************************************************************************************************************************
*                                              ParsingCmdToPB2
*
* Description: read command for pump on/off event, then clear it before return.
*
* Arguments  : EventPb2Cmd, 1 has a event simulink PB2 prssed
*
* Note(s)    : none
************************************************************************************************************************
*/
int ParsingCmdToPB2(void)
{
	int EventPb2CmdTmp = EventPb2Cmd;
	EventPb2CmdTmp = 0;
	return EventPb2Cmd;
}

/*
************************************************************************************************************************
*                                              ParsingCmdToPumpPB
*
* Description: read command for pump on/off event, then clear it before return.
*
* Arguments  : EventPumpCmdTmp, 1 has a event reverse pump motor status
*
* Note(s)    : none
************************************************************************************************************************
*/
int ParsingCmdToPumpPB(void)
{
	int EventPumpCmdTmp = EventPumpCmd;
	EventPumpCmd = 0;
	return EventPumpCmdTmp;
}

/*
************************************************************************************************************************
*                                              ParsingCmd
*
* Description: Parsing command read from serail port, save to each global variable.
*
* Arguments  : cmd, command from serial port
*
* Note(s)    : none
************************************************************************************************************************
*/
void ParsingCmd(int cmd)
{
	EventPb1Cmd = (cmd & 0x04) >> 2;
	EventPb2Cmd = (cmd & 0x02) >> 1;
	EventPumpCmd = (cmd & 0x01) >> 0;	
}