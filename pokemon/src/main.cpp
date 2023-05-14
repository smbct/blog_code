#include <iostream>
#include <cstdlib>

#include <vector>
#include <list>

#include "logic/Expression.hpp"
#include "logic/Operation.hpp"
#include "logic/Variable.hpp"

#include "encoder/Encoder.hpp"
#include "simulator/Grids.hpp"

#include <fstream>

using namespace std;

/*----------------------------------------------------------------------------*/
int main() {

    cout << "Solving the grid problem !" << endl;

    unsigned int N = 3;

    Encoder encoder(N);
    
    encoder.createEncoding();
    encoder.solveEncoding();

    return 0;
}
