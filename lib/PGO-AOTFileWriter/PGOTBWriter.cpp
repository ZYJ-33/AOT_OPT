#include "PGO-AOTFileWriter/PGOTBWriter.hpp"
#include "Vistor/AssembleVistor.hpp"
#include <cstring>
#include <set>

static u_int32_t rel_cur_index = 0;

void PGOTBWriter::handle_rels(std::shared_ptr<TB> tb_ptr, AOT_rel* rel_buf, AOT_TB* target_tb, AOT_Header* hdr)
{
    //TODO figure out what to do if there is no valid rel
    bool any_valid = true;
    for(auto valid: tb_ptr->rels_valid)
        any_valid |= valid;
    if(!any_valid)
    {
        target_tb->rel_start_index = 1;
        target_tb->rel_end_index = 0;
        return;
    }

    target_tb->rel_start_index = rel_cur_index;
    for(u_int32_t i=0; i<tb_ptr->rels.size(); i++)
    {
        if(tb_ptr->rels_valid[i])
        {
            if(rel_cur_index == 0)
                hdr->reltable_offset = ((uintptr_t)rel_buf - (uintptr_t)hdr);

            memcpy((void*)rel_buf, (void*)&tb_ptr->rels[i], sizeof(AOT_rel));
            rel_cur_index += 1;
            rel_buf += 1; 
        }
    }
    target_tb->rel_end_index = rel_cur_index - 1;
}

static void write_tb_helper_function(std::shared_ptr<TB> tb_ptr, AOT_TB* target_tb)
{
        const AOT_TB* origin_tb = tb_ptr->origin_aot_tb;
        memcpy(target_tb, origin_tb, sizeof(AOT_TB));
        target_tb->tb_cache_offset = 0;
        target_tb->tb_cache_size = 0;
        target_tb->rel_start_index = 0;
        target_tb->rel_end_index = 0;
}

void PGOTBWriter::handle_assemble(std::shared_ptr<TB> tb_ptr, u_int32_t* insn_ptr, AOT_TB* target_tb, AOT_Header* hdr)
{
        
        target_tb->tb_cache_offset = (uintptr_t)insn_ptr - (uintptr_t)hdr;
        target_tb->tb_cache_size = tb_ptr->dis_insns.total_size() * 4;
        for(auto iter = tb_ptr->dis_insns.begin(); iter != tb_ptr->dis_insns.end(); iter = iter->next)
        {
            u_int32_t insn = AssmbleVistor::tocode(iter->data);
            *insn_ptr = insn;
            insn_ptr += 1;
        }
}

void set_trace(std::vector<std::shared_ptr<TB>>& res, std::set<u_int64_t> seen, std::shared_ptr<TB> tb)
{
    seen.insert(tb->x86_addr);
    res.push_back(tb);
    if(tb->true_branch != nullptr && tb->false_branch != nullptr)
    {
        auto max_exec_branch = tb->max_exec_branch();
        if(seen.find(max_exec_branch->x86_addr) != seen.end())
            set_trace(res, seen, max_exec_branch);
        auto other_branch = tb->other_branch(max_exec_branch);
        if(seen.find(other_branch->x86_addr) != seen.end())
            set_trace(res, seen, other_branch);
    }
    else if(tb->true_branch != nullptr 
    && seen.find(tb->true_branch->x86_addr) != seen.end())
        set_trace(res, seen, tb->true_branch);
    else if(tb->false_branch != nullptr
    && seen.find(tb->false_branch->x86_addr) != seen.end())
        set_trace(res, seen, tb->false_branch);
}

void sort_TBS(std::vector<std::shared_ptr<TB>>& tbs)
{
     std::set<u_int64_t> visited;
     std::vector<std::shared_ptr<TB>> res;
     res.reserve(tbs.size());

     for(auto tb_ptr : tbs)
     {
        if(tb_ptr->is_entry_block() 
        && visited.find(tb_ptr->x86_addr) != visited.end())
            set_trace(res, visited, tb_ptr);      
     }

     for(auto tb_ptr : tbs)
     {
         if(visited.find(tb_ptr->x86_addr) == visited.end())
         {
                res.push_back(tb_ptr);
                visited.insert(tb_ptr->x86_addr);
         }
     }
     
     tbs.clear();
     for(auto tb_ptr: res)
        tbs.push_back(tb_ptr);
}

void handle_linking(u_int32_t* code_buf)
{
    
}

void PGOTBWriter::write_to_buf(AOT_Header* hdr, AOT_Segment* seg, AOT_TB* tb_buf, AOT_rel* rel_buf, char* code_buf, std::vector<std::shared_ptr<TB>>& tbs)
{
    AOT_TB* cur = tb_buf;
    u_int32_t* insn_ptr = (u_int32_t*) code_buf;
    
    sort_TBS(tbs);
    for(auto tb_ptr : tbs)
    {
        write_tb_helper_function(tb_ptr, cur);
        handle_assemble(tb_ptr, insn_ptr, cur, hdr);
        handle_rels(tb_ptr, rel_buf, cur, hdr);

        cur += 1;
        insn_ptr += tb_ptr->dis_insns.total_size();
        rel_buf += tb_ptr->how_many_rel();
    }
    hdr->reltable_num = rel_cur_index; 

    //handle_linking((u_int32_t)code_buf);   
}
