#pragma once

#include <vector>
#include <iostream>

#include "../logic/Expression.hpp"
#include "../logic/Operation.hpp"
#include "../logic/Variable.hpp"

typedef std::vector<std::vector<std::vector<Variable*>>> GridVar;

//-------------------------------------------------
class Encoding {

    public:

        //-------------------------------------------------
        Encoding();

        //-------------------------------------------------
        void createGridVariables(GridVar& grid_vars);

    private:

        std::vector<std::pair<GridVar, GridVar>> _sequence;

        
};