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

void drawMainUI(bool connected, tempo &t)
{
    tft.fillScreen(TFT_LIGHTGREY);

    // ===== TOP BAR =====
    tft.fillRect(0, 0, 320, 40, TFT_WHITE);

    tft.setTextColor(TFT_BLACK);
    tft.setTextSize(1);

    int y = 12;

    tft.setCursor(5, y);
    tft.println("Date d'aujourd'hui");

    tft.setCursor(200, y);
    tft.print("Connecter :");

    // cercle aligné
    tft.fillCircle(300, y + 4, 6, connected ? TFT_GREEN : TFT_RED);

    // ===== TEXTE CENTRAL =====
    tft.setTextColor(TFT_BLACK);
    tft.setTextSize(3);

    tft.setCursor(20, 100);
    tft.println("Aujourd'Hui :");

    tft.setCursor(20, 140);
    tft.println(t.getCurrentColor());

    // ===== SEPARATION =====
    tft.drawLine(0, 200, 320, 200, TFT_BLACK);

    // ===== BOTTOM BAR =====
    tft.setTextSize(2);

    tft.setCursor(20, 210);
    tft.setTextColor(TFT_BLUE);
    tft.println("Bleu:" + String(t.GetRemainingColor(Couleurs::Bleu)));

    tft.setCursor(120, 210);
    tft.setTextColor(TFT_RED);
    tft.println("Rouge:" + String(t.GetRemainingColor(Couleurs::Rouge)));

    tft.setCursor(220, 210);
    tft.setTextColor(TFT_BLACK);
    tft.println("Blanc:" + String(t.GetRemainingColor(Couleurs::Blanc)));
}