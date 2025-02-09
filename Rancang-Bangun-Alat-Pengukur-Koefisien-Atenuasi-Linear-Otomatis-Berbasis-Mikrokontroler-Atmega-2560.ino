#include <BH1750.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd1(0x27,20,4);  // set the LCD address to 0x27 for a 20 chars and 4 line display
LiquidCrystal_I2C lcd2(0x26,20,4);  // set the LCD address to 0x26 for a 20 chars and 4 line display
/*

Example of BH1750 library usage.

This example initialises the BH1750 object using the default high resolution
continuous mode and then makes a light level reading every second.

Connections

  - VCC to 3V3 or 5V
  - GND to GND
  - SCL to SCL (A5 on Arduino Uno, Leonardo, etc or 21 on Mega and Due, on
    esp8266 free selectable)
  - SDA to SDA (A4 on Arduino Uno, Leonardo, etc or 20 on Mega and Due, on
    esp8266 free selectable)
  - ADD to (not connected) or GND

ADD pin is used to set sensor I2C address. If it has voltage greater or equal
to 0.7VCC voltage (e.g. you've connected it to VCC) the sensor address will be
0x5C. In other case (if ADD voltage less than 0.7 * VCC) the sensor address
will be 0x23 (by default).

*/

#include <BH1750.h>
#include <Wire.h>
#define trigPin  5
#define echoPin  6

//Tetapan-Tetapan khusus

double Io = 712.50; //Io adalah Intensitas cahaya lampu sebelum melewati bahan;
double H = 33.81; //Ketinggian sensor dari dasar alat;

BH1750 lightMeter;
int NilaiAnalog;//Variabel untuk menyimpan hasil pembacaan analog;
int pinLM35 = A0;// Variabel untuk PIN Analog yang disambungkan pada sensor;
double volts;// Varaibel untuk menyimpan hasil dalam satuan volts;
double SuhuCelcius;// Suhu Celcius adalah suhu yang terbaca pada sensor
double PerhitunganSuhu;
double PembandingKecepatan;
double v0 = 331;//v0 adalah kecepatan gelombang ultrasonik pada suhu 0 Derajat Celcius;
double v;//v adalah kecepatan gelombang ultrasonik pada suhu T;
void setup() {
  Serial.begin(9600);

  // Initialize the I2C bus (BH1750 library doesn't do this automatically)
  Wire.begin();
  // On esp8266 you can select SCL and SDA pins using Wire.begin(D4, D3);
  // For Wemos / Lolin D1 Mini Pro and the Ambient Light shield use
  // Wire.begin(D2, D1);
  
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  lightMeter.begin();
  
  Serial.println(F("Penentuan koefisien atenuasi minyak"));
  lcd1.init();                      // initialize the lcd 
  lcd1.init();
  // Print a message to the LCD.
  lcd1.backlight();
  lcd2.init();                      // initialize the lcd 
  lcd2.init();
  // Print a message to the LCD.
  lcd2.backlight();
  delay(1000);
}

void loop() {
  long ToF; //Durasi adalah waktu tempuh gelombang ultrasonik (Time of Flight);
  double s; //Jarak sensor ke permukaan bagian atas sampel;
  double l; //l = panjang lintasan optis;
  double la;
  double I = lightMeter.readLightLevel(); //I adalah intensitas cahaya yang keluar dari bahan uji dalam satuan lumen/m^2;
  double T ; //T adalah Transmitansi merupakan perbandingan Intensitas cahaya yang keluar dari bahan uji dengan Intensitas cahaya tanpa bahan uji;
  double Ta ;
  double logI; //logI adalah nilai logaritma dari intensitas terukur;
  double logIo;//logIo adalah logaritma dari intensitas Io;
  double lnT;//logT adalah nilai logaritma dari Transmitansi;
  double k; //k adalah Koefisien atenuasi cahaya;
  double A;//A adalah besarnya Atenuasi (-lnT);

  NilaiAnalog = analogRead(pinLM35); //Membaca nilai analog dari PIN sensor;
  volts = (NilaiAnalog/1024.0)*5000; //Merubah nilai analog menjadi skala 10 Bit ADC;
  // Membagi angka voltase yang telah diubah menjadi milivolts dengan angka 10;
  SuhuCelcius = volts/10;
  Serial.print("t = ");
  Serial.print(SuhuCelcius);
  Serial.println(" Derajat Celcius");
  PerhitunganSuhu = (SuhuCelcius/273)+1;
  PembandingKecepatan = sqrt(PerhitunganSuhu);
  v = v0*PembandingKecepatan;
  Serial.print("v = ");
  Serial.print(v);
  Serial.println(" m/s");
  //////////////////////////////Pengukuran Panjang Lintasan Optis//////////////////////////////
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  ToF = pulseIn(echoPin,HIGH);
  Serial.print("ToF = ");
  Serial.print(ToF);
  Serial.println(" Mikrosekons ");
  lcd1.setCursor(0,0);
  lcd1.print("TOF = ");
  lcd1.print(ToF);
  lcd1.print(" Mikrosekon ");
  
  s =(ToF*v)*0.0001/2;
  Serial.print("s = ");
  Serial.print(s);
  Serial.println(" cm ");
  
  la = H - s;
  if(la<0.4){
    l=0;
  }else if (la>=0.4){
    l=la;}
  Serial.print("l = "); 
  Serial.print(l);
  Serial.println(" cm ");
  lcd1.setCursor(0,1);
  lcd1.print("l = ");
  lcd1.print(l);
  lcd1.print(" cm  ");
  Serial.print("I =  ");
  Serial.print(I);
  Serial.println(" Lux ");
  lcd1.setCursor(0,2);
  lcd1.print("I = ");
  lcd1.print(I);
  lcd1.print(" Lux  ");
   //////////////////////////////Pengukuran Transmitansi//////////////////////////////
  Ta=(100*I)/Io;
  if((Ta>99.5)&&(Ta<101)){
    T=100;}
    else if(Ta<=99.5){
    T=Ta;}
  Serial.print("T = ");
  Serial.print(T);
  Serial.println("  % ");
  lcd1.setCursor(0,3);
  lcd1.print("T = ");
  lcd1.print(T);
  lcd1.print(" % ");
  
  logI = log(I);
  logIo = log(Io);
  lnT = logI-logIo; //Log T = log(I/Io) atau log I-log Io;
  
  A = lnT*(-1);// A adalah Atenuasi, merupakan negatif dari Ln T
  Serial.print("A = ");
  Serial.print(A);
  Serial.println(" ");
  lcd2.setCursor(0,0);
  lcd2.print("A = ");
  lcd2.print(A);
    //////////////////////////////Pengukuran Koefisien Atenuasi Linear//////////////////////////////
  k = (100*A)/l; //Koefisien atenuasi adalah Atenuasi dibagi panjang lintasan optis;
  Serial.print("k =  ");
  Serial.print(k);
  Serial.println(" /m ");
  Serial.println(" ");
  lcd2.setCursor(0,1);
  lcd2.print("k = ");
  lcd2.print(k);
  lcd2.print(" /m ");

  delay(1000);
}
