#pragma once

#include <string>
#include <vector>


class Move;
class RowMove;
class ColMove;


// class representing a move in the grids, to facilitate constraint encoding
class Move {

  public:

    enum Type {Row, Column};

    virtual Type type() = 0;

    virtual unsigned int moveIndex() = 0; // assign an index to each different move

    virtual std::string to_string() = 0;

    Move() {

    }

    virtual ~Move() { }

};

// a move concerning a row
class RowMove: public Move {

    public:

        enum Direction {Left, Right};

        RowMove(Direction pdir, unsigned int prow_index): dir(pdir), row_index(prow_index)
        {

        }

        virtual Type type() {
            return Move::Row;
        }

        virtual unsigned int moveIndex() {
            if(dir == Left) {
                return row_index;
            } else {
                return row_index+3;
            }
        }

        std::string to_string() {

            std::string res = "row_";
            res += std::to_string(row_index);
            res += "_";
        
            if(dir == Left) {
                res += "Left";
            } else {
                res += "Right";
            }

            return res;
        }

        static unsigned int getMoveIndex(RowMove::Direction dir, unsigned int index) {
            if(dir == RowMove::Left) {
                return index;
            } else {
                return index+3;
            }
        }



        Direction dir;

        unsigned int row_index;

};

// a move concerning a column
class ColMove: public Move {

    public:

        enum Grid {Left, Right};

        enum Direction {Up, Down};

        ColMove(Grid pgrid, Direction pdir, unsigned int pcol_index): grid(pgrid), dir(pdir), col_index(pcol_index) {

        }

        virtual Type type() {
            return Move::Column;
        }

        virtual unsigned int moveIndex() {
            unsigned int res = 6;
            if(dir == Down) {
                res += 3;
            }
            if(grid == Right) {
                res += 6;
            }
            res += col_index;
            return res;
        }

        std::string to_string() {

            std::string res = "col_";

            if(grid == Left) {
                res += "left_";
            } else {
                res += "right_";
            }

            res += std::to_string(col_index);
            res += "_";
        
            if(dir == Up) {
                res += "Up";
            } else {
                res += "Down";
            }

            return res;
        }

        static unsigned int getMoveIndex(ColMove::Grid grid, ColMove::Direction dir, unsigned int index) {
            unsigned int res = 6;
            if(dir == ColMove::Down) {
                res += 3;
            }
            if(grid == ColMove::Right) {
                res += 6;
            }
            res += index;
            return res;
        }

        Grid grid; // left or right

        Direction dir;

        unsigned int col_index;


};

// all the moves:
// column: left/right_grid, col index, up/down
// row: row_index, left/right