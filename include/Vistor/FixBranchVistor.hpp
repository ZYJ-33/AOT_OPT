#ifndef FIX_BNE_VISTOR
#define FIX_BNE_VISTOR

#include "AOT_class/aot_class.hpp"
#include "Vistor/TB_Vistor.hpp"

class FixBranchVistor: TB_Vistor
{
    public:
            void visit(TB& tb);
            void start(Segment& seg);
            void fix_bne(TB& tb);
            void fix_b(TB& tb);
};

#endif
