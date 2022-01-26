// Lidar_tester_01: Simple test program for Garmin Lidar Lite v3
// Modified from arduino_cmps12_i2c.ino, CMPS12 I2C example for Arduino by James Henderson 2014  
// TS 27.1.2021



#define USE_LCD 0

#include <LiquidCrystal.h>                            // include LCD library

#include <TimerOne.h>
#include <Wire.h>
#include <LIDARLite.h>

LIDARLite myLidarLite;

//                RS  E   D4  D5  D6  D7
LiquidCrystal lcd(37, 36, 35, 34, 33, 32);            // LCD pin wiring settings


int dist_r(void);


void setup()
{
  Serial.begin(9600);

  Serial.println("++++++++++++++++++++++++++++++++++++");
  Serial.println("+++ Lidar_tester_01: booting UP  +++");
  Serial.println("++++++++++++++++++++++++++++++++++++");

  lcd.begin(20, 4);                     // Display size defination 20 char  4 row
  //         012345678901234567890
  lcd.print("+++++++++++++++++++++");   // Print to lCD
  lcd.setCursor(0, 1);                  // Set cursor to beginning of 2. row
  lcd.print("++ Lidar_tester_01 ++");
  lcd.setCursor(0, 2);
  lcd.print("++ TAMK 2021 ++++++++");
  lcd.setCursor(0, 3);
  lcd.print("+++++++++++++++++++++");

  delay(1000);

  /*
     begin(int configuration, bool fasti2c, char lidarliteAddress)

     Starts the sensor and I2C.

     Parameters
     ----------------------------------------------------------------------------
     configuration: Default 0. Selects one of several preset configurations.
     fasti2c: Default 100 kHz. I2C base frequency.
       If true I2C frequency is set to 400kHz.
     lidarliteAddress: Default 0x62. Fill in new address here if changed. See
       operating manual for instructions.
  */
  myLidarLite.begin(0, true); // Set configuration to default and I2C to 400 kHz

  /*
    configure(int configuration, char lidarliteAddress)

    Selects one of several preset configurations.

    Parameters
    ----------------------------------------------------------------------------
    configuration:  Default 0.
    0: Default mode, balanced performance.
    1: Short range, high speed. Uses 0x1d maximum acquisition count.
    2: Default range, higher speed short range. Turns on quick termination
        detection for faster measurements at short range (with decreased
        accuracy)
    3: Maximum range. Uses 0xff maximum acquisition count.
    4: High sensitivity detection. Overrides default valid measurement detection
        algorithm, and uses a threshold value for high sensitivity and noise.
    5: Low sensitivity detection. Overrides default valid measurement detection
        algorithm, and uses a threshold value for low sensitivity and noise.
    lidarliteAddress: Default 0x62. Fill in new address here if changed. See
    operating manual for instructions.
  */
  myLidarLite.configure(1); // Change this number to try out alternate configurations
}

///////////////////////////////////////////////////////////////////////////////////////////////////////
////
////    LOOP LOOP LOOP
////
///////////////////////////////////////////////////////////////////////////////////////////////////////
void loop()
{
  Serial.println("Entered loop().");

  Serial.print("Single dist. cm = ");
  Serial.println(dist_ave(1));
  delay(2000);

  while (true)
  {
    Serial.print("Avg. dist. cm = ");
    Serial.println(dist_ave(5));

    lcd.setCursor(0, 1);                                // set curosor to left upper corner
    //         012345678901234567890
    lcd.print("                    ");                  // print to lCD

    lcd.setCursor(0, 1);                                // set curosor to left upper corner
    //         012345678901234567890
    lcd.print("Dist = ");                  // print to lCD
    lcd.print(dist_ave(1));                  // print to lCD

    // delay(200);
  }
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////
///
///    END LOOP END LOOP END LOOP
///
///////////////////////////////////////////////////////////////////////////////////////////////////////////
int dist_ave(int n_meas)
{
  if (n_meas <= 0) return -1;
  else
  {
    static unsigned int n = 0;
    long sum = 0;
    
    sum = myLidarLite.distance(true); // First measurement always with calibration
    n = 1;   
  
    for (int i = 1; i < n_meas; i++)
    {
      if (n >= 100) // Make a calibrated measurement after avery 100 measurements
      {
        sum += myLidarLite.distance(true);
        n = 1;
      }
      else
      {
        sum += myLidarLite.distance();
        n++;
      }
    }

    sum = sum / n_meas;
    //Serial.println(sum);
    return (int) sum;
  }
}
