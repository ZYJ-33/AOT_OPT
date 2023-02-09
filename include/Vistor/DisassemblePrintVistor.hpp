#ifndef DISASSEMBLE_PRINT_VISTOR
#define DISASSEMBLE_PRINT_VISTOR

#include "AOT_class/aot_class.hpp"
#include "Vistor/TB_Vistor.hpp"


class DisassmblePrinterVistor: TB_Vistor
{
    public:
            u_int64_t seg_start;
            void visit(TB& tb);
            void start(Segment& seg);
            void start(Segment& seg, u_int64_t entry);
    private:
            void print_one_insn(LoongArchInsInfo& insn);
};

#endif
