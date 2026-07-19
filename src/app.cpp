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
        struct tm ti;
        bool ntpReady = getLocalTime(&ti);
        if (ntpReady)
        {
            lastDay = ti.tm_mday;
            midnightShifted = true;
        }

        if (tempo.updateColors())
        {
            if (ntpReady)
            {
                char timeBuf[6];
                strftime(timeBuf, sizeof(timeBuf), "%H:%M", &ti);
                lastFetchTime = String(timeBuf);
                bool after11h05 = (ti.tm_hour > 11) || (ti.tm_hour == 11 && ti.tm_min >= 5);
                if (after11h05)
                    updatedToday = true;
            }
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
        // else if (ty >= 67 && ty <= 97)
        // {
        //     Alarme = !Alarme;
        //     saveSettings();
        //     settingsDrawn = false;
        // }
        // // Toggle Sombre auto : drawToggle(18, 106, ...) → y=106..128
        // else if (ty >= 101 && ty <= 131)
        // {
        //     AutoSombre = !AutoSombre;
        //     saveSettings();
        //     settingsDrawn = false;
        // }
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
        else if (tx >= 100 && tx <= 230 && ty >= 208 && ty <= 236)
        {
            // Update Button : drawRoundedCard(100, 208, 130, 28)
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

// Software Update

const char* app::UPDATE_SERVER_URL = "http://TON_SERVEUR_ICI";

bool app::isNewerVersion(const String &remote)
{
    int rMaj = 0, rMin = 0, rPatch = 0;
    int lMaj = 0, lMin = 0, lPatch = 0;
    sscanf(remote.c_str(), "%d.%d.%d", &rMaj, &rMin, &rPatch);
    sscanf(APP_VERSION,    "%d.%d.%d", &lMaj, &lMin, &lPatch);

    if (rMaj != lMaj) return rMaj > lMaj;
    if (rMin != lMin) return rMin > lMin;
    return rPatch > lPatch;
}

void app::checkForSoftwareUpdate()
{
    if (WiFi.status() != WL_CONNECTED)
        return;

    HTTPClient http;
    // Le serveur expose un fichier version.txt contenant uniquement "X.Y.Z"
    String versionUrl = String(UPDATE_SERVER_URL) + "/version.txt";
    http.begin(versionUrl);
    int code = http.GET();

    if (code != HTTP_CODE_OK)
    {
        Serial.println("[OTA] Impossible de joindre le serveur: " + String(code));
        http.end();
        return;
    }

    String remoteVersion = http.getString();
    remoteVersion.trim();
    http.end();

    Serial.println("[OTA] Version distante: " + remoteVersion + " | locale: " + APP_VERSION);

    if (isNewerVersion(remoteVersion))
    {
        Serial.println("[OTA] Nouvelle version disponible, lancement de la mise a jour...");
        String firmwareUrl = String(UPDATE_SERVER_URL) + "/firmware_" + remoteVersion + ".bin";
        updateSoftware(firmwareUrl);
    }
    else
    {
        Serial.println("[OTA] Logiciel deja a jour.");
        // TODO: afficher "Logiciel déjà à jour" sur l'écran
    }
}

void app::updateSoftware(const String &firmwareUrl)
{
    WiFiClient client;
    t_httpUpdate_return ret = httpUpdate.update(client, firmwareUrl);

    switch (ret)
    {
    case HTTP_UPDATE_OK:
        Serial.println("[OTA] Mise a jour reussie, redemarrage...");
        // L'ESP redémarre automatiquement après httpUpdate.update()
        break;
    case HTTP_UPDATE_FAILED:
        Serial.printf("[OTA] Echec: (%d) %s\n",
                      httpUpdate.getLastError(),
                      httpUpdate.getLastErrorString().c_str());
        break;
    case HTTP_UPDATE_NO_UPDATES:
        Serial.println("[OTA] Aucune mise a jour.");
        break;
    }
}