#include "Encoder.hpp"
#include <vector>

#include <memory>
#include <fstream>
#include "Solution.hpp"

using namespace std;

#include "Move.hpp"
std::vector<std::shared_ptr<Move>> Move::_moves(18, nullptr); // initialize the static array of moves ptr



//--------------------------------------------------------------------
Encoder::Encoder(unsigned int N): _N(N) {

    Move::create();
    RowMove::create();
    ColMove::create();

    for(unsigned int index = 0; index < 18; index ++) {
        _moves.push_back(Move::getMoveFromIndex(index));
    }

    // subset of digits summing to 15
    enumerate_subsets(_N*_N, _subsets);
    
    // debug printing the subsets of digits summing to 15
    // cout << "subsets: " << endl;    
    // for(auto elt: _subsets) {
    //     int ind = 1;
    //     for(auto elt2: elt) {
            
    //         if(elt2) {
    //             cout << ind;
    //         } else {
    //             cout << "_";
    //         }
            
    //         cout << ", ";
    //         ind ++;
    //     }
    //     cout << endl;
    // }

    // debug printing the moves and their indexes
    // for(auto move: _moves) {
    //     cout << move->to_string() << " -> " << move->moveIndex() << endl;
    // }
}


//--------------------------------------------------------------------
void Encoder::createEncoding() {
    
    // initial grids
    int init_gleft[] = {4, 9, 6, 5, 8, 3, 2, 4, 1};
    int init_gright[] = {1, 2, 5, 7, 7, 9, 6, 3, 8};

    GridVar grid_left;
    createGridVariables(grid_left);

    // variables creation
    unsigned int n_states = 20;
    // unsigned int n_states = 18;
    // unsigned int n_states = 16;
    // unsigned int n_states = 14; // 542s
    // unsigned int n_states = 12; // still running after 4644.59

    _stateSequence.resize(n_states);
    for(auto it = _stateSequence.begin(); it != _stateSequence.end(); it ++) {
        createStateVariables(*it);
    }
    _movesVar.resize(n_states-1);
    for(auto it = _movesVar.begin(); it != _movesVar.end(); it ++) {
        createMoveVariables(*it);    
    }

    createDigitVariables(_ldigits_final);
    createDigitVariables(_rdigits_final);

    // initial state of the grids
    addInitStateConstraint(_stateSequence.front(), init_gleft, init_gright);


    // move constraints and atmostone move
    for(unsigned int move_ind = 0; move_ind < _movesVar.size(); move_ind ++) {
        addMoveConstraint(_stateSequence[move_ind], _stateSequence[move_ind+1], _movesVar[move_ind]);
    }

    for(unsigned int move_ind = 0; move_ind < _movesVar.size(); move_ind ++) {
        addAtMostOneMove(_movesVar[move_ind]);
    }

    // add at least one move
    // for(unsigned int move_ind = 0; move_ind < _movesVar.size(); move_ind ++) {
    //     addAtLeastOneMove(_movesVar[move_ind]);
    // }

    // set move 0 -> first column left
    // term_list.push_back(_movesVar[0][RowMove::getMoveIndex(RowMove::Right, 0)]);
    // for(unsigned int ind = 0; ind < _movesVar.size(); ind ++) {
    //     term_list.push_back(_movesVar[ind][ind]);
    // }


    addDigitConstraints(_ldigits_final, _stateSequence.back().left_grid);
    addDigitConstraints(_rdigits_final, _stateSequence.back().right_grid);

    // add at most one digit constraint

    // add at least one digit activated per grid at the last state
    // for(unsigned int row_ind = 0; row_ind < _N; row_ind ++) {
    //     for(unsigned int col_ind = 0; col_ind < _N; col_ind ++) {

    //         vector<Term*> or_terms_left, or_terms_right;
    //         for(unsigned int cell_ind = 0; cell_ind < _N*_N; cell_ind ++) {
    //             or_terms_left.push_back(_stateSequence.back().left_grid[row_ind][col_ind][cell_ind]);
    //             or_terms_right.push_back(_stateSequence.back().right_grid[row_ind][col_ind][cell_ind]);
    //         }
    //         term_list.push_back(new OrOp(or_terms_left));
    //         term_list.push_back(new OrOp(or_terms_right));
        
    //     }
    // }

    // sum constraint for the digits (of the last grid)
    addSumConstraints(_ldigits_final);
    addSumConstraints(_rdigits_final);

    _ex.setMainTerm(new AndOp(term_list));

    // debug printing the formula
    // cout << _ex.toString() << endl;

}

