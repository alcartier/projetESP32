#include "tempo.h"

CTempo::CTempo()
{
    loadFromNVS();
}

void CTempo::saveToNVS()
{
    prefs.begin("tempo", false);
    prefs.putInt("colorToday", (int)currentColor);
    prefs.putInt("colorTomorrow", (int)nextColor);
    prefs.putInt("bleuRem", nbBleuRemaining);
    prefs.putInt("blancRem", nbBlancRemaining);
    prefs.putInt("rougeRem", nbRougeRemaining);
    prefs.putULong("fetchTs", fetchTimestamp);
    prefs.putString("fetchDate", fetchDate);
    prefs.end();
}

bool CTempo::loadFromNVS()
{
    prefs.begin("tempo", true);
    if (!prefs.isKey("colorToday"))
    {
        prefs.end();
        return false;
    }
    currentColor = (Couleurs)prefs.getInt("colorToday", NONE);
    nextColor = (Couleurs)prefs.getInt("colorTomorrow", NONE);
    nbBleuRemaining = prefs.getInt("bleuRem", 0);
    nbBlancRemaining = prefs.getInt("blancRem", 0);
    nbRougeRemaining = prefs.getInt("rougeRem", 0);
    fetchTimestamp = prefs.getULong("fetchTs", 0);
    fetchDate = prefs.getString("fetchDate", "");
    prefs.end();
    uncertain = true;
    return true;
}

bool CTempo::updateColors()
{
    uncertain = false;
    Couleurs tempCurrentColor, tempNextColor;
    int tempNbRougeRemaining, tempNbBleuRemaining, tempNbBlancRemaining;

    HTTPClient http;
    http.begin(requete.c_str());
    http.setTimeout(10000);

    int httpCode = http.GET();

    if (httpCode > 0)
    {
        String response = http.getString();

        // -------- dateJ0 --------
        int startJ0 = response.indexOf("<dateJ0>") + 8;
        int endJ0 = response.indexOf("</dateJ0>");
        String dateJ0 = response.substring(startJ0, endJ0);
        String dateJ0Only = dateJ0.substring(0, dateJ0.indexOf(","));
        String colorJ0 = dateJ0.substring(dateJ0.indexOf(",") + 1);

        // -------- dateJ1 --------
        int startJ1 = response.indexOf("<dateJ1>") + 8;
        int endJ1 = response.indexOf("</dateJ1>");
        String dateJ1 = response.substring(startJ1, endJ1);
        String dateJ1Only = dateJ1.substring(0, dateJ1.indexOf(","));
        String colorJ1 = dateJ1.substring(dateJ1.indexOf(",") + 1);

        // Verification des dates
        struct tm timeinfo;
        if (getLocalTime(&timeinfo))
        {
            char todayBuf[11], tomorrowBuf[11];
            strftime(todayBuf, sizeof(todayBuf), "%Y-%m-%d", &timeinfo);

            time_t now = mktime(&timeinfo);
            time_t tomorrow = now + 86400;
            struct tm tmTomorrow;
            localtime_r(&tomorrow, &tmTomorrow);
            strftime(tomorrowBuf, sizeof(tomorrowBuf), "%Y-%m-%d", &tmTomorrow);

            if (dateJ0Only != String(todayBuf) || dateJ1Only != String(tomorrowBuf))
            {
#ifdef DEBUG_MODE
                Serial.println("date incorrecte");
#endif
                uncertain = true;
            }
        }

        // -------- dcpt --------
        int startDcpt = response.indexOf("<dcpt>") + 6;
        int endDcpt = response.indexOf("</dcpt>");
        String dcpt = response.substring(startDcpt, endDcpt);

        int firstComma = dcpt.indexOf(",");
        int secondComma = dcpt.indexOf(",", firstComma + 1);

        tempNbBleuRemaining = dcpt.substring(0, firstComma).toInt();
        tempNbBlancRemaining = dcpt.substring(firstComma + 1, secondComma).toInt();
        tempNbRougeRemaining = dcpt.substring(secondComma + 1).toInt();

        tempCurrentColor = stringToColor(colorJ0);
        tempNextColor = stringToColor(colorJ1);
    }
    else
    {
        Serial.println("Erreur HTTP");
        http.end();
        return false;
    }

    http.end();

    currentColor = tempCurrentColor;
    nextColor = tempNextColor;
    nbRougeRemaining = nbRougeMax - tempNbRougeRemaining;
    nbBleuRemaining = nbBleuMax - tempNbBleuRemaining;
    nbBlancRemaining = nbBlancMax - tempNbBlancRemaining;

    // Sauvegarder en NVS
    struct tm ti;
    if (getLocalTime(&ti))
    {
        char dateBuf[11];
        strftime(dateBuf, sizeof(dateBuf), "%Y-%m-%d", &ti);
        fetchDate = String(dateBuf);
        fetchTimestamp = millis();
    }
    saveToNVS();

    return true;
}


void CTempo::shiftToNextDay()
{
    currentColor = nextColor;
    nextColor = Couleurs::NONE;
    uncertain = false;
}

bool CTempo::hasNextColor()
{
    return nextColor != Couleurs::NONE;
}

String CTempo::getCurrentColor()
{
    return colorToString(currentColor);
}

String CTempo::GetNextColor()
{
    return colorToString(nextColor);
}

int CTempo::GetRemainingColor(Couleurs color)
{
    switch (color)
    {
    case Couleurs::Bleu:
        return nbBleuRemaining;
    case Couleurs::Rouge:
        return nbRougeRemaining;
    case Couleurs::Blanc:
        return nbBlancRemaining;
    default:
        return 0;
    }
}

Couleurs CTempo::stringToColor(String color)
{
    if (color == "BLEU")
        return Couleurs::Bleu;
    if (color == "ROUGE")
        return Couleurs::Rouge;
    if (color == "BLANC")
        return Couleurs::Blanc;
    if (color == "TEMPO_INCONNU" || color == "0")
        return Couleurs::Inconnu;
    return Couleurs::NONE;
}

String CTempo::colorToString(Couleurs color)
{
    switch (color)
    {
    case Bleu:
        return "BLEU";
    case Rouge:
        return "ROUGE";
    case Blanc:
        return "BLANC";
    case Inconnu:
        return "INCONNU";
    default:
        return "???";
    }
}
