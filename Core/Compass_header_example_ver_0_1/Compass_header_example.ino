/*	Author = helscream (Omer Ikram ul Haq) 
	Date = 2014-05-11
	Website: http://hobbylogs.me.pn/?p=17
        Location: Pakistan
	Ver: 0.1 beta
*/


#include <Wire.h>
#include "compass.h"

#define Task_t 10          // Task Time in milli seconds

int dt=0;
unsigned long t;
// Main code -----------------------------------------------------------------
void setup(){
  Serial.begin(9600);
  // Serial.print("Setting up I2C ........\n");
  Wire.begin();
  compass_x_offset = 122.17;
  compass_y_offset = 230.08;
  compass_z_offset = 389.85;
  compass_x_gainError = 1.12;
  compass_y_gainError = 1.13;
  compass_z_gainError = 1.03;
  
  compass_init(2);
  // compass_offset_calibration(3);


}

// Main loop 
// Main loop -----------------------------------------------------------------
void loop(){
  
  t = millis();
 
  float load;
 
  
  
  compass_scalled_reading();
  
  Serial.print("x = ");
  Serial.println(compass_x_scalled);
  Serial.print("y = ");
  Serial.println(compass_y_scalled);
  Serial.print("z = ");
  Serial.println(compass_z_scalled);
  

  compass_heading();
  Serial.print ("Heading angle = ");
  Serial.print (bearing);
  Serial.println(" Degree");
  
  dt = millis()-t;
  load = (float)dt/(Task_t/100);
  Serial.print ("Load on processor = ");
  Serial.print(load);
  Serial.println("%");

  
  delay(3000);
  
  
}











