/*

Air quality Meter 
The circuit:
 * LCD RS pin to digital pin 12
 * LCD Enable pin to digital pin 10
 * LCD D4 pin to digital pin 7
 * LCD D5 pin to digital pin 6
 * LCD D6 pin to digital pin 5
 * LCD D7 pin to digital pin 4
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
 * 

Autor:    zhp@126.com
Website:  http://www.hellophp.cn
Weibo:    http://weibo.com/superengine
Twitter:  https://twitter.com/Mfrbee
*/


#include <SoftwareSerial.h>
#include <LiquidCrystal.h>


SoftwareSerial pm25Serial(2, 3); // RX, TX  ,PIN2,3 ,3 IS Empty now
//170,0,16,0,73,89,255

int data[7];
int data_index=0;

LiquidCrystal lcd(12, 10, 7, 6, 5, 4);


void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(57600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  lcd.begin(16, 2);
  // Print a message to the LCD.
  lcd.print("LX 18FE6C7 PM2.5");
  

  // set the data rate for the SoftwareSerial port
  pm25Serial.begin(2400);

}

void loop() { // run over and over
  
  fill_pm25_queue();
  
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print(String(read_pm25())+" ug/m3");
  
  //Serial.println("PM2.5 :"+String(read_pm25()));
  //debug_read_pm25();
  delay(500);
  
}

void fill_pm25_queue()
{
   while (pm25Serial.available()) {
     int intbit=pm25Serial.read();
     data[data_index]=intbit;
     data_index=(data_index+1)%7;
     if(data_index==0) break; //new data frame ready, release the control priv   
   }
}


float read_pm25()
{
    int start_idx=0;
    for(int i=0;i<7;i++)
    {
      // first 255,second 170 for ready data frame
      if(data[i]==255 && data[(i+1)%7]==170)
      {
       start_idx=(i+2)%7;
       break;
      }
    }

    long vout_h=data[start_idx];
    long vout_l=data[(start_idx+1)%7];
    long vref_h=data[(start_idx+2)%7];
    long vref_l=data[(start_idx+3)%7];
    long v_check=data[(start_idx+4)%7];
    float A=550; //Magic  number
    double ucd=((vout_h*256+vout_l)*5*A)/1024;
    return ucd;
    
}


void debug_read_pm25()
{
  for(int i=0;i<7;i++)
  {
      Serial.print(String(data[i])+",");
  }
  Serial.println("idx:"+String(data_index));
}

