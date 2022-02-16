#include<LiquidCrystal.h> 
#include<math.h>
#include<Wire.h>

#define CMPS14_address 0x60   //for compass 

 

LiquidCrystal lcd(37, 36, 35, 34, 33, 32);
int counter=0;
int check=0;
 
int xPin=A0;
int yPin=A1;
int swPin=18;
int xPos, yPos=0;
boolean swState=0;
int pulse_right=0;
int pulse_left=0;
int previousRight=0;
int previousLeft=0;
int checkMove=0;
int previousCounter=0;
int motor_speed = 200;
bool buttonPress = false;
bool gobackward = false;
int move_distance = 1;
bool turnBool = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(10, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(2, INPUT);//direction
  pinMode(3, INPUT);//speed of left
  pinMode(22, INPUT);
  pinMode(24, INPUT);
  pinMode(swPin,  INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), isr_right, RISING); //counting pulse from right motor
  attachInterrupt(digitalPinToInterrupt(3), isr_left, RISING);  //counting pulse from left motor
  lcd.begin(20,4);  //start LCD screen
  Serial.begin(9600); //start Serial monitor
  Serial.println("Write something to the serial monitor.");
}

double raw, dir;

void loop() {  
  // put your main code here, to run repeatedly:
  double input_x = analogRead(A0);  //reading signal from x-asix joystick, running from 0-1024
  double input_y = (1023 - analogRead(A1)); //reading signal from y-axis joystick, running from 0-1024, adjusted so that it can be inverted 
  
  motor(input_x, input_y);  //controlling bot from joystick
  button_press();  //using button from joystick, changing buttonPress to true
  serial_control(); //controlling bot from serial
  while (buttonPress){  //whenever the button is pressed, buttonPress switch to true => commit this code
    while(analogControl(move_distance, motor_speed, gobackward)); //control the bot with 3 known parameters
    //compass_control(); //not implemented on the board at the moment, but this function is intended to control the bot using the potential meters
  }
  showS();  //show bot information on LCD screen
}

#define Dir_left    7
#define Dir_right   8
#define Power_left  9
#define Power_right 10
#define forw LOW
#define back HIGH

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

//functions not used for anything at the moment
int check_speed(double x, double y){
  if (x + y > 255){
    return 255;
  }
  if (x + y < -255){
    return -255;
  }
  if (x + y < 255 && x + y > -255){
    return x + y;
  }
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

//while other measures can be implemented to make it work, i choose to use a function
//so that i make what the bot will do in each states of button, and make it doesnt press 
//in a single push coninuously
void button_press()
{
  int reads=digitalRead(swPin); 
  
  if(reads==0 && check==0)
   {
     counter++;
     check = 1;
     swState = 1;
     buttonPress = true;
   }
   else if(reads == 1)
   {
     check = 0;
     swState = 0;
   }   
}

//just a normal lcd screen function
void showS(){
  lcd.setCursor(0, 2);
  lcd.print("Raw ");
  lcd.print(compass());
  lcd.print("   ");
  lcd.setCursor(0, 3);
  lcd.print("Converted: ");
  dir = compass();
  dir = dir / 255 * 360;
  if (dir > 294 && dir <= 338) lcd.print("NW");
  if (dir > 338 && dir <= 360) lcd.print("N ");
  if (dir >= 0 && dir <= 23) lcd.print("N ");
  if (dir > 23 && dir <= 67) lcd.print("NE");
  if (dir > 67 && dir <= 112) lcd.print("E ");
  if (dir > 112 && dir <= 157) lcd.print("SE");
  if (dir > 157 && dir <= 202) lcd.print("S ");
  if (dir > 202 && dir <= 247) lcd.print("SW");
  if (dir > 247 && dir <= 294) lcd.print("W ");
  lcd.print("  ");
  lcd.print(round(dir));
  lcd.print(" ");
}

//example code from moodle, adjusted the global raw value.
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

//not implemented at the moment, use joystick to control the direction the bot is supposed
//to be facing, but could also work for a potential meter by changing the pin 
void compass_control(){
  double input_x = analogRead(A0);
  double input_y = (1023 - analogRead(A1));
  long intended_dir = input_x * 360 / 1024;
  long dir = compass();
  if (dir > (intended_dir)){
    motor(200, input_y);
  }
  if (dir < intended_dir){
    motor(800, input_y);
  }
  if (dir == intended_dir || dir == intended_dir + 1 || dir == intended_dir - 1) motor(511, input_y);
  lcd.setCursor(0,0);
  lcd.print("Go to: ");
  Serial.println(intended_dir);
  lcd.print(intended_dir);
  lcd.print("   ");
}

//controlling the bot with the steering wheel on the web server hosted by ESP
//first, get the initual value of the direcion the bot is facing
//then compare it to the direction the bot have to face
//to decide the bot has to spin left or right
//until facing the value it is needed to face
void wheel_control(int intent){
  int init = compass();  
  intent = round(intent * 128 / 180);
  int goal = init + intent;
  Serial.println(goal);
  while (turnBool){
    int current = compass();
    if (intent > 0){
      digitalWrite(Dir_left, forw);
      digitalWrite(Dir_right, back);
      analogWrite(Power_left, 100);
      analogWrite(Power_right, 100);
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
      analogWrite(Power_left, 100);
      analogWrite(Power_right, 100);
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

//Serial controlling the bot, ESP and webserver will also use this fuction to control the bot
//it checked for what is written on the screen with a certain formula, then convert the value 
//behind the command and pass it to the functions above to make them work
void serial_control(){
  if (Serial.available() > 0){
    String message = Serial.readStringUntil('\n');
    String stat;
    int colon, value; 
    int movePos = message.indexOf("Move");
    int dirPos = message.indexOf("Turn");
    int lcdPos = message.indexOf("LCD");
    int printPos = message.indexOf("Print");
    
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

    if (dirPos > -1){
      colon = message.indexOf(":");
      if (colon > -1){
        stat = message.substring(colon + 1);
        value = stat.toInt();
        Serial.print("Turn: "); Serial.println(value);
        turnBool = true;
        wheel_control(value);        
      }
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

//count the pulses of both motors
void isr_right(){
  pulse_right++;
  previousRight++;
}

void isr_left(){
  pulse_left++;
  previousLeft++;
}
