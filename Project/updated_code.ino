#include <Wire.h>
#undef I2C_BUFFER_LENGTH  // tránh bị redefine
#include <MAX30105.h>
#include "heartRate.h"
#include <ESP8266WiFi.h>
#include "ThingSpeak.h"
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ===== SSD1306 OLED =====
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ===== WiFi & ThingSpeak =====
const char* ssid        = "Anh An 0103";
const char* password    = "0913524247";
const unsigned long channelID   = 2840798;
const char* writeAPIKey         = "WATQA4GV3ZR4KZIU";
const uint32_t UPLOAD_INTERVAL  = 15000;

// ===== GPS =====
static const uint32_t GPSBaud = 9600;
SoftwareSerial gpsSerial(D6 /*RX*/, D7 /*TX*/);
TinyGPSPlus gps;

// ===== Biến toàn cục =====
WiFiClient  client;
MAX30105    particleSensor;

const uint8_t IBI_SAMPLES = 8;
uint32_t      ibiTimes[IBI_SAMPLES];
uint8_t       ibiIndex     = 0;
bool          ibiFilled    = false;
uint32_t      lastBeatTime = 0;
float         beatsPerMinute = 0.0;
float         spO2Value      = 0.0;
float         latitude   = NAN;
float         longitude  = NAN;
uint32_t lastGpsMsg = 0;
const uint32_t GPS_MSG_INTERVAL = 5000;

void setup() {
  Serial.begin(115200);
  delay(10);
  Serial.println(F("ESP8266 + MAX30102 + GPS + OLED + ThingSpeak"));

  // 1) Kết nối WiFi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print(F("Connecting to WiFi"));
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println(F("\nWiFi connected"));

  // 2) ThingSpeak
  ThingSpeak.begin(client);

  // 3) MAX30102
  if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) {
    Serial.println(F("ERROR: MAX30102 not found!"));
    while (1) delay(10);
  }
  particleSensor.setup();
  Serial.println(F("MAX30102 initialized"));

  // 4) GPS
  gpsSerial.begin(GPSBaud);
  Serial.println(F("GPS initialized"));

  // 5) SSD1306 OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("ERROR: SSD1306 not found!"));
    while (1);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(F("OLED initialized"));
  display.display();
  delay(1000);
}

void loop() {
  // —— GPS —— 
  while (gpsSerial.available()) gps.encode(gpsSerial.read());
  if (gps.location.isValid()) {
    latitude  = gps.location.lat();
    longitude = gps.location.lng();
  } else if (millis() - lastGpsMsg > GPS_MSG_INTERVAL) {
    lastGpsMsg = millis();
    Serial.println(F("Waiting for GPS lock..."));
  }

  // —— MAX30102: BPM & SpO2 —— 
  long irValue = particleSensor.getIR();
  if (checkForBeat(irValue)) {
    uint32_t now = millis();
    uint32_t delta = now - lastBeatTime;
    lastBeatTime = now;

    if (delta > 250 && delta < 2000) {
      ibiTimes[ibiIndex] = delta;
      ibiIndex = (ibiIndex + 1) % IBI_SAMPLES;
      if (ibiIndex == 0) ibiFilled = true;

      uint32_t sum = 0;
      uint8_t count = ibiFilled ? IBI_SAMPLES : ibiIndex;
      for (uint8_t i = 0; i < count; i++) sum += ibiTimes[i];
      float avgIBI = float(sum) / float(count);
      beatsPerMinute = 60000.0 / avgIBI;

      int16_t redLED = particleSensor.getRed();
      spO2Value = 110.0 - 25.0 * (float(redLED) / float(irValue));
      spO2Value = constrain(spO2Value, 70.0, 100.0);

      // —— Serial Debug —— 
      Serial.print(F("BPM=")); Serial.print(beatsPerMinute, 1);
      Serial.print(F("  SpO2=")); Serial.print(spO2Value, 1);
      if (!isnan(latitude) && !isnan(longitude)) {
        Serial.print(F("  Lat=")); Serial.print(latitude, 6);
        Serial.print(F("  Lon=")); Serial.println(longitude, 6);
      } else {
        Serial.println();
      }

      // —— OLED Display —— 
      display.clearDisplay();
      display.setTextSize(2);
      display.setCursor(0, 0);
      display.print("BPM: ");
      display.println(beatsPerMinute, 1);
      display.setTextSize(1);
      display.print("SpO2: ");
      display.print(spO2Value, 1);
      display.println("%");

      if (!isnan(latitude) && !isnan(longitude)) {
        display.setTextSize(1);
        display.print("Lat: ");
        display.println(latitude, 4);
        display.print("Lon: ");
        display.println(longitude, 4);
      } else {
        display.println("Waiting GPS...");
      }
      display.display();
    }
  }

  // —— ThingSpeak —— 
  static uint32_t lastUpload = 0;
  if (millis() - lastUpload >= UPLOAD_INTERVAL) {
    lastUpload = millis();
    ThingSpeak.setField(1, beatsPerMinute);
    ThingSpeak.setField(2, spO2Value);
    if (!isnan(latitude))  ThingSpeak.setField(3, latitude);
    if (!isnan(longitude)) ThingSpeak.setField(4, longitude);
    int httpCode = ThingSpeak.writeFields(channelID, writeAPIKey);
    if (httpCode == 200) {
      Serial.println(F("✔ ThingSpeak upload successful"));
    } else {
      Serial.print(F("✖ Upload failed, HTTP code="));
      Serial.println(httpCode);
    }
  }

  delay(10);
}
