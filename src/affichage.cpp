#include "affichage.h"

// Couleurs Tempo CDC
#define TEMPO_BLEU   0x1338  // #1565C0
#define TEMPO_BLANC  0xE71C  // #E0E0E0
#define TEMPO_ROUGE  0xC285  // #C62828
#define TEMPO_INCONNU 0x4208 // #424242

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
        return TEMPO_BLEU;
    case Couleurs::Rouge:
        return TEMPO_ROUGE;
    case Couleurs::Blanc:
        return TEMPO_BLANC;
    case Couleurs::Inconnu:
        return TEMPO_INCONNU;
    default:
        return TEMPO_INCONNU;
    }
}

void Caffichage::drawColorDot(int cx, int cy, int r, Couleurs c)
{
    uint16_t color = couleurToTFT(c);
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
    tft.setTextColor(TEMPO_ROUGE, theme.cardBg);
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
    tft.setCursor(34, 24);
    tft.print("Aujourd'hui");

    if (todayEnum == Couleurs::Inconnu)
    {
        tft.setFreeFont(&FreeSansBold9pt7b);
        tft.setTextColor(TEMPO_INCONNU, theme.cardBg);
        tft.setCursor(20, 60);
        tft.print("Hors saison");
    }
    else
    {
        drawColorDot(25, 55, 12, todayEnum);
        tft.setFreeFont(&FreeSansBold9pt7b);
        tft.setTextColor(theme.textPrimary, theme.cardBg);
        tft.setCursor(45, 60);
        tft.print(todayColor);
    }

    // Demain (droite)
    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextColor(theme.textSecondary, theme.cardBg);
    tft.setCursor(204, 24);
    tft.print("Demain");

    if (tomorrowEnum == Couleurs::NONE)
    {
        tft.setFreeFont(&FreeSansBold9pt7b);
        tft.setTextColor(ACCENT_ORANGE, theme.cardBg);
        tft.setCursor(180, 55);
        tft.print("?");
        tft.setFreeFont(&FreeSans9pt7b);
        tft.setTextColor(theme.textSecondary, theme.cardBg);
        tft.setCursor(174, 70);
        tft.print("Vers 11h");
    }
    else if (tomorrowEnum == Couleurs::Inconnu)
    {
        tft.setFreeFont(&FreeSansBold9pt7b);
        tft.setTextColor(TEMPO_INCONNU, theme.cardBg);
        tft.setCursor(174, 60);
        tft.print("Hors saison");
    }
    else
    {
        drawColorDot(185, 55, 12, tomorrowEnum);
        tft.setFreeFont(&FreeSansBold9pt7b);
        tft.setTextColor(theme.textPrimary, theme.cardBg);
        tft.setCursor(205, 60);
        tft.print(tomorrowColor);
    }

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
    drawRoundedCard(4, 208, 80, 28, theme.cardBg, ACCENT_CYAN);
    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(ACCENT_CYAN, theme.cardBg);
    tft.setCursor(16, 228);
    tft.print("Retour");
}

void Caffichage::drawDisconnectButton()
{
    drawRoundedCard(196, 176, 116, 22, theme.cardBg, TEMPO_ROUGE);
    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextColor(TEMPO_ROUGE, theme.cardBg);
    tft.setCursor(203, 192);
    tft.print("Deconnexion");
}

void Caffichage::drawUpdateButton()
{
    drawRoundedCard(100, 208, 130, 28, theme.cardBg, ACCENT_CYAN);
    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(ACCENT_CYAN, theme.cardBg);
    tft.setCursor(110, 228);
    tft.print("Mettre a jour");
}

void Caffichage::drawSettingsPage(bool sombre, bool alarme, bool autoSombre, String ssid)
{
    tft.fillScreen(theme.bg);
    tft.setTextSize(1);

    // Titre
    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(theme.textPrimary, theme.bg);
    tft.setCursor(110, 18);
    tft.print("Reglages");

    // Carte des toggles
    drawRoundedCard(4, 28, 312, 120, theme.cardBg, theme.cardBorder);

    // Toggle 1 : Mode sombre (y=38)
    drawToggle(18, 38, sombre, "Mode sombre");

    // Toggle 2 : Alarme sonore (y=72)
    //drawToggle(18, 72, alarme, "Alarme sonore");

    // Toggle 3 : Sombre auto (y=106)
    //drawToggle(18, 106, autoSombre, "Sombre auto");

    // Section WiFi
    drawRoundedCard(4, 156, 312, 44, theme.cardBg, theme.cardBorder);

    tft.setFreeFont(&FreeSans9pt7b);
    tft.setTextColor(theme.textSecondary, theme.cardBg);
    tft.setCursor(14, 172);
    tft.print("WiFi:");

    tft.setFreeFont(&FreeSansBold9pt7b);
    tft.setTextColor(ACCENT_CYAN, theme.cardBg);
    tft.setCursor(14, 192);
    if (ssid.length() > 15)
        ssid = ssid.substring(0, 14) + "..";
    tft.print(ssid);

    // Bouton deconnexion (a droite, meme ligne)
    drawDisconnectButton();

    // Bouton Update (en bas au centre)
    drawUpdateButton();

    // Bouton retour
    drawBackButton();


}