//--------------------------------------------------------------------
void Encoder::solveEncoding() {

    // export the formula to cnf (conjunctive normal form)
    cnf::CnfExpression cnfEx;
    vector<cnf::Variable*> cnfVar;
    _ex.toCnf(cnfEx, cnfVar);
    vector<bool> cnfVal(cnfVar.size()); /* values of the cnf variables */

    // call the sat solver to solve the formula
    // this returns true if there is a solution, and it returns the cnf variable assignment in the solution
    bool res = solve_cnf(cnfEx, cnfVal);

    if(res) {

        Solution solution(_N);
        solution.extractSolution(_stateSequence, _movesVar, cnfVar, cnfVal);
    
        // print the solution to the terminal
        solution.printSolution(true, true);
        
        cout << endl;
        solution.printMoveSequence();

        // check the consistency of the solution
        cout << endl << endl;
        cout << "checking the solution: ";

        if(solution.checkSolution()) {
            cout << "the solution is valid!";
        } else {
            cout << "the solution is not valid :(";
        }
        cout << endl;
    
    } else {
        cout << "No solution has been found!" << endl;
    }

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

                // column moves up
                or_terms_left.push_back(new AndOp({move[ColMove::getMoveIndex(ColMove::Left, ColMove::Up, col_ind)], left.left_grid[(row_ind+1)%_N][col_ind][digit_ind]}));
                or_terms_right.push_back(new AndOp({move[ColMove::getMoveIndex(ColMove::Right, ColMove::Up, col_ind)], left.right_grid[(row_ind+1)%_N][col_ind][digit_ind]}));

                // column moves down
                or_terms_left.push_back(new AndOp({move[ColMove::getMoveIndex(ColMove::Left, ColMove::Down, col_ind)], left.left_grid[(row_ind+_N-1)%_N][col_ind][digit_ind]}));
                or_terms_right.push_back(new AndOp({move[ColMove::getMoveIndex(ColMove::Right, ColMove::Down, col_ind)], left.right_grid[(row_ind+_N-1)%_N][col_ind][digit_ind]}));


                // row moves
                
                // here col indexes are considered from 0 to 2*_N, one index for each col of each grid
                unsigned int col_ind_left = col_ind;
                unsigned int col_ind_right = col_ind+_N;

                // row moved to the right
                or_terms_left.push_back(new AndOp({move[RowMove::getMoveIndex(RowMove::Right, row_ind)], getGridVar(left, row_ind, (col_ind_left+2*_N-1)%(2*_N), digit_ind)}));
                or_terms_right.push_back(new AndOp({move[RowMove::getMoveIndex(RowMove::Right, row_ind)], getGridVar(left, row_ind, (col_ind_right+2*_N-1)%(2*_N), digit_ind)}));

                // row moved to the left
                or_terms_left.push_back(new AndOp({move[RowMove::getMoveIndex(RowMove::Left, row_ind)], getGridVar(left, row_ind, (col_ind_left+1)%(2*_N), digit_ind)}));
                or_terms_right.push_back(new AndOp({move[RowMove::getMoveIndex(RowMove::Left, row_ind)], getGridVar(left, row_ind, (col_ind_right+1)%(2*_N), digit_ind)}));

                // no change

                or_terms_left.push_back(new AndOp({left.left_grid[row_ind][col_ind][digit_ind], 
                new NotOp(move[RowMove::getMoveIndex(RowMove::Left, row_ind)]),
                new NotOp(move[RowMove::getMoveIndex(RowMove::Right, row_ind)]),
                new NotOp(move[ColMove::getMoveIndex(ColMove::Left, ColMove::Down, col_ind)]),
                new NotOp(move[ColMove::getMoveIndex(ColMove::Left, ColMove::Up, col_ind)])}));

                or_terms_right.push_back(new AndOp({left.right_grid[row_ind][col_ind][digit_ind], 
                new NotOp(move[RowMove::getMoveIndex(RowMove::Left, row_ind)]),
                new NotOp(move[RowMove::getMoveIndex(RowMove::Right, row_ind)]),
                new NotOp(move[ColMove::getMoveIndex(ColMove::Right, ColMove::Down, col_ind)]),
                new NotOp(move[ColMove::getMoveIndex(ColMove::Right, ColMove::Up, col_ind)])}));

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
                term_list.push_back(new ImplyOp(right.left_grid[row_ind][col_ind][digit_ind], new AndOp(and_terms_left))); // left grid
                term_list.push_back(new ImplyOp(right.right_grid[row_ind][col_ind][digit_ind], new AndOp(and_terms_right))); // right grid


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

    for(unsigned int ind = 0; ind < _N; ind ++) { // iterate over rows and columns
        for(unsigned int digit_ind = 0; digit_ind < _N*_N; digit_ind ++) { // iterate over the digits
            vector<Term*> or_row, or_col;
            for(unsigned int ind2 = 0; ind2 < _N; ind2 ++) { // iterate over the cells of the rows and columns
                or_row.push_back(grid_vars[ind][ind2][digit_ind]);
                or_col.push_back(grid_vars[ind2][ind][digit_ind]);
            }
            term_list.push_back(new ImplyOp(digit_vars.rows[ind][digit_ind], new OrOp(or_row)));
            term_list.push_back(new ImplyOp(digit_vars.cols[ind][digit_ind], new OrOp(or_col)));
        }
    }

    // diagonals digits
    for(unsigned int digit_ind = 0; digit_ind < _N*_N; digit_ind ++) {
        vector<Term*> or_diag1, or_diag2;
        for(unsigned int ind = 0; ind < _N; ind ++) {
            or_diag1.push_back(grid_vars[ind][ind][digit_ind]);
            or_diag2.push_back(grid_vars[ind][_N-ind-1][digit_ind]);
        }
        term_list.push_back(new ImplyOp(digit_vars.diag1[digit_ind], new OrOp(or_diag1)));
        term_list.push_back(new ImplyOp(digit_vars.diag2[digit_ind], new OrOp(or_diag2)));
    }

}


