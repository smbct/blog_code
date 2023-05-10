#include "Encoder.hpp"
#include <vector>

#include <memory>

using namespace std;


//--------------------------------------------------------------------
Encoder::Encoder(unsigned int N): _N(N) {

    Move* move = nullptr;

    _moves.resize(18, nullptr);

    // _moves; // all moves
    for(unsigned int ind = 0; ind < _N; ind ++) {
        move = new RowMove(RowMove::Left, ind);
        _moves[move->moveIndex()] = move;
        move = new RowMove(RowMove::Right, ind);
        _moves[move->moveIndex()] = move;
    }

    for(unsigned int ind = 0; ind < _N; ind ++) {
        move = new ColMove(ColMove::Left, ColMove::Up, ind);
        _moves[move->moveIndex()] = move;
        move = new ColMove(ColMove::Left, ColMove::Down, ind);
        _moves[move->moveIndex()] = move;
        move = new ColMove(ColMove::Right, ColMove::Up, ind);
        _moves[move->moveIndex()] = move;
        move = new ColMove(ColMove::Right, ColMove::Down, ind);
        _moves[move->moveIndex()] = move;    
    }


    // subset of digits summing to 15
    int sequence[] = {1,2,3,4,5,6,7,8,9};
    Encoder::enumerate_subsets(sequence, 9, _subsets);
    for(auto& subset: _subsets) {
        for(auto& elt: subset) {
            cout << elt << ", ";
        }
        cout << endl;
    }

}


//--------------------------------------------------------------------
void Encoder::createEncoding() {
    
    GridVar grid_left;
    createGridVariables(grid_left);

    for(Move* move: _moves) {
        cout << move->to_string() << " -> " << move->moveIndex() << endl;
    }



    // _ex.setMainTerm(new AndOp(term_list));


}


////////////////////////////////////
// Variables creation
////////////////////////////////////

//--------------------------------------------------------------------
void Encoder::createGridVariables(GridVar& grid_vars) {

    // variables for each digit in each cell of the grid    
    grid_vars.resize(_N, vector<vector<Variable*>>(_N, vector<Variable*>(_N*_N,nullptr)));

    for(unsigned int i = 0; i < grid_vars.size(); i ++) {
        for(unsigned int j = 0; j < grid_vars[i].size(); j ++) {
            
            for(unsigned int ind = 0; ind < _N*_N; ind ++) {
                grid_vars[i][j][ind] = new Variable(_ex);
            }
        
        }
    }

    // gridVar[i][j][k] : digit k (between 1 and _N) at row i and column j

}

//--------------------------------------------------------------------
void Encoder::createStateVariables(StateVar& state_var) {
    createGridVariables(state_var.left_grid);
    createGridVariables(state_var.right_grid);
}

//--------------------------------------------------------------------
void Encoder::createDigitVariables(DigitVar& digit_vars) {

    // row variables -> to decide if a digit is selected for a row
    
    // N*N digits in total

    digit_vars.rows.resize(_N, vector<Variable*>(_N*_N, nullptr));
    digit_vars.cols.resize(_N, vector<Variable*>(_N*_N, nullptr));
    digit_vars.diag1.resize(_N*_N, nullptr);
    digit_vars.diag2.resize(_N*_N, nullptr);


    for(unsigned int ind = 0; ind < _N; ind ++) {
        for(unsigned int ind2 = 0; ind2 < _N*_N; ind2 ++) {
            digit_vars.rows[ind][ind2] = new Variable(_ex);
            digit_vars.cols[ind][ind2] = new Variable(_ex);
        }
    }

    digit_vars.diag1.resize(_N, nullptr);
    digit_vars.diag2.resize(_N, nullptr);

    for(unsigned int ind = 0; ind < _N*_N; ind ++) {
        digit_vars.diag1[ind] = new Variable(_ex);
        digit_vars.diag2[ind] = new Variable(_ex);
    }

}

//--------------------------------------------------------------------
void Encoder::createMoveVariables(MoveVar& move_vars) {
    move_vars.resize(18);
    for(unsigned int index = 0; index < move_vars.size(); index ++) {
        move_vars[index] = new Variable(_ex);
    }
}


////////////////////////////////////
// Constraints creation
////////////////////////////////////


