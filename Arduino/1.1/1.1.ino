#include <LiquidCrystal.h>
#include <Wire.h>
#include <LIDARLite.h>
#include <math.h>

#define CMPS14_address 0x60   //for compass 

LIDARLite myLidar;  //for lidar

LiquidCrystal lcd(37, 36, 35, 34, 33, 32);

//joystick pin
int xPin = A0;
int yPin = A1;
int swPin = 18;

//button
int buttonState = 0;
bool taken = false;

//move stuffs
bool gobackward = false;
int move_distance = 1;
bool buttonPress = false;

//compass stuff
int angle = 0;

//motors
#define Dir_left    7
#define Dir_right   8
#define Power_left  9
#define Power_right 10
#define forw LOW
#define back HIGH
int motor_speed = 200;
int pulse_right = 0;
int pulse_left = 0;
int previousRight = 0;
int previousLeft = 0;

//keep a certain distance from an object
int adjDis = 30;
bool done = true;

//set path have to follow
int path = 0;

//for compass when to turn
bool turnBool = false;
double raw;
int turnMode = 0; //0 straight, 1 turn left, 2 turn right

//"turn" command
void compassControl(int intent);

//transfer data stuff
void distanceAndCompass(){
	angle = compass();
	angle = angle / 255 * 360;
	Serial.println(angle);


}

//controlling the bot using a set of values
bool runState = true; //make the bot run at the first run
bool analogControl(long distance, int motor_speed, bool gobackward){
  	//going to the front or back
  	if (distance > 0) gobackward = false;  
  	else {
    	gobackward = true;
    	distance = -distance;
  	}

  	//measured, for each 13.37 pulse from motor we moved 10mm. 
  	//after moving a certain distance., stop
  	if (previousRight < round(13.37 * distance) && previousLeft < round(13.37 * distance)){    //10mm = 13.37*10
    	runState = true;
    	digitalWrite(Dir_right, gobackward);
    	analogWrite(Power_right, motor_speed);
    	digitalWrite(Dir_left, gobackward);
    	analogWrite(Power_left, motor_speed);
  	} else {
    	previousRight = 0;
    	previousLeft = 0;
    	buttonPress = false;
    	runState = false;
  	}
  	return runState;
}

void setup(){
	pinMode(10, OUTPUT);
	pinMode(9, OUTPUT);
  	pinMode(8, OUTPUT);
  	pinMode(7, OUTPUT);
  	pinMode(6, OUTPUT);
	pinMode(swPin, INPUT);
	pinMode(2, INPUT);
	pinMode(3, INPUT);
	pinMode(22, INPUT);
  	pinMode(24, INPUT);
	attachInterrupt(digitalPinToInterrupt(2), isr_right, RISING); //counting pulse from right motor
	attachInterrupt(digitalPinToInterrupt(3), isr_left, RISING);  //counting pulse from left motor

	Serial.begin(9600);
	Serial.println("Lidar booting up");

	lcd.begin(20, 4);

	myLidar.begin(0, true); 
	//Default configuration(0) and I2C frequency to 400kHz(true = 400/false = 100)

	myLidar.configure(1); 
	//Better accuracy close range
}

bool ui = true;

void loop(){
	/*
	* For lidar setting 
	*/
	if (ui == true){
		int lidarValueTrue = disAve(10);
		showInfo(lidarValueTrue);
	} else {
		showInfoPanel();
	}

	/*
	* Motor controlled by joystick
	*/
	double joystickX = analogRead(xPin);
	double joystickY = 1023 - analogRead(yPin);
	motor(joystickX, joystickY);
	
	//whenever the button is pressed, buttonPress switch to true => commit this code
	while (buttonPress){  
    	while(analogControl(move_distance, motor_speed, gobackward)); //control the bot with 3 known parameters
    	//compass_control(); //not implemented on the board at the moment, but this function is intended to control the bot using the potential meters
  	}
	/*
	* Calculate volume and control bot using serial;
	*/
	serialControl();
}

