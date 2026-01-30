# ESP32 Weather Display & Relay Control

This project uses an **ESP32**, an **OLED display (SSD1306 I2C)**, and a **capacitive touch sensor** to display live weather information, time, and control a relay with a long press.

## Features

- **Live Time & Date:** Updates seconds in real-time.
- **Weather Information:**  
  - Current weather (Clear, Cloudy, Rain, etc.)  
  - Day/Night indicator (☀️/🌙)  
  - Sunrise & Sunset times  
  - Wind speed  
  - Optional hourly forecast with weather icons
- **UI Pages:** Swipe/tap to change views:
  1. **Main Page:** Date, time, temperature, day/night indicator  
  2. **Wind Page:** Wind speed  
  3. **Weather Page:** Current weather description  
  4. **Sun Page:** Sunrise & Sunset times  
  5. **Relay Page:** Shows relay status and allows toggling
- **Relay Control:** Long press the touch sensor for **8 seconds** on the Relay page to toggle the relay.
- **Progress Feedback:** Relay page shows how long to hold the touch sensor before activation.

## Hardware Required

- ESP32 development board  
- SSD1306 OLED display (I2C)  
- Capacitive touch sensor (e.g., TTP223)  
- Relay module  
- Jumper wires

## Wiring

| Component           | ESP32 Pin |
|--------------------|-----------|
| OLED SDA           | GPIO 21   |
| OLED SCL           | GPIO 22   |
| Touch Sensor OUT   | GPIO 4    |
| Relay IN           | GPIO 5    |
| Relay VCC          | 3.3V/5V   |
| Relay GND          | GND       |

## Software Requirements

- Arduino IDE with ESP32 board support  
- Libraries:
  - `Adafruit_SSD1306`
  - `Adafruit_GFX`
  - `ArduinoJson`
  - `WiFi`

## Usage

1. Connect the ESP32 and peripherals.  
2. Set your **WiFi credentials** in the code.  
3. Replace the weather API URL with your Open-Meteo API URL.  
4. Upload the code.  
5. Use the **capacitive touch sensor**:
   - Tap: change UI page  
   - Long press 8 seconds on Relay page: toggle relay  

The OLED will show the current weather, time, date, wind, sunrise/sunset, and relay status.

## Notes

- Make sure the API URL returns JSON in the correct Open-Meteo format.  
- Main page updates seconds live without freezing.  
- Relay toggling works **only on the Relay page**.
