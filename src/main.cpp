#include <Arduino.h>
#include <TFT_eSPI.h>
#include <WiFiManager.h>
#include <time.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "fonction.h"
#include "tempo.h"

TFT_eSPI tft = TFT_eSPI();

enum AppState {
    BOOT,
    WIFI_CONFIG,
    CONNECTING,
    CONNECTED
};

AppState currentState = BOOT;

bool lastConnectedState = false;
bool wifiConnected = false;

tempo t;

void setup()
{
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

    Serial.begin(115200);

    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    tft.init();
    tft.setRotation(1);

    currentState = BOOT;
    drawBoot();
    delay(1000);

    WiFiManager wm;

    currentState = WIFI_CONFIG;
    drawWifiConfig();

    currentState = CONNECTING;
    drawConnecting();

    bool res = wm.autoConnect("ESP32_Config");

    if (res)
    {
        currentState = CONNECTED;
        configTime(3600, 3600, "pool.ntp.org", "time.google.com");
        drawConnected();
    }
    else
    {
        currentState = WIFI_CONFIG;
        drawWifiConfig();
    }

    t.updateColors();
    
}

void loop()
{
    if (currentState == CONNECTED && WiFi.status() != WL_CONNECTED)
    {
        currentState = WIFI_CONFIG;

        WiFiManager wm;
        drawWifiConfig();

        if (wm.autoConnect("ESP32_Config"))
        {
            currentState = CONNECTED;
            drawConnected();
        }
    }

    bool isConnected = (WiFi.status() == WL_CONNECTED);

    if (isConnected != lastConnectedState)
    {
        lastConnectedState = isConnected;
        drawMainUI(isConnected, t);
    }
}
