#include <iostream>
#include <cstdlib>

#include <vector>
#include <list>

#include "logic/Expression.hpp"
#include "logic/Operation.hpp"
#include "logic/Variable.hpp"

#include <fstream>

using namespace std;

/*----------------------------------------------------------------------------*/
class Grids {

    public:

        enum Loc { Left, Right };

        enum ColMoveType {Col_1, Col_2, Col_3};
        enum ColDir {Col_Up, Col_Down};

        enum RowMovType {Row_1, Row_2, Row_3};
        enum RowDir {Row_Left, Row_Right};

    public:

        Grids(int N):
        grid_left(N, vector<int>(N, 0)),
        grid_right(N, vector<int>(N, 0))
        {
            this->_N = N;
        }

        /*------------------------------------------------------------*/
        void move_col(ColMoveType type, ColDir dir, Loc loc) {

            vector<vector<int>>* grid_ptr;    
            if(loc == Left) {
                grid_ptr = &grid_left;
            } else {
                grid_ptr = &grid_right;
            }

            unsigned int colIndex = 2;

            if(type == Col_1) {
                colIndex = 0;
            } else if(type == Col_2) {
                colIndex = 1;
            }

            if(dir == Col_Up) {
                int init_cell = (*grid_ptr)[0][colIndex];
                (*grid_ptr)[0][colIndex] = (*grid_ptr)[1][colIndex];
                (*grid_ptr)[1][colIndex] = (*grid_ptr)[2][colIndex];
                (*grid_ptr)[2][colIndex] = init_cell;
            } else {
                int init_cell = (*grid_ptr)[2][colIndex];
                (*grid_ptr)[2][colIndex] = (*grid_ptr)[1][colIndex];
                (*grid_ptr)[1][colIndex] = (*grid_ptr)[0][colIndex];
                (*grid_ptr)[0][colIndex] = init_cell;
            }

        }
        

        /*------------------------------------------------------------*/
        void move_row(RowMovType type, RowDir dir) {

            unsigned int row_ind = 0;
            if(type == Row_2) {
                row_ind = 1;
            } else if(type == Row_3) {
                row_ind = 2;
            }

            if(dir == Row_Right) {
                int init_cell = grid_right[row_ind][2];
                grid_right[row_ind][2] = grid_right[row_ind][1];
                grid_right[row_ind][1] = grid_right[row_ind][0];
                grid_right[row_ind][0] = grid_left[row_ind][2];
                grid_left[row_ind][2] = grid_left[row_ind][1];
                grid_left[row_ind][1] = grid_left[row_ind][0];
                grid_left[row_ind][0] = init_cell;
            } else {
                int init_cell = grid_left[row_ind][0];
                grid_left[row_ind][0] = grid_left[row_ind][1];
                grid_left[row_ind][1] = grid_left[row_ind][2];
                grid_left[row_ind][2] = grid_right[row_ind][0];
                grid_right[row_ind][0] = grid_right[row_ind][1];
                grid_right[row_ind][1] = grid_right[row_ind][2];
                grid_right[row_ind][2] = init_cell;
            }

        }

        /*------------------------------------------------------------*/
        void assign(Loc loc, int* values) {

            vector<vector<int>>* grid_ptr;    
            if(loc == Left) {
                grid_ptr = &grid_left;
            } else {
                grid_ptr = &grid_right;
            }

            int ind = 0;
            for(unsigned int ind1 = 0; ind1 < grid_ptr->size(); ind1 ++) {
                for(unsigned int ind2 = 0; ind2 < (*grid_ptr)[ind1].size(); ind2 ++) {
                    (*grid_ptr)[ind1][ind2] = values[ind];
                    ind ++;
                }
            }
            cout << endl;

        } 

        /*------------------------------------------------------------*/
        bool check_grids() {

            return check_grid(Left) && check_grid(Right); 
        }

        /*------------------------------------------------------------*/
        bool check_grid(Loc loc) {

            vector<vector<int>>* grid_ptr;    
            if(loc == Left) {
                grid_ptr = &grid_left;
            } else {
                grid_ptr = &grid_right;
            }

            int magicSum = 15;

            // check the cols
            for(unsigned int colInd = 0; colInd < 3; colInd ++) {
                int sum = 0;
                for(unsigned int rowInd = 0; rowInd < 3; rowInd ++) {
                    sum += (*grid_ptr)[rowInd][colInd];
                }
                if(sum != magicSum) {
                    return false;
                }
            }

            // check the cols
            for(unsigned int rowInd = 0; rowInd < 3; rowInd ++) {
                int sum = 0;
                for(unsigned int colInd = 0; colInd < 3; colInd ++) {
                    sum += (*grid_ptr)[rowInd][colInd];
                }
                if(sum != magicSum) {
                    return false;
                }
            }

            // diagonal
            int sum1 = 0, sum2 = 0;
            for(unsigned int ind; ind < 3; ind ++) {
                sum1 += (*grid_ptr)[ind][ind];
                sum2 += (*grid_ptr)[ind][2-ind];
            }

            if(sum1 != magicSum) {
                return false;
            }

            if(sum2 != magicSum) {
                return false;
            }

            return true;
        }

        /*------------------------------------------------------------*/
        void print(Loc loc) {

            vector<vector<int>>* grid_ptr;    
            if(loc == Left) {
                grid_ptr = &grid_left;
            } else {
                grid_ptr = &grid_right;
            }

            for(unsigned int ind = 0; ind < grid_ptr->size(); ind ++) {
                for(unsigned int ind2 = 0; ind2 < (*grid_ptr)[ind].size(); ind2 ++) {
                    cout << (*grid_ptr)[ind][ind2] << " ";
                }
                cout << endl;
            }

        }

        /*------------------------------------------------------------*/
        void print_grids() {

            for(unsigned int ind = 0; ind < grid_left.size(); ind ++) {
                
                for(unsigned int ind2 = 0; ind2 < grid_left[ind].size(); ind2 ++) {
                    cout << grid_left[ind][ind2] << " ";
                }

                cout << "\t\t";

                 for(unsigned int ind2 = 0; ind2 < grid_right[ind].size(); ind2 ++) {
                    cout << grid_right[ind][ind2] << " ";
                }

                cout << endl;
            }

        }

    public:

        int _N;

        vector<vector<int>> grid_left;
        vector<vector<int>> grid_right;
};


/*----------------------------------------------------------------------------*/
void enumerate_subsets(int* sequence, unsigned int size, list<vector<int>>& subsets) {

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



    int sequence[] = {1,2,3,4,5,6,7,8,9};
    list<vector<int>> subsets;
    enumerate_subsets(sequence, 9, subsets);
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

    ex.setMainTerm(new AndOp(term_list));

    cnf::CnfExpression cnfEx;

    vector<cnf::Variable*> cnfVar;
    
    ex.toCnf(cnfEx, cnfVar);
    cnfEx.exportDimacs("temp.dm");

    // ./glucose temp.dm res

    /* values of the cnf variables */
    vector<bool> cnfVal(cnfVar.size());
    ifstream file("res");
    string str;
    while(!file.eof()) {
        file >> str;
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

    for(unsigned int row_ind = 0; row_ind < N; row_ind ++) {
        for(unsigned int col_ind = 0; col_ind < N; col_ind ++) {
            cout << row_ind << "," << col_ind << ": ";
            for(unsigned int ind = 0; ind < N; ind ++) {
                cout << cnfVal[cnfVar[gridVar[row_ind][col_ind][ind]->index()]->index()] << ", ";
            }
            cout << endl;
        }
    }



    return 0;
}
