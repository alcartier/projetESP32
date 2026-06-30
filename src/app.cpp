#include "app.h"

// Utilitaires

CTempo &app::getTempo()
{
    return tempo;
}

// Settings NVS

void app::loadSettings()
{
    settingsPrefs.begin("settings", true);
    Sombre = settingsPrefs.getBool("sombre", true);
    Alarme = settingsPrefs.getBool("alarme", true);
    AutoSombre = settingsPrefs.getBool("autoSombre", false);
    settingsPrefs.end();
}

void app::saveSettings()
{
    settingsPrefs.begin("settings", false);
    settingsPrefs.putBool("sombre", Sombre);
    settingsPrefs.putBool("alarme", Alarme);
    settingsPrefs.putBool("autoSombre", AutoSombre);
    settingsPrefs.end();
}

// Procedure

void app::init()
{
    loadSettings();
    affichage.setDarkMode(Sombre);
    affichage.drawBoot();
    delay(1000);
    connectWiFi();
}

void app::update()
{
    handleStates();
#ifdef DEBUG_MODE
    Debug();
#endif

    switch (currentState)
    {
    case CONNECTION:
        if (!connected)
        {
            connected = false;
            mainUIDrawn = false;
            affichage.drawConnectionLost();
            delay(2000);
            connectWiFi();
        }
        break;

    case MAIN_PAGE:
    {
        if (!mainUIDrawn)
        {
            drawMainUI();
            mainUIDrawn = true;
        }

        struct tm timeinfo;
        if (getLocalTime(&timeinfo))
        {
            if (timeinfo.tm_mday != lastDay)
            {
                lastDay = timeinfo.tm_mday;
                updatedToday = false;
                midnightShifted = false;
                lastRetryTime = 0;
            }

            // Minuit : shift couleurs
            if (!midnightShifted)
            {
                shiftToNextDayandUI();
                midnightShifted = true;
            }

            // 11h05+ : fetch API
            bool after11h05 = (timeinfo.tm_hour > 11) || (timeinfo.tm_hour == 11 && timeinfo.tm_min >= 5);
            if (!updatedToday && after11h05 && timeinfo.tm_hour < 13)
            {
                if (millis() - lastRetryTime >= 300000 || lastRetryTime == 0)
                {
                    lastRetryTime = millis();
                    if (tempo.updateColors())
                    {
                        char timeBuf[6];
                        strftime(timeBuf, sizeof(timeBuf), "%H:%M", &timeinfo);
                        lastFetchTime = String(timeBuf);
                        updatedToday = true;
                        drawMainUI();
                    }
                }
            }

            // 13h+ sans update : flag incertain
            if (!updatedToday && timeinfo.tm_hour >= 13)
            {
                tempo.uncertain = true;
                updatedToday = true;
                drawMainUI();
            }

            if (timeinfo.tm_min != lastMinute)
            {
                lastMinute = timeinfo.tm_min;
                affichage.updateTime();
            }
        }
        break;
    }

    case SETTING_PAGE:
    {
        if (!settingsDrawn)
        {
            drawSettingsUI();
            settingsDrawn = true;
        }
        break;
    }
    }

    handleTouch();
}

void app::handleStates()
{
    bool isConnected = (WiFi.status() == WL_CONNECTED);

    if (currentState == MAIN_PAGE && !isConnected)
    {
        currentState = CONNECTION;
        mainUIDrawn = false;
    }

    if (currentState == CONNECTION && isConnected)
    {
        currentState = MAIN_PAGE;
    }
}

void app::updateColorsandUI()
{
    tempo.updateColors();
    drawMainUI();
    delay(300);
}

void app::shiftToNextDayandUI()
{
    tempo.shiftToNextDay();
    drawMainUI();
}

#ifdef DEBUG_MODE
void app::Debug()
{
    uint16_t tx, ty;
    if (tft.getTouch(&tx, &ty))
    {
        Serial.println("Touch: x=" + String(tx) + " y=" + String(ty));
        if (tx > 240 && ty > 120) // Clic haut droite : shift couleur
        {
            Serial.println("Passage au jour suivant");
            tempo.shiftToNextDay();
            drawMainUI();
            delay(300);
        }
        if (tx < 120 && ty > 120)
        {
            Serial.println("Mise a jour des couleurs");
            tempo.updateColors();
            drawMainUI();
            delay(300);
        }
    }
}
#endif

