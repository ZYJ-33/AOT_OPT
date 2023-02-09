#ifndef REMOVE_TAIL_CHECK_VISTOR
#define REMVOE_TAIL_CHECK_VISTOR


#include "AOT_class/aot_class.hpp"
#include "Vistor/TB_Vistor.hpp"

class RemoveTailCheckVistor: TB_Vistor
{
    public:
       void visit(TB& tb);
       void start(Segment& seg);
};

#endif
