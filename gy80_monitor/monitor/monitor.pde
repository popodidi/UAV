import processing.serial.*;

PGraphics cube;

Serial myPort;  // Create object from Serial class
String val;     // Data received from the serial port
String lab;
float[] gyro = new float[3]; //degree/sec
float[] a = new float[3];

float[] w = new float[3];//radians/sec
float[] theta = new float[3]; //radians
float[] thetaDot = new float[3]; //radians/sec

float[] position = new float[3]; //radians

float[][] wToThetaDot = new float[3][3];
float[][] bodyToInertial = new float[3][3];

boolean calibrationDone = false;


void setup(){
  
  size(600, 600, P3D);
  cube = createGraphics(width, height, P3D);
  
    // I know that the first port in the serial list on my mac
  // is Serial.list()[0].
  // On Windows machines, this generally opens COM1.
  // Open whatever port is the one you're using.
  String portName = Serial.list()[8]; //change the 0 to a 1 or 2 etc. to match your port
  myPort = new Serial(this, portName, 9600); 
  //myPort.bufferUntil('\n'); 
  
  initialize();
  
}

void initialize(){
  for(int i=0;i<3;i++) {
        a[i] = 0.0;
        w[i] = 0.0;
        theta[i] = 0.0;
        thetaDot[i] = 0.0;
        position[i] = 0.0;
   }
}

void draw(){
  background(0); 
  
  getWInInertial();
  getTheta();  
  
  drawCube();
  image(cube, 0, 0);
  
}

void drawCube() { 
  cube.beginDraw();
  cube.lights();
  cube.background(0);
  cube.noStroke();
  cube.translate(width/2, height/2);

  cube.rotateZ(theta[0]);
  cube.rotateX(theta[1]);
  cube.rotateZ(theta[2]);
  
  
  cube.box(200,50,200);
  cube.endDraw();
}

void getTheta(){
  wToThetaDot[0][0]= 1.0;
  wToThetaDot[0][1]= sin(theta[0])*tan(theta[1]);
  wToThetaDot[0][2]= cos(theta[0])*tan(theta[1]);
  wToThetaDot[1][0]= 0.0;
  wToThetaDot[1][1]= cos(theta[0]);
  wToThetaDot[1][2]= -sin(theta[0]);
  wToThetaDot[2][0]= 0.0;
  wToThetaDot[2][1]= sin(theta[0])/cos(theta[1]);
  wToThetaDot[2][2]= cos(theta[0])/cos(theta[1]);
  
  for(int i=0;i<3;i++) {
    thetaDot[i] = 0.0;
    for(int j=0;j<3;j++) {
        thetaDot[i] += wToThetaDot[i][j]*w[j];
    }
    theta[i] += thetaDot[i];
  }
}

void getWInInertial(){
  
  bodyToInertial[0][0]= cos(theta[0])*cos(theta[1])-cos(theta[1])*sin(theta[0])*sin(theta[2]);
  bodyToInertial[0][1]= -cos(theta[2])*sin(theta[0])-cos(theta[0])*cos(theta[1])*sin(theta[2]);
  bodyToInertial[0][2]= sin(theta[1])*sin(theta[2]);
  bodyToInertial[1][0]= cos(theta[1])*cos(theta[2])*sin(theta[0])+cos(theta[0])*sin(theta[2]);
  bodyToInertial[1][1]= cos(theta[1])*cos(theta[2])*cos(theta[0])-sin(theta[0])*sin(theta[2]);
  bodyToInertial[1][2]= -cos(theta[2])*sin(theta[1]);
  bodyToInertial[2][0]= sin(theta[0])*sin(theta[1]);
  bodyToInertial[2][1]= cos(theta[0])*sin(theta[1]);
  bodyToInertial[2][2]= cos(theta[1]);
  
  for(int i=0;i<3;i++) {
    for(int j=0;j<3;j++) {
        w[i] += bodyToInertial[i][j]*radians(gyro[j]);
    }
  }
}

void serialEvent( Serial myPort) {
  //put the incoming data into a String - 
  //the '\n' is our end delimiter indicating the end of a complete packet
  val = myPort.readStringUntil('\n');
  //make sure our data isn't empty before continuing
  if (val != null) {
    //trim whitespace and formatting characters (like carriage return)
    val = trim(val);
    println(val);
    
    if(val.equals("Calibration Done")){
      calibrationDone = true;
    }
    
    if (calibrationDone){
      lab = val.substring(0,3);
      if(lab.equals("a:[")){
        //storing the acceleration
        String aStr[] = val.substring(3, val.length()-1).split(",");
        for(int i=0;i<3;i++) {
          a[i] = Float.parseFloat(String.valueOf(aStr[i]));
        }
        
      }else if(lab.equals("w:[")){
        //storing the angular velocity
        String wStr[] = val.substring(3, val.length()-1).split(",");
        for(int i=0;i<3;i++) {
          gyro[i] = Float.parseFloat(String.valueOf(wStr[i]))*0.007;
        }
      }
    }
    

  }else{ 
    //if we've already established contact, keep getting and parsing data
    //println(val);
  }
}
