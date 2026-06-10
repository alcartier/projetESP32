#include "affichage.h"

#define ACCENT_BLUE  0x34BF
#define ACCENT_RED   0xF800
#define ACCENT_GREEN 0x07E0
#define ACCENT_ORANGE 0xFD20
#define ACCENT_CYAN  0x07FF

// Themes
static const Theme DARK_THEME  = {0x0841, 0x2104, 0x3186, 0xFFFF, 0xAD55};
static const Theme LIGHT_THEME = {0xFFFF, 0xEF5D, 0xC618, 0x0000, 0x528A};

Caffichage::Caffichage()
{
    applyTheme();
}

void Caffichage::applyTheme()
{
    theme = darkMode ? DARK_THEME : LIGHT_THEME;
}

void Caffichage::setDarkMode(bool dark)
{
    darkMode = dark;
    applyTheme();
}

bool Caffichage::isDarkMode() const
{
    return darkMode;
}

//Utilitaires

uint16_t Caffichage::couleurToTFT(Couleurs c)
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

void Caffichage::drawColorDot(int cx, int cy, int r, Couleurs c)
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
        color = darkMode ? 0xFFFF : 0xC618;
        break;
    default:
        color = darkMode ? 0xFFFF : 0xC618;
    }
    tft.fillCircle(cx, cy, r, color);
    tft.drawCircle(cx, cy, r, theme.cardBorder);
}

void Caffichage::drawRoundedCard(int x, int y, int w, int h, uint16_t bgColor, uint16_t borderColor)
{
    tft.fillRoundRect(x, y, w, h, 8, bgColor);
    tft.drawRoundRect(x, y, w, h, 8, borderColor);
}

void Caffichage::drawBoot()
{
    tft.fillScreen(theme.bg);
    tft.setTextSize(1);
    drawRoundedCard(40, 80, 240, 80, theme.cardBg, theme.cardBorder);

    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(theme.textPrimary, theme.cardBg);
    tft.setCursor(100, 115);
    tft.print("TEMPO ESP32");

    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextColor(theme.textSecondary, theme.cardBg);
    tft.setCursor(95, 140);
    tft.print("Demarrage...");
}

void Caffichage::drawWifiPortal()
{
    tft.fillScreen(theme.bg);
    tft.setTextSize(1);

    drawRoundedCard(10, 20, 300, 130, theme.cardBg, theme.cardBorder);

    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(theme.textPrimary, theme.cardBg);
    tft.setCursor(20, 45);
    tft.print("Configuration WiFi");

    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextColor(theme.textSecondary, theme.cardBg);
    tft.setCursor(20, 75);
    tft.print("Connectez-vous au reseau :");

    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(ACCENT_CYAN, theme.cardBg);
    tft.setCursor(20, 100);
    tft.print("ESP32_Config");

    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextColor(theme.textSecondary, theme.cardBg);
    tft.setCursor(20, 125);
    tft.print("Puis ouvrir : ");

    tft.setTextColor(ACCENT_ORANGE, theme.cardBg);
    tft.print("192.168.4.1");
}

void Caffichage::drawConnecting()
{
    tft.fillScreen(theme.bg);
    tft.setTextSize(1);
    drawRoundedCard(40, 80, 240, 80, theme.cardBg, theme.cardBorder);

    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(ACCENT_ORANGE, theme.cardBg);
    tft.setCursor(90, 115);
    tft.print("Connexion...");

    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextColor(theme.textSecondary, theme.cardBg);
    tft.setCursor(80, 140);
    tft.print("Veuillez patienter");
}

void Caffichage::drawConnected()
{
    tft.fillScreen(theme.bg);
    tft.setTextSize(1);
    drawRoundedCard(30, 60, 260, 110, theme.cardBg, theme.cardBorder);

    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(ACCENT_GREEN, theme.cardBg);
    tft.setCursor(110, 90);
    tft.print("Connecte !");

    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextColor(theme.textSecondary, theme.cardBg);
    tft.setCursor(50, 120);
    tft.print("IP : ");

    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(ACCENT_CYAN, theme.cardBg);
    tft.print(WiFi.localIP().toString());

    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextColor(theme.textSecondary, theme.cardBg);
    tft.setCursor(50, 148);
    tft.print("Chargement des donnees...");
}

void Caffichage::drawConnectionLost()
{
    tft.fillScreen(theme.bg);
    tft.setTextSize(1);
    drawRoundedCard(20, 50, 280, 140, theme.cardBg, theme.cardBorder);

    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(ACCENT_RED, theme.cardBg);
    tft.setCursor(70, 80);
    tft.print("Connexion perdue");

    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextColor(theme.textSecondary, theme.cardBg);
    tft.setCursor(30, 110);
    tft.print("Reconnexion en cours...");

    tft.setTextColor(theme.textSecondary, theme.cardBg);
    tft.setCursor(30, 140);
    tft.print("Ou configurez via :");

    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(ACCENT_CYAN, theme.cardBg);
    tft.setCursor(30, 165);
    tft.print("ESP32_Config");
}

