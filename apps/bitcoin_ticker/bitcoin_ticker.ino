#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <time.h>

// ---------------- OLED CONFIG ----------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_SDA 8
#define OLED_SCL 9

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---------------- WIFI CONFIG ----------------
const char* ssid = "WIFI_NAME";
const char* password = "WIFI_PASSWORD";

// ---------------- COINGECKO API ----------------
const char* bitcoinAPI = "https://api.coingecko.com/api/v3/simple/price?ids=bitcoin&vs_currencies=usd";

// ---------------- GLOBALS ----------------
float lastPrice = 0.0;
float previousPrice = 0.0;
String lastUpdateTime = "";

// ---------------- SETUP ----------------
void setup() {
    Wire.begin(OLED_SDA, OLED_SCL);

    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
        for(;;); // halt if OLED not found
    }
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);

    // Wi-Fi
    display.setTextSize(1);
    display.setCursor(0,0);
    display.println("Connecting Wi-Fi...");
    display.display();

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        display.print(".");
        display.display();
    }

    // NTP: Eastern Time, UTC-5 standard, UTC-4 DST
    configTime(-5 * 3600, 3600, "time.google.com", "pool.ntp.org"); 
    delay(1000);

    display.clearDisplay();
    display.setCursor(0,0);
    display.println("Wi-Fi connected!");
    display.display();
    delay(1000);
}

// ---------------- LOOP ----------------
void loop() {
    if(WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(bitcoinAPI);
        int httpCode = http.GET();

        if(httpCode == 200) {
            String payload = http.getString();
            StaticJsonDocument<256> doc;
            DeserializationError error = deserializeJson(doc, payload);
            if(error) {
                displayError("JSON parse error!");
            } else {
                previousPrice = lastPrice;
                lastPrice = round(doc["bitcoin"]["usd"].as<float>()); // round to nearest dollar
                lastUpdateTime = getCurrentTime();
                displayPrice();
            }
        } 
        else if(httpCode == 429) {
            displayError("Rate limit hit!");
        } 
        else {
            displayError("HTTP Error: " + String(httpCode));
        }
        http.end();
    } else {
        displayError("Wi-Fi disconnected!");
    }

    delay(60000); // 60-second refresh
}

// ---------------- FUNCTIONS ----------------
void displayPrice() {
    display.clearDisplay();

    // Header
    display.setTextSize(1);
    display.setCursor(0,0);
    display.println("Bitcoin Price (USD)");

    // Price
    display.setTextSize(2);
    display.setCursor(0,18);
    display.print("$");
    display.print(formatPrice((int)lastPrice).c_str());

    // Arrow (ASCII)
    display.setTextSize(2);
    display.setCursor(100,18);
    if(lastPrice > previousPrice) display.println("^");
    else if(lastPrice < previousPrice) display.println("v");
    else display.println("-");

    // Timestamp bottom
    display.setTextSize(1);
    display.setCursor(0, 54);
    display.println(lastUpdateTime);

    display.display();
}

void displayError(String message) {
    display.clearDisplay();

    display.setTextSize(1);
    display.setCursor(0,0);
    display.println(message);

    if(lastPrice > 0) {
        display.setTextSize(2);
        display.setCursor(0,18);
        display.print("$");
        display.print(formatPrice((int)lastPrice).c_str());

        display.setTextSize(2);
        display.setCursor(100,18);
        if(lastPrice > previousPrice) display.println("^");
        else if(lastPrice < previousPrice) display.println("v");
        else display.println("-");

        display.setTextSize(1);
        display.setCursor(0, 54);
        display.println(lastUpdateTime);
    }

    display.display();
}

// ---------------- HELPER ----------------
// Format integer with commas for thousands
String formatPrice(int price) {
    String result = "";
    int count = 0;

    if(price == 0) return "0";

    while(price > 0) {
        if(count > 0 && count % 3 == 0) {
            result = "," + result;
        }
        result = String(price % 10) + result;
        price /= 10;
        count++;
    }
    return result;
}

// Get current time in Eastern Time, formatted YYYY-MM-DD hh:MMAM/PM
String getCurrentTime() {
    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)) return "---- --:--";

    char buf[25];
    strftime(buf, sizeof(buf), "%Y-%m-%d %I:%M%p", &timeinfo);
    return String(buf);
}
