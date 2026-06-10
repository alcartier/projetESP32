#pragma once
#include <string>
#include <HTTPClient.h>
#include <Preferences.h>

enum Couleurs
{
    Bleu,
    Rouge,
    Blanc,
    Inconnu,
    NONE
};

class CTempo
{
private:
    Couleurs currentColor = NONE;
    Couleurs nextColor = NONE;

#ifdef DEBUG_MODE
    std::string requete = "http://192.168.1.57/test.xml";
#endif

#ifdef RELEASE_MODE
    std::string requete = "http://cartelectronic-cloud.fr:5200/prte$/getcolor/x";
#endif

    int nbRougeRemaining = 0, nbBleuRemaining = 0, nbBlancRemaining = 0;
    int nbRougeMax = 22,
        nbBleuMax = 300,
        nbBlancMax = 43;

    Preferences prefs;

public:
    bool uncertain = false;
    String fetchDate = "";
    unsigned long fetchTimestamp = 0;

    CTempo();

    // Methodes :
    bool updateColors();
    void shiftToNextDay();
    Couleurs stringToColor(String color);
    String colorToString(Couleurs color);

    // Cache NVS :
    void saveToNVS();
    bool loadFromNVS();

    // Getters :
    String getCurrentColor();
    String GetNextColor();
    int GetRemainingColor(Couleurs color);
    bool hasNextColor();

    std::string getRequete() const
    {
        return requete;
    };
    void setRequete(std::string newRequete)
    {
        requete = newRequete;
    };
};
