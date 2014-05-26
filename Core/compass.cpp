/*	Author = helscream (Omer Ikram ul Haq) 
	Date = 2014-05-11
	Website: http://hobbylogs.me.pn/?p=17
        Location: Pakistan
	Ver: 0.1 beta
*/


#include <Arduino.h>
#include <Wire.h>
#include "compass.h"




// Definitions ----------------------------------------------------------------
#define compass_address 0x1E       // The I2C address of the Magnetometer
#define compass_XY_excitation 1160 // The magnetic field excitation in X and Y direction during Self Test (Calibration)
#define compass_Z_excitation 1080  // The magnetic field excitation in Z direction during Self Test (Calibration)
#define compass_rad2degree 57.3


#define compass_cal_x_offset 116   // Manually calculated offset in X direction
#define compass_cal_y_offset 225   // Manually calculated offset in Y direction
#define compass_cal_x_gain 1.1     // Stored Gain offset at room temperature
#define compass_cal_y_gain 1.12    // Stored Gain offset at room temperature

// Global Variables ----------------------------------------------------------
float compass_x_offset=0, compass_y_offset=0, compass_z_offset=0,compass_gain_fact=1,compass_x_scalled,compass_y_scalled,compass_z_scalled;
float compass_x_gainError=1,compass_y_gainError=1,compass_z_gainError=1,bearing=0;
int compass_x=0,compass_y=0,compass_z=0;






//***************************************************************************
//***************************************************************************
//****************************  FUNCTIONS    ********************************
//***************************************************************************
//***************************************************************************

// --------------------------------------------------------------------------
// read the data 
// This function updates the Global X Y Z Variables

void compass_read_XYZdata(){

  Wire.beginTransmission(compass_address);
  Wire.write(0x02);
  Wire.write(0b10000001); 
  // Writing the register value 0000 0000 for continous mode
  // Writing the register value 0000 0001 for single
  Wire.endTransmission();
  Wire.requestFrom(compass_address,6);

  if (6 <= Wire.available()){

    compass_x = Wire.read()<<8 | Wire.read();
    

    compass_z = Wire.read()<<8 | Wire.read(); 
    

    compass_y = Wire.read()<<8 | Wire.read(); 
    
  }

}


