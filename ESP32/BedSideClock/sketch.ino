#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <time.h>

// ---------- USER SETTINGS ----------
const char* WIFI_SSID = "Your_SSID_Here"; // WiFi SSID
const char* WIFI_PASS = "Your_Password_Here"; // WiFi Password
const char* WEATHER_API_URL = "Your_Weather_API_URL_Here"; //Open-Meteo API URL

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1

#define TOUCH_PIN 4     // TTP223 output connected to GPIO4
#define RELAY_PIN 5     // Relay control pin

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---------- GLOBAL VARIABLES ----------
float currentTemperature = 0;
bool relayState = false;
bool isDay = true;
float windSpeed = 0;
String weatherDesc = "";
String sunriseTime = "";
String sunsetTime = "";
// ---------- GLOBAL ----------
bool needsRedraw = true; // Force redraw initially
const unsigned long RELAY_HOLD_TIME = 8000; // 8 seconds for long press
int uiPage = 0; // Current UI page
unsigned long lastWeatherUpdate = 0;
const unsigned long weatherUpdateInterval = 10 * 60 * 1000; // 10 minutes
unsigned long lastMainUpdate = 0; // tracks last main page update
const unsigned long MAIN_REFRESH_INTERVAL = 1000; // 1 second

// ---------- TOUCH HANDLING ----------
unsigned long touchPressTime = 0;
bool touchActive = false;

// ---------- WIFI & TIME ----------
void connectWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected!");
}

void syncTime() {
  configTime(19800, 0, "pool.ntp.org", "time.nist.gov"); // IST
  struct tm timeinfo;
  while(!getLocalTime(&timeinfo)){
    delay(500);
  }
}

// ---------- WEATHER ----------
void fetchWeather() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(WEATHER_API_URL);
    int httpCode = http.GET();
    if (httpCode == 200) {
      String payload = http.getString();
      DynamicJsonDocument doc(6000); // Increase if needed
      deserializeJson(doc, payload);

      currentTemperature = doc["current"]["temperature_2m"];
      isDay = doc["current"]["is_day"];
      windSpeed = doc["current"]["wind_speed_10m"];

      int code = doc["hourly"]["weather_code"][0]; // Using first hourly entry as example
      switch (code) {
        case 0: weatherDesc = "Clear"; break;
        case 1: weatherDesc = "Partly Cloudy"; break;
        case 2: weatherDesc = "Cloudy"; break;
        case 3: weatherDesc = "Rain"; break;
        case 45: weatherDesc = "Fog"; break;
        default: weatherDesc = "Unknown"; break;
      }

      sunriseTime = String(doc["daily"]["sunrise"][0].as<const char*>());
      sunsetTime  = String(doc["daily"]["sunset"][0].as<const char*>());

    }
    http.end();
  }
}

// ---------- DISPLAY ----------
void drawMainPage(struct tm &timeinfo) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Date
  char dateStr[11];
  sprintf(dateStr, "%02d-%02d-%04d", timeinfo.tm_mday, timeinfo.tm_mon + 1, timeinfo.tm_year + 1900);
  display.setCursor(0,0);
  display.println(dateStr);

  // Time
  char timeStr[9];
  sprintf(timeStr, "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  display.setCursor(0,15);
  display.setTextSize(2);
  display.println(timeStr);

  // Temperature
  display.setTextSize(1);
  display.setCursor(0, 40);
  display.print("Temp: ");
  display.print(currentTemperature);
  display.println(" C");

  // Day/Night
  display.setCursor(0, 55);
  display.print(isDay ? "Day" : "Night");

  display.display();
}

void drawWindPage() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0,20);
  display.print("Wind:     ");
  display.print(windSpeed);
  display.println(" km/h");
  display.display();
}

void drawWeatherPage() {
  display.clearDisplay();         // Clear the screen
  display.setTextSize(2);         // Bigger text
  display.setTextColor(WHITE);    // White text
  display.setCursor(0, 20);       // Starting position

  display.print("Weather:  ");     // Add semicolon!
  display.println(weatherDesc);   // Display the weather description

  display.display();              // Send data to OLED
}

