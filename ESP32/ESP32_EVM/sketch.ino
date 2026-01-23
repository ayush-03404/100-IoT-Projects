#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>

WebServer server(80);

/* ================= CONFIG ================= */

const char* ssid = "ESP32_EVM";  // SoftAP SSID you can change it according to your preference
const char* password = "11223344"; // SoftAP Password you can change it according to your preference

#define BUZZER_PIN 25      // GPIO25
#define LED_PIN    21      // GPIO21 

const int buttonPins[6] = {13, 14, 15, 18, 17, 16};

const char* candidateNames[6] = {  // Names of candidates
  "Candidate A", 
  "Candidate B",
  "Candidate C",
  "Candidate D",
  "Candidate E",
  "Candidate F"
};

/* ========================================= */

int votes[6] = {0};

bool lastButtonState[6] = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};
bool buttonState[6]     = {HIGH, HIGH, HIGH, HIGH, HIGH, HIGH};

unsigned long lastDebounceTime[6] = {0};
const unsigned long debounceDelay = 80;

unsigned long buzzerOffTime = 0;
bool buzzerActive = false;

/* ============ EEPROM ===================== */

void loadVotes() {
  for (int i = 0; i < 6; i++) {
    EEPROM.get(i * sizeof(int), votes[i]);
    if (votes[i] < 0 || votes[i] > 10000) votes[i] = 0;
  }
}

void saveVote(int index) {
  EEPROM.put(index * sizeof(int), votes[index]);
  EEPROM.commit();
}

void resetVotes() {
  for (int i = 0; i < 6; i++) {
    votes[i] = 0;
    EEPROM.put(i * sizeof(int), votes[i]);
  }
  EEPROM.commit();
}

/* ============ WEB ======================== */

String buildPage() {
  String html = "<!DOCTYPE html><html><head>";
  html += "<meta http-equiv='refresh' content='2'>";
  html += "<title>EVM</title>";
  html += "<style>body{font-family:Arial;text-align:center;}</style>";
  html += "</head><body><h2>ESP32 Voting Machine</h2><ul>";

  for (int i = 0; i < 6; i++) {
    html += "<li>";
    html += candidateNames[i];
    html += " : <b>";
    html += votes[i];
    html += "</b></li>";
  }

  html += "</ul>";
  html += "<form action='/reset' method='POST'>";
  html += "<button style='font-size:20px;'>RESET VOTES</button>";
  html += "</form></body></html>";
  return html;
}

void handleRoot() {
  server.send(200, "text/html", buildPage());
}

void handleReset() {
  resetVotes();
  Serial.println("Votes RESET from web");
  server.sendHeader("Location", "/");
  server.send(303);
}

/* ============ SETUP ====================== */

void setup() {
  Serial.begin(9600);
  delay(200);

  EEPROM.begin(64);
  loadVotes();

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  for (int i = 0; i < 6; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
  }

  WiFi.softAP(ssid, password);
  Serial.println("\nSoftAP Started");
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.on("/reset", HTTP_POST, handleReset);
  server.begin();

  Serial.println("Web server started");
}

/* ============ LOOP ======================= */

void loop() {
  server.handleClient();

  for (int i = 0; i < 6; i++) {
    bool reading = digitalRead(buttonPins[i]);

    if (reading != lastButtonState[i]) {
      lastDebounceTime[i] = millis();
    }

    if ((millis() - lastDebounceTime[i]) > debounceDelay) {
      if (reading != buttonState[i]) {
        buttonState[i] = reading;

        // BUTTON PRESSED (LOW)
        if (buttonState[i] == LOW) {
          votes[i]++;
          saveVote(i);

          Serial.print(candidateNames[i]);
          Serial.print(" voted. Total: ");
          Serial.println(votes[i]);

          digitalWrite(BUZZER_PIN, HIGH);
          digitalWrite(LED_PIN, HIGH);
          buzzerActive = true;
          buzzerOffTime = millis() + 120;
        }
      }
    }
    lastButtonState[i] = reading;
  }

  if (buzzerActive && millis() > buzzerOffTime) {
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
    buzzerActive = false;
  }
}


