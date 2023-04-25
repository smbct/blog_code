#pragma once

#include <vector>
#include <iostream>

#include "../logic/Expression.hpp"
#include "../logic/Operation.hpp"
#include "../logic/Variable.hpp"

typedef std::vector<std::vector<std::vector<Variable*>>> GridVar; // for each cell of the grid, a list of variable representing every digit

typedef std::vector<std::vector<Variable*>> DigitVar; // for each row col and diagonal, an array of variable representing each digits, true if the are used

/* represent all propositional variables necessary to represent a state of the sequence */
struct StateVar {
    GridVar left_grid, right_grid;
    DigitVar left_digits, right_digits;
};

//-------------------------------------------------
class Encoding {

    public:

        //-------------------------------------------------
        Encoding(unsigned int N);


        //-------------------------------------------------
        void createGridVariables(GridVar& grid_vars);

        //-------------------------------------------------
        void addAtMostConstraints(std::vector<Term*>& term_list, GridVar& grid_vars);

        //-------------------------------------------------
        void addActivationConstraints(std::vector<Term*>& term_list, GridVar& grid_vars);



        //-------------------------------------------------
        void createDigitVariables(Expression& ex, DigitVar& digit_vars);

        //-------------------------------------------------
        void addDigitConstraintes(std::vector<Term*>& term_list, DigitVar& digit_vars, GridVar& grid_vars);

        //-------------------------------------------------
        void addSumConstraints(DigitVar& digit_vars, std::vector<Term*>& term_list);



        //-------------------------------------------------
        void createStateVariables(StateVar& state_var);


        //-------------------------------------------------
        void createEncoding();

    private:

        unsigned int _N;

        std::vector<StateVar> _sequence;

        
};