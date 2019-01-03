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

#define LED13  		13

// digital pins output 
#define M_ROT_DIR  	6
#define M_ROT_DIR  	7
#define PUMP_PWR 	8

#define SPR_SET 	1
#define SPR_CLR 	0

#define LOCATION_SENSOR_HELD_CNT 	2
#define MAIN_LOOP_CYCLE 			20


// Global variable
int EventPb1;
int EventPb2;
int EventDET_R;	
int EventDET_L;	
unsigned long currentMillis;
unsigned long previousMillis;

void readPushButton(void);
void readLocationSensor(void);
int readCmd(void);
void processEvent(void);

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
	
	// make the pushbutton pins as output for relay on/off control and initial it
	pinMode(M_ROT_DIR, OUTPUT ); 
	digitalWrite(M_ROT_DIR, LOW); 
	pinMode(M_ROT_DIR, OUTPUT ); 
	digitalWrite(M_ROT_DIR, LOW);
	pinMode(PUMP_PWR, OUTPUT ); 
	digitalWrite(PUMP_PWR, LOW);

	pinMode(LED13, OUTPUT ); 
	digitalWrite(LED13, LOW);

	digitalWrite(M_ROT_DIR, HIGH);
	
}

void loop()
{
	currentMillis = millis();	
	if ((currentMillis - previousMillis) > MAIN_LOOP_CYCLE)
	{
		//Serial.println(currentMillis);
		previousMillis = previousMillis + MAIN_LOOP_CYCLE;
		readPushButton();
		readLocationSensor();
		processEvent();
	}
	else
	{
		readCmd();		
	}
}

void readPushButton()
{
	static int curPb1 = 0;	//current pb1 status
	static int prePb1 = 0;	//previous value
	static int curPb2 = 0;
	static int prePb2 = 0;
	
	curPb1 = digitalRead(PB1);
	curPb2 = digitalRead(PB2);
	
	if(curPb1 != prePb1 && curPb1 == SPR_CLR)	// if pb1 pressed
	{
		Serial.println("PB1 press");	
		EventPb1 = SPR_SET;
	}
	prePb1 = curPb1;								//save current value to history variable
	
	if(curPb2 != prePb2 && curPb2 == SPR_CLR)
	{
		Serial.println("PB2 press");	
		EventPb2 = SPR_SET;
	}
	prePb2 = curPb2;
}

void readLocationSensor()
{
	static int curLocationR = 0;	//current location right status
	static int PreLocationR = 0;	//previous location right status
	static int curLocationL = 0;
	static int PreLocationL = 0;
	static int LocationRcnt = 0;	//count the time for location sensor hold
	static int LocationLcnt = 0;
	
	LocationRcnt++;
	LocationLcnt++;	
	
	curLocationR = digitalRead(DET_R);
	curLocationL = digitalRead(DET_L);
	
	if(curLocationR != PreLocationR && curLocationR == SPR_CLR)	// if location Right sensor pressed
	{
		Serial.println("EventDET_R press");	
		EventDET_R = SPR_SET;
	}	
	PreLocationR = curLocationR;								//save current value to history variable
	
	if(curLocationL != PreLocationL && curLocationL == SPR_CLR)
	{
		Serial.println("EventDET_L press");	
		EventDET_L = SPR_SET;
	}
	PreLocationL = curLocationL;
}

int readCmd()
{
	String strInput;
	int cmdTmp = 0;
	int cmd = 0;
		
	if (Serial.available() > 0)
	{
		strInput = Serial.readString();
		Serial.print("strInput:");
		Serial.println(strInput);
		if(strInput.endsWith("\r"))
		{
			Serial.print("strInput r:");
			Serial.println(strInput);
			cmdTmp = strInput.toInt();
			Serial.print("cmdTmp :");
			Serial.println(cmdTmp, DEC);
						
				
			cmdTmp = strInput.toInt();	
			cmd |= (cmdTmp/100 > 0 ? 1 : 0) << 2;
			cmd |= (cmdTmp%100/10 > 0 ? 1 : 0) << 1;
			cmd |= (cmdTmp%100%10 > 0 ? 1 : 0) << 0;
			Serial.print("cmd: ");
			Serial.println(cmd, HEX);
		}
	}
	
	return cmd;
}

void processEvent()
{
	//Serial.println("processEvent");
}

/*
	Serial.println(result++);
	digitalWrite(LED13, LOW);	
	digitalWrite(PUMP_PWR, LOW); 
	digitalWrite(M_ROT_DIR, LOW); 
	delay(1500);        // delay in between reads for stability
	digitalWrite(LED13, HIGH);	
	digitalWrite(PUMP_PWR, HIGH); 
	digitalWrite(M_ROT_DIR, HIGH); 
	delay(1500);        // delay in between reads for stability
*/