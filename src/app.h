#pragma once
#include <WiFiManager.h>
#include <time.h>
#include "tempo.h"
#include "affichage.h"

// CLASSE DE L APPLICATION PRINCIPALE POUR CENTRALISER L'AFFICHAGE LA LOGIQUE / LES PROCEDURES

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
    Caffichage affichage;

    bool connected = false;
    bool mainUIDrawn = false;
    bool updatedToday = false;
    bool midnightShifted = false;
    int lastDay = -1;
    int lastMinute = -1;
    unsigned long lastRetryTime = 0;
    String lastFetchTime = "--:--";

    // Settings
    Preferences settingsPrefs;
    bool Alarme = true;
    bool Sombre = true;
    bool AutoSombre = false;

    void loadSettings();
    void saveSettings();

    // Touch
    bool settingsDrawn = false;
    unsigned long lastTouchTime = 0;
    bool touchWasPressed = false;

public:
    // Getters :
    CTempo &getTempo();

    // PROCEDURES :
    void init();
    void connectWiFi();
    void update();
    void handleStates();
    void handleTouch();

    void updateColorsandUI();
    void shiftToNextDayandUI();

    void drawSettingsUI();

#ifdef DEBUG_MODE
    void Debug();
#endif

    // Wrapper affichage
    void drawMainUI();
};
