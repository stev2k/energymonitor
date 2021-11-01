

/******************************************************************************
   Copyright 2018 Google
   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
 *****************************************************************************/

#if defined(ARDUINO_SAMD_MKR1000) or defined(ESP8266)
#define __SKIP_ESP32__
#endif

#if defined(ESP32)
#define __ESP32_MQTT_H__
#endif

#ifdef __SKIP_ESP32__

#include <Arduino.h>

void setup() {
  Serial.begin(115200);
}

void loop() {
  Serial.println("Hello World");
}

#endif

#ifdef __ESP32_MQTT_H__
#include "esp32-mqtt.h"
#include "EmonLib.h"
#include <LiquidCrystal_I2C.h>
#include <jwt.h>
#include "time.h"

LiquidCrystal_I2C lcd(0x27, 16, 2);    // Create an instance of 16 column 2 row lcd on 0x27
EnergyMonitor emon;
const long gmtOffset_sec = 0;
const int daylightOffset_sec = 3600;
unsigned long epochTime;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  emon.current(34, 6);
  configTime(0, 0,  ntp_primary);
  pinMode(LED_BUILTIN, OUTPUT);
  setupCloudIoT();
}

// Function that gets current epoch time
String getTime() {
  time_t now;
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return "0";
  }
  time(&now);
  return (String)now;
}

unsigned long lastMillis = 0;
void loop() {
  mqtt->loop();
  delay(10);  // <- fixes some issues with WiFi stability

  if (!mqttClient->connected()) {
    connect();
  }
  double Irms = emon.calcIrms(1480);  // Calculate Irms only
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(round(Irms * 230)); // current * uk mains voltage = power
  lcd.print("w");
  Serial.println(round(Irms * 230));
  delay (1000);
  String json = "{\"timestamp\":\"" + getTime() + "\",\"power\": \"" + String(round(Irms * 230)) + "\"}";
  publishTelemetry(json);
}
#endif