//adjust distance
void adjDistance(){
	done = false;
	int lidarValue;
	while (done == false){
		lidarValue = disAve(10);
		showInfoPanel();
		if (lidarValue > adjDis + 1){
		digitalWrite(Dir_left, forw);
		digitalWrite(Dir_right, forw);
		analogWrite(Power_left, 200);
		analogWrite(Power_right, 200);
		} else if (lidarValue < adjDis - 1){
		digitalWrite(Dir_left, back);
		digitalWrite(Dir_right, back);
		analogWrite(Power_left, 200);
		analogWrite(Power_right, 200);
		} else {
		done = true;
		analogWrite(Power_left, 0);
		analogWrite(Power_right, 0);
		}
	}
}

void serialControl(){
	if (Serial.available() > 0){
		String message = Serial.readStringUntil('\n');
		String stat;
		int colon, value; 
		int lcdPos = message.indexOf("LCD");
		int printPos = message.indexOf("Print");
		int area = message.indexOf("Area");
		int volume = message.indexOf("Volume");
		int panel = message.indexOf("Panel");
		int adjusted = message.indexOf("Adjust");
		int path = message.indexOf("Path");
		int turn = message.indexOf("Turn");
		int preset = message.indexOf("Run");
		int movePos = message.indexOf("Move");

		if (movePos > -1){
      		colon = message.indexOf(":");
      		if (colon > -1){
        		stat = message.substring(colon + 1);
        		move_distance = stat.toInt();
        		gobackward = true;
        		buttonPress = true;
        		Serial.print("Move :");  Serial.println(move_distance);
      		}
    	}

		if (preset > -1){
			colon = message.indexOf(":");
			if (colon > -1){
				stat = message.substring(colon + 1);
				value = stat.toInt();
				if (value == 1){
					Serial.println("Run 1");
					adjDis = 25;
					adjDistance();
					Serial.println("continue 1");
					turnBool = true;
					compassControl(-90);
					Serial.println("continue 2");
					adjDis = 20;
					adjDistance();
					turnBool = true;
					compassControl(-88);
					adjDis = 20;
					adjDistance();        
				}
				if (value == 2){
					adjDis = 25;
					adjDistance();
					turnBool = true;
					compassControl(-130);
					adjDis = 20;
					adjDistance();
					turnBool = true;
					compassControl(140);
					adjDis = 20;
					adjDistance();
				}
			}
		}

		if (turn > -1){
			colon = message.indexOf(":");
			if (colon > -1){
				stat = message.substring(colon + 1);
				value = stat.toInt();
				Serial.print("Turn: "); Serial.println(value);
				turnBool = true;
				compassControl(value); 
			}
		}

		if (path > -1){
			colon = message.indexOf(":");
			if (colon > -1){
				stat = message.substring(colon + 1);
				value = stat.toInt();
				if (value == 1){
					path = 1;
					Serial.println("Path 1");
				} else if (value == 2){
					path = 2;
					Serial.println("Path 2");
				} else {
					Serial.println("Please set path to 1 or 2");
				}
			}
		}

		if (adjusted > -1){
			colon = message.indexOf(":");
			if (colon > -1){
				stat = message.substring(colon + 1);
				adjDis = stat.toInt();
				Serial.print("Adjusted distance = "); Serial.println(adjDis);
			}
		}
		
		if (panel > -1){
			ui = !ui;
			lcd.clear();
		}

		if (area > -1){
			lcd.setCursor(0, 3);
			lcd.print("                    ");
					//01234567890123456789
					
			int x, y;
			Serial.println("Keep the robot in one direction then wait for 3 seconds");
			delay(3000);
			x = myLidar.distance(true);
			Serial.println(x);
			Serial.println("Keep the robot in another direction then wait for 3 seconds");
			delay(3000);
			y = myLidar.distance(true);
			Serial.println(y);
			x = x * y;
			double area = x / 100;
			lcd.setCursor(0, 3);
			lcd.print("Area: ");      lcd.print(area);    lcd.print(" ");   lcd.print("m^2"); 
		}

		if (volume > -1){
			lcd.setCursor(0, 3);
			lcd.print("                    ");
					//01234567890123456789
					
			int x, y, z;
			Serial.println("Keep the robot in one direction then wait for 3 seconds");
			delay(3000);
			x = myLidar.distance(true);
			Serial.println(x);
			Serial.println("Keep the robot in another direction then wait for 3 seconds");
			delay(3000);
			y = myLidar.distance(true);
			Serial.println(y);
			Serial.println("Keep the robot in another direction then wait for 3 seconds");
			delay(3000);
			z = myLidar.distance(true);
			Serial.println(z);
			x = x * y * z;
			double volume = x / 1000000;
			lcd.setCursor(0, 3);
			lcd.print("Volume: ");      lcd.print(volume);    lcd.print(" ");   lcd.print("m^3");
		}

		if (lcdPos > -1){
			colon = message.indexOf(":");
			if (colon > -1){
				stat = message.substring(colon + 1);
				lcd.setCursor(0,0);
				lcd.print("                    ");
				lcd.setCursor(0,0);
				lcd.print(stat);  
				Serial.println("Print to LCD");
			}
		}

		if (printPos > -1){
			colon = message.indexOf(":");
			if (colon > -1){
				stat = message.substring(colon + 1);
				Serial.print("Print = ");  Serial.println(stat);  
			}
		}

	}  
}

