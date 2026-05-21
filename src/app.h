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

    //Getters :
    CTempo& getTempo();

    // PROCEDURES :

    //AFFICHAGES :

    //Components de base :
    void drawRoundedCard(int x, int y, int w, int h, uint16_t bgColor, uint16_t borderColor);
    void drawColorDot(int cx, int cy, int r, Couleurs c);

    //MAIN UI :
    void drawMainUI(bool connected);
    void updateTime();

    //ECRAN DE CONNEXION :
    void drawBoot();
    void drawWifiPortal();
    void drawConnecting();
    void drawConnected();
    void drawConnectionLost();
};