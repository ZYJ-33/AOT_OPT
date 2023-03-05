#include "Vistor/TU_Vistor.hpp"
#include "Vistor/util.hpp"
#include <cassert>
#include <set>

void get_all_entrys_block(Segment& seg, std::vector<std::shared_ptr<TB>>& result)
{
    for(auto tb_ptr: seg._tbs)
    {
        if(tb_ptr->parents.size() == 0)
                result.push_back(tb_ptr);
    }
}

bool only_one_successor(std::shared_ptr<TB> tb)
{
    return (tb->origin_aot_tb->jmp_reset_offsets[0] == TB_JMP_RESET_OFFSET_INVALID
           && tb->origin_aot_tb->jmp_reset_offsets[1] != TB_JMP_RESET_OFFSET_INVALID)
           ||(tb->origin_aot_tb->jmp_reset_offsets[0] != TB_JMP_RESET_OFFSET_INVALID
           && tb->origin_aot_tb->jmp_reset_offsets[1] == TB_JMP_RESET_OFFSET_INVALID);
}

bool no_successor(std::shared_ptr<TB> tb)
{
    return (tb->origin_aot_tb->jmp_reset_offsets[0] == TB_JMP_RESET_OFFSET_INVALID
           && tb->origin_aot_tb->jmp_reset_offsets[1] == TB_JMP_RESET_OFFSET_INVALID);
}

std::shared_ptr<TB> get_the_onlyone_successor(std::shared_ptr<TB> tb)
{
    if(!only_one_successor(tb))
        return nullptr;
    return tb->false_branch != nullptr? tb->false_branch:tb->true_branch;
}

std::shared_ptr<TB> unify_srcblock_no_successor(std::shared_ptr<TB> dstblock, std::shared_ptr<TB> srcblock, bool is_true_branch)
{
    std::cout<<"0x"<<std::hex<<dstblock->x86_addr<<" unify "<<"0x"<<std::hex<<srcblock->x86_addr<<std::endl;
    if(!no_successor(srcblock))
            return nullptr;
    
    u_int32_t delete_start_index = branch_insn_index(dstblock, is_true_branch);
    u_int32_t delete_end_index = dstblock->dis_insns.total_size()-1;
    /*
    if(is_true_branch)
            delete_end_index = dstblock->dis_insns.total_size()-1;
    else
            delete_end_index = branch_insn_index(dstblock, true)-1;
    */
    
    u_int32_t count = delete_end_index - delete_start_index + 1;
    u_int32_t new_base_offset = delete_start_index * 4;

    ListNode<LoongArchInsInfo>* cur_insn = dstblock->dis_insns.get(delete_start_index);
    ListNode<LoongArchInsInfo>* cur_unify = cur_insn->prev;

    assert(cur_insn->data->opc == OPC_B);
    while(count > 0)
    {
        cur_insn = dstblock->delete_ith_insn_alongwith_rel(cur_insn, delete_start_index);
        count -= 1;
    }

    for(u_int32_t i=0; i < srcblock->rels.size(); i++)
    {
        if(srcblock->rels_valid[i])
        {
            dstblock->rels_valid.push_back(true);
            dstblock->rels.push_back(srcblock->rels[i]);
            u_int32_t dst_rel_last_index = dstblock->rels.size()-1;
            dstblock->rels[dst_rel_last_index].tc_offset += new_base_offset;
        }
    }
    
    u_int32_t cur_index = delete_start_index;
    for(auto iter = srcblock->dis_insns.begin(); iter != srcblock->dis_insns.end(); iter=iter->next, cur_index++)
    {
        LoongArchInsInfo* new_insn = (LoongArchInsInfo*)std::malloc(sizeof(LoongArchInsInfo));
        std::memcpy(new_insn, iter->data, sizeof(LoongArchInsInfo));
        ListNode<LoongArchInsInfo>* new_node = new ListNode<LoongArchInsInfo>(new_insn);

        
        dstblock->dis_insns.insert_after(cur_unify, new_node);
        cur_unify = new_node;
        check_and_add_branch_insn(*dstblock, new_insn, cur_index);
    }
    
    if(srcblock->origin_aot_tb->jmp_reset_offsets[0] != TB_JMP_RESET_OFFSET_INVALID)
    {
      dstblock->origin_aot_tb->jmp_reset_offsets[0] = new_base_offset + srcblock->origin_aot_tb->jmp_reset_offsets[0];
      dstblock->origin_aot_tb->jmp_target_arg[0] = new_base_offset + srcblock->origin_aot_tb->jmp_target_arg[0];
    }
    else
        dstblock->origin_aot_tb->jmp_reset_offsets[0] = TB_JMP_RESET_OFFSET_INVALID;

    if(srcblock->origin_aot_tb->jmp_reset_offsets[1] != TB_JMP_RESET_OFFSET_INVALID)
    {
      dstblock->origin_aot_tb->jmp_reset_offsets[1] = new_base_offset + srcblock->origin_aot_tb->jmp_reset_offsets[1];
      dstblock->origin_aot_tb->jmp_target_arg[1] = new_base_offset + srcblock->origin_aot_tb->jmp_target_arg[1];
    }
    else
        dstblock->origin_aot_tb->jmp_reset_offsets[1] = TB_JMP_RESET_OFFSET_INVALID;

    dstblock->true_branch = srcblock->true_branch;
    dstblock->false_branch = srcblock->false_branch;
    return dstblock;
}

