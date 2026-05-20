#include <TFT_eSPI.h>
#include <WiFi.h>
#include "tempo.h"

extern TFT_eSPI tft;
// AFFICHAGE :

void drawBoot()
{
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);

    tft.setCursor(10, 100);
    tft.println("Demarrage...");
}

void drawWifiConfig()
{
    tft.fillScreen(TFT_BLACK);

    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(2);

    tft.setCursor(10, 40);
    tft.println("Config WiFi");

    tft.setCursor(10, 80);
    tft.println("Connecte toi a :");

    tft.setTextColor(TFT_CYAN);
    tft.setCursor(10, 110);
    tft.println("ESP32_Config");

    tft.setTextColor(TFT_YELLOW);
    tft.setCursor(10, 150);
    tft.println("192.168.4.1");
}

void drawConnecting()
{
    tft.fillScreen(TFT_BLACK);

    tft.setTextColor(TFT_ORANGE);
    tft.setTextSize(2);

    tft.setCursor(10, 100);
    tft.println("Connexion...");
}

void drawConnected()
{
    tft.fillScreen(TFT_BLACK);

    tft.setTextColor(TFT_GREEN);
    tft.setTextSize(2);

    tft.setCursor(10, 60);
    tft.println("Connecte !");

    tft.setTextColor(TFT_WHITE);
    tft.setCursor(10, 100);
    tft.println("IP :");

    tft.setTextColor(TFT_CYAN);
    tft.setCursor(10, 130);
    tft.println(WiFi.localIP().toString());
}