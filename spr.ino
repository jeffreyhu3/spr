/*
  DigitalReadSerial

  Reads a digital input on pin 2, prints the result to the Serial Monitor

  This example code is in the public domain.

  http://www.arduino.cc/en/Tutorial/DigitalReadSerial
*/

// digital pins pushbutton
int pb1 = 2;
int pb2 = 3;
int det_R = 4;
int det_L = 5;

int led13 = 13;

// digital pins output 
int m_ROT_DIR = 6;
int motor_PWR = 7;
int pump_PWR = 8;

// the setup routine runs once when you press reset:
void setup() {
	// initialize serial communication at 9600 bits per second:
	Serial.begin(9600);

	// make the pushbutton pins as input and internal pull-up
	pinMode(pb1, INPUT);
	digitalWrite(pb1, HIGH); 
	pinMode(pb2, INPUT);
	digitalWrite(pb2, HIGH); 
	pinMode(det_R, INPUT);
	digitalWrite(det_R, HIGH); 
	pinMode(det_L, INPUT);
	digitalWrite(det_L, HIGH); 
	
	// make the pushbutton pins as output for relay on/off control and initial it
	pinMode(m_ROT_DIR, OUTPUT ); 
	digitalWrite(m_ROT_DIR, LOW); 
	pinMode(motor_PWR, OUTPUT ); 
	digitalWrite(motor_PWR, LOW);
	pinMode(pump_PWR, OUTPUT ); 
	digitalWrite(pump_PWR, LOW);

	pinMode(led13, OUTPUT ); 
	digitalWrite(led13, LOW);

	digitalWrite(motor_PWR, HIGH);
	
}

int result = 0;
// the loop routine runs over and over again forever:
void loop() {
	
	Serial.println(result++);
	digitalWrite(led13, LOW);	
	digitalWrite(pump_PWR, LOW); 
	digitalWrite(m_ROT_DIR, LOW); 
	delay(1500);        // delay in between reads for stability
	digitalWrite(led13, HIGH);	
	digitalWrite(pump_PWR, HIGH); 
	digitalWrite(m_ROT_DIR, HIGH); 
	delay(1500);        // delay in between reads for stability
}
