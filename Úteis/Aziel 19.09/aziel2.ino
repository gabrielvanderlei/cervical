//Biblioteca pra comunicaÃ§Ã£o via scl e sda pelo arduino
#include<Wire.h>
//Biblioteca para comunicaÃ§Ã£o com dispositivos perifÃ©ricos
#include <SPI.h>

//DefiniÃ§Ãµes para monitoramento
int led = 8;
const int chipSelect = 4;
const int MPU_addr=0x68;
const int MPU_addr_1=0x69;
double AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ,AcX_1,AcY_1,AcZ_1,Tmp_1,GyX_1,GyY_1,GyZ_1; //Eles vÎ³o ler os dados brutos dos MPUs.
uint32_t timing; //Î™ um temporizador, salvo como um unsigned int. NÏƒs usamos isso para economizar tempo do comando "micros ()" e subtrair o tempo atual em microssegundos do tempo armazenado no temporizador para calcular o tempo para cada loop.
double compRoll, compYaw, compPitch; //Esses sÎ³o os angulos em filtros complementares.
double compRoll_1, compPitch_1, compYaw_1;
double compRoll_i, compPitch_i,compRoll_1_i;
double compPitch_1_i, compYaw_i, compYaw_1_i;
double angulodifer_Roll, angulodifer_Pitch, angulodifer_Yaw;
double compX, compY, compZ;
double dif=45.00;
uint32_t ciclos=1;
uint32_t temp=0;
#define degconvert 57.2957786 //Aqui, 57 graus em radianos.
#define pi 6.2831853
#define CS_pin      10 
double dife = 45.00;


