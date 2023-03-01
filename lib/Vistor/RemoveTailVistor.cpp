#include "Vistor/RemoveTailVistor.hpp"

void RemoveTailVistor::remove_tail_of(TB& tb, uint which)
{
    assert(which == 0 || which == 1);

    int32_t branch_insn_index = tb.origin_aot_tb->jmp_target_arg[which]>>2;
    int32_t total_insn_count = tb.dis_insns.total_size();
    int remain = -1;
    if(which == 0)
    {
         if(tb.origin_aot_tb->jmp_reset_offsets[1] != TB_JMP_RESET_OFFSET_INVALID)
                 remain = (tb.origin_aot_tb->jmp_target_arg[1]>>2)-branch_insn_index-1;
         else
                 remain = total_insn_count - branch_insn_index - 1;
    }
    else
        remain = total_insn_count - branch_insn_index - 1;

    ListNode<LoongArchInsInfo>* insn = tb.dis_insns.get(branch_insn_index);
    assert(insn->data->opc == OPC_B);

    int32_t i = branch_insn_index;
    if(( which ? tb.true_branch : tb.false_branch) != nullptr)
    {
        ListNode<LoongArchInsInfo>* go = insn->next;
        i += 1;

        while(remain > 0)
        {
            go = tb.delete_ith_insn_alongwith_rel(go, i);
            remain -= 1;
        }
    }
    else
        return;
}

void RemoveTailVistor::start(Segment& seg)
{
    for(auto tb_ptr: seg._tbs)
        tb_ptr->visit(*this);
}

void RemoveTailVistor::visit(TB& tb)
{
    if((tb.origin_aot_tb->jmp_reset_offsets[0] != TB_JMP_RESET_OFFSET_INVALID
       && tb.origin_aot_tb->jmp_reset_offsets[1] != TB_JMP_RESET_OFFSET_INVALID)
       &&(tb.true_branch != nullptr && tb.false_branch != nullptr))
    {
       remove_tail_of(tb, 0);
       remove_tail_of(tb, 1);
    }
    else if(tb.origin_aot_tb->jmp_reset_offsets[0] != TB_JMP_RESET_OFFSET_INVALID && tb.false_branch != nullptr)
        remove_tail_of(tb, 0);
    else if(tb.origin_aot_tb->jmp_reset_offsets[1] != TB_JMP_RESET_OFFSET_INVALID && tb.true_branch != nullptr)
        remove_tail_of(tb, 1);
    else
    {}
}

