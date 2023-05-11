#include "Solution.hpp"
#include <cassert>

using namespace std;

/*------------------------------------------------------------------------------------------------------------------------------*/
Solution::Solution(unsigned int N): _N(N) {

}


/*------------------------------------------------------------------------------------------------------------------------------*/
void Solution::extractStates(std::vector<StateVar>& state_vars, std::vector<cnf::Variable*>& cnfVar, std::vector<bool>& cnfVal) {

    for(auto state_var: state_vars) {

        _states.push_back(State(_N));

        for(unsigned int row_ind = 0; row_ind < _N; row_ind ++) {
            for(unsigned int col_ind = 0; col_ind < _N; col_ind ++) {
                
                unsigned int left_digit = 0, right_digit = 0;
                
                for(unsigned int digit_ind = 0; digit_ind < _N*_N; digit_ind ++) {

                    // left digit
                    int cnf_ind = cnfVar[state_var.left_grid[row_ind][col_ind][digit_ind]->index()]->index();
                    assert(left_digit == 0 || !cnfVal[cnf_ind]);
                    if(cnfVal[cnf_ind] == true && left_digit == 0) {
                        left_digit = digit_ind+1;
                    }

                    // right digit
                    cnf_ind = cnfVar[state_var.right_grid[row_ind][col_ind][digit_ind]->index()]->index();
                    assert(right_digit == 0 || !cnfVal[cnf_ind]);
                    if(cnfVal[cnf_ind] == true && right_digit == 0) {
                        right_digit = digit_ind+1;
                    }

                }

                assert(left_digit != 0 && right_digit != 0);

                _states.back().left_grid[row_ind][col_ind] = left_digit;
                _states.back().right_grid[row_ind][col_ind] = right_digit;

            }
        }

        cout << "state read" << endl;
        drawState(_states.back());

    }

}


/*------------------------------------------------------------------------------------------------------------------------------*/
void Solution::extractMoves(std::vector<MoveVar>& move_vars, std::vector<cnf::Variable*>& cnfVar, std::vector<bool>& cnfVal) {

    for(auto move_var: move_vars) {

        unsigned int move_index = 0;

        for(unsigned int ind = 0; ind < move_var.size(); ind ++) {

            int cnf_ind = cnfVar[move_var[ind]->index()]->index();

            if(cnfVal[cnf_ind]) {
                assert(move_index == 0);
                move_index = ind;
            }

        }

        // assert(move_index != 0);

        _moves.push_back(move_index);

        cout << "move extracted: " << move_index << endl;

    }

}



/*------------------------------------------------------------------------------------------------------------------------------*/
void Solution::drawState(State& state) {

    for(unsigned int row_ind = 0; row_ind < _N; row_ind ++) {

        for(unsigned int col_ind = 0; col_ind < _N; col_ind ++) {
            cout << state.left_grid[row_ind][col_ind] << " ";
        }
        cout << "    ";

        for(unsigned int col_ind = 0; col_ind < _N; col_ind ++) {
            cout << state.right_grid[row_ind][col_ind] << " ";
        }

        cout << endl;

    }

}