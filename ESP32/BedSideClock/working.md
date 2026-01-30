# ESP32 Weather Display & Relay Control

This project integrates an **ESP32**, **SSD1306 OLED display**, **capacitive touch sensor**, and a **relay module** to create an interactive weather dashboard. It fetches real-time weather data from the [Open-Meteo API](https://open-meteo.com/) and allows relay control using a long touch press.

---

## Features

- **Main UI Page:**  
  Displays current date, time (HH:MM:SS), temperature, and day/night status with ☀️/🌙 emojis.

- **Wind Page:**  
  Shows current wind speed.

- **Weather Page:**  
  Shows current weather description (Clear, Cloudy, Rain, etc.).

- **Sun Page:**  
  Displays sunrise and sunset times.

- **Relay Page:**  
  Shows relay status and includes a visual timer for long press toggle. Relay can only be toggled on this page using an 8-second long touch press.

- **Hourly Forecast (Optional):**  
  Displays hourly weather with icons (☀️/☁️/🌧️).

- **Smooth UI Transitions:**  
  Animated transitions between UI pages when swiping/tapping.

---

## Components

- ESP32 Development Board  
- SSD1306 OLED Display (I2C, 128x64)  
- Capacitive Touch Sensor (e.g., TTP223)  
- Relay Module (5V, 1-channel)  
- Jumper Wires & Breadboard  
- Power supply (3.3V/5V)  

---

## Connections

**OLED Display (I2C):**  
- VCC → 3.3V  
- GND → GND  
- SDA → GPIO 21  
- SCL → GPIO 22  

**Capacitive Touch Sensor:**  
- VCC → 3.3V or 5V  
- GND → GND  
- OUT → GPIO 4  

**Relay Module:**  
- IN → GPIO 5  
- VCC → 5V  
- GND → GND  

> Ensure all GNDs are common for stable operation.

---

## Libraries Required

- `Adafruit_SSD1306` – OLED driver  
- `Adafruit_GFX` – Graphics primitives  
- `ArduinoJson` – JSON parsing from Open-Meteo API  
- `WiFi` – ESP32 Wi-Fi connectivity  
- `HTTPClient` – HTTP GET requests  

> Install these libraries via Arduino IDE Library Manager.

---

## How It Works

1. **Wi-Fi & Weather Data:**  
   - ESP32 connects to Wi-Fi.  
   - Fetches JSON weather data from Open-Meteo API.  
   - Parses temperature, wind speed, day/night, sunrise/sunset, and weather description.

2. **UI & Pages:**  
   - OLED display updates according to the current page.  
   - Pages include Main, Wind, Weather, Sun, and Relay.  
   - Tap the capacitive sensor to switch pages.

3. **Time Display:**  
   - `getLocalTime()` keeps ESP32 time updated.  
   - Seconds are updated live on the Main Page using `millis()` and redraw flags.

4. **Relay Control:**  
   - Relay toggling is **only active on Relay Page**.  
   - Press and hold capacitive touch for 8 seconds to toggle relay.  
   - Relay Page shows a **progress timer** for long-press feedback.

5. **Weather Updates:**  
   - Weather data refreshes periodically (default 10 minutes).  
   - UI redraw only occurs when necessary to prevent ESP32 freezes.

---

## Optional Features

- Hourly forecast display with icons (☀️/☁️/🌧️).  
- Smooth transition animations when switching UI pages.  
- Animated progress bar for relay toggle long-press.

---

## Screenshots / UI Layout

1. **Main Page:**  
2026-01-30 17:30:45
Temp: 24.2°C
Day ☀️

2. **Wind Page:**  
Wind Speed: 5.8 km/h

3. **Weather Page:**  
Weather: Clear

4. **Sun Page:**  
Sunrise: 06:25
Sunset: 17:31

5. **Relay Page:**  
Relay: OFF
Press 8s to toggle
[===== ] // Progress bar

---

## Notes

- Ensure all components share a **common ground**.  
- Relay module behavior may vary: some are **active LOW**. Adjust code if needed.  
- Use a **stable power supply** for OLED and relay module.  

---

## License

This project is **open-source** and free to use for educational and personal projects.


