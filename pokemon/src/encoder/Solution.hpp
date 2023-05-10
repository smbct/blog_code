#pragma once

#include <vector>

// representing a solution returned by a sat solver: grid states and moves
class Solution {

    typedef std::vector<std::vector<unsigned int>> Grid;
    
    typedef unsigned int Move;

    struct State {
        Grid left_grid;
        Grid right_grid;
    };



    public:

        // constructor
        Solution(unsigned int N, unsigned int length); 

        void extract();

    public:

        unsigned int _N;

        std::vector<State> _states;
        std::vector<Move> _moves;

};