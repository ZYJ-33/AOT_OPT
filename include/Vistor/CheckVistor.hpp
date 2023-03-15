#ifndef CHECK_VISTOR
#define CHECK_VISTOR
#include "AOT_class/aot_class.hpp"
#include "Vistor/TB_Vistor.hpp"

class CheckVistor: TB_Vistor
{
    public:
        void visit(TB& tb);
        void start(Segment& seg);
};

#endif
