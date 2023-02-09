#ifndef REMOVE_TAIL_VISTOR
#define REMOVE_TAIL_VISTOR

#include "AOT_class/aot_class.hpp"
#include "Vistor/TB_Vistor.hpp"

class RemoveTailVistor: TB_Vistor
{
    public:
        void visit(TB& tb);
        void start(Segment& seg);
    private:
        void remove_tail_of(TB& tb, uint which);
};

#endif
