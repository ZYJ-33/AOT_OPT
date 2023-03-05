#ifndef TU_VISTOR
#define TU_VISTOR
#include "AOT_class/aot_class.hpp"
#include "Vistor/TB_Vistor.hpp"
class TU_Vistor: TB_Vistor
{
    public:
            void visit(TB& tb);
            void start(Segment& seg);
};
#endif
