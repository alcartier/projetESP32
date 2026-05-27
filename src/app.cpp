#include "app.h"

#define BG_DARK      0x0841
#define CARD_BG      0x2104
#define CARD_BORDER  0x3186
#define TEXT_WHITE   0xFFFF
#define TEXT_GREY    0xAD55
#define ACCENT_BLUE  0x34BF
#define ACCENT_RED   0xF800
#define ACCENT_GREEN 0x07E0
#define ACCENT_ORANGE 0xFD20
#define ACCENT_CYAN  0x07FF

//Utilitaires

uint16_t app::couleurToTFT(Couleurs c)
{
    switch (c)
    {
    case Couleurs::Bleu:
        return TFT_BLUE;
    case Couleurs::Rouge:
        return TFT_RED;
    case Couleurs::Blanc:
        return TFT_WHITE;
    default:
        return TFT_WHITE;
    }
}

CTempo& app::getTempo()
{
    return tempo;
}



//Procedure

void app::update()
{

    handleStates();
    handleModes();

    switch (currentState)
    {
    case CONNECTION:
        if (!connected)
        {
            connected = false;
            mainUIDrawn = false;
            drawConnectionLost();
            delay(2000);
            connectWiFi();
        }
        break;

    case MAIN_PAGE:
    {
        if (!mainUIDrawn)
        {
            drawMainUI(true);
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
                tempo.shiftToNextDay();
                midnightShifted = true;
                drawMainUI(true);
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
                        drawMainUI(true);
                    }
                }
            }

            // 13h+ sans update : flag incertain
            if (!updatedToday && timeinfo.tm_hour >= 13)
            {
                tempo.uncertain = true;
                updatedToday = true;
                drawMainUI(true);
            }

            if (timeinfo.tm_min != lastMinute)
            {
                lastMinute = timeinfo.tm_min;
                updateTime();
            }
        }
        break;
    }

    case SETTING_PAGE:
        break;
    }
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

/// @brief Gère les modes de l'application (Release, Debug)
void app::handleModes()
{
    switch (currentMode)
    {
        case Release:
            // Pas de debug, pas de mode alternatif pour l'instant
            if (tempo.getRequete() == "http://192.168.1.57/test.xml"){
                tempo.setRequete("http://cartelectronic-cloud.fr:5200/prte$/getcolor/x");  
            }
            break;
        case Debug:
            tempo.setRequete("http://192.168.1.57/test.xml");
            uint16_t tx, ty;
            if (tft.getTouch(&tx, &ty))
            {
                Serial.println("Touch: x=" + String(tx) + " y=" + String(ty));
                if (tx > 240 && ty > 120)
                {
                    tempo.shiftToNextDay();
                    drawMainUI(true);
                    delay(300);
                }
            }
            break;
    }
}

void app::connectWiFi()
{
    WiFiManager wm;

    wm.setAPCallback([this](WiFiManager *mgr) {
        drawWifiPortal();
    });

    drawConnecting();
    bool res = wm.autoConnect("ESP32_Config");

    if (res)
    {
        configTime(3600, 3600, "pool.ntp.org", "time.google.com");
        drawConnected();
        delay(1500);
        handleModes();
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
        //tempo.shiftToNextDay(); //Test : simule minuit
        drawMainUI(true);
        mainUIDrawn = true;
        connected = true;
    }
}

//AFFICHAGES

void app::drawColorDot(int cx, int cy, int r, Couleurs c)
{
    uint16_t color;
    switch (c)
    {
    case Couleurs::Bleu:
        color = ACCENT_BLUE;
        break;
    case Couleurs::Rouge:
        color = ACCENT_RED;
        break;
    case Couleurs::Blanc:
        color = 0xFFFF;
        break;
    default:
        color = 0xFFFF;
    }
    tft.fillCircle(cx, cy, r, color);
    tft.drawCircle(cx, cy, r, CARD_BORDER);
}

void app::drawRoundedCard(int x, int y, int w, int h, uint16_t bgColor, uint16_t borderColor)
{
    tft.fillRoundRect(x, y, w, h, 8, bgColor);
    tft.drawRoundRect(x, y, w, h, 8, borderColor);
}

void app::drawBoot()
{
    tft.fillScreen(BG_DARK);
    tft.setTextSize(1);
    drawRoundedCard(40, 80, 240, 80, CARD_BG, CARD_BORDER);

    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(TEXT_WHITE, CARD_BG);
    tft.setCursor(100, 115);
    tft.print("TEMPO ESP32");

    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextColor(TEXT_GREY, CARD_BG);
    tft.setCursor(95, 140);
    tft.print("Demarrage...");
}

void app::drawWifiPortal()
{
    tft.fillScreen(BG_DARK);
    tft.setTextSize(1);

    drawRoundedCard(10, 20, 300, 130, CARD_BG, CARD_BORDER);

    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(TEXT_WHITE, CARD_BG);
    tft.setCursor(20, 45);
    tft.print("Configuration WiFi");

    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextColor(TEXT_GREY, CARD_BG);
    tft.setCursor(20, 75);
    tft.print("Connectez-vous au reseau :");

    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(ACCENT_CYAN, CARD_BG);
    tft.setCursor(20, 100);
    tft.print("ESP32_Config");

    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextColor(TEXT_GREY, CARD_BG);
    tft.setCursor(20, 125);
    tft.print("Puis ouvrir : ");

    tft.setTextColor(ACCENT_ORANGE, CARD_BG);
    tft.print("192.168.4.1");
}

