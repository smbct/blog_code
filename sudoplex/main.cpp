#include <iostream>
#include <vector>
#include <fstream>
#include <exception>
#include <glpk.h>

using namespace std;

//classe représentant une grille de Sudoku
class Sudoku {

    public:

        //construction d'une grille à partir d'un fichier
        Sudoku(string file) : _nbFixed(0) {

            _grid.resize(9);
            for(auto& line : _grid) {
                line.resize(9);
            }

            parse(file);
        }

        //construction d'une grille vide
        Sudoku() : _nbFixed(0) {

            _grid.resize(9);
            for(auto& line : _grid) {
                line.resize(9, -1);
            }
        }

        //remplie la grille à partir du résultat de la résolution
        void fillFromSolver(const double* grid) {
            _nbFixed = 81;
            for(unsigned int line = 0; line < 9; line ++) {
                for(unsigned int col = 0; col < 9; col ++) {

                    for(unsigned int var = 0; var < 9; var ++) {
                        int ind = line*81+col*9+var;
                        if(grid[ind] > 0.5) {
                            _grid[line][col] = var+1;
                        }
                    }

                }
            }
        }

        //retourne une élément de la grille en fonction de ses coordonnées
        const int& elt(unsigned int i, unsigned int j) const {
            return _grid.at(i).at(j);
        }

        //accesseur du nombre de case fixé
        unsigned int nbFixed() {
            return _nbFixed;
        }

        //lecture du fichier contenant la grille
        void parse(string name) {

            ifstream file(name);
            char c;

            if(!file) {
                throw exception();
            }

            for(int i = 0; i < 9; i++) {
                for(int j = 0; j < 9; j++) {
                    file >> c;
                    if(c != '.') {
                        _grid[i][j] = static_cast<int>(c-'0');
                        _nbFixed ++;
                    } else {
                        _grid[i][j] = -1;
                    }
                }
            }
        }

        //retourne la grille sous forme d'une chaîne de caractère
        string toString() {
            string res;
            for(int i = 0; i < _grid.size(); i++) {
                for(int j = 0; j < _grid[i].size(); j++) {
                    if(_grid[i][j] != -1) {
                        res += static_cast<char>(_grid[i][j]) + '0';
                        res += " ";
                    } else {
                        res += "  ";
                    }
                }
                res += '\n';
            }
            return res;
        }

    private:

        vector<vector<int>> _grid;
        unsigned int _nbFixed;
};

//classe représentant un solver pour une grille de sudoku
class Solver {

    public:

        //création d'une instance de solver dédié à une grille
        Solver(Sudoku& grid) : _grid(grid) {

        }

        //résolution de la grille
        void solve(Sudoku& grid) {

            createModel();

            glp_load_matrix(_prob, _nbEltCst, _rowCst, _colCst, _eltCst);

            //lancement de la résolution
            glp_simplex(_prob, nullptr);


            int res = glp_intopt(_prob, nullptr);

            if(res == 0) {
                //récupération des valeurs des variables
                const int nbVar = 9*9*9;
                double* x = new double[nbVar];
                for(int i = 0; i < nbVar; i++) {
                    x[i] = glp_mip_col_val(_prob, i+1);
                }
                //remplissage de la grille résultat
                grid.fillFromSolver(x);
                //libération de la mémoire
                delete[] x;
            }

            //libération de la mémoire du modèle
            freeModel();

            if(res != 0) {
                throw exception();
            }
        }

    private:

