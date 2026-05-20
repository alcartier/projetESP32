#include "tempo.h"

CTempo::CTempo()
{
}

void CTempo::updateColors()
{
    Couleurs tempCurrentColor, tempNextColor;
    int tempNbRougeRemaining, tempNbBleuRemaining, tempNbBlancRemaining;

    HTTPClient http;
    http.begin("http://cartelectronic-cloud.fr:5200/prte$/getcolor/x");

    int httpCode = http.GET();

    if (httpCode > 0)
    {
        String response = http.getString();

        // -------- dateJ0 --------
        int startJ0 = response.indexOf("<dateJ0>") + 8;
        int endJ0 = response.indexOf("</dateJ0>");
        String dateJ0 = response.substring(startJ0, endJ0);
        String colorJ0 = dateJ0.substring(dateJ0.indexOf(",") + 1);

        // -------- dateJ1 --------
        int startJ1 = response.indexOf("<dateJ1>") + 8;
        int endJ1 = response.indexOf("</dateJ1>");
        String dateJ1 = response.substring(startJ1, endJ1);
        String colorJ1 = dateJ1.substring(dateJ1.indexOf(",") + 1);

        // -------- dcpt --------
        int startDcpt = response.indexOf("<dcpt>") + 6;
        int endDcpt = response.indexOf("</dcpt>");
        String dcpt = response.substring(startDcpt, endDcpt);

        // split dcpt → rouge, blanc, bleu
        int firstComma = dcpt.indexOf(",");
        int secondComma = dcpt.indexOf(",", firstComma + 1);

        tempNbBleuRemaining = dcpt.substring(0, firstComma).toInt();
        tempNbBlancRemaining = dcpt.substring(firstComma + 1, secondComma).toInt();
        tempNbRougeRemaining = dcpt.substring(secondComma + 1).toInt();

        // conversion string → enum
        tempCurrentColor = stringToColor(colorJ0);
        tempNextColor = stringToColor(colorJ1);
    }
    else
    {
        Serial.println("Erreur HTTP");
        http.end();
        return;
    }

    http.end();

    // ---

    currentColor = tempCurrentColor;
    nextColor = tempNextColor;


    nbRougeRemaining = nbRougeMax - tempNbRougeRemaining;
    nbBleuRemaining = nbBleuMax - tempNbBleuRemaining;
    nbBlancRemaining = nbBlancMax - tempNbBlancRemaining;
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

    return Couleurs::Bleu;
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
    default:
        return "BLEU";
    }
}
