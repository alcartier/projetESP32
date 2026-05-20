#pragma once
#include <TFT_eSPI.h>
#include <WiFi.h>
#include "tempo.h"

// CLASSE DE L APPLICATION PRINCIPALE POUR CENTRALISER L'AFFICHAGE LA LOGIQUE / LES PROCEDURES

extern TFT_eSPI tft;

class app
{

private:
    CTempo tempo;

public:
    uint16_t couleurToTFT(Couleurs c);

    CTempo& getTempo();

    void drawRoundedCard(int x, int y, int w, int h, uint16_t bgColor, uint16_t borderColor);
    void drawColorDot(int cx, int cy, int r, Couleurs c);
    void drawMainUI(bool connected);
    void updateTime();
};