void showInfo(int lidar){
	//Print to Serial
	lcd.setCursor(0, 0);
	lcd.print("                    ");
	lcd.setCursor(2, 0);
	lcd.print("Distance ");
		//01234567890123456789
	lcd.setCursor(0, 2);
	lcd.print("20    40    60    80");
			//01234567890123456789

	//Print to LCD with a bar to determine the distance
	//Print raw value to LCD
	lcd.setCursor(11, 0);
	//if i actually want more accuracy
	//double actual = lidar*1-15.4;
	//actual = round(actual);
	//lcd.print((int)actual);
	lcd.print(lidar);
	lcd.print("  ");
	lcd.print("cm");
	lcd.print("  ");
	//Print the asterisk based on a metric
	int spaceAmount = 0;
	//lcd.setCursor(0, 0);
	//lcd.print("                    ");
	lcd.setCursor(0, 1);
	lcd.print("                    ");
			//01234567890123456789

	if (lidar <= 20){
		spaceAmount = 0;
	} else 
	if (lidar >= 80){
		spaceAmount = 19;
	} else {
		//18 positions for 59 spaces (21 to 79)
		spaceAmount = round(0.29 * lidar - 6.16);
	}

	lcd.setCursor(spaceAmount, 1);
	lcd.print("*");
}

//for various other exercise
void showInfoPanel(){
	//lcd.clear();
	lcd.setCursor(0, 0);
	lcd.print("                    ");
	lcd.setCursor(0, 0);
	lcd.print("Adj. dist ");
	lcd.print(adjDis);
	lcd.print(" cm");

	int measureDistance = disAve(10);
	lcd.setCursor(0, 1);
	lcd.print("                    ");
	lcd.setCursor(0, 1);
	lcd.print("Meas. dist ");
	lcd.print(measureDistance);

	lcd.setCursor(0, 2);
	lcd.print("                    ");
	lcd.setCursor(0, 2);
	lcd.print("Motor stat ");
	//         012345678901234567890

	lcd.setCursor(0, 3);
	lcd.print("                    ");
	lcd.setCursor(0, 3);
	lcd.print("Dire: ");  
	//convert it to 0-360 degree, not accurate but here it is
	double dir = compass() / 255 * 360;
	showCompass((int)dir);
}

