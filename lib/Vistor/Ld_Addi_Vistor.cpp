#include "Vistor/Ld_Addi_Vistor.hpp"
bool is_ld_addi_pattern(ListNode<LoongArchInsInfo>* node)
{
    LoongArchInsInfo* cur = node->data;
    if(cur != nullptr && cur->opc == OPC_LD_D && cur->si == 0)
    {
        u_int8_t reg = cur->srcs_regs[0];
        node = node->next;
        cur = node->data;
        if(cur != nullptr
           && cur->opc == OPC_ADDI_D && cur->si == 8 && cur->dst_reg == reg && cur->srcs_regs[0] == reg)
                return true;
    }
    return false;
}

#define __MAX_LD_D 255
void opt_sequence_of_ld_addi(TB& tb, ListNode<LoongArchInsInfo>* insn, u_int32_t total, u_int32_t ld_insn_index)
{
    u_int32_t count = ((total < __MAX_LD_D)? total:__MAX_LD_D);
    ListNode<LoongArchInsInfo>* cur_ld_insn = insn;
    int i = 0;
    int32_t addi_imm = (int32_t)((int32_t)count * 8);

    while(count > 0)
    {
        cur_ld_insn->data->si = i * 8;
        ListNode<LoongArchInsInfo>* addi_insn = cur_ld_insn->next;
        u_int32_t addi_insn_index = ld_insn_index + 1;
        
        cur_ld_insn = addi_insn->next;
        if(count == 1)
            addi_insn->data->si = addi_imm;
        else
        {
            tb.delete_ith_insn(addi_insn, addi_insn_index);
            ld_insn_index = addi_insn_index;
        }
        count -= 1;
        i += 1;
    }
    
    if(total > __MAX_LD_D)
            opt_sequence_of_ld_addi(tb, cur_ld_insn, total-__MAX_LD_D, ld_insn_index);
}

void Ld_Addi_Vistor::visit(TB& tb)
{
    u_int32_t cur = 0;
    for(auto iter=tb.dis_insns.begin(); iter != tb.dis_insns.end();)
    {
        if(iter->data->opc == OPC_BNE || iter->data->opc == OPC_B)
                break;

        if(is_ld_addi_pattern(iter))
        {
            u_int32_t count = 1;
            auto save = iter;
            u_int8_t reg = iter->data->srcs_regs[0];
            iter = iter->next->next;

            while(iter != tb.dis_insns.end() && is_ld_addi_pattern(iter)
                  && iter->data->srcs_regs[0] == reg)
            {
                count += 1;
                iter = iter->next->next;
            }

            if(count != 1)
            {
                std::cout<<"ld_addi_opt 0x"<<std::hex<<tb.x86_addr<<std::endl;
                opt_sequence_of_ld_addi(tb, save, count, cur);
            }
            cur += (count + 1);
        }
        else
        {
            iter = iter->next;
            cur += 1;
        }
    }
}

void Ld_Addi_Vistor::start(Segment& seg)
{
    for(auto tb_ptr: seg._tbs)
    {
        if(tb_ptr->pipehole_available)
                tb_ptr->visit(*this);
    }
}
