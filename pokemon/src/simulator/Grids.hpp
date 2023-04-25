#pragma once

#include <vector>

/*----------------------------------------------------------------------------*/
class Grids {

    public:

        enum Loc { Left, Right };

        enum ColMoveType {Col_1, Col_2, Col_3};
        enum ColDir {Col_Up, Col_Down};

        enum RowMovType {Row_1, Row_2, Row_3};
        enum RowDir {Row_Left, Row_Right};

    public:

        /*------------------------------------------------------------*/
        Grids(int N);

        /*------------------------------------------------------------*/
        void move_col(ColMoveType type, ColDir dir, Loc loc);
        
        /*------------------------------------------------------------*/
        void move_row(RowMovType type, RowDir dir);

        /*------------------------------------------------------------*/
        void assign(Loc loc, int* values);

        /*------------------------------------------------------------*/
        bool check_grids();

        /*------------------------------------------------------------*/
        bool check_grid(Loc loc);

        /*------------------------------------------------------------*/
        void print(Loc loc);

        /*------------------------------------------------------------*/
        void print_grids();

    public:

        int _N;

        std::vector<std::vector<int>> grid_left;
        std::vector<std::vector<int>> grid_right;
};