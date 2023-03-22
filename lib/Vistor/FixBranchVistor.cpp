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
    auto branch_insn_offset = is_true_branch? tb.origin_aot_tb->jmp_target_arg[1] : tb.origin_aot_tb->jmp_target_arg[0];
    LoongArchInsInfo* branch_insn = tb.dis_insns.get(branch_insn_offset>>2)->data;
    if(!(branch_insn->opc == OPC_B))
        std::cout<<std::hex<<tb.x86_addr<<std::endl;
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
       && (tb.origin_aot_tb->jmp_reset_offsets[0] != TB_JMP_RESET_OFFSET_INVALID && tb.origin_aot_tb->jmp_reset_offsets[1] != TB_JMP_RESET_OFFSET_INVALID))
    {
        __fix_b(tb, true);
        __fix_b(tb, false);
    }
    else if(tb.false_branch == nullptr && tb.origin_aot_tb->jmp_reset_offsets[0] != TB_JMP_RESET_OFFSET_INVALID)
        __fix_b(tb, false);
    else if(tb.true_branch == nullptr && tb.origin_aot_tb->jmp_reset_offsets[1] != TB_JMP_RESET_OFFSET_INVALID)
        __fix_b(tb, true);
}

void FixBranchVistor::fix_bne(TB& tb)
{
   assert(tb.origin_aot_tb->jmp_reset_offsets[0] != TB_JMP_RESET_OFFSET_INVALID && tb.origin_aot_tb->jmp_reset_offsets[1] != TB_JMP_RESET_OFFSET_INVALID);
   
   u_int32_t false_branch_index = tb.origin_aot_tb->jmp_target_arg[0]>>2;
   u_int32_t true_branch_index = tb.origin_aot_tb->jmp_target_arg[1]>>2;
   u_int32_t cur_index = 0;
   u_int32_t bne_index = 0;
   LoongArchInsInfo* bne_insn = nullptr;

   for(auto iter = tb.dis_insns.begin(); iter != tb.dis_insns.end(); iter=iter->next)
   {
        if(iter->data->opc == OPC_BNE)
        {
            bne_insn = iter->data;
            bne_index = cur_index;
        }
        cur_index += 1;
   }
   assert(bne_insn != nullptr);
   bne_insn->offs = true_branch_index - bne_index;
}

void FixBranchVistor::visit(TB& tb)
{
    if(tb.origin_aot_tb->jmp_reset_offsets[0] != TB_JMP_RESET_OFFSET_INVALID
       && tb.origin_aot_tb->jmp_reset_offsets[1] != TB_JMP_RESET_OFFSET_INVALID)
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
