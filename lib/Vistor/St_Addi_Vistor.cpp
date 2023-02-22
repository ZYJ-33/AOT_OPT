#include "Vistor/St_Addi_Vistor.hpp"
bool is_st_addi_pattern(ListNode<LoongArchInsInfo>* node)
{
    LoongArchInsInfo* cur = node->data;
    if(cur != nullptr && cur->opc == OPC_ST_D && cur->si == -8)
    {
        u_int8_t reg = cur->srcs_regs[0];
        node = node->next;
        cur = node->data;
        if(cur != nullptr 
           && cur->opc == OPC_ADDI_D && cur->si == -8 && cur->dst_reg == reg && cur->srcs_regs[0] == reg)
            return true;
    }
    return false;
}

#define __MAX_ST_D 256
void opt_sequence_of_st_addi(TB& tb, ListNode<LoongArchInsInfo>* insn, u_int32_t total, u_int32_t st_insn_index)
{
     u_int32_t count = ((total < __MAX_ST_D)? total:__MAX_ST_D);
     ListNode<LoongArchInsInfo>* cur_st_insn = insn;
     int i = 1;
     int32_t addi_imm =(int32_t)((int32_t)count * -8);
     
     while(count > 0)
     {
         cur_st_insn->data->si = i * -8;
         ListNode<LoongArchInsInfo>* addi_insn = cur_st_insn->next;
         u_int32_t addi_insn_index = st_insn_index + 1;

         cur_st_insn = addi_insn->next;

         if(count == 1)
             addi_insn->data->si = addi_imm;
         else
         {
             tb.delete_ith_insn(addi_insn, addi_insn_index);
             st_insn_index = addi_insn_index;
         }
         count -= 1;
         i += 1;
     }

     if(total > __MAX_ST_D)
          opt_sequence_of_st_addi(tb, cur_st_insn, total-__MAX_ST_D, st_insn_index);

}

void St_Addi_Vistor::visit(TB& tb)
{
    u_int32_t cur = 0;
    for(auto iter=tb.dis_insns.begin(); iter != tb.dis_insns.end();)
    {
        if(iter->data->opc == OPC_BNE || iter->data->opc == OPC_B)
                break;

        if(is_st_addi_pattern(iter))
        {
            u_int32_t count = 1;
            auto save = iter;
            u_int8_t reg = iter->data->srcs_regs[0];
            iter = iter->next->next;

            while(iter != tb.dis_insns.end() && is_st_addi_pattern(iter)
                  && iter->data->srcs_regs[0] == reg)
            {
                 count += 1;
                 iter = iter->next->next;
            }

            if(count != 1)
            {
                std::cout<<"st_addi_opt 0x"<<std::hex<<tb.x86_addr<<std::endl;
                opt_sequence_of_st_addi(tb, save, count, cur);          
            }
            cur += (count+1);
        }
        else
        {
            iter = iter->next;
            cur += 1;
        }
    }
}

void St_Addi_Vistor::start(Segment& seg)
{
    for(auto tb_ptr: seg._tbs)
    {
        if(tb_ptr->pipehole_available)
            tb_ptr->visit(*this);
    }
}
