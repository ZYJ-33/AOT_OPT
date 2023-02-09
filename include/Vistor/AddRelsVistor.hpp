#ifndef ADD_RELS_VISTOR
#define ADD_RELS_VISTOR

#include "AOT_class/aot_class.hpp"
#include "Vistor/TB_Vistor.hpp"

class TB_AddRels_Vistor: TB_Vistor
{
    const std::vector<AOT_rel>* rels;
    public:
        void visit(TB& tb);
        void start(Segment& seg, const std::vector<AOT_rel>* rels);
};


#endif
