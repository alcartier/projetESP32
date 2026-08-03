#pragma once
#include <TFT_eSPI.h>
#include <WiFi.h>
#include <time.h>
#include "tempo.h"

extern TFT_eSPI tft;

struct Theme // theme colors
{
    uint16_t bg;
    uint16_t cardBg;
    uint16_t cardBorder;
    uint16_t textPrimary;
    uint16_t textSecondary;
};

class Caffichage
{
private:
    bool darkMode = true;
    Theme theme;
    void applyTheme();

public:
    Caffichage();
    void setDarkMode(bool dark);
    bool isDarkMode() const;

    uint16_t couleurToTFT(Couleurs c);

    void drawRoundedCard(int x, int y, int w, int h, uint16_t bgColor, uint16_t borderColor);
    void drawColorDot(int cx, int cy, int r, Couleurs c, uint16_t outlineColor = 0, int outlineThickness = 0);

    //MAIN UI :
    void drawMainUI(String todayColor, Couleurs todayEnum, String tomorrowColor, Couleurs tomorrowEnum,
                    int bleuRemaining, int blancRemaining, int rougeRemaining,
                    bool uncertain, String lastFetchTime);
    void updateTime();

    //ECRAN DE CONNEXION :
    void drawBoot();
    void drawWifiPortal();
    void drawConnecting();
    void drawConnected();
    void drawConnectionLost();

    //ECRAN DE CONFIG :
    void drawSettingsPage(bool sombre, bool alarme, bool autoSombre, String ssid);
    void drawToggle(int x, int y, bool state, const char* label);
    void drawBackButton();
    void drawUpdateButton();
    void drawDisconnectButton();

    //Pop up Update
    void cardUpdateStatus(String status);
    void cardUpdateStatusClear();
};