//------------------------------------------------------------------------------------------------------------
void Encoder::addSumConstraints(DigitVar& digit_vars) {

    // sum = 15 on rows and cols
    for(unsigned int ind = 0; ind < _N; ind ++) {
        vector<Term*> row_or, col_or;
        for(auto& subset: _subsets) {
            vector<Term*> row_and, col_and;
            for(unsigned int digit_ind = 0; digit_ind < _N*_N; digit_ind ++) {
                if(subset[digit_ind]) {
                    row_and.push_back(digit_vars.rows[ind][digit_ind]);
                    col_and.push_back(digit_vars.cols[ind][digit_ind]);
                } else {
                    row_and.push_back(new NotOp(digit_vars.rows[ind][digit_ind]));
                    col_and.push_back(new NotOp(digit_vars.cols[ind][digit_ind]));
                }
            }
            row_or.push_back(new AndOp(row_and));
            col_or.push_back(new AndOp(col_and));

        }
        term_list.push_back(new OrOp(row_or));
        term_list.push_back(new OrOp(col_or));    
    }

    // sum = 15 on diagonals
    vector<Term*> diag1_or, diag2_or;
    for(auto& subset: _subsets) {
        vector<Term*> diag1_and, diag2_and;
        for(unsigned int digit_ind = 0; digit_ind < _N*_N; digit_ind ++) {
            if(subset[digit_ind]) {
                diag1_and.push_back(digit_vars.diag1[digit_ind]);
                diag2_and.push_back(digit_vars.diag2[digit_ind]);
            } else {
                diag1_and.push_back(new NotOp(digit_vars.diag1[digit_ind]));
                diag2_and.push_back(new NotOp(digit_vars.diag2[digit_ind]));
            }
        }
        diag1_or.push_back(new AndOp(diag1_and));
        diag2_or.push_back(new AndOp(diag2_and));
    }
    term_list.push_back(new OrOp(diag1_or));
    term_list.push_back(new OrOp(diag2_or));

}


