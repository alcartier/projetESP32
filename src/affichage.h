#pragma once
#include <TFT_eSPI.h>
#include <WiFi.h>
#include <time.h>
#include "tempo.h"

extern TFT_eSPI tft;

class Caffichage
{
public:
    uint16_t couleurToTFT(Couleurs c);

    void drawRoundedCard(int x, int y, int w, int h, uint16_t bgColor, uint16_t borderColor);
    void drawColorDot(int cx, int cy, int r, Couleurs c);

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
};