void app::drawConnecting()
{
    tft.fillScreen(BG_DARK);
    tft.setTextSize(1);
    drawRoundedCard(40, 80, 240, 80, CARD_BG, CARD_BORDER);

    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(ACCENT_ORANGE, CARD_BG);
    tft.setCursor(90, 115);
    tft.print("Connexion...");

    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextColor(TEXT_GREY, CARD_BG);
    tft.setCursor(80, 140);
    tft.print("Veuillez patienter");
}

void app::drawConnected()
{
    tft.fillScreen(BG_DARK);
    tft.setTextSize(1);
    drawRoundedCard(30, 60, 260, 110, CARD_BG, CARD_BORDER);

    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(ACCENT_GREEN, CARD_BG);
    tft.setCursor(110, 90);
    tft.print("Connecte !");

    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextColor(TEXT_GREY, CARD_BG);
    tft.setCursor(50, 120);
    tft.print("IP : ");

    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(ACCENT_CYAN, CARD_BG);
    tft.print(WiFi.localIP().toString());

    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextColor(TEXT_GREY, CARD_BG);
    tft.setCursor(50, 148);
    tft.print("Chargement des donnees...");
}

void app::drawConnectionLost()
{
    tft.fillScreen(BG_DARK);
    tft.setTextSize(1);
    drawRoundedCard(20, 50, 280, 140, CARD_BG, CARD_BORDER);

    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(ACCENT_RED, CARD_BG);
    tft.setCursor(70, 80);
    tft.print("Connexion perdue");

    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextColor(TEXT_GREY, CARD_BG);
    tft.setCursor(30, 110);
    tft.print("Reconnexion en cours...");

    tft.setTextColor(TEXT_GREY, CARD_BG);
    tft.setCursor(30, 140);
    tft.print("Ou configurez via :");

    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(ACCENT_CYAN, CARD_BG);
    tft.setCursor(30, 165);
    tft.print("ESP32_Config");
}

void app::drawMainUI(bool connected)
{

    tft.fillScreen(BG_DARK);
    tft.setTextSize(1);

    // ===== SECTION HAUTE : AUJOURD'HUI / DEMAIN =====
    drawRoundedCard(4, 4, 152, 72, CARD_BG, CARD_BORDER);
    drawRoundedCard(164, 4, 152, 72, CARD_BG, CARD_BORDER);

    Couleurs todayColor = tempo.stringToColor(tempo.getCurrentColor());
    Couleurs tomorrowColor = tempo.stringToColor(tempo.GetNextColor());

    // Aujourd'hui (gauche)
    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextColor(TEXT_GREY, CARD_BG);
    tft.setCursor(14, 24);
    tft.print("Aujourd'hui");

    drawColorDot(30, 55, 8, todayColor);
    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(TEXT_WHITE, CARD_BG);
    tft.setCursor(45, 60);
    tft.print(tempo.getCurrentColor());

    // Demain (droite)
    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextColor(TEXT_GREY, CARD_BG);
    tft.setCursor(174, 24);
    tft.print("Demain");

    drawColorDot(190, 55, 8, tomorrowColor);
    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(TEXT_WHITE, CARD_BG);
    tft.setCursor(205, 60);
    tft.print(tempo.GetNextColor());

    // ===== SECTION MILIEU : JOURS RESTANTS =====
    drawRoundedCard(4, 84, 312, 80, CARD_BG, CARD_BORDER);

    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextColor(TEXT_GREY, CARD_BG);
    tft.setCursor(14, 104);
    tft.print("Jours restants");

    if (tempo.uncertain)
    {
        tft.setFreeFont(&FreeSans9pt7b);
        tft.setTextColor(ACCENT_ORANGE, CARD_BG);
        tft.setCursor(150, 104);
        tft.print("! Incertain (" + lastFetchTime + ")");
    }

    int dotY = 140;

    // Bleu
    drawColorDot(35, dotY, 7, Couleurs::Bleu);
    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(TEXT_WHITE, CARD_BG);
    tft.setCursor(50, dotY + 5);
    tft.print(String(tempo.GetRemainingColor(Couleurs::Bleu)) + "/300");

    // Blanc
    drawColorDot(135, dotY, 7, Couleurs::Blanc);
    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(TEXT_WHITE, CARD_BG);
    tft.setCursor(150, dotY + 5);
    tft.print(String(tempo.GetRemainingColor(Couleurs::Blanc)) + "/43");

    // Rouge
    drawColorDot(235, dotY, 7, Couleurs::Rouge);
    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(TEXT_WHITE, CARD_BG);
    tft.setCursor(250, dotY + 5);
    tft.print(String(tempo.GetRemainingColor(Couleurs::Rouge))+ "/22");

    // ===== SECTION BASSE : DATE ET HEURE =====
    updateTime();
}

void app::updateTime()
{
    tft.setTextSize(1);
    drawRoundedCard(4, 172, 312, 44, CARD_BG, CARD_BORDER);

    struct tm timeinfo;
    if (getLocalTime(&timeinfo))
    {
        char buf[32];
        strftime(buf, sizeof(buf), "%d/%m/%Y  %H:%M", &timeinfo);
        tft.setFreeFont(&FreeSansBold9pt7b);
        tft.setTextColor(TEXT_WHITE, CARD_BG);
        int16_t tw = tft.textWidth(buf);
        tft.setCursor((320 - tw) / 2, 200);
        tft.print(buf);
    }
    else
    {
        tft.setFreeFont(&FreeSans9pt7b);
        tft.setTextColor(TEXT_GREY, CARD_BG);
        tft.setCursor(80, 200);
        tft.print("Synchronisation...");
    }
}
