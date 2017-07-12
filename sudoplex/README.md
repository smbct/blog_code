# Sudoplex
Sudoku solver using linear programming

### Description
This program demonstrates how to solve a 9*9 sudoku grid using a linear programming solver. The solver used is GLPK. The model contains only binary variables.

### Dependencies
To compile, you need the glpk library.  You can install it on ubuntu with aptitude : libglpk-dev

### Compiling
You can compile the program using g++ with the command : g++ main.cpp -std=c++11 -lglpk -o SudoPlex.

### Sudoku format
The grid contains 9*9 cells. The gris is read from a file. If the value of a cell is uknown, there is a '.' in the file. Otherwise, the value is written.

### Testing
You can test the program with the grid "grid.sk" given as example.

### Warning
This is not the most efficient way to solver a sudoku. It is just an example of the power of linear programming and modeling with binary variables.
