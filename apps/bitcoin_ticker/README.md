## Bitcoin Ticker (bitcoin_ticker)

### Overview

Small Arduino/ESP sketch that fetches the current Bitcoin price (USD) from the CoinGecko API and shows it on a small SSD1306 OLED.

---

### Files

- `bitcoin_ticker.ino` — main sketch.

---

### Hardware

- ESP32-compatible board (sketch uses `WiFi.h` and ESP time functions).
- SSD1306-compatible 128x64 OLED (I2C). Default pins in the sketch: `OLED_SDA = 8`, `OLED_SCL = 9`, I2C address `0x3C`.

---

### Software & Libraries

Install these libraries via the Arduino Library Manager or `arduino-cli` before building:

- `ArduinoJson`
- `Adafruit GFX Library`
- `Adafruit SSD1306`

Notes: `WiFi.h`, `HTTPClient.h`, and the NTP/time functions are provided by the ESP32 Arduino core.

arduino-cli example installs:

```bash
arduino-cli lib install "ArduinoJson"
arduino-cli lib install "Adafruit GFX Library"
arduino-cli lib install "Adafruit SSD1306"
```

---

### Configuration

- Edit `ssid` and `password` constants at the top of `bitcoin_ticker.ino` to match your Wi‑Fi network.
- If your board uses different I2C pins or a different OLED address, update `OLED_SDA`, `OLED_SCL`, and the address in `display.begin(...)`.
- Timezone: the sketch uses Eastern Time (UTC-5) by default. Modify the `configTime()` call if you need a different timezone.

---

### Deploy

Arduino IDE:

1. Open the sketch folder (the folder that contains `bitcoin_ticker.ino`).
2. Select your ESP32 board and the correct serial port.
3. Click Upload.

arduino-cli (example):

```bash
cd path/to/bitcoin_ticker
arduino-cli compile --fqbn <your_fqbn> .
arduino-cli upload -p /dev/ttyUSB0 --fqbn <your_fqbn> .
```

Replace `<your_fqbn>` with your board FQBN and `/dev/ttyUSB0` with your serial device.

---

### Behavior & Notes

- The sketch pulls price data from CoinGecko: `https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=usd`.
- Refresh interval is 60 seconds (adjust the `delay(60000)` in `loop()` if you want faster or slower updates).
- CoinGecko applies rate limits — avoid very short polling intervals.

---

### Troubleshooting

- OLED not detected: verify wiring, I2C pins, and address (0x3C). If `display.begin()` fails the sketch halts in the current code.
- Wi‑Fi not connecting: confirm SSID/password and that the board has Wi‑Fi support.
- JSON parse errors: ensure the API is reachable and not returning an error page.

---

### License

See the repository `LICENSE`.
