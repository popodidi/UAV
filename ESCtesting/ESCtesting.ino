#include <Servo.h>

// 支援 PWM 的 pin 腳
int motor_pin[4] = {
	5, 6, 9, 10};
 
// 四個馬達
Servo motor[4];
 
// 四個油門（出力）
//int throttle[4] = {
//	20, 20 ,20 ,20};
 
// Arduino 初始化
void setup(){
  Serial.begin(9600);
  int c;
  // 初始化油門輸出量，馬達對應到的腳位
  for(c=0; c<4; c++){
    //throttle[c] = 15;
    motor[c].attach(motor_pin[c]);
    if(c==3){
      Serial.print("the motors have been attached");
      Serial.print('\n');
    }
  }
  setAllThrottle(60);
  
  Serial.print("enter a number for throttle");
  Serial.print('\n');
}
 
void loop(){
  
  if(Serial.available()>0){
    String input = Serial.readString();
    Serial.print("the throttle will be set as ");
    Serial.print('\n');
    Serial.print(input);
    Serial.print('\n');
    
    setAllThrottle(input.toInt());
  }


}

void setAllThrottle(int th) {
  int c;
  for(c=0; c<4; c++) {
    // 將資料送給 ESC
    motor[c].write(th);
    if(c==3){
      Serial.print("the motors have all been set");
      Serial.print('\n');
    }
  }
}
