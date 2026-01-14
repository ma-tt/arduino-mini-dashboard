## Bitcoin Ticker (bitcoin_ticker)

### Overview

Small Arduino/ESP sketch that fetches the current Bitcoin price (USD) from the CoinGecko API and shows it on a small SSD1306 OLED.

---

### Files

- `bitcoin_ticker.ino` — main sketch (path: `apps/bitcoin_ticker/bitcoin_ticker.ino`).

---

### Hardware

- ESP32-compatible board.
- SSD1306-compatible 128x64 OLED (I2C).

---

### Software & Libraries

The sketch requires three third-party libraries installed from the Arduino Library Manager. Install these before building:

| Header | Library name to search | Author |
| --- | --- | --- |
| `#include <ArduinoJson.h>` | ArduinoJson | Benoit Blanchon |
| `#include <Adafruit_SSD1306.h>` | Adafruit SSD1306 | Adafruit |
| `#include <Adafruit_GFX.h>` | Adafruit GFX Library | Adafruit |

Notes:
- `WiFi.h`, `WiFiClientSecure`, `HTTPClient.h`, and the NTP/time functions are provided by the ESP32 Arduino core — install the ESP32 board package in your IDE or `arduino-cli` before compiling.

arduino-cli example installs (exact library names):

```bash
arduino-cli lib install "ArduinoJson"
arduino-cli lib install "Adafruit SSD1306"
arduino-cli lib install "Adafruit GFX Library"
```

---

### Configuration

- Wi‑Fi credentials:

```cpp
const char* WIFI_SSID = "your_ssid";
const char* WIFI_PASS = "your_password";
```

- Timezone: by default the sketch sets `TZ` to `EST5EDT` (Eastern Time with DST). Change the `setenv("TZ", ...)` line in the sketch to a POSIX TZ string for your timezone.

- Fetch interval: default is 60 seconds. Adjust `FETCH_INTERVAL_MS` in the sketch if you want a different update frequency. Beware CoinGecko rate limits.

---

### Deploy

Arduino IDE:

1. Open the folder `apps/bitcoin_ticker/` in the Arduino IDE (it should detect `bitcoin_ticker.ino`).
2. Select your ESP32 board and serial port.
3. Upload.

arduino-cli (example):

```bash
cd apps/bitcoin_ticker
arduino-cli compile --fqbn <your_fqbn> .
arduino-cli upload -p /dev/ttyUSB0 --fqbn <your_fqbn> .
```

Replace `<your_fqbn>` with your board FQBN (example: `esp32:esp32:esp32dev`) and `/dev/ttyUSB0` with your serial device. Verify FQBNs with `arduino-cli board listall`.

---

### License

See the repository `LICENSE`.
