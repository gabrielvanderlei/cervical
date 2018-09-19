//Seção de definições para ultilização do Blynk com Bluetooth
#define BLYNK_PRINT Serial
//Destino para qual as informções são enviadas pelo bluetooth (identidade da aplicação)
char auth[] = "178ba334092548d984ee4906a6e395a6";

//Biblioteca utilizada para se comunicar com o módulo bluetooth pelo mesmo não ter endereço
#include <SoftwareSerial.h>

//Definição das pórtas que serão utilizadas para a comunicação serial com o módulo bluetooth
SoftwareSerial Bluetooth(2, 3); // RX, TX

//Acionando que a comunicação inteligente ocorrerá via portas 10 e 11
SoftwareSerial SerialBLE(2, 3); // RX, TX

//Biblioteca pra comunicação via scl e sda pelo arduino
#include<Wire.h>

//Biblioteca para comunicação com dispositivos periféricos
#include <SPI.h>

//Definições para monitoramento
int led = 8;
int tempo;
const int chipSelect = 4;
const int MPU_addr=0x68;
const int MPU_addr_1=0x69;
double AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ,AcX_1,AcY_1,AcZ_1,Tmp_1,GyX_1,GyY_1,GyZ_1; //Eles vγo ler os dados brutos dos MPUs.
uint32_t timing; //Ι um temporizador, salvo como um unsigned int. Nσs usamos isso para economizar tempo do comando "micros ()" e subtrair o tempo atual em microssegundos do tempo armazenado no temporizador para calcular o tempo para cada loop.
double compRoll, compPitch; //Esses sγo os angulos em filtros complementares.
double compRoll_1, compPitch_1;
double angulodifer_Roll, angulodifer_Pitch;
double dif=45.00;
uint32_t temp=0;
#define degconvert 57.2957786 //Aqui, 57 graus em radianos.
#define CS_pin      10 
double dife = 45.0;

void setup()
{
  Serial.begin(9600);
  
  //Inicia comunicação via Bluetooth
  Bluetooth.begin(9600);
  
  //define o led como saνda
  pinMode(led, OUTPUT);
  
  // Ajusta os MPUs
  Wire.begin();
  #if ARDUINO >= 157
    Wire.setClock(400000UL);  //Ajusta a frequκncia da comunicaηγo I2C para 400kHz
  #else
    TWBR = ((F_CPU / 400000UL) - 16) / 2; //  Ajusta a frequκncia da comunicaηγo I2C para 400kHz
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

  //Configuraηγo do βngulo inicial
  
  //1) Coleta os dados
  
  //MPU 1
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  //comeηando com o registrador 0x3B (ACCEL_XOUT_H)
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
  Wire.write(0x3B);  //comeηando com o registrador 0x3B (ACCEL_XOUT_H)
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

  //3) Define o βngulo de partida para esse pitch e roll
  double compRoll = roll;
  double compPitch = pitch;
  double compRoll_1 = roll_1;
  double compPitch_1 = pitch_1;
 
}

void loop()
{
  
  //A cada 1 segundo são escritos nas labels do Aplicativo os dados
  if(tempo%100==0) Bluetooth.println(angulodifer_Roll);
  
 
  // Recolhe os dados brutos dos sensores.
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x3B);  // comeηando com o registrador 0x3B (ACCEL_XOUT_H)
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
  Wire.write(0x3B);  // comeηando com o registrador 0x3B (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(MPU_addr_1,14,true);  // Requere um total de 14 registradores
  AcX_1=Wire.read()<<8|Wire.read();  // 0x3B (ACCEL_XOUT_H) & 0x3C (ACCEL_XOUT_L)     
  AcY_1=Wire.read()<<8|Wire.read();  // 0x3D (ACCEL_YOUT_H) & 0x3E (ACCEL_YOUT_L)
  AcZ_1=Wire.read()<<8|Wire.read();  // 0x3F (ACCEL_ZOUT_H) & 0x40 (ACCEL_ZOUT_L)
  Tmp_1=Wire.read()<<8|Wire.read();  // 0x41 (TEMP_OUT_H) & 0x42 (TEMP_OUT_L)
  GyX_1=Wire.read()<<8|Wire.read();  // 0x43 (GYRO_XOUT_H) & 0x44 (GYRO_XOUT_L)
  GyY_1=Wire.read()<<8|Wire.read();  // 0x45 (GYRO_YOUT_H) & 0x46 (GYRO_YOUT_L)
  GyZ_1=Wire.read()<<8|Wire.read();  // 0x47 (GYRO_ZOUT_H) & 0x48 (GYRO_ZOUT_L)
  
  double dt = (double)(micros() - timing) / 1000000; //Esta linha faz trκs coisas: 1) para o temporizador, 2) converte a saνda do temporizador em segundos de microssegundos, 3) molda o valor como um double e salva para "dt".
  timing = micros(); // inicia o temporizador novamente para que possamos calcular o prσximo dt.

   //As duas linhas seguintes calculam a orientaηγo do acelerτmetro em relaηγo ΰ Terra e convertem a saνda de atan2 de radianos em graus
  //Usaremos esses dados para corrigir erros cumulativos na orientaηγo que o giroscσpio desenvolve.
  double roll = atan2(AcY, AcZ)*degconvert;
  double pitch = atan2(-AcX, AcZ)*degconvert;
  double roll_1 = atan2(AcY_1, AcZ_1)*degconvert;
  double pitch_1 = atan2(-AcX_1, AcZ_1)*degconvert;

  //O giroscσpio produz velocidades angulares. Para converter essas velocidades dos dados brutos para o grau / segundo, divida-se por 131.
  //Observe, estamos dividindo um double "131.0" em vez de um int 131.
  double gyroRoll = GyX/131.0;
  double gyroPitch = GyY/131.0;
  double gyroRoll_1 = GyX_1/131.0;
  double gyroPitch_1 = GyY_1/131.0;
  
  //O FILTRO COMPLEMENTAR
  //Este filtro calcula o βngulo com base na integraηγo da velocidade angular ao deslocamento angular.
  //dt, recall, ι o tempo entre a coleta de dados do MPU6050. Vamos fingir que a 
  //a velocidade angular permaneceu constante ao longo do tempo dt, e multiplica a velocidade angular por
  //tempo de deslocamento.
  //O filtro adiciona um pequeno fator de correηγo do acelerτmetro ("roll" ou "pitch"), de modo que o giroscσpio sabe de que maneira estα em baixo. 
  compRoll = 0.99 * (compRoll + gyroRoll * dt) + 0.01 * roll; // Calcula o βngulo de um MPU usando um filtro complementar
  compPitch = 0.99 * (compPitch + gyroPitch * dt) + 0.01 * pitch; 
  compRoll_1 = 0.99 * (compRoll_1 + gyroRoll_1 * dt) + 0.01 * roll_1; // Calcula o βngulo do outro MPU usando um filtro complementar
  compPitch_1 = 0.99 * (compPitch_1 + gyroPitch_1 * dt) + 0.01 * pitch_1; 
  angulodifer_Roll = compRoll - compRoll_1;
  angulodifer_Pitch = compPitch - compPitch_1;
  
  Serial.println(compRoll);

  //Essa linha ι responsαvel por ligar o led/buzzer quando a diferenηa de βngulos superar o angulo dado como crítico atravez dfo aplicativo.
  if (angulodifer_Roll >= dife | angulodifer_Pitch >= 60) digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  else digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW

  delayMicroseconds(100);
  tempo++; 
}
