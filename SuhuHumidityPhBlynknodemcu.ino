#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>

#define BLYNK_PRINT Serial

char auth[] = "UF0-GyUbm1zCBTz9tfYpx_PnXah5tv_K";
char ssid[] = "Bjir";
char pass[] = "samasaja";

union { //data dikirim ke NodeMCU
  struct {
    float suhuDHT;
    float humDHT;
    float suhuDS;
    float PH;
  } param;
  byte packet[16];
} dataTerima;

int comIndex;
bool valid;
char c1;

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
}

void loop() {
  if (Serial.available()) {
    c1 = Serial.read();
    if (c1 == '$'){
      comIndex = 0;
      valid = true;
    }
    else if (valid){
      dataTerima.packet[comIndex] = c1;
      comIndex++;
      if (comIndex >= sizeof(dataTerima.packet)) valid = false;
    }
  }
  
  Serial.print("tb = ");Serial.println(dataTerima.param.suhuDHT);
  Serial.print("hb = ");Serial.println(dataTerima.param.suhuDHT);
  Serial.print("suhu = ");Serial.println(dataTerima.param.suhuDS);
  Serial.print("ph = ");Serial.println(dataTerima.param.PH);
  Blynk.virtualWrite(V5, dataTerima.param.suhuDHT);
  Blynk.virtualWrite(V6, dataTerima.param.humDHT);
  Blynk.virtualWrite(V2, dataTerima.param.suhuDS);
  Blynk.virtualWrite(V1, dataTerima.param.PH);
  Blynk.run();

  delay(100);
}
