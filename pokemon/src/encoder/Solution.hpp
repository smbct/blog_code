#pragma once

#include <vector>
#include "Encoder.hpp"


// representing a solution returned by a sat solver: grid states and moves
class Solution {

    typedef std::vector<std::vector<unsigned int>> Grid;
    
    typedef int MoveIndex;

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
         * \brief extract solution: states and moves from the cfn solution
         * \param state_vars the list if states variables in the encoding
         * \param moves_vars the list if moves variables in the encoding
         * \param cnfVar the list of cnf variables ordered according to the encoding variables indexes
         * \param cnfVal the values of the cnf variables in the solution
         */
        void extractSolution(std::vector<StateVar>& state_vars, std::vector<MoveVar>& move_vars, std::vector<cnf::Variable*>& cnfVar, std::vector<bool>& cnfVal);

        // print the solution: the two grids at each iteration plus the moves applied between each
        void printSolution(bool print_grids, bool print_moves);

        // print the sequence of moves that leads to the solution
        void printMoveSequence();

        // check if the solution is valid thanks to a simulator, returns true iif the solution is valid
        bool checkSolution();



   private:

        void extractStates(std::vector<StateVar>& state_vars, std::vector<cnf::Variable*>& cnfVar, std::vector<bool>& cnfVal);

        void extractMoves(std::vector<MoveVar>& move_vars, std::vector<cnf::Variable*>& cnfVar, std::vector<bool>& cnfVal);
        
        void printState(State& state);

        void printMove(MoveIndex move_ind);

    public:

        unsigned int _N;

        std::vector<State> _states;
        std::vector<MoveIndex> _moveIndexes;

 

};