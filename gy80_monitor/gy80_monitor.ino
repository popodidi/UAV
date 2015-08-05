//Arduino 1.0+ only

#include <Wire.h>
#include <ADXL345.h>

#define CTRL_REG1 0x20
#define CTRL_REG2 0x21
#define CTRL_REG3 0x22
#define CTRL_REG4 0x23
#define CTRL_REG5 0x24

ADXL345 adxl; //variable adxl is an instance of the ADXL345 library

int L3G4200D_Address = 105; //I2C address of the L3G4200D

//gyro
int wx, wy, wz;
float cwx, cwy, cwz;

//adxl345 accelerator
int ax, ay, az;
float cax, cay, caz;

void setup(){

  Wire.begin();
  Serial.begin(9600);

  Serial.println("adxl poweron");
  adxl.powerOn();

  Serial.println("starting up L3G4200D");
  setupL3G4200D(2000); // Configure L3G4200  - 250, 500 or 2000 deg/sec
  
  //Calibration
  Serial.println("Calibration");
  
  for (int count = 1; count < 11; count++) { 
    
    getGyroValues();  // This will update x, y, and z with new values
    cwx += wx;
    cwy += wy;
    cwz += wz;
    adxl.readAccel(&ax, &ay, &az); //read the accelerometer values and store them in variables  x,y,z
    cax += ax;
    cay += ay;
    caz += az;
  }
  
  cwx /= 10.0;
  cwy /= 10.0;
  cwz /= 10.0;
  cax /= 10.0;
  cay /= 10.0;
  caz /= 10.0;
  Serial.println("Calibration Done");


  delay(1500); //wait for the sensor to be ready 
}

void loop(){
   getGyroValues();  // This will update x, y, and z with new values

  Serial.print("w:[");
  Serial.print(wx-cwx);
  Serial.print(",");
  Serial.print(wy-cwy);
  Serial.print(",");
  Serial.print(wz-cwz);
  Serial.println("]");


  
  //adxl345
  //Boring accelerometer stuff   
  adxl.readAccel(&ax, &ay, &az); //read the accelerometer values and store them in variables  x,y,z

  // Output x,y,z values - Commented out
  Serial.print("a:[");
  Serial.print(ax-cax);
  Serial.print(",");
  Serial.print(ay-cay);
  Serial.print(",");
  Serial.print(az-caz);
  Serial.println("]");
  

  delay(100); //Just here to slow down the serial to make it more readable
}

void getGyroValues(){

  byte xMSB = readRegister(L3G4200D_Address, 0x29);
  byte xLSB = readRegister(L3G4200D_Address, 0x28);
  wx = ((xMSB << 8) | xLSB);

  byte yMSB = readRegister(L3G4200D_Address, 0x2B);
  byte yLSB = readRegister(L3G4200D_Address, 0x2A);
  wy = ((yMSB << 8) | yLSB);

  byte zMSB = readRegister(L3G4200D_Address, 0x2D);
  byte zLSB = readRegister(L3G4200D_Address, 0x2C);
  wz = ((zMSB << 8) | zLSB);
}

int setupL3G4200D(int scale){
  //From  Jim Lindblom of Sparkfun's code

  // Enable x, y, z and turn off power down:
  writeRegister(L3G4200D_Address, CTRL_REG1, 0b00001111);

  // If you'd like to adjust/use the HPF, you can edit the line below to configure CTRL_REG2:
  writeRegister(L3G4200D_Address, CTRL_REG2, 0b00000000);

  // Configure CTRL_REG3 to generate data ready interrupt on INT2
  // No interrupts used on INT1, if you'd like to configure INT1
  // or INT2 otherwise, consult the datasheet:
  writeRegister(L3G4200D_Address, CTRL_REG3, 0b00001000);

  // CTRL_REG4 controls the full-scale range, among other things:

  if(scale == 250){
    writeRegister(L3G4200D_Address, CTRL_REG4, 0b00000000);
  }else if(scale == 500){
    writeRegister(L3G4200D_Address, CTRL_REG4, 0b00010000);
  }else{
    writeRegister(L3G4200D_Address, CTRL_REG4, 0b00110000);
  }

  // CTRL_REG5 controls high-pass filtering of outputs, use it
  // if you'd like:
  writeRegister(L3G4200D_Address, CTRL_REG5, 0b00000000);
}

void writeRegister(int deviceAddress, byte address, byte val) {
    Wire.beginTransmission(deviceAddress); // start transmission to device 
    Wire.write(address);       // send register address
    Wire.write(val);         // send value to write
    Wire.endTransmission();     // end transmission
}

int readRegister(int deviceAddress, byte address){

    int v;
    Wire.beginTransmission(deviceAddress);
    Wire.write(address); // register to read
    Wire.endTransmission();

    Wire.requestFrom(deviceAddress, 1); // read a byte

    while(!Wire.available()) {
        // waiting
    }

    v = Wire.read();
    return v;
}
