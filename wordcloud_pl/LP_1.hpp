#ifndef LP_1_HPP
#define LP_1_HPP

#include <vector>
#include "Rect.hpp"

class LP_1 {

    public:
        LP_1();
        void ajouterRect(Rect& rect);
        void resoudre();
        void calculer();

    private:
        std::vector<Rect*> _rect;
};

#endif