//--------------------------------------------------------------------
void Encoder::addAtMostOneMove(MoveVar& move) {

    for(unsigned int move_ind = 0; move_ind < move.size(); move_ind ++) {
        vector<Term*> and_terms;
        for(unsigned int move_ind2 = 0; move_ind2 < move.size(); move_ind2 ++) {
            if(move_ind != move_ind2) {
                and_terms.push_back(new NotOp(move[move_ind2]));
            }
        }
        term_list.push_back(new ImplyOp(move[move_ind], new AndOp(and_terms)));
    }

}

//--------------------------------------------------------------------
void Encoder::addAtLeastOneMove(MoveVar& move) {

    vector<Term*> or_terms;

    for(unsigned int move_ind = 0; move_ind < move.size(); move_ind ++) {
        or_terms.push_back(move[move_ind]);
    }
    
    term_list.push_back(new OrOp(or_terms));

}


//--------------------------------------------------------------------
void Encoder::addInitStateConstraint(StateVar& state_vars, int* gleft, int* gright) {

    for(unsigned int ind = 0; ind < _N*_N; ind ++) {
        
        unsigned int row_ind = ind/_N;
        unsigned int col_ind = ind%_N;

        vector<Term*> and_terms_left, and_terms_right;

        for(unsigned int digit_ind = 0; digit_ind < _N*_N; digit_ind ++) {
            if(gleft[ind] == static_cast<int>(digit_ind+1)) {
                and_terms_left.push_back(state_vars.left_grid[row_ind][col_ind][digit_ind]);
            } else {
                and_terms_left.push_back(new NotOp(state_vars.left_grid[row_ind][col_ind][digit_ind]));
            }
            if(gright[ind] == static_cast<int>(digit_ind+1)) {
                and_terms_right.push_back(state_vars.right_grid[row_ind][col_ind][digit_ind]);
            } else {
                and_terms_right.push_back(new NotOp(state_vars.right_grid[row_ind][col_ind][digit_ind]));
            }
        }

        term_list.push_back(new AndOp(and_terms_left));
        term_list.push_back(new AndOp(and_terms_right));

    }


}





//--------------------------------------------------------------------
Encoder::~Encoder() {

    // for(auto& move: _moves) {
    //     delete move;
    // }

}





/*----------------------------------------------------------------------------*/
void Encoder::enumerate_subsets(unsigned int size, list<vector<bool>>& subsets) {
    
    vector<bool> subset(size, false);
    for(unsigned int ind1 = 0; ind1 < size; ind1 ++) {
        subset[ind1] = true;
        for(unsigned int ind2 = ind1+1; ind2 < size; ind2 ++) {
            subset[ind2] = true;
            for(unsigned int ind3 = ind2+1; ind3 < size; ind3 ++) {
                subset[ind3] = true;
                int sum = ind1+ind2+ind3+3; // indexes starts at 0
                if(sum == 15) {
                    subsets.push_back(subset);
                } 
                subset[ind3] = false;
            }
            subset[ind2] = false;
        }
        subset[ind1] = false;
    }

    // bool == true: belongs to the subset, otherwise, does not belong to the subset
}


/*----------------------------------------------------------------------------*/
bool Encoder::solve_cnf(cnf::CnfExpression& cnfEx, vector<bool>& cnfVal) {

    string cnf_filename = "temp.dm";
    string sol_filename = "sol.txt";

    cnfEx.exportDimacs(cnf_filename);

    string cmd;
    cmd += "./kissat";
    cmd += " " + cnf_filename;
    // cmd += " > " + sol_filename;
    cmd += " | tee " + sol_filename;
    // system(cmd.c_str());

    // solution extraction
    cmd = "python extract_solution.py";
    // system(cmd.c_str());

    ifstream file(sol_filename);

    string str;
    while(!file.eof()) {
        file >> str;

        if(str == "UNSAT") {
            return false;
        }

        if(str != "0") {
            bool pol = true;
            if(str.at(0) == '-') {
                pol = false;
                str = str.substr(1);
            }
            int ind = stoi(str)-1;
            cnfVal.at(ind) = pol;
        }

    }
    file.close();

    return true;
}
