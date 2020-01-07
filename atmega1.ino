//kütüphaneler
#include <Wire.h>
#include <I2Cdev.h> //I2C kütüphanesini ekledik
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <TinyGPS++.h>
#include <Adafruit_MPL115A2.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>

int sif = 0;
RF24 yeristasyonu(10, 8);
const byte adress[6] = "00002"; //okuma adresi.
const byte address[6] = "00001"; //yazma adresi.
//sensör tanımları ve atamaları
char text[32];
int Servoesik = 50; //default değer 1000.
int ak = 0;
char bekliyor[9] = {"BEKLIYOR"};
char iniyor[9] = {"INIYOR"};
char yukseliyor[9] = {"YUKSELIYOR"};
int boy = 0;
char xy;
int lo = 1;
double gy;
double sure = 0;
double gecik = 0;
int z = 0;
static const int RXPin = 4, TXPin = 3;
static const uint32_t GPSBaud = 9600;
Adafruit_MPL115A2 mpl115a2;
TinyGPSPlus gps;
String servok;
char gpsdata[9];
// seri haberleşme
SoftwareSerial ss(RXPin, TXPin);
//SoftwareSerial yeristasyonu(10,11);//degisecek.
SoftwareSerial rasp(6, 7);
char incoming;
String sifre;
//yükseklik hesabı için değişken
String alinan = "y";
int yukseklik = 0;
const int esik = 101;
int sum = 0, y = 0;
float altimeter = 0.00;
//pil gerilimi için değişken
int analogInput = 0;
float vout = 0.0;
float vin = 0.0;
int value = 0;
//hız için değişken
int hiz = 0;
int hiz1 = 0;
//sıcaklık ve basınc icin değişken
float pressureKPA = 0, temperatureC = 0;
//eeprom için değişken
int c = 0, adres = 0, a = 0;
//servo için değişken
char oku[10] = {"12345"};
char mpuveri[32] = {""};
char mpuveriyedek[32] = {"-1.36#-3.73#2.67#0"};
char none[20] = {"Sifirlandi."};

int mpu = 0, mpu2,uzunm=0;
int derece = 0;
int sifir = 0;
int ck = 0, gpsvd = 0, ck2 = 0, p = 0, p1 = 0;
int delayhesapla = 0;
char enter[] = {"r"};
char bitis[] = {"v"}; //Veri bitişi işareti.
char yer5[32] = {};
char yer6[32];//Servo ve hakem şifresi kaydedilen dizi.
char ser[] = {"SE"};
char ser2[] = {"UN"};
int yeryaz = 0;
int dongu;
char bosluk[2] = {" "};
float transData = 12.3456;
char sayi[5];
char floatyaz[7];
int basinc;
int sayi1;
int saat;
int dakika;
int saniye;
boolean kontrol=1;
void setup()
{
  pinMode(2, OUTPUT);
  pinMode(5,INPUT);
  pinMode(2, HIGH); //inaktif
  Serial.begin(38400);//mpu arduinosu.
  rasp.begin(9600);
  //okumamodu();
  nrfinit();
  ss.begin(GPSBaud);
  ss.listen();
  //sifirla(); //sistemi sifirlamak için aktif hale getir ve geri kapat.
  mpl115a2.begin();
  pinMode(analogInput, INPUT);
  if (EEPROM.read(10) != 1)
  {
    EEPROMWritelong(adres, 1);
    EEPROM.write(10, 1);
  }
  if (EEPROM.read(11) != 1)
  {
    EEPROM.write(12, 0);
    EEPROM.write(11, 1);
  }

}

