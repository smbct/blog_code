#include "Encoder.hpp"
#include <vector>

#include <memory>
#include <fstream>
#include "Solution.hpp"

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
    
    // initial grids
    int init_gleft[] = {4, 9, 6, 5, 8, 3, 2, 4, 1};
    int init_gright[] = {1, 2, 5, 7, 7, 9, 6, 3, 8};

    GridVar grid_left;
    createGridVariables(grid_left);

    for(Move* move: _moves) {
        cout << move->to_string() << " -> " << move->moveIndex() << endl;
    }

    // variables creation
    unsigned int n_states = 2;
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

    addDigitConstraints(_ldigits_final, _stateSequence.back().left_grid);
    addDigitConstraints(_rdigits_final, _stateSequence.back().right_grid);

    // add at most one digit constraint

    // add at least one digit activated per grid at the last state
    for(unsigned int row_ind = 0; row_ind < _N; row_ind ++) {
        for(unsigned int col_ind = 0; col_ind < _N; col_ind ++) {

            vector<Term*> or_terms_left, or_terms_right;
            for(unsigned int cell_ind = 0; cell_ind < _N*_N; cell_ind ++) {
                or_terms_left.push_back(_stateSequence.back().left_grid[row_ind][col_ind][cell_ind]);
                or_terms_right.push_back(_stateSequence.back().right_grid[row_ind][col_ind][cell_ind]);
            }
            term_list.push_back(new OrOp(or_terms_left));
            term_list.push_back(new OrOp(or_terms_right));
        
        }
    }

    _ex.setMainTerm(new AndOp(term_list));

    // cout << _ex.toString() << endl;

    cnf::CnfExpression cnfEx;
    vector<cnf::Variable*> cnfVar;
    _ex.toCnf(cnfEx, cnfVar);
    vector<bool> cnfVal(cnfVar.size()); /* values of the cnf variables */

    // this returns true if there is a solution, and it returns the cnf variable assignment in the solution
    bool res = solve_cnf(cnfEx, cnfVal);

    if(res) {

        Solution solution(_N);

        solution.extractMoves(_movesVar, cnfVar, cnfVal);

        solution.extractStates(_stateSequence, cnfVar, cnfVal);



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
            row_or.push_back(new AndOp({digit_vars.rows[ind][subset[0]-1], digit_vars.rows[ind][subset[1]-1], digit_vars.rows[ind][subset[2]-1]}));
            col_or.push_back(new AndOp({digit_vars.cols[ind][subset[0]-1], digit_vars.cols[ind][subset[1]-1], digit_vars.cols[ind][subset[2]-1]}));
        }
        term_list.push_back(new OrOp(row_or));
        term_list.push_back(new OrOp(col_or));    
    }

    // sum = 15 on diagonals
    vector<Term*> diag1_or, diag2_or;
    for(auto& subset: _subsets) {
        diag1_or.push_back(new AndOp({digit_vars.diag1[subset[0]-1], digit_vars.diag1[subset[1]-1], digit_vars.diag1[subset[2]-1]}));
        diag2_or.push_back(new AndOp({digit_vars.diag2[subset[0]-1], digit_vars.diag2[subset[1]-1], digit_vars.diag2[subset[2]-1]}));
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


/*----------------------------------------------------------------------------*/
bool Encoder::solve_cnf(cnf::CnfExpression& cnfEx, vector<bool>& cnfVal) {

    string cnf_filename = "temp.dm";
    string sol_filename = "res";

    cnfEx.exportDimacs(cnf_filename);

    string cmd;
    cmd += "./glucose";


    cmd += " -verb=0";
    // cmd += " -cpu-lim=1";
    // cmd += " --help";

    cmd += " " + cnf_filename;
    cmd += " " + sol_filename;

    // ./glucose temp.dm res
    system(cmd.c_str());

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