void app::connectWiFi()
{
    WiFiManager wm;

    wm.setConnectTimeout(10);
    wm.setConfigPortalTimeout(300);

    wm.setAPCallback([this](WiFiManager *mgr)
                     { affichage.drawWifiPortal(); });

    affichage.drawConnecting();
    bool res = wm.autoConnect("TEMPO-Config");

    if (res)
    {
        configTzTime("CET-1CEST,M3.5.0/2,M10.5.0/3", "pool.ntp.org", "time.google.com");
        affichage.drawConnected();
        delay(1500);
        if (tempo.updateColors())
        {
            struct tm ti;
            if (getLocalTime(&ti))
            {
                char timeBuf[6];
                strftime(timeBuf, sizeof(timeBuf), "%H:%M", &ti);
                lastFetchTime = String(timeBuf);
                lastDay = ti.tm_mday;
            }
            updatedToday = true;
            midnightShifted = true;
        }
        else
        {
            if (tempo.fetchDate.length() > 0)
            {
                lastFetchTime = "NVS";
                tempo.uncertain = true;
            }
        }
        drawMainUI();
        mainUIDrawn = true;
        connected = true;
    }
}

void app::handleTouch()
{
    uint16_t tx, ty;
    bool pressed = tft.getTouch(&tx, &ty);

    if (!pressed)
    {
        touchWasPressed = false;
        return;
    }

    // Convertir en coordonnees ecran (y=0 tactile = bas ecran)
    ty = 240 - ty;

    // Ne reagir qu'au front montant (nouveau toucher)
    if (touchWasPressed)
        return;
    touchWasPressed = true;

    // Debounce supplementaire
    if (millis() - lastTouchTime < 400)
        return;
    lastTouchTime = millis();

    switch (currentState)
    {
    case MAIN_PAGE:
        currentState = SETTING_PAGE;
        settingsDrawn = false;
        break;

    case SETTING_PAGE:
        // Toggle Mode sombre : drawToggle(18, 38, ...) → y=38..60
        if (ty >= 33 && ty <= 63)
        {
            Sombre = !Sombre;
            affichage.setDarkMode(Sombre);
            saveSettings();
            settingsDrawn = false;
        }
        // Toggle Alarme : drawToggle(18, 72, ...) → y=72..94
        else if (ty >= 67 && ty <= 97)
        {
            Alarme = !Alarme;
            saveSettings();
            settingsDrawn = false;
        }
        // Toggle Sombre auto : drawToggle(18, 106, ...) → y=106..128
        else if (ty >= 101 && ty <= 131)
        {
            AutoSombre = !AutoSombre;
            saveSettings();
            settingsDrawn = false;
        }
        // Bouton Deconnexion : drawRoundedCard(196, 176, 116, 22)
        else if (tx >= 196 && tx <= 312 && ty >= 173 && ty <= 200)
        {
            WiFi.disconnect(true, true);
            delay(500);
            ESP.restart();
        }
        // Bouton Retour : drawRoundedCard(4, 208, 80, 28)
        else if (tx >= 4 && tx <= 84 && ty >= 208 && ty <= 236)
        {
            currentState = MAIN_PAGE;
            mainUIDrawn = false;
        }
        break;

    default:
        break;
    }
}

void app::drawSettingsUI()
{
    String ssid = WiFi.SSID();
    if (ssid.length() == 0)
        ssid = "Non connecte";
    affichage.drawSettingsPage(Sombre, Alarme, AutoSombre, ssid);
}

// Wrapper affichage

void app::drawMainUI()
{
    affichage.drawMainUI(
        tempo.getCurrentColor(),
        tempo.stringToColor(tempo.getCurrentColor()),
        tempo.GetNextColor(),
        tempo.stringToColor(tempo.GetNextColor()),
        tempo.GetRemainingColor(Couleurs::Bleu),
        tempo.GetRemainingColor(Couleurs::Blanc),
        tempo.GetRemainingColor(Couleurs::Rouge),
        tempo.uncertain,
        lastFetchTime);
}
