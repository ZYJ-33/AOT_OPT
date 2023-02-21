#include "Vistor/Lu12i_Ori_Vistor.hpp"
#include "Vistor/util.hpp"
#include "ConstRegSet/ConstRegSet.hpp"

bool is_lu12i_ori_pattern(ListNode<LoongArchInsInfo>* node)
{
    LoongArchInsInfo* cur = node->data;
    if(cur != nullptr && cur->opc == OPC_LU12I_W)
    {
        u_int8_t dst_reg = cur->dst_reg;
        node = node->next;
        cur = node->data;
        if(cur != nullptr && cur->opc == OPC_ORI
          && cur->dst_reg == dst_reg && cur->srcs_regs[0] == dst_reg)
            return true;
    }
    return false;
}

int64_t get_lu12i_ori_res(ListNode<LoongArchInsInfo>* node)
{
    int64_t dst_reg = 0;
    LoongArchInsInfo* lu12i_insn = node->data;
    LoongArchInsInfo* ori_insn = node->next->data;
    
    dst_reg = simulate_lu12i_w(lu12i_insn->si);
    dst_reg = simulate_ori(dst_reg, ori_insn->ui);
    
    return dst_reg;
}

ListNode<LoongArchInsInfo>* opt_lu12i_ori_parttern(ListNode<LoongArchInsInfo>*node, TB& tb, u_int32_t* cur_insn_index, ConstRegSet& const_set)
{
    while(node->data != nullptr)
    {
        if(is_lu12i_ori_pattern(node) && !tb.in_scope_of_rel(*cur_insn_index, 2))
        {
             int64_t cur_imm = get_lu12i_ori_res(node);
             bool substitute_with_addi = false;
             for(auto i=1; i<32; i++)
             {
                 if(const_set.isconst(i))
                 {
                     int64_t imm = const_set.get(i);
                     if(can_substitue_with_addi_insn(cur_imm-imm))
                     {
                        node = substitue_with_addi_insn(node, 2, i, cur_imm-imm, tb, cur_insn_index);
                        substitute_with_addi = true;
                        break;
                     }
                 }
             }
             const_set.set(node->data->dst_reg, cur_imm);
             if(substitute_with_addi)
             {
                    std::cout<<"opt 0x"<<std::hex<<tb.x86_addr<<std::endl;
                    node = node->next;
                    *cur_insn_index += 1;
             }
             else
             {
                    node = node->next->next;
                    *cur_insn_index += 2;
             }
        }
        else
        {
            if(node->data->opc == OPC_B || node->data->opc == OPC_BNE)
                    return node;
            const_set.kill(node->data->dst_reg);
            node = node->next;
            *cur_insn_index += 1;
            if(const_set.size == 0)
                    return node;
        }
    }
    return node;
}

void Lu12i_Ori_Vistor::visit(TB& tb)
{
    u_int32_t cur = 0;
    ConstRegSet const_set;
    for(auto iter=tb.dis_insns.begin(); iter != tb.dis_insns.end();)
    {
        if(iter->data->opc == OPC_B || iter->data->opc == OPC_BNE)
            break;
        if(is_lu12i_ori_pattern(iter))
        {
            int64_t imm = get_lu12i_ori_res(iter);
            const_set.set(iter->data->dst_reg, imm);
            cur += 2;
            iter = iter->next->next;
            iter = opt_lu12i_ori_parttern(iter, tb, &cur, const_set);
        }
        else
        {
            iter = iter->next;
            cur += 1;
        }
    }
}

void Lu12i_Ori_Vistor::start(Segment& seg)
{
      for(auto tb_ptr: seg._tbs)
      {
            if(tb_ptr->pipehole_available)
                    visit(*tb_ptr);
      }
}
