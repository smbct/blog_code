#include "LP_1.hpp"

#include <coin/OsiClpSolverInterface.hpp>
#include <coin/CoinPackedMatrix.hpp>
#include <coin/CoinPackedVector.hpp>

using namespace std;

/*----------------------------------------------------------------------------*/
LP_1::LP_1() {


}


/*----------------------------------------------------------------------------*/
void LP_1::calculer(vector<Rect*>& _listeRect) {

    list<Rect*> _listeEssai;

    bool continuer = true;

    for(int i = 0; i < _listeRect.size(); i++) {

        _listeEssai.push_back(_listeRect.at(i));

        if(!resoudre(_listeEssai)) { // si le dernier ajouté ne passe pas, il est retiré
            _listeEssai.pop_back();
        } else { // sinon il est indiqué comme étant utilisé
            _listeEssai.back()->utilise = true;
        }

    }

}

/*----------------------------------------------------------------------------*/
bool LP_1::resoudre(list<Rect*>& _listeEssai) {

    // Create a problem pointer.  We use the base class here.
    OsiSolverInterface *si;

    // When we instantiate the object, we need a specific derived class.
    si = new OsiClpSolverInterface;

    // nombre de variables : 2 par textes (position x et position y)
    int n_cols = _listeEssai.size()*2;
    double * objective    = new double[n_cols];//the objective coefficients
    double * col_lb       = new double[n_cols];//the column lower bounds
    double * col_ub       = new double[n_cols];//the column upper bounds

    int i = 0;
    for(Rect* rect : _listeEssai) {

        // Define the objective coefficients.
        objective[i*2] = 0.;
        objective[i*2+1] = 0.;

        // Define the variable lower/upper bounds.
        // fonction de la taille de la fenetre
        col_lb[i*2] = 0.0;
        col_lb[i*1+1] = 0.0;
        col_ub[i*2] = 800-rect->larg;
        col_ub[i*2+1] = 600-rect->haut;

    }


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

    return true;
}

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
