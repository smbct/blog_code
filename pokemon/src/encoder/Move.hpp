#pragma once

#include <string>
#include <vector>
#include <memory>

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

        // return a pointer to a specific move from an index
        static std::shared_ptr<Move> getMoveFromIndex(unsigned int index) {
            return _moves[index];
        }

        static void create() {
            _moves.resize(18, nullptr);
        }


        Move() {

        }

        virtual ~Move() { }


    protected:

        // maintain a static collection of moves for all the program, this is not the ideal design
        static std::vector<std::shared_ptr<Move>> _moves;


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

        // allocate the row moves
        static void create() {
            for(unsigned int index = 0; index < 3; index ++) {
                _moves[index] = std::make_shared<RowMove>(RowMove(RowMove::Left, index));
                _moves[index+3] = std::make_shared<RowMove>(RowMove(RowMove::Right, index));
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

        // allocate the column moves
        static void create() {
            
            for(unsigned int index = 0; index < 3; index ++) {

                _moves[6+index] = std::make_shared<ColMove>(ColMove(ColMove::Left, ColMove::Up, index));
                _moves[6+index+3] = std::make_shared<ColMove>(ColMove(ColMove::Left, ColMove::Down, index));
                _moves[6+index+6] = std::make_shared<ColMove>(ColMove(ColMove::Right, ColMove::Up, index));
                _moves[6+index+9] = std::make_shared<ColMove>(ColMove(ColMove::Right, ColMove::Down, index));

            }
        
        }

        Grid grid; // left or right

        Direction dir;

        unsigned int col_index;


};

// all the moves:
// column: left/right_grid, col index, up/down
// row: row_index, left/right