#pragma once
#include <string>
#include <HTTPClient.h>

enum Couleurs
{
    Bleu,
    Rouge,
    Blanc
};

class tempo
{
private:
    Couleurs currentColor;
    Couleurs nextColor;

    std::string requete = "http://cartelectronic-cloud.fr:5200/prte$/getcolor/x";

    int nbRougeRemaining, nbBleuRemaining, nbBlancRemaining;

public:
    // Constructeur
    tempo();

    // Méthodes :
    void updateColors();
    Couleurs stringToColor(String color);
    String colorToString(Couleurs color);
    // Getters :
    String getCurrentColor();
    String GetNextColor();
    int GetRemainingColor(Couleurs color);
};
