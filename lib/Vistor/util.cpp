#include "Vistor/util.hpp"

extern int insn2format[];
extern void(*print_func_table[])(std::ostream&, LoongArchInsInfo*);
extern void (*gen_require_func_table[])(LoongArchInsInfo*, reg_info*);


bool can_substitue_with_addi_insn(int64_t imm)
{
        return -2048 <= imm && imm <= 2047;
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


void print_one_insn(LoongArchInsInfo& insn)
{
      if(insn.opc != OPC_INVALID)
      {
           int format = insn2format[insn.opc];
           auto print_func = print_func_table[format];
           (*print_func)(std::cout, &insn);
      }
      else
          std::cout<<"Invalid insn\n";
}

void print_all_insn(LinkList<LoongArchInsInfo>& insns)
{
    for(auto iter=insns.begin(); iter != insns.end(); )
    {
        print_one_insn(*(iter->data));
        iter = iter->next;
    }
}
