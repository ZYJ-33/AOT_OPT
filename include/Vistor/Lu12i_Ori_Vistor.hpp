#ifndef LU12_ORI_VISTOR
#define LU12_ORI_VISTOR
#include "AOT_class/aot_class.hpp"
#include "Vistor/TB_Vistor.hpp"

class Lu12i_Ori_Vistor: TB_Vistor
{
    public:
        void visit(TB& tb);
        void start(Segment& seg);
};

#endif