void loop()
{
  sure = millis();
  ss.listen();
  pressureKPA = mpl115a2.getPressure();  // basınç tanımı
  temperatureC = mpl115a2.getTemperature();// sıcaklık tanımı
  value = analogRead(analogInput);
  vout = (value * 5.0) / 1024.0;
  vin = vout * 11 ;
   if (vin<0.09)
   {
    vin=0.0;
  }
  

  while (!(alinan == "o") && EEPROM.read(12) != 1)
  {
    if (yeristasyonu.available())
    {
      yeristasyonu.read(&text, sizeof(text));
      alinan = (String)text;
      if(alinan == "g")
        Serial.print("1");
      if(alinan == "f")
        Serial.print("2");
    }
  }
  if (EEPROM.read(12) != 1)
  {


    rasp.listen();
    delay(1);                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                               
                                                                                                             
    EEPROM.write(12, 1);
    rasp.println("x");
    delay(50);//Raspberry hazirlansin.
    

  }
  yeristasyonu.stopListening();//NRF yazmamoduna geçildi
  rasp.listen();
  //Yazılımsal uarttan gps dinleniyor.
  rasp.println();
  yeryaz = 0;
  rasp.print("44106");//rasp olacak.
  // Serial.print(" ");
  yer("44106");
  yer("#");
  rasp.print("#");

  c = EEPROMReadlong(adres);
  rasp.print(c);
  // Serial.print(c);
  sprintf(sayi, "%d", c);
  yer(sayi);
  yer("#");
  c++;
  EEPROMWritelong(adres, c);
  rasp.print("#");
  if(kontrol)
  {
   smartDelay(500);  
  }
  printDateTime(gps.date, gps.time);//yer içine gömülü
  rasp.listen();
  //yer("\n");
  yer5[yeryaz] = '\0';
  yeryaz = 0;
  yeristasyonu.write(&yer5, sizeof(yer5));
  char result[9]; // Buffer big enough for 7-character float
  dtostrf(pressureKPA, 7, 4, result);
  yer(result);
  yer("#");


  rasp.print(pressureKPA, 4);
  rasp.print("#");

  altimeter = esik - pressureKPA;
  altimeter = altimeter * 85.714;
  rasp.print(altimeter, 4);
  rasp.print("#");

  dtostrf(altimeter, 7, 2, result);
  yer(result);
  yer("#");



  hiz = hiz - altimeter;
  hiz1 = hiz; 
  rasp.print(hiz);
  rasp.print("#");
  if(hiz < -200 )
  {
    sprintf(sayi, "%d", 0);
  }
  else
  {
    sprintf(sayi, "%d", hiz);
  }
  yer(sayi);
  yer("#");
  hiz = altimeter;


  rasp.print(temperatureC, 1);
  rasp.print("#");
  dtostrf(temperatureC, 4, 1, result);
  yer(result);
  yer("#");

  yer5[yeryaz] = '\0';
  yeryaz = 0;
  yeristasyonu.write(&yer5, sizeof(yer5));
  rasp.print(vin,2);
  rasp.print("#");
  dtostrf(vin, 3, 2, result);
  yer(result);
  yer("#");

 printFloat(gps.location.lng(), gps.location.isValid(), 7, 3);
  rasp.listen();
  yer(gpsdata);//gpsdata gelicek unutma !!.
  yer("#");
  rasp.print("#");
  yer5[yeryaz] = '\0';
  yeryaz = 0;
  yeristasyonu.write(&yer5, sizeof(yer5));

  printFloat(gps.location.lat(), gps.location.isValid(), 7, 3);
  rasp.listen();
  yer(gpsdata);
  yer("#");
  rasp.print("#");
  yer5[yeryaz] = '\0';
  yeryaz = 0;
  yeristasyonu.write(&yer5, sizeof(yer5));
  printFloat(gps.altitude.meters(), gps.altitude.isValid(), 7, 2);
  rasp.listen();
  yer(gpsdata);
  yer("#");
  rasp.print("#");
  yer5[yeryaz] = '\0';
  yeryaz = 0;
  yeristasyonu.write(&yer5, sizeof(yer5));

  if ((-30 < hiz1) && (hiz1 < 30))
   {
        rasp.listen();
        rasp.print("BEKLIYOR");
        rasp.print("#");
        yer(bekliyor);
        yer("#");
        
   }
  if (hiz1 >= 30)
  {
    rasp.listen();
    rasp.print("YUKSELIYOR");
    rasp.print("#");
    yer(yukseliyor);
    yer("#");
    
  }
  if (hiz1 <= -30)
  {
    rasp.listen();
    rasp.print("INIYOR");
    rasp.print("#");
    yer(iniyor);
    yer("#");
    
  }
  yer5[yeryaz] = '\0';
  yeryaz = 0;
  yeristasyonu.write(&yer5, sizeof(yer5));
  mpu = 0;
  //MPU gelicek.
  digitalWrite(2, LOW); //Interrup aktif.
  digitalWrite(2, HIGH); //Interrupt kapalı.
  delay(15);
  if(strlen(mpuveri)>17)
  {
      yer(mpuveri);
   
      for(uzunm=0;uzunm < 32 ; uzunm++)
      {
        mpuveriyedek[uzunm]=mpuveri[uzunm];  
      }
  }
  else
  {
    yer(mpuveriyedek);  
    
  }
  yer5[yeryaz] = '\0';
  yeryaz = 0;
  yeristasyonu.write(&yer5, sizeof(yer5));
  for (mpu2 = 0; mpu2 < 32; mpu2++)
  {
    mpuveri[mpu2] = '\0';
  }
  //yeristasyonu.write(&bitis, sizeof(bitis));
  yeristasyonu.write(&enter, sizeof(enter));
  yeristasyonu.startListening();//Servo ve hakem şifresi için dinlemeye başla.
  //  yeristasyonu.write("t");

  /* yer5[yeryaz]='p';
    yer5[yeryaz+1]='\0';
    yeryaz=0;
    yeristasyonu.write(&yer5,sizeof(yer5));*/
  if (altimeter < Servoesik)
  {

  }
  while (ak < 150)

  { if (yeristasyonu.available())
    {
      yeristasyonu.read(&yer6, sizeof(yer6));
      servok = (String)yer6;
    }
    if (servok == "j")
    {
       sifirla();
       while(1)
      {
        rasp.println("j");
        
      }//Görev bitti sonsuz döngü.
      
    }
    if (servok == "g")
    {
      //yeristasyonu.stopListening();
      //yeristasyonu.write(&ser, sizeof(ser));
      Serial.print("1");
      servok = "0";
      break;
      //yeristasyonu.startListening();
    }
    if (servok == "f")
    {
      //yeristasyonu.stopListening();
      //yeristasyonu.write(&ser2, sizeof(ser2));
        Serial.print("2");
      servok = "0";
      //yeristasyonu.startListening();
      break;
    }
    
    if (servok == "s")
    {
      yeristasyonu.startListening();
      rasp.print("Hakem Sifresi:");
      while (!(sifre == "u"))
      {
        if (yeristasyonu.available())
        {
          yeristasyonu.read(&oku, sizeof(oku));
          sifre = (String)oku;
          rasp.print(sifre);

        }

      }
     rasp.println();
    }
    delay(1);
    ak++;
    servok = "0"; sifre = "0";
  }

  //rasp.println();
  servok = "0"; sifre = "0";
  ak = 0;

  gecik = millis() - sure; //Sensör okunması ve diğer işlemler ne kadar zaman alıyor.

 if ((1000 - gecik) <  1)
    gecik = 0;       //gecikmenin negatif gelme ihtimaline karşılık.Hakem şifresinde u harfi görene kadar bekliyor hemen atılması lazım.
    
  smartDelay(1000 - gecik); //İşlem sürelerini 1snden çıkarıp smartdelayla gps encode uygula.
 }

