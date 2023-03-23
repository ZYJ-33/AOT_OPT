#include "Vistor/FixBranchVistor.hpp"

const AOT_rel* find_closest_LOAD_TB_ADDR_rel_entry(TB& tb, u_int32_t branch_insn_offset)
{
    const AOT_rel* closest = nullptr;
    for(auto i=0; i<tb.rels.size(); i++)
    {
        if(tb.rels[i].rel.kind == LOAD_TB_ADDR
          &&tb.rels[i].valid
          &&branch_insn_offset < tb.rels[i].rel.tc_offset)
        {
            if(closest == nullptr || tb.rels[i].rel.tc_offset < closest->tc_offset)
              closest = &tb.rels[i].rel;
        }
    }
    return closest;
}

static void __fix_b(TB& tb, bool is_true_branch)
{
    auto branch_insn_offset = is_true_branch? tb.true_branch_offset : tb.false_branch_offset;
    LoongArchInsInfo* branch_insn = tb.dis_insns.get(branch_insn_offset>>2)->data;
    assert(branch_insn->opc == OPC_B);
   
    const AOT_rel* load_tb_addr = find_closest_LOAD_TB_ADDR_rel_entry(tb, branch_insn_offset);
    if(load_tb_addr == nullptr)
            return;

    u_int32_t numberofinsn = (load_tb_addr->tc_offset - branch_insn_offset) >> 2;
    branch_insn->offs = numberofinsn;
}

void FixBranchVistor::fix_b(TB& tb)
{
   assert(tb.false_branch == nullptr || tb.true_branch == nullptr);

    if((tb.false_branch == nullptr && tb.true_branch == nullptr)
       && (tb.true_branch_offset != TB_JMP_RESET_OFFSET_INVALID && tb.false_branch_offset != TB_JMP_RESET_OFFSET_INVALID))
    {
        __fix_b(tb, true);
        __fix_b(tb, false);
    }
    else if(tb.false_branch == nullptr && tb.false_branch_offset != TB_JMP_RESET_OFFSET_INVALID)
        __fix_b(tb, false);
    else if(tb.true_branch == nullptr && tb.true_branch_offset != TB_JMP_RESET_OFFSET_INVALID)
        __fix_b(tb, true);
}

void FixBranchVistor::fix_bne(TB& tb)
{
   assert(tb.condi_branch_offset != TB_JMP_RESET_OFFSET_INVALID);
   ListNode<LoongArchInsInfo>* condi_branch = tb.dis_insns.get(tb.condi_branch_offset);

   assert(condi_branch != nullptr && condi_branch->data->opc == OPC_BNE || condi_branch->data->opc == OPC_BEQ);

   if(condi_branch->data->opc == OPC_BNE)
        condi_branch->data->offs = tb.true_branch_offset - tb.condi_branch_offset;
   else
        condi_branch->data->offs = tb.false_branch_offset - tb.condi_branch_offset;
}

void FixBranchVistor::visit(TB& tb)
{
    if(tb.condi_branch_offset != TB_JMP_RESET_OFFSET_INVALID)
        fix_bne(tb);            //there exist a bne insn
    if( !(tb.true_branch == nullptr && tb.false_branch == nullptr)
        && (tb.true_branch == nullptr || tb.false_branch == nullptr))
        fix_b(tb);
}

void FixBranchVistor::start(Segment& seg)
{
    for(auto tb_ptr: seg._tbs)
            visit(*tb_ptr);
}
