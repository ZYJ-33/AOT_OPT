#ifndef REMOVE_NON_VISTOR
#define REMOVE_NON_VISTOR

#include "AOT_class/aot_class.hpp"
#include "Vistor/TB_Vistor.hpp"

class RemoveNonVistor: TB_Vistor
{
    public:
            void visit(TB& tb);
            void start(Segment& seg);
};

#endif
