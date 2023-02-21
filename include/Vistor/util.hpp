#ifndef VISTOR_UTIL
#define VISTOR_UTIL
#include "Disassmbler/disassmbler.hpp"
#include "LinkList/linklist.hpp"
#include "Basic/aot.hpp"
#include "AOT_class/aot_class.hpp"
#include <cstring>
void print_one_insn(LoongArchInsInfo& insn);
void print_all_insn(LinkList<LoongArchInsInfo>& insns);
int64_t simulate_lu12i_w(int32_t si);
int64_t simulate_ori(int64_t reg, u_int64_t imm);
int64_t simulate_lu32i_d(int64_t reg, int32_t si);
bool is_reg_killed(LoongArchInsInfo* insn, u_int8_t reg);
bool can_substitue_with_addi_insn(int64_t imm);
ListNode<LoongArchInsInfo>* substitue_with_addi_insn(ListNode<LoongArchInsInfo>* node, u_int8_t insn_count, u_int8_t src_reg, int imm, TB& tb, u_int32_t* cur_index);
bool can_substitue_with_addi_insn(int64_t imm);
#endif