void Caffichage::drawMainUI(String todayColor, Couleurs todayEnum, String tomorrowColor, Couleurs tomorrowEnum,
                            int bleuRemaining, int blancRemaining, int rougeRemaining,
                            bool uncertain, String lastFetchTime)
{
    tft.fillScreen(theme.bg);
    tft.setTextSize(1);

    // ===== SECTION HAUTE : AUJOURD'HUI / DEMAIN =====
    drawRoundedCard(4, 4, 152, 72, theme.cardBg, theme.cardBorder);
    drawRoundedCard(164, 4, 152, 72, theme.cardBg, theme.cardBorder);

    // Aujourd'hui (gauche)
    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextColor(theme.textSecondary, theme.cardBg);
    tft.setCursor(14, 24);
    tft.print("Aujourd'hui");

    drawColorDot(30, 55, 8, todayEnum);
    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(theme.textPrimary, theme.cardBg);
    tft.setCursor(45, 60);
    tft.print(todayColor);

    // Demain (droite)
    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextColor(theme.textSecondary, theme.cardBg);
    tft.setCursor(174, 24);
    tft.print("Demain");

    drawColorDot(190, 55, 8, tomorrowEnum);
    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(theme.textPrimary, theme.cardBg);
    tft.setCursor(205, 60);
    tft.print(tomorrowColor);

    // ===== SECTION MILIEU : JOURS RESTANTS =====
    drawRoundedCard(4, 84, 312, 80, theme.cardBg, theme.cardBorder);

    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextColor(theme.textSecondary, theme.cardBg);
    tft.setCursor(14, 104);
    tft.print("Jours restants");

    if (uncertain)
    {
        tft.setFreeFont(&FreeSans9pt7b);
        tft.setTextColor(ACCENT_ORANGE, theme.cardBg);
        tft.setCursor(150, 104);
        tft.print("! Incertain (" + lastFetchTime + ")");
    }

    int dotY = 140;

    // Bleu
    drawColorDot(35, dotY, 7, Couleurs::Bleu);
    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(theme.textPrimary, theme.cardBg);
    tft.setCursor(50, dotY + 5);
    tft.print(String(bleuRemaining) + "/300");

    // Blanc
    drawColorDot(135, dotY, 7, Couleurs::Blanc);
    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(theme.textPrimary, theme.cardBg);
    tft.setCursor(150, dotY + 5);
    tft.print(String(blancRemaining) + "/43");

    // Rouge
    drawColorDot(235, dotY, 7, Couleurs::Rouge);
    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(theme.textPrimary, theme.cardBg);
    tft.setCursor(250, dotY + 5);
    tft.print(String(rougeRemaining) + "/22");

    // ===== SECTION BASSE : DATE ET HEURE =====
    updateTime();
}

void Caffichage::updateTime()
{
    tft.setTextSize(1);
    drawRoundedCard(4, 172, 312, 44, theme.cardBg, theme.cardBorder);

    struct tm timeinfo;
    if (getLocalTime(&timeinfo))
    {
        char buf[32];
        strftime(buf, sizeof(buf), "%d/%m/%Y  %H:%M", &timeinfo);
        tft.setFreeFont(&FreeSansBold9pt7b);
        tft.setTextColor(theme.textPrimary, theme.cardBg);
        int16_t tw = tft.textWidth(buf);
        tft.setCursor((320 - tw) / 2, 200);
        tft.print(buf);
    }
    else
    {
        tft.setFreeFont(&FreeSans9pt7b);
        tft.setTextColor(theme.textSecondary, theme.cardBg);
        tft.setCursor(80, 200);
        tft.print("Synchronisation...");
    }
}

// ===== ECRAN SETTINGS =====

void Caffichage::drawToggle(int x, int y, bool state, const char* label)
{
    int toggleW = 44;
    int toggleH = 22;
    int radius = toggleH / 2;

    uint16_t trackColor = state ? ACCENT_GREEN : theme.cardBorder;
    tft.fillRoundRect(x, y, toggleW, toggleH, radius, trackColor);
    tft.drawRoundRect(x, y, toggleW, toggleH, radius, theme.cardBorder);

    int knobX = state ? (x + toggleW - radius - 2) : (x + radius + 2);
    int knobY = y + radius;
    tft.fillCircle(knobX, knobY, radius - 3, 0xFFFF);

    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextColor(theme.textPrimary, theme.cardBg);
    tft.setCursor(x + toggleW + 12, y + 16);
    tft.print(label);
}

void Caffichage::drawBackButton()
{
    drawRoundedCard(110, 185, 100, 34, theme.cardBg, ACCENT_CYAN);
    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(ACCENT_CYAN, theme.cardBg);
    tft.setCursor(132, 208);
    tft.print("Retour");
}

void Caffichage::drawSettingsPage(bool sombre, bool alarme, bool autoSombre)
{
    tft.fillScreen(theme.bg);
    tft.setTextSize(1);

    // Titre
    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(theme.textPrimary, theme.bg);
    tft.setCursor(110, 22);
    tft.print("Reglages");

    // Carte des toggles
    drawRoundedCard(10, 35, 300, 140, theme.cardBg, theme.cardBorder);

    // Toggle 1 : Mode sombre (y=50)
    drawToggle(24, 50, sombre, "Mode sombre");

    // Toggle 2 : Alarme sonore (y=90)
    drawToggle(24, 90, alarme, "Alarme sonore");

    // Toggle 3 : Sombre auto (y=130)
    drawToggle(24, 130, autoSombre, "Sombre auto");

    // Bouton retour
    drawBackButton();
}
