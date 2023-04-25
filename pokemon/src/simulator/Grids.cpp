#include "Grids.hpp"

#include <iostream>

using namespace std;

/*----------------------------------------------------------------------------*/
Grids::Grids(int N):
grid_left(N, vector<int>(N, 0)),
grid_right(N, vector<int>(N, 0))
{
    this->_N = N;
}

/*------------------------------------------------------------*/
void Grids::move_col(ColMoveType type, ColDir dir, Loc loc) {

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
void Grids::move_row(RowMovType type, RowDir dir) {

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
void Grids::assign(Loc loc, int* values) {

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
bool Grids::check_grids() {

    return check_grid(Left) && check_grid(Right); 
}

/*------------------------------------------------------------*/
bool Grids::check_grid(Loc loc) {

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
void Grids::print(Loc loc) {

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
void Grids::print_grids() {

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
