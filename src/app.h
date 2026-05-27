#pragma once
#include <TFT_eSPI.h>
#include <WiFiManager.h>
#include <time.h>
#include "tempo.h"

// CLASSE DE L APPLICATION PRINCIPALE POUR CENTRALISER L'AFFICHAGE LA LOGIQUE / LES PROCEDURES


extern TFT_eSPI tft;

enum State
    {
        CONNECTION,
        MAIN_PAGE,
        SETTING_PAGE
    };

class app
{

private:

    State currentState = CONNECTION;
    CTempo tempo;
    bool connected = false;
    bool mainUIDrawn = false;
    bool updatedToday = false;
    bool midnightShifted = false;
    int lastDay = -1;
    int lastMinute = -1;
    unsigned long lastRetryTime = 0;
    String lastFetchTime = "--:--";

    //Setings

    bool Alarme = true;
    bool Sombre = true;
    bool AutoSombre = false;

public:
    uint16_t couleurToTFT(Couleurs c);

    //Getters :
    CTempo& getTempo();

    // PROCEDURES :
    void connectWiFi();
    void update();
    void handleStates();
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