void unify_one_successor(std::shared_ptr<TB> cur, std::set<u_int64_t>& parents, std::set<u_int64_t>& finished)
{
    if(parents.find(cur->x86_addr) != parents.end() || finished.find(cur->x86_addr) != finished.end())
            return;

    if(only_one_successor(cur))
    {
        std::shared_ptr<TB> succ = get_the_onlyone_successor(cur); 
        if(no_successor(succ))
        {
                bool is_true_branch = cur->origin_aot_tb->jmp_reset_offsets[1] != TB_JMP_RESET_OFFSET_INVALID ? true:false;
                unify_srcblock_no_successor(cur, succ, is_true_branch);
                return; 
        }
        else if(only_one_successor(succ))
        {
        }
        else
        {
        }
    }
    parents.insert(cur->x86_addr);
    if(cur->false_branch != nullptr)
            unify_one_successor(cur->false_branch, parents, finished);
    if(cur->true_branch != nullptr)
            unify_one_successor(cur->true_branch, parents, finished);
    parents.erase(cur->x86_addr);
    finished.insert(cur->x86_addr);
}

void iter_all(std::shared_ptr<TB> tb, std::set<u_int64_t>& record)
{
    if(record.find(tb->x86_addr) == record.end())
    {
        record.insert(tb->x86_addr);
        if(tb->true_branch != nullptr)
                iter_all(tb->true_branch, record);
        if(tb->false_branch != nullptr)
                iter_all(tb->false_branch, record);
    }
}

void iter_can_be_tu(std::shared_ptr<TB> tb, std::set<u_int64_t>& parents, std::set<u_int64_t>& finished, std::vector<u_int64_t>& res)
{
    if(parents.find(tb->x86_addr) == parents.end() && finished.find(tb->x86_addr) == finished.end())
    {
        parents.insert(tb->x86_addr);
        
        if(only_one_successor(tb))
                res.push_back(tb->x86_addr);
        if(tb->true_branch != nullptr)
                iter_can_be_tu(tb->true_branch, parents, finished, res);
        if(tb->false_branch != nullptr)
                iter_can_be_tu(tb->false_branch, parents, finished, res);
        
        parents.erase(tb->x86_addr);
        finished.insert(tb->x86_addr);
    }
}

void output_tb_culster(Segment& seg)
{
    std::vector<std::shared_ptr<TB>> entry_blocks;
    get_all_entrys_block(seg, entry_blocks);
    for(auto tb_ptr : entry_blocks)
    {
        std::set<u_int64_t> result;
        iter_all(tb_ptr, result);
        std::cout<<"0x"<<std::hex<<tb_ptr->x86_addr<<std::endl;
        std::cout<<"{"<<std::endl;
        for(auto addr: result)
            std::cout<<"0x"<<std::hex<<addr<<std::endl;
        std::cout<<"}"<<std::endl;
    }
}

void output_tb_can_be_unify(Segment& seg)
{
    std::vector<std::shared_ptr<TB>> entry_blocks;
    get_all_entrys_block(seg, entry_blocks);
    for(auto tb_ptr : entry_blocks)
    {
        std::set<u_int64_t> parents;
        std::set<u_int64_t> finished;
        std::vector<u_int64_t> res;
        iter_can_be_tu(tb_ptr, parents, finished, res);
        std::cout<<"0x"<<std::hex<<tb_ptr->x86_addr<<std::endl;
        std::cout<<"{"<<std::endl;
        for(auto addr: res)
            std::cout<<"0x"<<std::hex<<addr<<std::endl;
        std::cout<<"}"<<std::endl;
    }

}

void TU_Vistor::visit(TB& tb)
{}

void TU_Vistor::start(Segment& seg)
{
    std::vector<std::shared_ptr<TB>> entry_blocks;
    get_all_entrys_block(seg, entry_blocks);
    for(auto tb_ptr : entry_blocks)
    {
        std::set<u_int64_t> parents;
        std::set<u_int64_t> finished;
        unify_one_successor(tb_ptr, parents, finished);
    }
}
