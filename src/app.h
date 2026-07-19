#pragma once
#include <WiFiManager.h>
#include <HTTPClient.h>
#include <HTTPUpdate.h>
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

    // Instances
    CTempo tempo;
    Caffichage affichage;

    const char* version = APP_VERSION;  // "1.0.0"

    // WIFI
    bool connected = false;

    //AFFICHAGE :
    bool mainUIDrawn = false;
    bool settingsDrawn = false;

    //FETCHING :
    bool updatedToday = false;
    bool midnightShifted = false;
    int lastDay = -1;
    int lastMinute = -1;
    unsigned long lastRetryTime = 0;
    String lastFetchTime = "--:--";

    // Settings
    Preferences settingsPrefs;
    bool Sombre = true;

        //TODO: implementer les settings pour l'alarme et l'auto sombre
    bool Alarme = false;
    bool AutoSombre = false;
    //
    void loadSettings();
    void saveSettings();

    // Software Update
    static const char* UPDATE_SERVER_URL; // base URL du serveur de mise à jour
    bool isNewerVersion(const String &remote); // compare APP_VERSION vs version distante
    void checkForSoftwareUpdate();
    void updateSoftware(const String &firmwareUrl);

    // Touch

    unsigned long lastTouchTime = 0;
    bool touchWasPressed = false;

public:
    // Getters :
    CTempo &getTempo();

    // PROCEDURES :
    void init();
    void connectWiFi();
    void update(); // main loop
    void handleStates();
    void handleTouch();

    void updateColorsandUI(); 
    void shiftToNextDayandUI(); 

    void drawSettingsUI();

#ifdef DEBUG_MODE
    void Debug();
#endif

    void drawMainUI();
};