// --------------------------------------------------------------------------
// This Function calculates the offset in the Magnetometer
// using Positive and Negative bias Self test capability
// This function updates X_offset Y_offset and Z_offset Global variables
// Call Initialize before 
void compass_offset_calibration(int select){
  // ***********************************************************
  // offset_calibration() function performs two taskes
  // 1. It calculates the diffrence in the gain of the each axis magnetometer axis, using 
  //    inbuilt self excitation function of HMC5883L (Which is useless if it is used as a compass
  //    unless you are very unlucy and got a crapy sensor or live at very High or low temperature)
  // 2. It calculates the mean of each axes magnetic field, when the Magnetometer is rotated 360 degree
  // 3. Do Both
  // ***********************************************************    
  
  
  // *****************************************************************************************
  // Gain offset estimation
  // ***************************************************************************************** 
  if (select == 1 | select == 3){ // User input in the function
 // Configuring the Control register for Positive Bais mode
  Serial.println("Calibrating the Magnetometer ....... Gain");
  Wire.beginTransmission(compass_address);
  Wire.write(0x00);
  Wire.write(0b01110001); // bit configuration = 0 A A DO2 DO1 DO0 MS1 MS2

  /*
  A A                        DO2 DO1 DO0      Sample Rate [Hz]      MS1 MS0    Measurment Mode
   0 0 = No Average            0   0   0   =   0.75                   0   0   = Normal  
   0 1 = 2 Sample average      0   0   1   =   1.5                    0   1   = Positive Bias
   1 0 = 4 Sample Average      0   1   0   =   3                      1   0   = Negative Bais
   1 1 = 8 Sample Average      0   1   1   =   7.5                    1   1   = -
   1   0   0   =   15 (Default)
   1   0   1   =   30
   1   1   0   =   75
   1   1   1   =   -
   */
  Wire.endTransmission();
  
  compass_read_XYZdata(); // Disregarding the first data

  // Reading the Positive baised Data
  while(compass_x<200 | compass_y<200 | compass_z<200){   // Making sure the data is with Positive baised
     compass_read_XYZdata();
  }
  
  compass_x_scalled=compass_x*compass_gain_fact;
  compass_y_scalled=compass_y*compass_gain_fact;
  compass_z_scalled=compass_z*compass_gain_fact;
  
  
  // Offset = 1160 - Data_positive
  compass_x_gainError = (float)compass_XY_excitation/compass_x_scalled;
  compass_y_gainError = (float)compass_XY_excitation/compass_y_scalled;
  compass_z_gainError = (float)compass_Z_excitation/compass_z_scalled;


  // Configuring the Control register for Negative Bais mode
  Wire.beginTransmission(compass_address);
  Wire.write(0x00);
  Wire.write(0b01110010); // bit configuration = 0 A A DO2 DO1 DO0 MS1 MS2

  /*
  A A                        DO2 DO1 DO0      Sample Rate [Hz]      MS1 MS0    Measurment Mode
   0 0 = No Average            0   0   0   =   0.75                   0   0   = Normal  
   0 1 = 2 Sample average      0   0   1   =   1.5                    0   1   = Positive Bias
   1 0 = 4 Sample Average      0   1   0   =   3                      1   0   = Negative Bais
   1 1 = 8 Sample Average      0   1   1   =   7.5                    1   1   = -
                               1   0   0   =   15 (Default)
                               1   0   1   =   30
                               1   1   0   =   75
                               1   1   1   =   -
   */
  Wire.endTransmission();
  
  
  compass_read_XYZdata(); // Disregarding the first data
  // Reading the Negative baised Data
  while(compass_x>-200 | compass_y>-200 | compass_z>-200){   // Making sure the data is with negative baised
     compass_read_XYZdata();
  }
  
  compass_x_scalled=compass_x*compass_gain_fact;
  compass_y_scalled=compass_y*compass_gain_fact;
  compass_z_scalled=compass_z*compass_gain_fact;
  

  // Taking the average of the offsets
  compass_x_gainError = (float)((compass_XY_excitation/abs(compass_x_scalled))+compass_x_gainError)/2;
  compass_y_gainError = (float)((compass_XY_excitation/abs(compass_y_scalled))+compass_y_gainError)/2;
  compass_z_gainError = (float)((compass_Z_excitation/abs(compass_z_scalled))+compass_z_gainError)/2;
  
  
  Serial.print("x_gain_offset = ");
  Serial.println(compass_x_gainError);
  Serial.print("y_gain_offset = ");
  Serial.println(compass_y_gainError);
  Serial.print("z_gain_offset = ");
  Serial.println(compass_z_gainError);
  
  }
  
   // Configuring the Control register for normal mode
  Wire.beginTransmission(compass_address);
  Wire.write(0x00);
  Wire.write(0b01111000); // bit configuration = 0 A A DO2 DO1 DO0 MS1 MS2

  /*
  A A                        DO2 DO1 DO0      Sample Rate [Hz]      MS1 MS0    Measurment Mode
   0 0 = No Average            0   0   0   =   0.75                   0   0   = Normal  
   0 1 = 2 Sample average      0   0   1   =   1.5                    0   1   = Positive Bias
   1 0 = 4 Sample Average      0   1   0   =   3                      1   0   = Negative Bais
   1 1 = 8 Sample Average      0   1   1   =   7.5                    1   1   = -
                               1   0   0   =   15 (Default)
                               1   0   1   =   30
                               1   1   0   =   75
                               1   1   1   =   -
   */
  Wire.endTransmission();
  
  // *****************************************************************************************
  // Offset estimation
  // *****************************************************************************************
   if (select == 2 | select == 3){// User input in the function
   Serial.println("Calibrating the Magnetometer ....... Offset");
    Serial.println("Please rotate the magnetometer 2 or 3 times in complete circules with in one minute ............. ");
    
    for(byte i=0;i<10;i++){   // Disregarding first few data
         compass_read_XYZdata();
     }
    
    float x_max=-4000,y_max=-4000,z_max=-4000; 
    float x_min=4000,y_min=4000,z_min=4000;  
    unsigned long t = millis();
    while(millis()-t <= 30000){
      
      compass_read_XYZdata();
      
      compass_x_scalled=(float)compass_x*compass_gain_fact*compass_x_gainError;
      compass_y_scalled=(float)compass_y*compass_gain_fact*compass_y_gainError;
      compass_z_scalled=(float)compass_z*compass_gain_fact*compass_z_gainError;
  
      x_max = max(x_max,compass_x_scalled);
      y_max = max(y_max,compass_y_scalled);
      z_max = max(z_max,compass_z_scalled);
  
      
      x_min = min(x_min,compass_x_scalled);
      y_min = min(y_min,compass_y_scalled);
      z_min = min(z_min,compass_z_scalled);
   
  
    }
  
    compass_x_offset = ((x_max-x_min)/2)-x_max;
    compass_y_offset = ((y_max-y_min)/2)-y_max;
    compass_z_offset = ((z_max-z_min)/2)-z_max;
    
    
    Serial.print("Offset x  = ");
    Serial.print(compass_x_offset);
    Serial.println(" mG");
    Serial.print("Offset y  = ");
    Serial.print(compass_y_offset);
    Serial.println(" mG");
    Serial.print("Offset z  = ");
    Serial.print(compass_z_offset);
    Serial.println(" mG");
    
  } 
 
}





