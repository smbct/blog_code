#include "WordCloud.hpp"

#include <fstream>
#include <iostream>

using namespace std;

/*----------------------------------------------------------------------------*/
WordCloud::WordCloud(std::string nomFichier) {

    chargerNoms(nomFichier);

    if (!_font.loadFromFile("IMMORTAL.ttf")) {
        cout << "erreur de chargement de police" << endl;
    }

    genererTextes();
    generer();

}

/*----------------------------------------------------------------------------*/
void WordCloud::genererTextes() {

    int nb = 500;
    double tailleMin = 10;
    double tailleMax = 60;

    string nom;
    int ind;
    double taille;
    Rect* rect;

    for(int i = 1; i <= nb; i++) {
        // piocher un nom aléatoirement
        ind = (int)rand() % (int)_noms.size();
        nom = _noms.at(ind);

        // choisir une taille aléatoire
        taille = (double)(rand() % 10000) / 10000.;
        taille = taille*(tailleMax-tailleMin)+tailleMin;

        rect = new Rect;
        rect->texte.setFont(_font);
        rect->texte.setString(nom);
        rect->texte.setCharacterSize(taille);
        rect->texte.setFillColor(sf::Color::Red);
        rect->texte.setOrigin(0, rect->texte.getLocalBounds().height/4);
        rect->larg = rect->texte.getLocalBounds().width;
        rect->haut = rect->texte.getLocalBounds().height;
        rect->utilise = false;

        double x = ((double)(rand() % 10000) / 10000.)*800.;
        double y = ((double)(rand() % 10000) / 10000.)*600.;
        rect->texte.setPosition(x, y);

        _rect.push_back(rect);
    }

    // tri du plus gros (en aire) au plus petit
    sort(_rect.begin(), _rect.end(), [](Rect* gch, Rect* dte) {
                                double aireGch = gch->larg * gch->haut;
                                double aireDte = dte->larg * dte->haut;
                                return aireGch > aireDte;} );

}


/*----------------------------------------------------------------------------*/
void WordCloud::chargerNoms(std::string nomFichier) {
    fstream fic(nomFichier);
    if(fic) {
        string nom;
        while(!fic.eof()) {
            fic >> nom;
            if(!fic.eof()) {
                _noms.push_back(nom);
            }
        }
    }

}

/*----------------------------------------------------------------------------*/
void WordCloud::generer() {

    LP_1 lp;
    lp.calculer(_rect);
}

/*----------------------------------------------------------------------------*/
WordCloud::~WordCloud() {

    for(Rect* rect : _rect) {
        delete rect;
    }

}
