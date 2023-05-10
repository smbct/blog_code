#pragma once

#include <vector>
#include "Encoder.hpp"


// representing a solution returned by a sat solver: grid states and moves
class Solution {

    typedef std::vector<std::vector<unsigned int>> Grid;
    
    typedef unsigned int Move;

    struct State {

        State(unsigned int N) {
            left_grid = std::vector<std::vector<unsigned int>>(N, std::vector<unsigned int>(N, 0));
            right_grid = std::vector<std::vector<unsigned int>>(N, std::vector<unsigned int>(N, 0));
        }

        Grid left_grid;
        Grid right_grid;
    };



    public:

        // constructor
        Solution(unsigned int N); 

        /*!
         * \brief extract the states from the cfn solution
         * \param state_vars the list if states variables in the encoding
         * \param cnfVar the list of cnf variables ordered according to the encoding variables indexes
         * \param cnfVal the values of the cnf variables in the solution
         */
        void extractStates(std::vector<StateVar>& state_vars, std::vector<cnf::Variable*>& cnfVar, std::vector<bool>& cnfVal);

        void extractMoves(std::vector<MoveVar>& move_vars, std::vector<cnf::Variable*>& cnfVar, std::vector<bool>& cnfVal);


        void drawState(State& state);

    public:

        unsigned int _N;

        std::vector<State> _states;
        std::vector<Move> _moves;

};