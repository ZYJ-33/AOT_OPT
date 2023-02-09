#ifndef DISASSEMBLE_VISTOR
#define DISASSEMBLE_VISTOR
#include "AOT_class/aot_class.hpp"
#include "Vistor/TB_Vistor.hpp"

class DisassmbleVistor: TB_Vistor
{
    public:
            void visit(TB& tb);
            void start(Segment& seg);
            void toinsns(u_int32_t* bin_insns, u_int32_t size, std::vector<LoongArchInsInfo*>& insns);
};

#endif
