#include <iostream>
#include <cstdlib>

#include <vector>
#include <list>

#include "logic/Expression.hpp"
#include "logic/Operation.hpp"
#include "logic/Variable.hpp"

#include "encoder/Encoder.hpp"
#include "simulator/Grids.hpp"

#include <fstream>

using namespace std;




/*----------------------------------------------------------------------------*/
int main() {

    cout << "hello poke" << endl;

    unsigned int N = 3;

    // Grids grids(N);

    // int gleft[] = {4, 9, 6, 5, 8, 3, 2, 4, 1};
    // int gright[] = {1, 2, 5, 7, 7, 9, 6, 3, 8};

    // grids.assign(Grids::Left, gleft);
    // grids.assign(Grids::Right, gright);

    // grids.print_grids();
    // cout << endl;

    // grids.move_row(Grids::Row_1, Grids::Row_Left);

    // grids.print_grids();
    // cout << endl;

    // grids.move_row(Grids::Row_2, Grids::Row_Right);

    // grids.print_grids();
    // cout << endl;

    // cout << grids.check_grids() << endl;


    Encoder encoder(N);
    encoder.createEncoding();
    return 0;

    int sequence[] = {1,2,3,4,5,6,7,8,9};
    list<vector<int>> subsets;
    Encoder::enumerate_subsets(sequence, 9, subsets);
    // for(auto& subset: subsets) {
    //     for(auto& elt: subset) {
    //         cout << elt << ", ";
    //     }
    //     cout << endl;
    // }


    vector<Term*> term_list;

    Expression ex;

    // variables for each digit in each cell of the grid
    vector<vector<vector<Variable*>>> gridVar(N, vector<vector<Variable*>>(N, vector<Variable*>(N*N,nullptr)));
    for(unsigned int i = 0; i < gridVar.size(); i ++) {
        for(unsigned int j = 0; j < gridVar[i].size(); j ++) {
            for(unsigned int ind = 0; ind < N*N; ind ++) {
                gridVar[i][j][ind] = new Variable(ex);
            }
        }
    }

    // only one digit is selected for each cell
    for(unsigned int i = 0; i < N; i ++) {
        for(unsigned int j = 0; j < N; j ++) {
            for(unsigned int digit_ind = 0; digit_ind < N*N; digit_ind ++) {
                vector<Term*> and_op;
                for(unsigned int digit_ind2 = 0; digit_ind2 < N*N; digit_ind2 ++) {
                    if(digit_ind == digit_ind2) {
                        continue;
                    }
                    and_op.push_back(new NotOp(gridVar[i][j][digit_ind2]));
                }
                term_list.push_back(new ImplyOp(gridVar[i][j][digit_ind], new AndOp(and_op)));
            }
        }
    }

    // at least one digit is selected for each cell
    for(unsigned int i = 0; i < N; i ++) {
        for(unsigned int j = 0; j < N; j ++) {
            vector<Term*> or_op;
            for(unsigned int digit_ind = 0; digit_ind < N*N; digit_ind ++) {
                or_op.push_back(gridVar[i][j][digit_ind]);
            }
            term_list.push_back(new OrOp(or_op));            
        }
    }

    // row variables -> to decide if a digit is selected for a row
    // N*N digits in total
    vector<vector<Variable*>> rowVar(N, vector<Variable*>(N*N, nullptr));
    vector<vector<Variable*>> colVar(N, vector<Variable*>(N*N, nullptr));
    vector<vector<Variable*>> diagVar(2, vector<Variable*>(N*N, nullptr));

    for(unsigned int ind = 0; ind < N; ind ++) {
        for(unsigned int ind2 = 0; ind2 < N*N; ind2 ++) {
            rowVar[ind][ind2] = new Variable(ex);
            colVar[ind][ind2] = new Variable(ex);
        }
    }
    diagVar.push_back(vector<Variable*>(N, nullptr));
    diagVar.push_back(vector<Variable*>(N, nullptr));
    for(unsigned int ind = 0; ind < N*N; ind ++) {
        diagVar[0][ind] = new Variable(ex);
        diagVar[1][ind] = new Variable(ex);
    }


    // row and col digits
    for(unsigned int ind = 0; ind < N; ind ++) { // iterate over rows and columns
        for(unsigned int digit_ind = 0; digit_ind < N*N; digit_ind ++) { // iterate over the digits
            vector<Term*> or_row, or_col;
            for(unsigned int cell_ind = 0; cell_ind < N; cell_ind ++) { // iterate over the cells of the rows and columns
                or_row.push_back(gridVar[ind][cell_ind][digit_ind]);
                or_col.push_back(gridVar[cell_ind][ind][digit_ind]);
            }
            term_list.push_back(new ImplyOp(rowVar[ind][digit_ind], new OrOp(or_row)));
            term_list.push_back(new ImplyOp(colVar[ind][digit_ind], new OrOp(or_col)));
        }
    }

    // diagonals digits
    for(unsigned int digit_ind = 0; digit_ind < N*N; digit_ind ++) {
        vector<Term*> or_diag1, or_diag2;
        for(unsigned int ind = 0; ind < N; ind ++) {
            or_diag1.push_back(gridVar[ind][ind][digit_ind]);
            or_diag2.push_back(gridVar[ind][N-ind-1][digit_ind]);
        }
        term_list.push_back(new ImplyOp(diagVar[0][digit_ind], new OrOp(or_diag1)));
        term_list.push_back(new ImplyOp(diagVar[1][digit_ind], new OrOp(or_diag2)));
    }
    

    // sum = 15 on rows and cols
    for(unsigned int ind = 0; ind < N; ind ++) {
        vector<Term*> row_or, col_or;
        for(auto& subset: subsets) {
            row_or.push_back(new AndOp({rowVar[ind][subset[0]-1], rowVar[ind][subset[1]-1], rowVar[ind][subset[2]-1]}));
            col_or.push_back(new AndOp({colVar[ind][subset[0]-1], colVar[ind][subset[1]-1], colVar[ind][subset[2]-1]}));
        }
        term_list.push_back(new OrOp(row_or));
        term_list.push_back(new OrOp(col_or));
        
    }
    // sum = 15 on diagonals
    vector<Term*> diag1_or, diag2_or;
    for(auto& subset: subsets) {
        diag1_or.push_back(new AndOp({diagVar[0][subset[0]-1], diagVar[0][subset[1]-1], diagVar[0][subset[2]-1]}));
        diag2_or.push_back(new AndOp({diagVar[1][subset[0]-1], diagVar[1][subset[1]-1], diagVar[1][subset[2]-1]}));
    }
    term_list.push_back(new OrOp(diag1_or));
    term_list.push_back(new OrOp(diag2_or));

    // term_list.push_back(gridVar[0][0][0]);
    // term_list.push_back(gridVar[0][0][1]);


    ex.setMainTerm(new AndOp(term_list));
    // cout << ex.toString() << endl;


    cnf::CnfExpression cnfEx;
    vector<cnf::Variable*> cnfVar;
    ex.toCnf(cnfEx, cnfVar);
    vector<bool> cnfVal(cnfVar.size()); /* values of the cnf variables */
    
    bool res = Encoder::solve_cnf(cnfEx, cnfVal);

    if(res) {

    // extract the solution
    for(unsigned int row_ind = 0; row_ind < N; row_ind ++) {
        for(unsigned int col_ind = 0; col_ind < N; col_ind ++) {
            // cout << row_ind << "," << col_ind << ": ";
            int digit = 0;
            for(unsigned int ind = 0; ind < N*N; ind ++) {
                // cout << cnfVal[cnfVar[gridVar[row_ind][col_ind][ind]->index()]->index()] << ", ";
                if(cnfVal[cnfVar[gridVar[row_ind][col_ind][ind]->index()]->index()]) {
                    digit = ind+1;
                }
            }
            cout << digit << " ";
            // cout << endl;
        }
        cout << endl;
    }

    }

    return 0;
}
