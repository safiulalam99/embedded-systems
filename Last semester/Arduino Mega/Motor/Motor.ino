#include<LiquidCrystal.h>
#include<math.h>
 

LiquidCrystal Icd(37, 36, 35, 34, 33, 32);
int counter=0;
int check=0;
 
int xPin=A0;
int yPin=A1;
int swPin=18;
int xPos, yPos=0;
boolean swState=0;
bool LED_state=false;
 
void setup() {
  // put your setup code here, to run once:
  pinMode(2,  INPUT_PULLUP);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  Icd.begin(20,4);
  Serial.begin(9600);
  Icd.setCursor(0,1);
  Icd.print("-100%    0%    +100%");
 
}
 
void loop() {
 
  
  // put your main code here, to run repeatedly:
  int input=analogRead(A0);
  int input_2=(1023-analogRead(A1));
  double voltage1 = input*5.0/1023.0;
  double voltage2 = input_2*5.0/1023.0;
//  voltage(voltage1);

  control_stick(voltage1,voltage2);
  button_press();
  test();
  delay(10);
  Icd.setCursor(0,2);
  Icd.print("                    ");
  Icd.setCursor(0,3);
  Icd.print("                    ");
 
}
 
void test()
{
  xPos=analogRead(xPin);
  yPos=analogRead(yPin);
  swState= digitalRead(swPin);
 
  Serial.println("X: "+ String(xPos)+" | Y:"+String(yPos)+" | Button: " + String(swState));
  //delay(100);
  }
void voltage(int voltage1)
{
  Icd.setCursor(0,3);
  Icd.print("Voltage: ");
  Icd.print(voltage1);
  Icd.print(" V");

}
 
void control_stick(double voltage1,double voltage2)
{
  int space=round(voltage1*19/5-0.5);
  Icd.setCursor(0,2);
  for(int x=0;x<space;x++)
  {
    Icd.print(" ");
    }
  Icd.print("|");
 
  int space_2=round(voltage2*19/5-0.5);
  Icd.setCursor(0,3);
  for(int x=0;x<space_2;x++)
  {
    Icd.print(" ");
    }
  Icd.print("|");
 
}
 
void button_press()
{
  int button=2;
  int reads=digitalRead(button);
 

  if(reads==0 && check==0)
   {
     counter++;
     LED_state=!LED_state;
     check=1; 
   }
   else if(reads==1)
   {
    check=0;
    }
 
    LED(LED_state);
 
    Icd.setCursor(0,0);
    Icd.print("count: ");
    Icd.setCursor(7,0);
    Icd.print(counter);


}
 
void LED(bool LED_state)
{
  if(LED_state==true)
  {
    int val_1=analogRead(xPin);
    int val_2=1023-analogRead(yPin);
    double signal_1=analogRead(xPin)/1023.0;
    double signal_2=1-signal_1;

    //motors will be put in here, check this shit out later, should work just like the 2 led
    analogWrite(9,(val_2*signal_1)/4);
    analogWrite(10,(val_2*signal_2)/4);

    }
    else{
      analogWrite(9,0);
      analogWrite(10,0);
    }
}
