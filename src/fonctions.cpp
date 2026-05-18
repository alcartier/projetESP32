#include <TFT_eSPI.h>
#include <WiFi.h>
#include "tempo.h"

extern TFT_eSPI tft;

#define BG_DARK      0x0841
#define CARD_BG      0x2104
#define CARD_BORDER  0x3186
#define TEXT_WHITE   0xFFFF
#define TEXT_GREY    0xAD55
#define ACCENT_BLUE  0x34BF
#define ACCENT_RED   0xF800
#define ACCENT_GREEN 0x07E0

void drawRoundedCard(int x, int y, int w, int h, uint16_t bgColor, uint16_t borderColor)
{
    tft.fillRoundRect(x, y, w, h, 8, bgColor);
    tft.drawRoundRect(x, y, w, h, 8, borderColor);
}

void drawColorDot(int cx, int cy, int r, Couleurs c)
{
    uint16_t color;
    switch (c)
    {
    case Couleurs::Bleu:
        color = ACCENT_BLUE;
        break;
    case Couleurs::Rouge:
        color = ACCENT_RED;
        break;
    case Couleurs::Blanc:
        color = 0xFFFF;
        break;
    default:
        color = 0xFFFF;
    }
    tft.fillCircle(cx, cy, r, color);
    tft.drawCircle(cx, cy, r, CARD_BORDER);
}

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
    tft.fillScreen(BG_DARK);
    tft.setTextSize(1);

    // ===== SECTION HAUTE : AUJOURD'HUI / DEMAIN =====
    drawRoundedCard(4, 4, 152, 72, CARD_BG, CARD_BORDER);
    drawRoundedCard(164, 4, 152, 72, CARD_BG, CARD_BORDER);

    Couleurs todayColor = t.stringToColor(t.getCurrentColor());
    Couleurs tomorrowColor = t.stringToColor(t.GetNextColor());

    // Aujourd'hui (gauche)
    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextColor(TEXT_GREY, CARD_BG);
    tft.setCursor(14, 24);
    tft.print("Aujourd'hui");

    drawColorDot(30, 55, 8, todayColor);
    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(TEXT_WHITE, CARD_BG);
    tft.setCursor(45, 60);
    tft.print(t.getCurrentColor());

    // Demain (droite)
    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextColor(TEXT_GREY, CARD_BG);
    tft.setCursor(174, 24);
    tft.print("Demain");

    drawColorDot(190, 55, 8, tomorrowColor);
    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(TEXT_WHITE, CARD_BG);
    tft.setCursor(205, 60);
    tft.print(t.GetNextColor());

    // ===== SECTION MILIEU : JOURS RESTANTS =====
    drawRoundedCard(4, 84, 312, 80, CARD_BG, CARD_BORDER);

    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextColor(TEXT_GREY, CARD_BG);
    tft.setCursor(14, 104);
    tft.print("Jours restants");

    int dotY = 140;

    // Bleu
    drawColorDot(35, dotY, 7, Couleurs::Bleu);
    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(TEXT_WHITE, CARD_BG);
    tft.setCursor(50, dotY + 5);
    tft.print(String(t.GetRemainingColor(Couleurs::Bleu)) + "/300");

    // Blanc
    drawColorDot(135, dotY, 7, Couleurs::Blanc);
    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(TEXT_WHITE, CARD_BG);
    tft.setCursor(150, dotY + 5);
    tft.print(String(t.GetRemainingColor(Couleurs::Blanc)) + "/43");

    // Rouge
    drawColorDot(235, dotY, 7, Couleurs::Rouge);
    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(TEXT_WHITE, CARD_BG);
    tft.setCursor(250, dotY + 5);
    tft.print(String(t.GetRemainingColor(Couleurs::Rouge))+ "/22");

    // ===== SECTION BASSE : DATE ET HEURE =====
    drawRoundedCard(4, 172, 312, 44, CARD_BG, CARD_BORDER);

    struct tm timeinfo;
    if (getLocalTime(&timeinfo))
    {
        char buf[32];
        strftime(buf, sizeof(buf), "%d/%m/%Y  %H:%M", &timeinfo);
        tft.setFreeFont(&FreeSansBold9pt7b);
        tft.setTextColor(TEXT_WHITE, CARD_BG);
        int16_t tw = tft.textWidth(buf);
        tft.setCursor((320 - tw) / 2, 200);
        tft.print(buf);
    }
    else
    {
        tft.setFreeFont(&FreeSans9pt7b);
        tft.setTextColor(TEXT_GREY, CARD_BG);
        tft.setCursor(80, 200);
        tft.print("Synchronisation...");
    }
}