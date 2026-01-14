#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <time.h>

// ---------------- CONFIG ----------------
// Wi‑Fi (edit these or set via a secure config method)
const char* WIFI_SSID = "WIFI_NAME";
const char* WIFI_PASS = "WIFI_PASSWORD";

// Display / I2C defaults (common for many ESP32 dev boards)
static const int I2C_SDA = 21; // change if your board uses different pins
static const int I2C_SCL = 22;
static const uint8_t OLED_ADDRESS = 0x3C; // change to 0x3D if your module uses that

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// CoinGecko API (HTTPS)
const char* BITCOIN_API = "https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=usd";

// Timing
const unsigned long FETCH_INTERVAL_MS = 60UL * 1000UL; // 60 seconds

// JSON buffer size — increase if you expect larger responses
static const size_t JSON_BUF_SIZE = 512;

// ---------------- STATE ----------------
double lastPrice = 0.0;
double previousPrice = 0.0;
String lastUpdateTime = "";
bool displayAvailable = false;
unsigned long lastFetchMs = 0;

// ---------------- HELPERS ----------------
void safeDisplayClear() {
    if (!displayAvailable) return;
    display.clearDisplay();
}

void safeDisplayDisplay() {
    if (!displayAvailable) return;
    display.display();
}

String formatPrice(long price) {
    String result = "";
    int count = 0;
    if (price == 0) return "0";
    while (price > 0) {
        if (count > 0 && count % 3 == 0) result = "," + result;
        result = String(price % 10) + result;
        price /= 10;
        count++;
    }
    return result;
}

String getCurrentTimeString() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) return String("---- --:--");
    char buf[32];
    strftime(buf, sizeof(buf), "%Y-%m-%d %I:%M%p", &timeinfo);
    return String(buf);
}

void showOnDisplayHeader(const char* title) {
    if (!displayAvailable) return;
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.println(title);
}

void showPriceOnDisplay(long priceInt, double prevPrice) {
    if (!displayAvailable) return;

    safeDisplayClear();
    showOnDisplayHeader("Bitcoin Price (USD)");

    String priceStr = String("$") + formatPrice(priceInt);

    // Compute text width and position price centered vertically
    display.setTextSize(2);
    int16_t x1, y1;
    uint16_t w, h;
    display.getTextBounds(priceStr, 0, 18, &x1, &y1, &w, &h);

    int priceX = 0; // default to left
    if (w < (SCREEN_WIDTH - 24)) {
        priceX = 0; // leave room for arrow at right
    }

    display.setCursor(priceX, 18);
    display.print(priceStr);

    // Arrow indicator positioned to the right of the price
    display.setTextSize(2);
    int arrowX = min(SCREEN_WIDTH - 18, (int)(priceX + w + 2));
    display.setCursor(arrowX, 18);
    if (priceInt > (long)prevPrice) display.println("^");
    else if (priceInt < (long)prevPrice) display.println("v");
    else display.println("-");

    // Timestamp bottom
    display.setTextSize(1);
    display.setCursor(0, SCREEN_HEIGHT - 10);
    display.println(lastUpdateTime);

    safeDisplayDisplay();
}

void showErrorOnDisplay(const String &msg) {
    if (!displayAvailable) return;
    safeDisplayClear();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.println(msg);
    if (lastPrice > 0) {
        showPriceOnDisplay((long)lastPrice, previousPrice);
    } else {
        safeDisplayDisplay();
    }
}

bool ensureWiFi() {
    if (WiFi.status() == WL_CONNECTED) return true;
    WiFi.disconnect();
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    unsigned long start = millis();
    while (millis() - start < 10000) {
        if (WiFi.status() == WL_CONNECTED) return true;
        delay(250);
    }
    return (WiFi.status() == WL_CONNECTED);
}

// ---------------- SETUP ----------------
void setup() {
    Serial.begin(115200);
    delay(100);

    // Initialize I2C and display
    Wire.begin(I2C_SDA, I2C_SCL);
    displayAvailable = display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS);
    if (!displayAvailable) {
        Serial.println("Warning: OLED not found at address " + String(OLED_ADDRESS, HEX));
    } else {
        display.clearDisplay();
        display.setTextColor(SSD1306_WHITE);
        display.setTextSize(1);
        display.setCursor(0, 0);
        display.println("Starting...");
        display.display();
    }

    // Connect Wi-Fi
    Serial.println("Connecting to Wi-Fi...");
    if (ensureWiFi()) {
        Serial.println("Wi-Fi connected");
        if (displayAvailable) {
            display.clearDisplay();
            display.setCursor(0, 0);
            display.println("Wi-Fi connected!");
            display.display();
        }
    } else {
        Serial.println("Wi-Fi connection failed");
        if (displayAvailable) {
            display.clearDisplay();
            display.setCursor(0, 0);
            display.println("Wi-Fi failed");
            display.display();
        }
    }

    // Timezone: EST/EDT with DST rules. Change TZ if needed for your location.
    setenv("TZ", "EST5EDT,M3.2.0/2,M11.1.0/2", 1);
    tzset();
    configTime(0, 0, "time.google.com", "pool.ntp.org");
    delay(1000);
}

// ---------------- FETCH & LOOP ----------------
void fetchAndDisplayPrice() {
    if (!ensureWiFi()) {
        Serial.println("Wi-Fi disconnected — skipping fetch");
        showErrorOnDisplay("Wi-Fi disconnected!");
        return;
    }

    WiFiClientSecure client;
    client.setInsecure(); // skipping cert validation; for production, pin certificate or use fingerprint

    HTTPClient http;
    if (!http.begin(client, BITCOIN_API)) {
        Serial.println("HTTP begin failed");
        showErrorOnDisplay("HTTP begin failed");
        return;
    }

    int httpCode = http.GET();
    if (httpCode == HTTP_CODE_OK) {
        String payload = http.getString();
        StaticJsonDocument<JSON_BUF_SIZE> doc;
        DeserializationError err = deserializeJson(doc, payload);
        if (err) {
            Serial.println("JSON parse error: " + String(err.c_str()));
            showErrorOnDisplay("JSON parse error!");
        } else {
            previousPrice = lastPrice;
            double val = doc["bitcoin"]["usd"].as<double>();
            lastPrice = round(val);
            lastUpdateTime = getCurrentTimeString();
            Serial.println("Price: $" + String((long)lastPrice) + " at " + lastUpdateTime);
            showPriceOnDisplay((long)lastPrice, previousPrice);
        }
    } else if (httpCode == 429) {
        Serial.println("Rate limited (429)");
        showErrorOnDisplay("Rate limit hit!");
    } else {
        Serial.println("HTTP error: " + String(httpCode));
        showErrorOnDisplay("HTTP Error: " + String(httpCode));
    }
    http.end();
}

void loop() {
    unsigned long now = millis();
    if (now - lastFetchMs >= FETCH_INTERVAL_MS || lastFetchMs == 0) {
        lastFetchMs = now;
        fetchAndDisplayPrice();
    }

    // light-weight background tasks: handle WiFi reconnect if dropped
    if (WiFi.status() != WL_CONNECTED) {
        static unsigned long lastReconnectAttempt = 0;
        if (now - lastReconnectAttempt > 10000) {
            lastReconnectAttempt = now;
            Serial.println("Attempting Wi-Fi reconnect...");
            WiFi.disconnect();
            WiFi.begin(WIFI_SSID, WIFI_PASS);
        }
    }
}
