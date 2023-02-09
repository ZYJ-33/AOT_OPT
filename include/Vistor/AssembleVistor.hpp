#ifndef ASSEMBLE_VISTOR
#define ASSEMBLE_VISTOR

#include "AOT_class/aot_class.hpp"
#include "Vistor/TB_Vistor.hpp"
#include "Vistor/DisassembleVistor.hpp"
#include "Vistor/DisassemblePrintVistor.hpp"

class AssmbleVistor: TB_Vistor
{
    public:
        std::vector<u_int32_t*> codes;
        void visit(TB& tb);
        void start(Segment& seg);
        void start(Segment& seg, DisassmbleVistor& dis, DisassmblePrinterVistor& disprint);
        u_int32_t* tocode(std::vector<LoongArchInsInfo*>& insns); 
        static u_int32_t tocode(LoongArchInsInfo* insn);
};

#endif
