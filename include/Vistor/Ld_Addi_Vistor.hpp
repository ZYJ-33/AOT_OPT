#ifndef LD_ADDI_VISTOR
#define LD_ADDI_VISTOR
#include "AOT_class/aot_class.hpp"
#include "Vistor/TB_Vistor.hpp"

class Ld_Addi_Vistor: TB_Vistor
{
    public:
            void visit(TB& tb);
            void start(Segment& seg);
};

#endif
