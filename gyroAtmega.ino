
#include <MPU6050_tockn.h>
#include <Wire.h>
#include <Servo.h>
MPU6050 mpu6050(Wire);

Servo servo;
int donus=0;
long int donustoplam=0;
float roll = 0;
long int esik=0;
String x;
char y;
void setup() {
  servo.attach(9);
  servo.write(50);
  Wire.begin();
  mpu6050.begin();
  mpu6050.calcGyroOffsets(true);
  pinMode(2,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), veriat, FALLING);
  Serial.begin(38400);
}

void loop() 
{
   if(Serial.available())
   {
       y=Serial.read();
       x = (String)y;
       if(x=="1")
        servo.write(50);
       if(x=="2")
       servo.write(150);
   }
   mpu6050.update();
   roll = mpu6050.getAngleZ();
   donus= (roll-esik)/360;
   if(donus<0)
   donus=donus*-1;
   
    if(donus!=0)
   {
   esik=donus*roll;
   donustoplam=donustoplam+donus; 
   }  
}
void veriat()
{
    Serial.print(mpu6050.getAngleX(),2);
    Serial.print("#");
    Serial.print(mpu6050.getAngleY(),2);
    Serial.print("#");
    Serial.print(mpu6050.getAngleZ(),2);
    Serial.print("#");
    Serial.print(donustoplam);
}
