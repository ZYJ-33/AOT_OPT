#include "Vistor/DisassembleVistor.hpp"
#include <cstring>
#include "Vistor/util.hpp"

void DisassmbleVistor::visit(TB& tb)
{
    u_int32_t* insn_ptr = (u_int32_t*)tb.code;
    u_int32_t count = tb.code_size>>2;
    u_int32_t insn;
    u_int32_t i = 0;
    while(i < count)
    {
        LoongArchInsInfo* res = new LoongArchInsInfo();
        memset(res, 0,  sizeof(LoongArchInsInfo));

        insn = *insn_ptr;
        if(!decode(res, insn))
        {
            std::cerr<<"disassmble insn 0x" << std::hex << insn <<" failed"<<std::endl;
            tb.has_invalid_insn = true;
        }

        check_and_add_branch_insn(tb, res, i);

        res->origin_binary = insn;
        tb.dis_insns.insert_at_tail(res);
        insn_ptr += 1;
        i += 1;
    }
}

void DisassmbleVistor::toinsns(u_int32_t* bin_insns, u_int32_t size, std::vector<LoongArchInsInfo*>& insns)
{
    while(size > 0)
    {
        LoongArchInsInfo* res = new LoongArchInsInfo();
        memset(res, 0,  sizeof(LoongArchInsInfo));
        if(!decode(res, *bin_insns))
        {
            std::cerr<<"disassmble insn 0x" << std::hex << *bin_insns <<" failed"<<std::endl;
            res->opc = OPC_INVALID;
        }
        insns.push_back(res);
        bin_insns += 1;
        size -= 1;
    }
}

void DisassmbleVistor::start(Segment& seg)
{
    for(auto tb_ptr : seg._tbs)
    {
       (*tb_ptr).visit(*this);
       tb_ptr->pipehole_available = tb_ptr->pipehole_opt_available();
    }
}
