#pragma once
#include <string>
#include <HTTPClient.h>

enum Couleurs
{
    Bleu,
    Rouge,
    Blanc,
    NONE
};

class CTempo
{
private:
    Couleurs currentColor;
    Couleurs nextColor;

#ifdef DEBUG_MODE
    std::string requete = "http://192.168.1.57/test.xml";
#endif

#ifdef RELEASE_MODE
    std::string requete = "http://cartelectronic-cloud.fr:5200/prte$/getcolor/x";
#endif

    

    int nbRougeRemaining, nbBleuRemaining, nbBlancRemaining;
    int nbRougeMax = 22, 
    nbBleuMax = 300, 
    nbBlancMax = 43;

public:
    bool uncertain = false;

    // Constructeur
    CTempo();

    // Méthodes :
    bool updateColors();
    void shiftToNextDay();
    Couleurs stringToColor(String color);
    String colorToString(Couleurs color);
    // Getters :
    String getCurrentColor();
    String GetNextColor();
    int GetRemainingColor(Couleurs color);
    bool hasNextColor();

    std::string getRequete() const {
        return requete;
    };
    void setRequete(std::string newRequete) {   
        requete = newRequete;
    };

};
