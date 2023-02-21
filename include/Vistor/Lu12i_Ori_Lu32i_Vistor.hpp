#ifndef LU12_ORI_LU32I_VISTOR
#define LU12_ORI_LU32I_VISTOR
#include "AOT_class/aot_class.hpp"
#include "Vistor/TB_Vistor.hpp"

class Lu12i_Ori_Lu32i_Vistor: TB_Vistor
{
    public:
         void visit(TB& tb);
         void start(Segment& seg);
};
#endif