//--------------------------------------------------------------------
void Encoder::addMoveConstraint(StateVar& left, StateVar& right, MoveVar& move) {

    // one digit activates implies a corresponding move has been activated

    for(unsigned int row_ind = 0; row_ind < _N; row_ind ++) {
        for(unsigned int col_ind = 0; col_ind < _N; col_ind ++) {


            for(unsigned int digit_ind = 0; digit_ind < _N*_N; digit_ind ++) {
                
                vector<Term*> or_terms_left, or_terms_right;

                // col moves

                // up
                or_terms_left.push_back(new AndOp({move[ColMove::getMoveIndex(ColMove::Left, ColMove::Up, col_ind)], left.left_grid[row_ind][col_ind][(digit_ind+_N-1)%_N]}));
                or_terms_right.push_back(new AndOp({move[ColMove::getMoveIndex(ColMove::Right, ColMove::Up, col_ind)], left.right_grid[row_ind][col_ind][(digit_ind+_N-1)%_N]}));

                // down
                or_terms_left.push_back(new AndOp({move[ColMove::getMoveIndex(ColMove::Left, ColMove::Down, col_ind)], left.left_grid[row_ind][col_ind][(digit_ind+1)%_N]}));
                or_terms_right.push_back(new AndOp({move[ColMove::getMoveIndex(ColMove::Right, ColMove::Down, col_ind)], left.right_grid[row_ind][col_ind][(digit_ind+1)%_N]}));


                // row moves
                
                // right
                or_terms_left.push_back(new AndOp({move[RowMove::getMoveIndex(RowMove::Right, row_ind)], getGridVar(left, row_ind, (col_ind+_N-1)%(2*_N), digit_ind)}));
                or_terms_right.push_back(new AndOp({move[RowMove::getMoveIndex(RowMove::Right, row_ind)], getGridVar(left, row_ind, (col_ind+_N-1)%(2*_N), digit_ind)}));

                // left
                or_terms_left.push_back(new AndOp({move[RowMove::getMoveIndex(RowMove::Left, row_ind)], getGridVar(left, row_ind, (col_ind+1)%(2*_N), digit_ind)}));
                or_terms_left.push_back(new AndOp({move[RowMove::getMoveIndex(RowMove::Left, row_ind)], getGridVar(left, row_ind, (col_ind+1)%(2*_N), digit_ind)}));

                // no change
                or_terms_left.push_back(left.left_grid[row_ind][col_ind][digit_ind]);
                or_terms_right.push_back(left.right_grid[row_ind][col_ind][digit_ind]);


                // global and for this digit in this cell
                vector<Term*> and_terms_left, and_terms_right;

                and_terms_left.push_back(new OrOp(or_terms_left));
                and_terms_right.push_back(new OrOp(or_terms_right));


                // no other digit for this cell
                for(unsigned int digit_ind2 = 0; digit_ind2 < _N*_N; digit_ind2 ++) {
                    if(digit_ind2 == digit_ind) {
                        continue;
                    }
                    and_terms_left.push_back(new NotOp(right.left_grid[row_ind][col_ind][digit_ind2]));
                    and_terms_right.push_back(new NotOp(right.right_grid[row_ind][col_ind][digit_ind2]));
                }


                // final operation, activation of the digit implies: (a corresponding operation or the digit was active) and no other digit
                term_list.push_back(new ImplyOp(right.left_grid[row_ind][col_ind][digit_ind], new AndOp(and_terms_left)));


            }

        }
    }

}

//--------------------------------------------------------------------
Variable* Encoder::getGridVar(StateVar& state_var, unsigned int row, unsigned int col, unsigned int digit_ind) {

    // return the variable corresponding to col
    // col is in between 0 and _N*2-1
    // this is used to add moves navigating between the two grids

    if(col < _N) {
        return state_var.left_grid[row][col%_N][digit_ind];
    } else {
        return state_var.right_grid[row][col%_N][digit_ind];
    }

}


//------------------------------------------------------------------------------------------------------------
void Encoder::addDigitConstraints(DigitVar& digit_vars, GridVar& grid_vars) {



}


//------------------------------------------------------------------------------------------------------------
void Encoder::addSumConstraints(DigitVar& digit_vars) {


}



//--------------------------------------------------------------------
Encoder::~Encoder() {

    for(auto& move: _moves) {
        delete move;
    }

}




/*----------------------------------------------------------------------------*/
void Encoder::enumerate_subsets(int* sequence, unsigned int size, list<vector<int>>& subsets) {
    for(unsigned int ind1 = 0; ind1 < size; ind1 ++) {
        for(unsigned int ind2 = 0; ind2 < size; ind2 ++) {
            for(unsigned int ind3 = 0; ind3 < size; ind3 ++) {
                if(ind1 < ind2 && ind2 < ind3) {
                    int sum = sequence[ind1]+sequence[ind2]+sequence[ind3];
                    if(sum == 15) {
                        subsets.push_back(vector<int>());
                        subsets.back().push_back(sequence[ind1]);
                        subsets.back().push_back(sequence[ind2]);
                        subsets.back().push_back(sequence[ind3]);
                    } 
                }
            }
        }
    }
}

