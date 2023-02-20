#include "Vistor/PeepHoleOPTVistor.hpp"
#include "ConstRegSet/ConstRegSet.hpp"
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

int64_t simulate_lu12i_w(int32_t si)
{
    int64_t res = si;
    res = res << 12;
    return res;
}

int64_t simulate_ori(int64_t reg, u_int64_t imm)
{
    u_int64_t mask = 0xfff;
    imm &= mask;
    return reg | imm;
}

int64_t simulate_lu32i_d(int64_t reg, int32_t si)
{
    reg = reg & 0x00000000ffffffff;
    reg |= ((int64_t)si) << 32;
    return reg;
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

bool is_reg_killed(LoongArchInsInfo* insn, u_int8_t reg)
{
    if(reg == 0 || insn->opc == OPC_ST_D || insn->opc == OPC_ST_W || insn->opc == OPC_ST_H || insn->opc == OPC_ST_B
                || insn->opc == OPC_STX_B|| insn->opc == OPC_STX_H|| insn->opc == OPC_STX_W|| insn->opc == OPC_STX_D
                || insn->opc ==OPC_STPTR_W || insn->opc == OPC_STR_D || insn->opc == OPC_STGT_B || insn->opc == OPC_STGT_D
                || insn->opc == OPC_STGT_W || insn->opc == OPC_STGT_H || insn->opc == OPC_STLE_B || insn->opc == OPC_STLE_D
                || insn->opc == OPC_STLE_H || insn->opc == OPC_STLE_W)
        return false;
    reg_info res;
    std::memset(&res, 0, sizeof(reg_info));
    gen_require_func_table[insn->opc](insn, &res); 
    if(res.kill == reg)
            return true;
    return false;
}

ListNode<LoongArchInsInfo>* substitue_with_addi_insn(ListNode<LoongArchInsInfo>* node, u_int8_t insn_count, u_int8_t src_reg, int imm, TB& tb, u_int32_t* cur_index)
{
      std::cout<<"subs "<<std::hex<<tb.x86_addr<<std::endl;
      ListNode<LoongArchInsInfo>* addi_node = node;
      u_int8_t dst_reg = node->data->dst_reg;
      LoongArchInsInfo* addi_insn = node->data;

      node = node->next;
      insn_count -= 1;
      u_int32_t cur = (*cur_index) + 1;
      while(insn_count > 0)
      {
         node = tb.delete_ith_insn_alongwith_rel(node, cur);
         insn_count -= 1;
      }
      addi_node->next = node;

      memset(addi_insn, 0, sizeof(LoongArchInsInfo));
      addi_insn->opc = OPC_ADDI_D;
      addi_insn->dst_reg = dst_reg;
      addi_insn->srcs_regs[0] = src_reg;
      addi_insn->num_srcs = 1;
      addi_insn->si = imm;
      return addi_node;
}

bool can_substitue_with_addi_insn(int64_t imm)
{
        return -2048 <= imm && imm <= 2047;
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

void PeepHoleOPTVistor::visit(TB& tb)
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

void PeepHoleOPTVistor::start(Segment& seg)
{
    for(auto tb_ptr : seg._tbs)
    {
            if(tb_ptr->pipehole_available)
                visit(*tb_ptr);
    }
}
