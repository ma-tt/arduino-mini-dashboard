## Bitcoin Ticker (bitcoin_ticker)

---

### Overview

Small Arduino/ESP sketch that fetches the current Bitcoin price (USD) from the CoinGecko API and shows it on a small SSD1306 OLED.

---

### Files

- `bitcoin_ticker.ino` — main sketch (path: `apps/bitcoin_ticker/bitcoin_ticker.ino`).

---

### Hardware

- ESP32-compatible board (sketch uses `WiFi.h`, `WiFiClientSecure`, `HTTPClient`, and NTP/time functions from the ESP core).
- SSD1306-compatible 128x64 OLED (I2C).
	- Defaults in the sketch: `I2C_SDA = 21`, `I2C_SCL = 22` (common ESP32 pins). Change these if your board uses different pins.
	- Default I2C address: `0x3C` (change to `0x3D` in the sketch if your module uses that address).

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
- The sketch uses `WiFiClientSecure` for HTTPS. The current code calls `client.setInsecure()` to skip certificate validation (convenient for testing). For production, replace this with proper CA or certificate pinning.

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
// secrets.h (example — add to .gitignore)
const char* WIFI_SSID = "your_ssid";
const char* WIFI_PASS = "your_password";
```

- I2C pins & address: the sketch defaults to `I2C_SDA = 21`, `I2C_SCL = 22`, and `OLED_ADDRESS = 0x3C`. Edit these constants in the sketch to match your wiring.

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

### Behavior & Notes

- The sketch pulls price data from CoinGecko: `https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=usd`.
- The code now uses a non-blocking `millis()`-based interval and attempts Wi‑Fi reconnects in the background.
- HTTPS: `WiFiClientSecure` is used. The current code sets `client.setInsecure()` for convenience. Replace with proper CA validation or certificate pinning for secure deployments.
- JSON buffer size: the sketch uses a 512-byte `StaticJsonDocument`. Increase this size if you expand parsed fields.
- Display fallback: if the OLED is not detected the sketch will continue and print diagnostics to Serial at 115200 instead of halting.

---

### Troubleshooting

- OLED not detected: confirm wiring (SDA/SCL pins), the I2C address (0x3C vs 0x3D), and change `I2C_SDA`/`I2C_SCL` in the sketch if needed. The sketch logs a warning to Serial when the display is missing.
- Wi‑Fi not connecting: verify credentials in `secrets.h` (or in the sketch) and that your board supports Wi‑Fi.
- HTTP/HTTPS failures: ensure NTP/time is working (time is used for TLS) and that you have working internet access. If you remove `setInsecure()`, configure certificate validation correctly.
- JSON parse errors: the API may return an error page or truncated response — check Serial output and increase `JSON_BUF_SIZE` if necessary.
- Rate limits (HTTP 429): increase `FETCH_INTERVAL_MS` to reduce request frequency.

---

### License

See the repository `LICENSE`.
