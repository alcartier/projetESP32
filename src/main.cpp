#include <Arduino.h>
#include <TFT_eSPI.h>
#include <WiFiManager.h>
#include <time.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "app.h"

TFT_eSPI tft = TFT_eSPI();

bool lastConnectedState = false;
bool updatedToday = false;
bool mainUIDrawn = false;
int lastDay = -1;
int lastMinute = -1;

app apli;

void connectWiFi()
{
    WiFiManager wm;
    wm.setConfigPortalTimeout(120);

    apli.drawConnecting();

    if (!wm.autoConnect("ESP32_Config"))
    {
        apli.drawWifiPortal();
        wm.startConfigPortal("ESP32_Config");
    }

    configTime(3600, 3600, "pool.ntp.org", "time.google.com");
    apli.drawConnected();
    delay(1500);

    apli.getTempo().updateColors();
    apli.drawMainUI(true);
    mainUIDrawn = true;
    lastConnectedState = true;
}

void setup()
{
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

    Serial.begin(115200);

    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, HIGH);

    tft.init();
    tft.setRotation(1);

    apli.drawBoot();
    delay(1000);

    connectWiFi();
}

void loop()
{
    bool isConnected = (WiFi.status() == WL_CONNECTED);

    if (!isConnected && lastConnectedState)
    {
        lastConnectedState = false;
        mainUIDrawn = false;
        apli.drawConnectionLost();

        delay(3000);
        connectWiFi();
        return;
    }

    if (isConnected && !mainUIDrawn)
    {
        apli.drawMainUI(true);
        mainUIDrawn = true;
        lastConnectedState = true;
    }

    if (isConnected)
    {
        struct tm timeinfo;
        if (getLocalTime(&timeinfo))
        {
            if (timeinfo.tm_mday != lastDay)
            {
                updatedToday = false;
                lastDay = timeinfo.tm_mday;
            }

            if (!updatedToday && timeinfo.tm_hour >= 11)
            {
                apli.getTempo().updateColors();
                updatedToday = true;
                apli.drawMainUI(true);
            }

            if (timeinfo.tm_min != lastMinute)
            {
                lastMinute = timeinfo.tm_min;
                apli.updateTime();
            }
        }
    }
}