// This custom version of delay() ensures that the gps object
// is being "fed".
static void smartDelay(unsigned long ms)
{
  ss.listen();
  unsigned long start = millis();
  do
  {
    while (ss.available())
      gps.encode(ss.read());
  } while (millis() - start < ms);
}

static void printFloat(float val, bool valid, int len, int prec)
{
  if (!valid)
  {
    
      rasp.print('*');
      yer("*");
  }
  else
  {
    rasp.print(val, prec);
    dtostrf(val, 8, prec + 3, gpsdata);
    int vi = abs((int)val);
    int flen = prec + (val < 0.0 ? 2 : 1); // . and -
    flen += vi >= 1000 ? 4 : vi >= 100 ? 3 : vi >= 10 ? 2 : 1;
    for (int i = flen; i < len; ++i)
      rasp.print(' ');
  }
  smartDelay(0);
}

static void printInt(unsigned long val, bool valid, int len)
{
  char sz[32] = "*****************";
  if (valid)
    sprintf(sz, "%ld", val);
  sz[len] = 0;
  for (int i = strlen(sz); i < len; ++i)
    sz[i] = ' ';
  if (len > 0)
    sz[len - 1] = ' ';
  // Serial.print(sz);
  smartDelay(0);
}

static void printDateTime(TinyGPSDate &d, TinyGPSTime &t)
{
  if (!d.isValid())
  {
    rasp.print(F("* "));
    yer("*,");
  }
  else
  {
    char sz[20];
    sprintf(sz, "%02d/%02d/%02d,", d.day() , d.month(), d.year());
    rasp.print(sz);
    yer(sz);
  }

  if (!t.isValid())
  {
    rasp.print(F("*#"));
    yer("*#") ;
  }
  else
  {
    char sz[20];
    
   if(kontrol) 
   {
     saat = t.hour();
     saat=saat+3;
     dakika = t.minute();
     saniye = t.second();
     kontrol = false;
   }
   /* if (t.hour() + 3 > 24)
    {
      hou = t.hour() + 3 - 24;
    }
    else
    {
      hou = t.hour() + 3;
    }*/
    sprintf(sz, "%02d:%02d:%02d#", saat, dakika, saniye);
    rasp.print(sz);
    yer(sz);
    saniye++;
    if(saniye==60)
    {
     saniye=0;  
     dakika++;
    }
    if(dakika==60)
    {
      dakika=0;
      saat++;  
    }
    if(saat==24)
    {
      saat=0;
    }
  }

  smartDelay(0);
}