void setup()
{
  
  //Inicia comunicaÃ§Ã£o via Bluetooth
  Serial.begin(9600); 
  
  //define o led como saÎ½da
  pinMode(led, OUTPUT);
  
  // Ajusta os MPUs
  Wire.begin();
  #if ARDUINO >= 157
  Wire.setClock(400000UL);  //Ajusta a frequÎºncia da comunicaÎ·Î³o I2C para 400kHz
  #else
    TWBR = ((F_CPU / 400000UL) - 16) / 2; //  Ajusta a frequÎºncia da comunicaÎ·Î³o I2C para 400kHz
  #endif

  
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // registrador PWR_MGMT_1
  Wire.write(0);     // seta pra zero (Liga o MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(9600);
  delay(100);

  Wire.beginTransmission(MPU_addr_1);
  Wire.write(0x6B);  // registrador PWR_MGMT_1
  Wire.write(0);     // seta pra zero (Liga o MPU-6050)
  Wire.endTransmission(true);
  Serial.begin(9600);
  delay(100);

  //ConfiguraÎ·Î³o do Î²ngulo inicial
  
  //1) Coleta os dados
  
  //MPU 1
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  //comeÎ·ando com o registrador 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  // Requere um total de 14 registradores
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

  //MPU 2
 
  Wire.beginTransmission(MPU_addr_1);
  Wire.write(0x3B);  //comeÎ·ando com o registrador 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr_1,14,true);  // Requere um total de 14 registradores
  AcX_1=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
  AcY_1=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ_1=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp_1=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX_1=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY_1=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ_1=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  
  //2) Calcula o pitch e roll
  double roll = atan2(AcY, AcZ)*degconvert;
  double pitch = atan2(-AcX, AcZ)*degconvert;
  double roll_1 = atan2(AcY_1, AcZ_1)*degconvert;
  double pitch_1 = atan2(-AcX_1, AcZ_1)*degconvert;
  double yaw = atan2(-AcX, AcY)*degconvert;
  double yaw_1 = atan2(-AcX_1, AcY_1)*degconvert;

  //3) Define o Î²ngulo de partida para esse pitch e roll
  compRoll = roll;
  compPitch = pitch;
  compRoll_1 = roll_1;
  compPitch_1 = pitch_1;
  compYaw = yaw;
  compYaw_1 = yaw_1;
  
  compRoll_i = roll;
  compRoll_1_i = roll_1;
  compPitch_i = pitch;
  compPitch_1_i = pitch_1;
  compYaw_i = yaw;
  compYaw_1_i = yaw_1;
}
void loop()
{

  // Recolhe os dados brutos dos sensores.
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // comeÎ·ando com o registrador 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr,14,true);  // Requere um total de 14 registradores
  AcX=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
  AcY=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)

  Wire.beginTransmission(MPU_addr_1);
  Wire.write(0x3B);  // comeÎ·ando com o registrador 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr_1,14,true);  // Requere um total de 14 registradores
  AcX_1=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
  AcY_1=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ_1=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp_1=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX_1=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY_1=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ_1=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  
  double dt = (double)(micros() - timing) / 1000000; //Esta linha faz trÎºs coisas: 1) para o temporizador, 2) converte a saÎ½da do temporizador em segundos de microssegundos, 3) molda o valor como um double e salva para "dt".
  timing = micros(); // inicia o temporizador novamente para que possamos calcular o prÏƒximo dt.

   //As duas linhas seguintes calculam a orientaÎ·Î³o do acelerÏ„metro em relaÎ·Î³o Î° Terra e convertem a saÎ½da de atan2 de radianos em graus
  //Usaremos esses dados para corrigir erros cumulativos na orientaÎ·Î³o que o giroscÏƒpio desenvolve.
  
  double roll = atan2(AcY, AcZ)*degconvert;
  double pitch = atan2(-AcX, AcZ)*degconvert;
  double roll_1 = atan2(AcY_1, AcZ_1)*degconvert;
  double pitch_1 = atan2(-AcX_1, AcZ_1)*degconvert;
  double yaw = atan2(-AcX, AcY)*degconvert;
  double yaw_1 = atan2(-AcX_1, AcY_1)*degconvert;
  
  //O giroscÏƒpio produz velocidades angulares. Para converter essas velocidades dos dados brutos para o grau / segundo, divida-se por 131.
  //Observe, estamos dividindo um double "131.0" em vez de um int 131.
  double gyroRoll = GyX/131.0;
  double gyroPitch = GyY/131.0;
  double gyroRoll_1 = GyX_1/131.0;
  double gyroPitch_1 = GyY_1/131.0;
  double gyroYaw = GyZ/131.0;
  double gyroYaw_1 = GyZ_1/131.0;
  
    if(ciclos<100)
  {
    compRoll = (0.99 * (compRoll + gyroRoll * dt) + 0.01 * roll); // Calcula o Î²ngulo de um MPU usando um filtro complementar
    compRoll_1 = (0.99 * (compRoll_1 + gyroRoll_1 * dt) + 0.01 * roll_1); // Calcula o Î²ngulo do outro MPU usando um filtro complementar
    compPitch = (0.99 * (compPitch + gyroPitch * dt) + 0.01 * pitch); 
    compPitch_1 = (0.99 * (compPitch_1 + gyroPitch_1 * dt) + 0.01 * pitch_1); 
    compYaw = (0.99 * (compYaw + gyroYaw * dt) + 0.01 * yaw);
    compYaw_1 = (0.99 * (compYaw_1 + gyroYaw_1 * dt) + 0.01 * yaw_1);
    
    compRoll_i = (compRoll_i + compRoll)/(ciclos + 1);
    compPitch_i = (compPitch_i + compPitch)/(ciclos + 1);
    compRoll_1_i = (compPitch_i + compPitch)/(ciclos + 1);
    compPitch_1_i = (compPitch_1_i + compPitch_1)/(ciclos + 1);
    compYaw_i = (compYaw_i + compYaw)/(ciclos + 1);
    compYaw_1_i = (compYaw_1_i + compYaw_1)/(ciclos + 1);
    ciclos++;
  }

  //O FILTRO COMPLEMENTAR
  //Este filtro calcula o Î²ngulo com base na integraÎ·Î³o da velocidade angular ao deslocamento angular.
  //dt, recall, Î¹ o tempo entre a coleta de dados do MPU6050. Vamos fingir que a 
  //a velocidade angular permaneceu constante ao longo do tempo dt, e multiplica a velocidade angular por
  //tempo de deslocamento.
  //O filtro adiciona um pequeno fator de correÎ·Î³o do acelerÏ„metro ("roll" ou "pitch"), de modo que o giroscÏƒpio sabe de que maneira estÎ± em baixo. 
  compRoll = (0.99 * (compRoll + gyroRoll * dt) + 0.01 * roll) - compRoll_i; // Calcula o Î²ngulo de um MPU usando um filtro complementar
  compRoll_1 = (0.99 * (compRoll_1 + gyroRoll_1 * dt) + 0.01 * roll_1) - compRoll_1_i; // Calcula o Î²ngulo do outro MPU usando um filtro complementar
  compPitch = (0.99 * (compPitch + gyroPitch * dt) + 0.01 * pitch) - compPitch_i; 
  compPitch_1 = (0.99 * (compPitch_1 + gyroPitch_1 * dt) + 0.01 * pitch_1) - compPitch_1_i; 
  compYaw = (0.99 * (compYaw + gyroYaw * dt) + 0.01 * yaw) - compYaw_i;
  compYaw_1 = (0.99 * (compYaw_1 + gyroYaw_1 * dt) + 0.01 * yaw_1) - compYaw_1_i;
  angulodifer_Roll = compRoll - compRoll_1;
  angulodifer_Pitch = compPitch - compPitch_1;
  angulodifer_Yaw = compYaw - compYaw_1;
  //transformação das informações em vetores
  compY = sen(compRoll);
  compX = ((compPitch/degconvert)/(pi))*1;
  compZ = ((compYaw/degconvert)/(pi))*1;

  String dataString = "Informações em relação ao corpo Roll:" + String(angulodifer_Roll) + ", Pitch:" + String(angulodifer_Pitch) + ", Yaw:" + String(angulodifer_Yaw) + ", " + String(ciclos) + "compY=" +  String(compY) + "compX=" +  String(compX) + "compZ=" + String(compZ);
  Serial.println(compY);

 //Serial.println(dataString); 
} 
