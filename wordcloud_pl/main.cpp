#include <iostream>
#include <SFML/Graphics.hpp>
// #include <SFML/Font.hpp>

using namespace std;

int main(int argc, char* argv[]) {

    sf::RenderWindow fenetre(sf::VideoMode(800, 600), "Wordcloud PL");

    sf::Font font;
    if (!font.loadFromFile("IMMORTAL.ttf"))
    {
        cout << "erreur" << endl;
    }

    sf::Text text;
    text.setFont(font);
    text.setString("Hello Bonjour gjpqy");
    text.setCharacterSize(30);
    text.setFillColor(sf::Color::Red);
    text.setOrigin(0, text.getLocalBounds().height/4);
    text.setPosition(50, 50);


    sf::RectangleShape rect(sf::Vector2f(text.getLocalBounds().width, text.getLocalBounds().height));
    rect.setPosition(50, 50);
    rect.setOutlineColor(sf::Color::Blue);

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

        fenetre.clear(sf::Color::Black);

        fenetre.draw(rect);
        fenetre.draw(text);
        fenetre.display();

    }


    return 0;
}