// --------------------------------------------------------------------------
// Setting the gain
// This Function updates the gain_fact variable

void compass_init(int gain){

  byte gain_reg,mode_reg;
  Wire.beginTransmission(compass_address);
  Wire.write(0x01);

  if (gain == 0){
    gain_reg = 0b00000000;
    compass_gain_fact = 0.73;
  }
  else if (gain == 1){
    gain_reg = 0b00100000;
    compass_gain_fact= 0.92;
  }
  else if (gain == 2){
    gain_reg = 0b01000000;
    compass_gain_fact= 1.22;
  }
  else if (gain == 3){
    gain_reg = 0b01100000;
    compass_gain_fact= 1.52;
  }
  else if (gain == 4){
    gain_reg = 0b10000000;
    compass_gain_fact= 2.27;
  }
  else if (gain == 5){
    gain_reg = 0b10100000;
    compass_gain_fact= 2.56;
  }
  else if (gain == 6){
    gain_reg = 0b11000000;
    compass_gain_fact= 3.03;
  }
  else if (gain == 7){
    gain_reg = 0b11100000;
    compass_gain_fact= 4.35;
  }
  
  Wire.write(gain_reg); // bit configuration = g2 g1 g0 0 0 0 0 0, g2 g1 g0 = 0 0 1 for 1.3 guass and 0 1 0 for 1.9 Guass
  Wire.write(0b00000011);  // Putting the Magnetometer in idle
  // Writing the register value 0000 0000 for continous mode
  // Writing the register value 0000 0001 for single
  // Writing the register value 0000 0011 for Idel
  Wire.endTransmission();
  
  Serial.print("Gain updated to  = ");
  Serial.print(compass_gain_fact);
  Serial.println(" mG/bit");
    
}

void compass_scalled_reading(){
  
  compass_read_XYZdata();
  
  compass_x_scalled=compass_x*compass_gain_fact*compass_x_gainError+compass_x_offset;
  compass_y_scalled=compass_y*compass_gain_fact*compass_y_gainError+compass_y_offset;
  compass_z_scalled=compass_z*compass_gain_fact*compass_z_gainError+compass_z_offset;


}


void compass_heading(){
  compass_scalled_reading();
  
  if (compass_y_scalled>0){
    bearing = 90-atan(compass_x_scalled/compass_y_scalled)*compass_rad2degree;
  }else if (compass_y_scalled<0){
    bearing = 270-atan(compass_x_scalled/compass_y_scalled)*compass_rad2degree;
  }else if (compass_y_scalled==0 & compass_x_scalled<0){
    bearing = 180;
  }else{
    bearing = 0;
  }
  
  

}
