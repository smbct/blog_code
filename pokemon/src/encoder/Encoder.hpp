#pragma once

#include <vector>
#include <iostream>
#include <list>

#include "../logic/Expression.hpp"
#include "../logic/Operation.hpp"
#include "../logic/Variable.hpp"

#include "Move.hpp"

// for each cell of the grid, a list of variable representing every digit
typedef std::vector<std::vector<std::vector<Variable*>>> GridVar;

// one variable for each move
typedef std::vector<Variable*> MoveVar;



// for each row col and diagonal, an array of variable representing each digits, true if the are used
struct DigitVar {
    std::vector<std::vector<Variable*>> rows;
    std::vector<std::vector<Variable*>> cols;
    std::vector<Variable*> diag1;
    std::vector<Variable*> diag2;
};


/* represent all propositional variables necessary to represent a state of the sequence */
struct StateVar {
    GridVar left_grid;
    GridVar right_grid;
};





//-------------------------------------------------
class Encoder {

    public:

        //-------------------------------------------------
        Encoder(unsigned int N);

        

        //-------------------------------------------------
        void addDigitConstraints(DigitVar& digit_vars, GridVar& grid_vars);

        //-------------------------------------------------
        void addSumConstraints(DigitVar& digit_vars);


        //-------------------------------------------------
        void addMoveConstraint(StateVar& left, StateVar& right, MoveVar& move);


        //-------------------------------------------------
        // Variable creation
        void createGridVariables(GridVar& grid_vars);

        void createStateVariables(StateVar& state_var);

        void createDigitVariables(DigitVar& digit_vars);

        void createMoveVariables(MoveVar& move_vars);


        //-------------------------------------------------
        Variable* getGridVar(StateVar& state_var, unsigned int row, unsigned int col, unsigned int digit_ind);

        //-------------------------------------------------
        void createEncoding();

        //-------------------------------------------------
        ~Encoder();

        //-------------------------------------------------
        static void enumerate_subsets(int* sequence, unsigned int size, std::list<std::vector<int>>& subsets);


    private:

        /* number of rows/columns in the grid */
        unsigned int _N;

        std::vector<Move*> _moves; // all moves

        std::list<std::vector<int>> _subsets; // subset of digits summing to 15


        /* the logic expression main object */
        Expression _ex;

        /* the list of terms (conjunction) used to build the expression */
        std::vector<Term*> term_list;

        /* list of all grid states in the sequence of action */
        std::vector<StateVar> _stateSequence;
        
        // moves at each step of the sequence
        std::vector<MoveVar> _movesVar;

        DigitVar _ldigits_final, _rdigits_final;


        
};