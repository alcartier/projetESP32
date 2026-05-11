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

uint16_t couleurToTFT(Couleurs c)
{
    switch (c)
    {
    case Couleurs::Bleu:
        return TFT_BLUE;
    case Couleurs::Rouge:
        return TFT_RED;
    case Couleurs::Blanc:
        return TFT_WHITE;
    default:
        return TFT_WHITE;
    }
}

void drawMainUI(bool connected, tempo &t)
{
    tft.fillScreen(TFT_LIGHTGREY);

    // ===== SECTION HAUTE : AUJOURD'HUI / DEMAIN =====
    tft.fillRect(0, 0, 320, 80, TFT_WHITE);
    tft.drawRect(0, 0, 320, 80, TFT_BLACK);

    // Aujourd'hui (gauche)
    Couleurs todayColor = t.stringToColor(t.getCurrentColor());
    uint16_t todayTFT = couleurToTFT(todayColor);

    tft.fillRect(10, 15, 30, 30, todayTFT);
    if (todayColor == Couleurs::Blanc)
        tft.drawRect(10, 15, 30, 30, TFT_BLACK);

    tft.setTextColor(TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(50, 15);
    tft.print("AUJOURD'HUI");

    tft.setTextColor(TFT_BLACK);
    tft.setCursor(50, 45);
    tft.print(t.getCurrentColor());

    // Demain (droite)
    Couleurs tomorrowColor = t.stringToColor(t.GetNextColor());
    uint16_t tomorrowTFT = couleurToTFT(tomorrowColor);

    tft.setTextColor(TFT_BLACK);
    tft.setCursor(200, 15);
    tft.print("DEMAIN");

    tft.drawRect(280, 15, 30, 30, TFT_BLACK);
    tft.fillRect(281, 16, 28, 28, tomorrowTFT);

    tft.setTextColor(TFT_BLACK);
    tft.setCursor(200, 45);
    tft.print(t.GetNextColor());

    // ===== SECTION MILIEU : JOURS RESTANTS =====
    tft.fillRect(0, 85, 320, 90, TFT_WHITE);
    tft.drawRect(0, 85, 320, 90, TFT_BLACK);

    tft.setTextColor(TFT_BLACK);
    tft.setTextSize(2);
    tft.setCursor(10, 95);
    tft.print("Jours restants saison :");

    int circleY = 140;

    // Bleu
    tft.fillCircle(40, circleY, 12, TFT_BLUE);
    tft.setTextColor(TFT_BLACK);
    tft.setCursor(60, circleY - 7);
    tft.print(String(t.GetRemainingColor(Couleurs::Bleu)));

    // Blanc
    tft.drawCircle(140, circleY, 12, TFT_BLACK);
    tft.setTextColor(TFT_BLACK);
    tft.setCursor(160, circleY - 7);
    tft.print(String(t.GetRemainingColor(Couleurs::Blanc)));

    // Rouge
    tft.fillCircle(240, circleY, 12, TFT_RED);
    tft.setTextColor(TFT_BLACK);
    tft.setCursor(260, circleY - 7);
    tft.print(String(t.GetRemainingColor(Couleurs::Rouge)));

    // ===== SECTION BASSE : DATE ET HEURE =====
    tft.fillRect(0, 180, 320, 60, TFT_LIGHTGREY);
    tft.drawRect(0, 180, 320, 60, TFT_BLACK);

    tft.setTextColor(TFT_BLACK);
    tft.setTextSize(2);

    struct tm timeinfo;
    if (getLocalTime(&timeinfo))
    {
        char buf[32];
        strftime(buf, sizeof(buf), "%d/%m/%Y  %H:%M", &timeinfo);
        int16_t tw = tft.textWidth(buf);
        tft.setCursor((320 - tw) / 2, 200);
        tft.print(buf);
    }
    else
    {
        tft.setCursor(30, 200);
        tft.print("DATE ET HEURE");
    }
}