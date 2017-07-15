#include <iostream>
#include <SFML/Graphics.hpp>

#include "LP_1.hpp"
#include "WordCloud.hpp"

using namespace std;

int main(int argc, char* argv[]) {

    srand(42);

    // test();

    WordCloud wc("noms");

    sf::RenderWindow fenetre(sf::VideoMode(800, 600), "Wordcloud PL");

    fenetre.clear(sf::Color::Black);
    for(Rect* rect : wc._rect) {
        fenetre.draw(rect->texte);
    }

    while(fenetre.isOpen()) {

        sf::Event event;

        while(fenetre.pollEvent(event)) {
            if(event.type == sf::Event::Closed) {
                fenetre.close();
            }
            if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                fenetre.close();
            }
        }

        // fenetre.clear(sf::Color::Black);

        // fenetre.draw(rect);
        // fenetre.draw(text);
        fenetre.display();

    }


    return 0;
}