        //création du modèle linéaire en variable binaire à partir de la grille
        void createModel() {

            _prob = glp_create_prob();
            glp_set_prob_name(_prob, "Sudoku solver");
            glp_set_obj_dir(_prob, GLP_MIN);

            //nombre de variables : 9 par case
            const unsigned int nbVar = 81*9;

            //création des variables
            glp_add_cols(_prob, nbVar);

            for(int line = 0; line < 9; line ++) {
                for(int col = 0; col < 9; col ++) {
                    for(int var = 0; var < 9; var ++) {

                        int ind = line*81 + col*9 + var + 1;

                        //les variables sont binaires
                        glp_set_col_kind(_prob, ind, GLP_BV);
                        //pas de fonction à optimiser
                        glp_set_obj_coef(_prob, ind, 0.);

                        if(_grid.elt(line, col) != -1) { //les variables correspondant aux valeurs connues sont fixées
                            if(_grid.elt(line, col) == var+1) {
                                glp_set_col_bnds(_prob, ind, GLP_FX, 1., 1.);
                            } else {
                                glp_set_col_bnds(_prob, ind, GLP_FX, 0., 0.);
                            }
                        } else {
                            glp_set_col_bnds(_prob, ind, GLP_DB, 0., 1.);
                        }

                    }
                }
            }


            //nombre de contrainte : 9 ligne + 9 colonnes +
            // 9 sous-grilles + une contrainte par case + les cases fixées
            const int nbCst = 9*9 + 9*9 + 9*9 + 81;

            //matrice creuse des contraintes
            //dans chaque contrainte, il y a 9 coefficients non nuls
            _nbEltCst = 9*9*9 + 9*9*9 + 9*9*9 + 81*9;

            glp_add_rows(_prob, nbCst);

            //les contraintes sont toutes des contraintes d'égalité
            for(unsigned int i = 1; i <= nbCst; i++) {
                glp_set_row_bnds(_prob, i, GLP_FX, 1., 1.);
            }

            _rowCst = new int[_nbEltCst+1];
            _colCst = new int[_nbEltCst+1];
            _eltCst = new double[_nbEltCst+1];

            int indCst = 1; //numéro de la contrainte
            int ind = 1; //indice de la matrice creuse

            //contraintes sur les lignes
            for(unsigned int line = 0; line < 9; line ++) {
                //pour chacunes des neufs possibilitées
                for(unsigned int var = 0; var < 9; var ++) {
                    //pour chaque colonne
                    for(unsigned int col = 0; col < 9; col ++) {
                        _rowCst[ind] = indCst;
                        _colCst[ind] = line*81 + col*9 + var + 1;
                        _eltCst[ind] = 1.;
                        ind ++;
                    }
                    indCst ++;
                }
            }

            //contraintes sur les colonnes
            for(unsigned int col = 0; col < 9; col ++) {
                //pour chacunes des neufs possibilitées
                for(unsigned int var = 0; var < 9; var ++) {
                    //pour chaque ligne
                    for(unsigned int line = 0; line < 9; line ++) {
                        _rowCst[ind] = indCst;
                        _colCst[ind] = line*81 + col*9 + var + 1;
                        _eltCst[ind] = 1.;
                        ind ++;
                    }
                    indCst ++;
                }
            }

            //contraintes sur les sous-grilles
            //(particulièrement indigeste, oui!)
            for(int line = 0; line < 9; line += 3) {
                for(int col = 0; col < 9; col += 3) {
                    for(int var = 0; var < 9; var ++) {
                        for(int sLine = 0; sLine < 3; sLine ++) {
                            for(int sCol = 0; sCol < 3; sCol ++) {
                                _rowCst[ind] = indCst;
                                _colCst[ind] = (line+sLine)*81 + (col+sCol)*9 + var + 1;
                                _eltCst[ind] = 1.;
                                ind ++;
                            }
                        }
                        indCst ++;
                    }
                }
            }

            //contraintes sur les cellules, une valeur à 1 par cellule
            for(unsigned int line = 0; line < 9; line ++) {
                for(unsigned int col = 0; col < 9; col ++) {
                    for(unsigned int var = 0; var < 9; var ++) {
                        _rowCst[ind] = indCst;
                        _colCst[ind] = line*81 + col*9 + var + 1;
                        _eltCst[ind] = 1.;
                        ind ++;
                    }
                    indCst ++;
                }
            }
        }

        //libération de la mémoire utilisée pour le modèle
        void freeModel() {
            glp_delete_prob(_prob);
            delete[] _rowCst;
            delete[] _colCst;
            delete[] _eltCst;
        }

        glp_prob* _prob;
        int* _rowCst;
        int* _colCst;
        double* _eltCst;
        unsigned int _nbEltCst;
        Sudoku& _grid;

};


int main() {

    //désactivation du log de glpk
    glp_term_out(0);

    string name;

    try {

        cout << "grille à résoudre : ";
        cin >> name;

        Sudoku grid(name);
        Sudoku res;
        cout << "Grille initiale : " << endl << endl;
        cout << grid.toString() << endl;
        Solver solver(grid);
        solver.solve(res);

        cout << "Grille résolue : " << endl << endl;
        cout << res.toString() << endl;
    } catch(exception& ex) {
        cout << "Impossible de résoudre la grille." << endl;
    }

    return 0;
}
