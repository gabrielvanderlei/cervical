
#include<Wire.h>
#include <SPI.h>

int MPU1[6];
int MPU2[6]; 

void ConnectI2C(int Addr, int* Msg, int lengthMsg){
  int *sendMsg = Msg;
  Wire.beginTransmission(Addr);
  for(int i = 0; i < lengthMsg; i++) Wire.write(sendMsg[i]);  
  Wire.endTransmission(true);
  Serial.begin(9600);
  delay(100);  
}

int* getMPU(int Addr){
  int dadosMPU[6];
  Wire.beginTransmission(Addr);
  Wire.write(0x3B);  
  Wire.endTransmission(false);
  Wire.requestFrom(Addr,14,true);  
  for(int i = 0; i <= 6; i++) dadosMPU[i] = Wire.read()<<8|Wire.read();   
  return dadosMPU;
}

void setup()
{
  
  Serial.begin(9600); 
  Wire.begin();
  #if ARDUINO >= 157
  Wire.setClock(400000UL);  //Ajusta a frequÎºncia da comunicaÎ·Î³o I2C para 400kHz
  #else
    TWBR = ((F_CPU / 400000UL) - 16) / 2; //  Ajusta a frequÎºncia da comunicaÎ·Î³o I2C para 400kHz
  #endif

  int Requisitar[2] = {0x6B, 0x00};
  ConnectI2C(0x68, Requisitar, 2);
  ConnectI2C(0x69, Requisitar, 2);

  *MPU1 = getMPU(0x68); 
  *MPU2 = getMPU(0x69); 
}

void loop()
{
  *MPU1 = getMPU(0x68); 
  *MPU2 = getMPU(0x69); 
  
  Serial.print("{");
  Serial.print("AcX_1: "+MPU1[0]);
  Serial.print(" ,AcY_1: "+MPU1[1]);
  Serial.print(" ,AcZ_1: "+MPU1[2]);
  Serial.print(" ,Tmp_1: "+MPU1[3]);
  Serial.print(" ,GyX_1: "+MPU1[4]);
  Serial.print(" ,GyY_1: "+MPU1[5]);
  Serial.print(" ,GyZ_1: "+MPU1[6]);
  Serial.print("}");
  Serial.println();
  
  delay(500);
} 
