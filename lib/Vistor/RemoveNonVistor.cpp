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
        visit(*tb_ptr);
}
