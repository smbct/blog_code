#include <iostream>
#include <SFML/Graphics.hpp>
// #include <SFML/Font.hpp>

#include <coin/OsiClpSolverInterface.hpp>
#include <coin/CoinPackedMatrix.hpp>
#include <coin/CoinPackedVector.hpp>

#include "LP_1.hpp"

using namespace std;

void test() {

    // Create a problem pointer.  We use the base class here.
    OsiSolverInterface *si;

    // When we instantiate the object, we need a specific derived class.
    si = new OsiClpSolverInterface;

    // Build our own instance from scratch

    /*
    * This section adapted from Matt Galati’s example
    * on the COIN-OR Tutorial website.
    *
    * Problem from Bertsimas, Tsitsiklis page 21
    *
    *  optimal solution: x* = (1,1)
    *
    *  minimize -1 x0 - 1 x1
    *  s.t       1 x0 + 2 x1 <= 3
    *            2 x0 + 1 x1 <= 3
    *              x0        >= 0
    *              x1        >= 0
    */
    int n_cols = 2;
    double * objective    = new double[n_cols];//the objective coefficients
    double * col_lb       = new double[n_cols];//the column lower bounds
    double * col_ub       = new double[n_cols];//the column upper bounds

    //Define the objective coefficients.
    //minimize -1 x0 - 1 x1
    objective[0] = -1.0;
    objective[1] = -1.0;

    //Define the variable lower/upper bounds.

    // x0 >= 0   =>  0 <= x0 <= infinity
    // x1 >= 0   =>  0 <= x1 <= infinity
    col_lb[0] = 0.0;
    col_lb[1] = 0.0;
    col_ub[0] = si->getInfinity();
    col_ub[1] = si->getInfinity();
    int n_rows = 2;
    double * row_lb = new double[n_rows]; //the row lower bounds
    double * row_ub = new double[n_rows]; //the row upper bounds

    //Define the constraint matrix.
    CoinPackedMatrix * matrix =  new CoinPackedMatrix(false,0,0);
    matrix->setDimensions(0, n_cols);

    //1 x0 + 2 x1 <= 3  =>  -infinity <= 1 x0 + 2 x2 <= 3
    CoinPackedVector row1;
    row1.insert(0, 1.0);
    row1.insert(1, 2.0);
    row_lb[0] = -1.0 * si->getInfinity();
    row_ub[0] = 3.0;
    matrix->appendRow(row1);

    //2 x0 + 1 x1 <= 3  =>  -infinity <= 2 x0 + 1 x1 <= 3
    CoinPackedVector row2;
    row2.insert(0, 2.0);
    row2.insert(1, 1.0);
    row_lb[1] = -1.0 * si->getInfinity();
    row_ub[1] = 3.0;
    matrix->appendRow(row2);

    //load the problem to OSI
    si->loadProblem(*matrix, col_lb, col_ub, objective, row_lb, row_ub);

    //write the MPS file to a file called example.mps
    // si->writeMps("example");

    // Solve the (relaxation of the) problem
    si->initialSolve();

    // Check the solution
    if ( si->isProvenOptimal() ) {
        std::cout << "Found optimal solution!" << std::endl;
        std::cout << "Objective value is " << si->getObjValue() << std::endl;
        int n = si->getNumCols();
        const double *solution;
        solution = si->getColSolution();
        // We could then print the solution or examine it.
    }
    else {
        std::cout << "Didn’t find optimal solution." << std::endl;
        // Could then check other status functions.
    }

}

int main(int argc, char* argv[]) {

    test();

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
