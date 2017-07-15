#ifndef LP_1_HPP
#define LP_1_HPP

#include <vector>
#include <list>
#include "Rect.hpp"

class LP_1 {

    public:
        LP_1();
        void calculer(std::vector<Rect*>& _listeRect);
        bool resoudre(std::list<Rect*>& _listeEssai);


};

#endif
