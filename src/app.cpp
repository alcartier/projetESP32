#include "app.h"

#define BG_DARK      0x0841
#define CARD_BG      0x2104
#define CARD_BORDER  0x3186
#define TEXT_WHITE   0xFFFF
#define TEXT_GREY    0xAD55
#define ACCENT_BLUE  0x34BF
#define ACCENT_RED   0xF800
#define ACCENT_GREEN 0x07E0


void app::drawColorDot(int cx, int cy, int r, Couleurs c)
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

void app::drawRoundedCard(int x, int y, int w, int h, uint16_t bgColor, uint16_t borderColor)
{
    tft.fillRoundRect(x, y, w, h, 8, bgColor);
    tft.drawRoundRect(x, y, w, h, 8, borderColor);
}

uint16_t app::couleurToTFT(Couleurs c)
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

CTempo& app::getTempo()
{
    return tempo;
}

void app::drawMainUI(bool connected)
{

    tft.fillScreen(BG_DARK);
    tft.setTextSize(1);

    // ===== SECTION HAUTE : AUJOURD'HUI / DEMAIN =====
    drawRoundedCard(4, 4, 152, 72, CARD_BG, CARD_BORDER);
    drawRoundedCard(164, 4, 152, 72, CARD_BG, CARD_BORDER);

    Couleurs todayColor = tempo.stringToColor(tempo.getCurrentColor());
    Couleurs tomorrowColor = tempo.stringToColor(tempo.GetNextColor());

    // Aujourd'hui (gauche)
    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextColor(TEXT_GREY, CARD_BG);
    tft.setCursor(14, 24);
    tft.print("Aujourd'hui");

    drawColorDot(30, 55, 8, todayColor);
    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(TEXT_WHITE, CARD_BG);
    tft.setCursor(45, 60);
    tft.print(tempo.getCurrentColor());

    // Demain (droite)
    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextColor(TEXT_GREY, CARD_BG);
    tft.setCursor(174, 24);
    tft.print("Demain");

    drawColorDot(190, 55, 8, tomorrowColor);
    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(TEXT_WHITE, CARD_BG);
    tft.setCursor(205, 60);
    tft.print(tempo.GetNextColor());

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
    tft.print(String(tempo.GetRemainingColor(Couleurs::Bleu)) + "/300");

    // Blanc
    drawColorDot(135, dotY, 7, Couleurs::Blanc);
    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(TEXT_WHITE, CARD_BG);
    tft.setCursor(150, dotY + 5);
    tft.print(String(tempo.GetRemainingColor(Couleurs::Blanc)) + "/43");

    // Rouge
    drawColorDot(235, dotY, 7, Couleurs::Rouge);
    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(TEXT_WHITE, CARD_BG);
    tft.setCursor(250, dotY + 5);
    tft.print(String(tempo.GetRemainingColor(Couleurs::Rouge))+ "/22");

    // ===== SECTION BASSE : DATE ET HEURE =====
    updateTime();
}

void app::updateTime()
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