//show compass infomration
void showCompass(int dir){
	if (dir > 294 && dir <= 338) lcd.print("NW");
	if (dir > 338 && dir <= 360) lcd.print("N ");
	if (dir >= 0 && dir <= 23) lcd.print("N ");
	if (dir > 23 && dir <= 67) lcd.print("NE");
	if (dir > 67 && dir <= 112) lcd.print("E ");
	if (dir > 112 && dir <= 157) lcd.print("SE");
	if (dir > 157 && dir <= 202) lcd.print("S ");
	if (dir > 202 && dir <= 247) lcd.print("SW");
	if (dir > 247 && dir <= 294) lcd.print("W ");
}

/*
* Controlling using joystick
*/
void motor(double x, double y){ 
	//control bot using joystick

	//convert analog value from joystick to motor speed value(0-255)
	//7 and 8 is direction
	//9 and 10 is power
	double power_L = x * 254 / 1023 - 127;
	double power_R = - power_L;
	double power_Y = y * 254 / 1023 - 127;
	//maths to change determined whether the car is shifting to left or right
	double left = power_L + power_Y;
	double right = power_R + power_Y;  

	if (left > 0){
		digitalWrite(Dir_left, forw);
	}
	if (right > 0){
		digitalWrite(Dir_right, forw);
	}
	if (left < 0){
		digitalWrite(Dir_left, back);
		left = -left;
	}
	if (right < 0){
		digitalWrite(Dir_right, back);
		right = -right;
	}
	analogWrite(Power_left, left);
	analogWrite(Power_right, right);
}

//Average distance
int disAve(int mean){
	if (mean <= 0) return -1;
	else {
		static unsigned int n = 0;
		long sum = 0;
		sum = myLidar.distance(true);
		n = 1;
		for (int i = 0; i < mean; i++){
		if (n > 100){
			n = 1;
			sum += myLidar.distance(true);
		} else {
			n++;
			sum += myLidar.distance();
		}
		}
		sum = sum / mean;
		
		//making it somewhat more accurate
		double actual = sum * 1 - 15.4;
		actual = round(actual);
			
		return (int) actual;
	}
}

//example code from moodle.
long compass(){
	Wire.begin();

	Wire.beginTransmission(CMPS14_address);

	Wire.write(1);

	Wire.endTransmission(false);

	Wire.requestFrom(CMPS14_address,1,true);

	if (Wire.available() >= 1){
		raw = Wire.read();    
	}

	return raw;
}

//controll the bot with compass
void compassControl(int intent){
	//intent is intention to where it should be going
	//face is the current direction
	//goal is how it should actually facing
	int face = compass();
	intent = round(intent * 128 / 180);
	int goal = face + intent;
	Serial.print(face); Serial.print("   ");
	Serial.println(goal);

	while(turnBool){
		int current = compass();
		if (intent > 0){
		digitalWrite(Dir_left, forw);
		digitalWrite(Dir_right, back);
		analogWrite(Power_left, 150);
		analogWrite(Power_right, 150);
			if (goal < 256){
				if (current > goal){
				analogWrite(Power_left, 0);
				analogWrite(Power_right, 0);
				turnBool = false;
				}
			}
			if (goal > 255){
				if (current < 128 && current > (goal - 255)){
				analogWrite(Power_left, 0);
				analogWrite(Power_right, 0);
				turnBool = false;
				}
			}
		}

		if (intent < 0){
		digitalWrite(Dir_left, back);
		digitalWrite(Dir_right, forw);
		analogWrite(Power_left, 150);
		analogWrite(Power_right, 150);
		if (goal > 0){
			if (current < goal){
			analogWrite(Power_left, 0);
			analogWrite(Power_right, 0);
			turnBool = false;
			}
		}
		if (goal < 0){
			if (current > 127 && current < (goal + 255)){
			analogWrite(Power_left, 0);
			analogWrite(Power_right, 0);
			turnBool = false;
			}
		}
		}
	}
}

//count the pulses of both motors
void isr_right(){
	pulse_right++;
	previousRight++;
}

void isr_left(){
	pulse_left++;
	previousLeft++;
}
