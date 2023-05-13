#include "Solution.hpp"
#include "Move.hpp"
#include "../simulator/Grids.hpp"

#include <cassert>

using namespace std;

/*------------------------------------------------------------------------------------------------------------------------------*/
Solution::Solution(unsigned int N): _N(N) {

}

/*------------------------------------------------------------------------------------------------------------------------------*/
void Solution::extractSolution(std::vector<StateVar>& state_vars, std::vector<MoveVar>& move_vars, std::vector<cnf::Variable*>& cnfVar, std::vector<bool>& cnfVal) {

    extractStates(state_vars, cnfVar, cnfVal);

    extractMoves(move_vars, cnfVar, cnfVal);

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

        // cout << "state read" << endl;
        // drawState(_states.back());

    }

}


/*------------------------------------------------------------------------------------------------------------------------------*/
void Solution::extractMoves(std::vector<MoveVar>& move_vars, std::vector<cnf::Variable*>& cnfVar, std::vector<bool>& cnfVal) {

    cout << "n move vars: " << move_vars.size() << endl;

    for(auto move_var: move_vars) {

        int move_index = -1;

        for(unsigned int ind = 0; ind < move_var.size(); ind ++) {

            int cnf_ind = cnfVar[move_var[ind]->index()]->index();

            if(cnfVal[cnf_ind]) {
                assert(move_index == -1);
                move_index = ind;
            }

        }

        // assert(move_index != 0);

        _moveIndexes.push_back(move_index);

        // cout << "move extracted: " << move_index << endl;

    }

}


/*------------------------------------------------------------------------------------------------------------------------------*/
bool Solution::checkSolution() {

    Grids grids(3);

    vector<int> left_grid;
    for(size_t i = 0; i < _states.front().left_grid.size(); i ++) {
        for(size_t j = 0; j < _states.front().left_grid[i].size(); j ++) {
            left_grid.push_back(_states.front().left_grid[i][j]);
        }
    }

    vector<int> right_grid;
    for(size_t i = 0; i < _states.front().right_grid.size(); i ++) {
        for(size_t j = 0; j < _states.front().right_grid[i].size(); j ++) {
            right_grid.push_back(_states.front().right_grid[i][j]);
        }
    }

    grids.assign(Grids::Left, left_grid.data());
    grids.assign(Grids::Right, right_grid.data());

    // apply all the moves
    for(auto& moveInd: _moveIndexes) {

        if(moveInd == -1) {
            continue;
        }

        if(moveInd < 6) { // row move

            auto which_dir = (moveInd < 3 ? Grids::Row_Left : Grids::Row_Right);

            if(moveInd % 3 == 0) {
                grids.move_row(Grids::Row_1, which_dir);
            } else if(moveInd % 3 == 1) {
                grids.move_row(Grids::Row_2, which_dir);
            } else {
                grids.move_row(Grids::Row_3, which_dir);
            }

        } else { // column move

            auto which_dir = (moveInd % 6 < 3 ? Grids::Col_Up : Grids::Col_Down);
            auto which_grid = (moveInd < 12 ? Grids::Left : Grids::Right);

            if(moveInd % 3 == 0) {
                grids.move_col(Grids::Col_1, which_dir, which_grid);                
            } else if(moveInd % 3 == 1) {
                grids.move_col(Grids::Col_2, which_dir, which_grid);
            } else {
                grids.move_col(Grids::Col_3, which_dir, which_grid);
            }

        }

    }

    // check the two grids
    return grids.check_grids();

}



/*------------------------------------------------------------------------------------------------------------------------------*/
void Solution::printSolution(bool print_grids, bool print_moves) {

    for(unsigned int sol_index = 0; sol_index < _states.size(); sol_index ++) {

        bool skip_move = (sol_index < _moveIndexes.size() && _moveIndexes[sol_index] == -1);

        if(print_grids && !skip_move) {
            printState(_states[sol_index]);
        }

        if(!skip_move) {
            cout << endl;
        }

        if(print_moves && sol_index < _moveIndexes.size() && !skip_move) {
            printMove(_moveIndexes[sol_index]);
        }

        if(!skip_move) {
            cout << endl << endl;
        }

    }

}

/*------------------------------------------------------------------------------------------------------------------------------*/
void Solution::printMoveSequence() {

    unsigned int length = 0;
    for(auto index: _moveIndexes) {
        if(index >= 0) {
            length ++;
        }
    }

    cout << "Sequence length: " << length << endl;

    for(size_t ind = 0; ind < _moveIndexes.size(); ind ++) {

        if(_moveIndexes[ind] >= 0) {
            
            cout << Move::getMoveFromIndex(_moveIndexes[ind])->to_string();

            if(ind < _moveIndexes.size()-1) {
                cout << "  ->   ";
            }

        }

    }

    cout << endl;

}

/*------------------------------------------------------------------------------------------------------------------------------*/
void Solution::printMove(MoveIndex moveIndex) {

    cout << Move::getMoveFromIndex(moveIndex)->to_string() << endl;

}



/*------------------------------------------------------------------------------------------------------------------------------*/
void Solution::printState(State& state) {

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