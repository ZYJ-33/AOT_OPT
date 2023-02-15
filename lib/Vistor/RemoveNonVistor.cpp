#include "Vistor/RemoveNonVistor.hpp"

void RemoveNonVistor::visit(TB& tb)
{
     u_int64_t i = 0;
     for(auto iter = tb.dis_insns.begin(); iter != tb.dis_insns.end();)
     {
        LoongArchInsInfo* insn = iter->data;
        if(insn->opc == OPC_ANDI 
           && insn->dst_reg == 0
           && insn->srcs_regs[0] == 0 
           && insn->ui == 0)
        {
             auto next = tb.delete_ith_insn(iter, i);
             if(iter == next)
             {
                iter = iter->next;
                i += 1;
             }
             else
                 iter = next;
        }
        else
        {
                iter = iter->next;
                i += 1;
        }
     }
}

void RemoveNonVistor::start(Segment& seg)
{
    for(auto tb_ptr : seg._tbs)
    {
        if(tb_ptr->pipehole_available)
            visit(*tb_ptr);
        else
            std::cout<<std::hex<<"0x"<<tb_ptr->x86_addr<<"is not available for pipehole opt"<<std::endl;
    }
}
