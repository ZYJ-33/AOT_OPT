#ifndef TB_VISTOR
#define TB_VISTOR
#include "AOT_class/aot_class.hpp"
class TB_Vistor
{
    public:
        virtual void visit(TB& tb) = 0;
};

#endif
