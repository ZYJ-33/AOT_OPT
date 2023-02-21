#include "Vistor/Lu12i_Ori_Lu32i_Vistor.hpp"
#include "ConstRegSet/ConstRegSet.hpp"
#include "Vistor/util.hpp"
#include <cstring>
extern void (*gen_require_func_table[])(LoongArchInsInfo* insn, reg_info* n);

bool is_lu12i_ori_lu32i_pattern(ListNode<LoongArchInsInfo>* node)
{
      LoongArchInsInfo* cur = node->data;
      if(cur != nullptr && cur->opc == OPC_LU12I_W)
      {
            u_int8_t dst_reg = cur->dst_reg;
            node = node->next;
            cur = node->data;
            if(cur != nullptr && cur->opc == OPC_ORI && cur->dst_reg == dst_reg && cur->srcs_regs[0] == dst_reg)
            {
                node = node->next;
                cur = node->data;
                if(cur != nullptr && cur->opc == OPC_LU32I_D && cur->dst_reg == dst_reg)
                    return true;
            }
      }
      return false;
}

int64_t get_lu12i_ori_lu32i_res(ListNode<LoongArchInsInfo>* node)
{
    int64_t dst_reg = 0;
    LoongArchInsInfo* lu12i_insn = node->data;
    LoongArchInsInfo* ori_insn = node->next->data;
    LoongArchInsInfo* lu32i_insn = node->next->next->data;

    dst_reg = simulate_lu12i_w(lu12i_insn->si);
    dst_reg = simulate_ori(dst_reg, ori_insn->ui);
    dst_reg = simulate_lu32i_d(dst_reg, lu32i_insn->si);

    return dst_reg;
}


ListNode<LoongArchInsInfo>* opt_lu12i_ori_lu32i_pattern(ListNode<LoongArchInsInfo>* node, TB& tb, u_int32_t* cur_insn_index, ConstRegSet& const_set)
{
    while(node->data != nullptr)
    {
        if(is_lu12i_ori_lu32i_pattern(node))
        {
            int64_t cur_imm = get_lu12i_ori_lu32i_res(node);
            bool substitute_with_addi = false;
            for(auto i=1; i<32; i++)
            {
                if(const_set.isconst(i))
                {
                      int64_t imm = const_set.get(i);
                      if(can_substitue_with_addi_insn(cur_imm - imm))
                      {
                            node = substitue_with_addi_insn(node, 3, i, cur_imm-imm, tb, cur_insn_index);
                            substitute_with_addi = true;
                            break;
                      }
                }
            }
            const_set.set(node->data->dst_reg, cur_imm);
            if(substitute_with_addi)
            {
                    node = node->next;
                    *cur_insn_index += 1;
            }
            else
            {
                    node = node->next->next->next;
                    *cur_insn_index += 3;
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

void Lu12i_Ori_Lu32i_Vistor::visit(TB& tb)
{
    u_int32_t cur = 0;
    ConstRegSet const_set;

    for(auto iter=tb.dis_insns.begin(); iter != tb.dis_insns.end();)
    {
         if(iter->data->opc == OPC_B || iter->data->opc == OPC_BNE)
                break;
         if(is_lu12i_ori_lu32i_pattern(iter))
         {
            int64_t imm = get_lu12i_ori_lu32i_res(iter);
            const_set.set(iter->data->dst_reg, imm);
            cur += 3;
            iter = iter->next->next->next;
            iter = opt_lu12i_ori_lu32i_pattern(iter, tb, &cur, const_set);
         }
         else
         {
            iter = iter->next;
            cur += 1;
         }
    }
}

void Lu12i_Ori_Lu32i_Vistor::start(Segment& seg)
{
    for(auto tb_ptr : seg._tbs)
    {
            if(tb_ptr->pipehole_available)
                visit(*tb_ptr);
    }
}
