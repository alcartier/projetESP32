#include "app.h"

// Utilitaires

CTempo &app::getTempo()
{
    return tempo;
}

// Procedure

void app::init()
{
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

            // 11h+ : fetch API
            if (!updatedToday && timeinfo.tm_hour >= 11 && timeinfo.tm_hour < 13)
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

    wm.setAPCallback([this](WiFiManager *mgr)
                     { affichage.drawWifiPortal(); });

    affichage.drawConnecting();
    bool res = wm.autoConnect("ESP32_Config");

    if (res)
    {
        configTime(3600, 3600, "pool.ntp.org", "time.google.com");
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
        // Toggle Mode sombre : drawToggle(24, 50, ...) → 44x22 + label
        if (ty >= 45 && ty <= 75)
        {
            Sombre = !Sombre;
            affichage.setDarkMode(Sombre);
            settingsDrawn = false;
        }
        // Toggle Alarme : drawToggle(24, 90, ...) → 44x22 + label
        else if (ty >= 85 && ty <= 115)
        {
            Alarme = !Alarme;
            settingsDrawn = false;
        }
        // Toggle Sombre auto : drawToggle(24, 130, ...) → 44x22 + label
        else if (ty >= 125 && ty <= 155)
        {
            AutoSombre = !AutoSombre;
            settingsDrawn = false;
        }
        // Bouton Retour : drawRoundedCard(110, 185, 100, 34)
        else if (tx >= 110 && tx <= 210 && ty >= 185 && ty <= 219)
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
    affichage.drawSettingsPage(Sombre, Alarme, AutoSombre);
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