static void printStr(const char *str, int len)
{
  int slen = strlen(str);
  for (int i = 0; i < len; ++i)
    rasp.print(i < slen ? str[i] : ' ');
  smartDelay(0);
}
void EEPROMWritelong(int address, long val)
{
  //Decomposition from a long to 4 bytes by using bitshift.
  //One = Most significant -> Four = Least significant byte
  byte four = (val & 0xFF);
  byte three = ((val >> 8) & 0xFF);
  byte two = ((val >> 16) & 0xFF);
  byte one = ((val >> 24) & 0xFF);

  //Write the 4 bytes into the eeprom memory.
  EEPROM.write(address, four);
  EEPROM.write(address + 1, three);
  EEPROM.write(address + 2, two);
  EEPROM.write(address + 3, one);
}
long EEPROMReadlong(long addresss)
{
  //Read the 4 bytes from the eeprom memory.
  long four = EEPROM.read(addresss);
  long three = EEPROM.read(addresss + 1);
  long two = EEPROM.read(addresss + 2);
  long one = EEPROM.read(addresss + 3);

  //Return the recomposed long by using bitshift.
  return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

void sifirla()
{
  EEPROM.write(10, 0);
  EEPROM.write(11, 0);
  EEPROM.write(12, 0);
  EEPROMWritelong(0, 0);
  yeristasyonu.stopListening();
  yeristasyonu.write(&none,sizeof(none));
  
}
void serialEvent() {

  while (Serial.available())
  {
    // get the new byte:
    incoming = (char)Serial.read();
    mpuveri[mpu] = incoming;
    mpu++;
    rasp.print(incoming);

  }

}

void nrfinit()
{
  yeristasyonu.begin();
  yeristasyonu.openWritingPipe(address);//00001
  yeristasyonu.openReadingPipe(1, adress); //00002
  yeristasyonu.setPALevel(RF24_PA_MAX);
  yeristasyonu.setDataRate(RF24_250KBPS);
  //yeristasyonu.setRetries(2,15);
  yeristasyonu.startListening();
}
void yer(unsigned char dizi[32])
{ sif = 0;
  dongu = boyut(dizi);
  while (dongu)
  {
    dongu--;
    if (dizi[sif] != '\0')
    {
      yer5[yeryaz] = dizi[sif];
    }
    yeryaz++;
    sif++;

  }
  sif = 0;
}
int boyut(char al[50])
{
  boy = 0;
  while (al[boy] != '\0')
  {
    boy++;
  }
  return boy;
}

