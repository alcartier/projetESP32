#include "affichage.h"

#define BG_DARK      0x0841
#define CARD_BG      0x2104
#define CARD_BORDER  0x3186
#define TEXT_WHITE   0xFFFF
#define TEXT_GREY    0xAD55
#define ACCENT_BLUE  0x34BF
#define ACCENT_RED   0xF800
#define ACCENT_GREEN 0x07E0
#define ACCENT_ORANGE 0xFD20
#define ACCENT_CYAN  0x07FF

//Utilitaires

uint16_t Caffichage::couleurToTFT(Couleurs c)
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

void Caffichage::drawColorDot(int cx, int cy, int r, Couleurs c)
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

void Caffichage::drawRoundedCard(int x, int y, int w, int h, uint16_t bgColor, uint16_t borderColor)
{
    tft.fillRoundRect(x, y, w, h, 8, bgColor);
    tft.drawRoundRect(x, y, w, h, 8, borderColor);
}

void Caffichage::drawBoot()
{
    tft.fillScreen(BG_DARK);
    tft.setTextSize(1);
    drawRoundedCard(40, 80, 240, 80, CARD_BG, CARD_BORDER);

    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(TEXT_WHITE, CARD_BG);
    tft.setCursor(100, 115);
    tft.print("TEMPO ESP32");

    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextColor(TEXT_GREY, CARD_BG);
    tft.setCursor(95, 140);
    tft.print("Demarrage...");
}

void Caffichage::drawWifiPortal()
{
    tft.fillScreen(BG_DARK);
    tft.setTextSize(1);

    drawRoundedCard(10, 20, 300, 130, CARD_BG, CARD_BORDER);

    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(TEXT_WHITE, CARD_BG);
    tft.setCursor(20, 45);
    tft.print("Configuration WiFi");

    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextColor(TEXT_GREY, CARD_BG);
    tft.setCursor(20, 75);
    tft.print("Connectez-vous au reseau :");

    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(ACCENT_CYAN, CARD_BG);
    tft.setCursor(20, 100);
    tft.print("ESP32_Config");

    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextColor(TEXT_GREY, CARD_BG);
    tft.setCursor(20, 125);
    tft.print("Puis ouvrir : ");

    tft.setTextColor(ACCENT_ORANGE, CARD_BG);
    tft.print("192.168.4.1");
}

void Caffichage::drawConnecting()
{
    tft.fillScreen(BG_DARK);
    tft.setTextSize(1);
    drawRoundedCard(40, 80, 240, 80, CARD_BG, CARD_BORDER);

    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(ACCENT_ORANGE, CARD_BG);
    tft.setCursor(90, 115);
    tft.print("Connexion...");

    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextColor(TEXT_GREY, CARD_BG);
    tft.setCursor(80, 140);
    tft.print("Veuillez patienter");
}

void Caffichage::drawConnected()
{
    tft.fillScreen(BG_DARK);
    tft.setTextSize(1);
    drawRoundedCard(30, 60, 260, 110, CARD_BG, CARD_BORDER);

    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(ACCENT_GREEN, CARD_BG);
    tft.setCursor(110, 90);
    tft.print("Connecte !");

    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextColor(TEXT_GREY, CARD_BG);
    tft.setCursor(50, 120);
    tft.print("IP : ");

    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(ACCENT_CYAN, CARD_BG);
    tft.print(WiFi.localIP().toString());

    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextColor(TEXT_GREY, CARD_BG);
    tft.setCursor(50, 148);
    tft.print("Chargement des donnees...");
}

void Caffichage::drawConnectionLost()
{
    tft.fillScreen(BG_DARK);
    tft.setTextSize(1);
    drawRoundedCard(20, 50, 280, 140, CARD_BG, CARD_BORDER);

    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(ACCENT_RED, CARD_BG);
    tft.setCursor(70, 80);
    tft.print("Connexion perdue");

    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextColor(TEXT_GREY, CARD_BG);
    tft.setCursor(30, 110);
    tft.print("Reconnexion en cours...");

    tft.setTextColor(TEXT_GREY, CARD_BG);
    tft.setCursor(30, 140);
    tft.print("Ou configurez via :");

    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(ACCENT_CYAN, CARD_BG);
    tft.setCursor(30, 165);
    tft.print("ESP32_Config");
}

void Caffichage::drawMainUI(String todayColor, Couleurs todayEnum, String tomorrowColor, Couleurs tomorrowEnum,
                            int bleuRemaining, int blancRemaining, int rougeRemaining,
                            bool uncertain, String lastFetchTime)
{
    tft.fillScreen(BG_DARK);
    tft.setTextSize(1);

    // ===== SECTION HAUTE : AUJOURD'HUI / DEMAIN =====
    drawRoundedCard(4, 4, 152, 72, CARD_BG, CARD_BORDER);
    drawRoundedCard(164, 4, 152, 72, CARD_BG, CARD_BORDER);

    // Aujourd'hui (gauche)
    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextColor(TEXT_GREY, CARD_BG);
    tft.setCursor(14, 24);
    tft.print("Aujourd'hui");

    drawColorDot(30, 55, 8, todayEnum);
    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(TEXT_WHITE, CARD_BG);
    tft.setCursor(45, 60);
    tft.print(todayColor);

    // Demain (droite)
    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextColor(TEXT_GREY, CARD_BG);
    tft.setCursor(174, 24);
    tft.print("Demain");

    drawColorDot(190, 55, 8, tomorrowEnum);
    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(TEXT_WHITE, CARD_BG);
    tft.setCursor(205, 60);
    tft.print(tomorrowColor);

    // ===== SECTION MILIEU : JOURS RESTANTS =====
    drawRoundedCard(4, 84, 312, 80, CARD_BG, CARD_BORDER);

    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextColor(TEXT_GREY, CARD_BG);
    tft.setCursor(14, 104);
    tft.print("Jours restants");

    if (uncertain)
    {
        tft.setFreeFont(&FreeSans9pt7b);
        tft.setTextColor(ACCENT_ORANGE, CARD_BG);
        tft.setCursor(150, 104);
        tft.print("! Incertain (" + lastFetchTime + ")");
    }

    int dotY = 140;

    // Bleu
    drawColorDot(35, dotY, 7, Couleurs::Bleu);
    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(TEXT_WHITE, CARD_BG);
    tft.setCursor(50, dotY + 5);
    tft.print(String(bleuRemaining) + "/300");

    // Blanc
    drawColorDot(135, dotY, 7, Couleurs::Blanc);
    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(TEXT_WHITE, CARD_BG);
    tft.setCursor(150, dotY + 5);
    tft.print(String(blancRemaining) + "/43");

    // Rouge
    drawColorDot(235, dotY, 7, Couleurs::Rouge);
    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(TEXT_WHITE, CARD_BG);
    tft.setCursor(250, dotY + 5);
    tft.print(String(rougeRemaining) + "/22");

    // ===== SECTION BASSE : DATE ET HEURE =====
    updateTime();
}

void Caffichage::updateTime()
{
    tft.setTextSize(1);
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
