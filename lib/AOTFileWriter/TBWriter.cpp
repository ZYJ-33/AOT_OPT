#include "AOTFileWriter/TBWriter.hpp"
#include "Vistor/AssembleVistor.hpp"
#include <cstring>

static u_int32_t rel_cur_index = 0;

void TBWriter::handle_rels(std::shared_ptr<TB> tb_ptr, AOT_rel* rel_buf, AOT_TB* target_tb, AOT_Header* hdr)
{
    //TODO figure out what to do if there is no valid rel
    bool any_valid = true;
    for(auto valid: tb_ptr->rels_valid)
        any_valid |= valid;
    if(!any_valid)
        return;
    
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

void TBWriter::handle_assemble(std::shared_ptr<TB> tb_ptr, u_int32_t* insn_ptr, AOT_TB* target_tb, AOT_Header* hdr)
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

void TBWriter::write_to_buf(AOT_Header* hdr, AOT_Segment* seg, AOT_TB* tb_buf, AOT_rel* rel_buf, char* code_buf, std::vector<std::shared_ptr<TB>>& tbs)
{
    AOT_TB* cur = tb_buf;
    u_int32_t* insn_ptr = (u_int32_t*) code_buf;
        
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
}