void drawSunPage() {
  display.clearDisplay();
  display.setTextSize(1.75);
  display.setCursor(0,20);
  display.print("Sunrise: ");
  display.println(sunriseTime.substring(11,16));
  display.setCursor(0,40);
  display.print("Sunset : ");
  display.println(sunsetTime.substring(11,16));
  display.display();
}

void drawRelayPageWithTimer() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);

  // Show relay status
  display.setCursor(0, 0);
  display.print("Relay: ");
  display.println(relayState ? "ON" : "OFF");

  // Only show timer if touch active
  if (touchActive) {
    unsigned long heldTime = millis() - touchPressTime;
    if (heldTime > RELAY_HOLD_TIME) heldTime = RELAY_HOLD_TIME;

    int barWidth = map(heldTime, 0, RELAY_HOLD_TIME, 0, 128); // OLED width 128
    display.drawRect(0, 20, 128, 10, WHITE); // Outline
    display.fillRect(0, 20, barWidth, 10, WHITE); // Progress

    display.setCursor(0, 35);
    display.print("Hold to toggle relay");
    display.setCursor(0, 45);
    display.print("Time: ");
    display.print(heldTime / 1000);
    display.print(" / 8 sec");
  } else {
    display.setCursor(0, 35);
    display.println("Long press to toggle");
  }

  display.display();
}
// ---------- SETUP ----------
void setup() {
  Serial.begin(115200);
  pinMode(TOUCH_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  display.clearDisplay();
  display.display();

  connectWiFi();
  syncTime();
  fetchWeather();
}

// ---------- LOOP ----------
void loop() {
  struct tm timeinfo;
  getLocalTime(&timeinfo);
  
  unsigned long currentMillis = millis();

  // --- Weather update ---
  if (currentMillis - lastWeatherUpdate > weatherUpdateInterval) {
    fetchWeather();
    lastWeatherUpdate = currentMillis;
    needsRedraw = true;
  }

  // --- Touch handling ---
  bool touchState = digitalRead(TOUCH_PIN);

  if (touchState == HIGH && !touchActive) {
    touchActive = true;
    touchPressTime = currentMillis;
  }
  else if (touchState == HIGH && touchActive) {
    // Handle relay long press only on Relay Page
    if (uiPage == 4) {
      unsigned long heldTime = currentMillis - touchPressTime;

      // Force redraw every 100ms for progress bar
      if (heldTime % 100 < 50) needsRedraw = true;

      // Toggle relay after 8 seconds hold
      if (heldTime >= RELAY_HOLD_TIME) {
        relayState = !relayState;
        digitalWrite(RELAY_PIN, relayState ? HIGH : LOW);
        touchActive = false;
        needsRedraw = true;
      }
    }
  }
  else if (touchState == LOW && touchActive) {
    // Tap: switch page if hold < RELAY_HOLD_TIME
    if (currentMillis - touchPressTime < RELAY_HOLD_TIME) {
      uiPage = (uiPage + 1) % 5;
      needsRedraw = true;
    }
    touchActive = false;
  }

  // --- Force main page redraw for live seconds ---
  if (uiPage == 0 && (currentMillis - lastMainUpdate >= MAIN_REFRESH_INTERVAL)) {
    needsRedraw = true;
    lastMainUpdate = currentMillis;
  }

  // --- Draw UI pages ---
  if (needsRedraw) {
    switch(uiPage) {
      case 0: drawMainPage(timeinfo); break; // Live seconds now
      case 1: drawWindPage(); break;
      case 2: drawWeatherPage(); break;
      case 3: drawSunPage(); break;
      case 4: drawRelayPageWithTimer(); break; // Relay page with progress
    }
    needsRedraw = false;
  }

  delay(50); // keeps loop responsive
}