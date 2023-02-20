#ifndef PEEPHOLE_OPT_VISTOR
#define PEEPHOLE_OPT_VISTOR
#include "AOT_class/aot_class.hpp"
#include "Vistor/TB_Vistor.hpp"

class PeepHoleOPTVistor: TB_Vistor
{
    public:
         void visit(TB& tb);
         void start(Segment& seg);
};
#endif
