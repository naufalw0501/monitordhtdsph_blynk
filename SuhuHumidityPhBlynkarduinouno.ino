#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHT.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <SoftwareSerial.h>

#define ONE_WIRE_BUS 2
#define DHTPIN 2
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial Nano(7, 8);
LiquidCrystal_I2C lcd(0x27, 16, 4);
// Deklarasi PIN DHT11 //
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensorSuhu(&oneWire);

float calibration_value = 21.34;
int phval = 0;
float t,h,volt,ph_act,suhuDS;
unsigned long int avgval;
int buffer_arr[10], temp;

// Tipe sensor yang digunakan (DHT11 atau DHT22) //


union { //data dikirim ke NodeMCU
  struct {
    float suhuDHT;
    float humDHT;
    float suhuDS;
    float PH;
  } param;
  byte packet[16];
} dataKirim;

// Membuat ikon kelelembaban //
byte suhu[8] =
{
  B00100,
  B01010,
  B01010,
  B01110,
  B11111,
  B11111,
  B01110,
  B00000
};

// Membuat ikon kelelembaban //
byte kelembaban[8] =
{
  B00100,
  B01010,
  B01010,
  B10001,
  B10001,
  B10001,
  B01110,
  B00000
};

void setup() {

  // put your setup code here, to run once
  Serial.begin(115200);
  Nano.begin(115200);
  lcd.init(); // initialize the lcd

  // Print a message to the LCD.

  lcd.backlight();
  lcd.init();
  //  lcd.backlight();
  //  lcd.createChar(1, kelembaban);
  //  lcd.createChar(2, suhu);
  lcd.setCursor(0, 0); lcd.print("Squilliam");
  lcd.setCursor(0, 1); lcd.print("Fancyson 2021");
  delay(2000);
  lcd.clear();
  //  lcd.write(2);
  lcd.setCursor(8, 0); lcd.print("T2:");
  lcd.setCursor(8, 1); lcd.print("Hum:  %");
  //  lcd.write(1);
  dht.begin();
}

void loop() {
  // Menampilkan pesan Error jika sensor tidak terbaca //
  //  lcd.setCursor(12, 0); lcd.print(t);
  //  lcd.setCursor(13, 1); lcd.print(h);
  //
  //  float hif = dht.computeHeatIndex(f, h);
  //  float hic = dht.computeHeatIndex(t, h, false);
  // Menampilkan data ke LCD //
  read_PH();
  readDHT();
  readDS();
  Serial.println(ph_act);
  Serial.println(t);
  Serial.println(h);
  Serial.println(suhuDS);

  lcd.setCursor(0, 0); lcd.print("T1:");
  lcd.setCursor(3, 0); lcd.print(suhuDS);

  lcd.setCursor(12, 0); lcd.print(t, 1);
  lcd.setCursor(13, 1); lcd.print(h, 0);

  lcd.setCursor(0, 1); lcd.print("pH:");
  lcd.setCursor(3, 1); lcd.print(ph_act);

  sendData();
  // Jeda Waktu, ubah menjadi 2000 untuk DHT22//
  delay(1000);

}

void read_PH() {
  for (int i = 0; i < 10; i++)
  {
    buffer_arr[i] = analogRead(A0);
    delay(30);
  }
  for (int i = 0; i < 9; i++)
  {
    for (int j = i + 1; j < 10; j++)
    {
      if (buffer_arr[i] > buffer_arr[j])
      {
        temp = buffer_arr[i];
        buffer_arr[i] = buffer_arr[j];
        buffer_arr[j] = temp;
      }
    }
  }
  avgval = 0;
  for (int i = 2; i < 8; i++)
    avgval += buffer_arr[i];
  volt = (float)avgval * 5.0 / 1024 / 6;
  ph_act = -3.225 * volt + calibration_value;
}

void readDHT() {
  // Membaca kelembaban //
  h = dht.readHumidity();
  // Membaca suhu dalam satuan Celsius //
  t = dht.readTemperature();
  // Membaca suhu dalam satuan Fahrenheit //
//  f = dht.readTemperature(true);
}

void readDS() {
  sensorSuhu.requestTemperatures();
  suhuDS = sensorSuhu.getTempCByIndex(0);
}

void sendData() {
  dataKirim.param.suhuDHT = t;
  dataKirim.param.humDHT = h;
  dataKirim.param.suhuDS = suhuDS;
  dataKirim.param.PH = ph_act;
  //  Serial.println(dataKirim.param.tb);
  //  Serial.println(dataKirim.param.hb);
  //  Serial.println(dataKirim.param.nilai_suhub);
  //  Serial.println(dataKirim.param.ph_actb);
  //  Serial.write("WOW");
  Nano.write('$');
  Nano.write(dataKirim.packet, sizeof(dataKirim.packet));
}
