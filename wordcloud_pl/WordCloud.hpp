#ifndef WORD_CLOUD_HPP
#define WORD_CLOUD_HPP

#include <string>
#include <SFML/Graphics.hpp>

#include "Rect.hpp"
#include "LP_1.hpp"

class WordCloud {

    public:
        WordCloud(std::string nomFichier);
        ~WordCloud();

    private:
        void chargerNoms(std::string nomFichier);
        void genererTextes();
        void generer();

    private:
        std::vector<std::string> _noms;
        sf::Font _font;

    public:
        std::vector<Rect*> _rect;
};

#endif
