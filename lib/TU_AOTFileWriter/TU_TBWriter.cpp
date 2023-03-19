#include "TU_AOTFileWriter/TU_TBWriter.hpp"
#include "Vistor/AssembleVistor.hpp"
#include <cstring>
#include <algorithm>
#include <map>
static u_int32_t TU_rel_cur_index = 0;
std::map<u_int64_t, u_int64_t> TU_TBWriter::x86addr_to_codecache_offset = std::map<u_int64_t, u_int64_t>(); 
   
void fix_offset_of_bne(std::shared_ptr<TB> tb)
{
    u_int16_t true_branch_offset = tb->true_branch_offset;
    u_int16_t false_branch_offset = tb->false_branch_offset;
    u_int16_t cur_index = 0;
    int32_t last_bne_index = -1;
    LoongArchInsInfo* last_bne_insn_ptr = nullptr;

    for(auto iter=tb->dis_insns.begin(); iter!=tb->dis_insns.end(); iter = iter->next)
    {
        if(iter->data->opc == OPC_BNE)
        {
            last_bne_index = cur_index;
            last_bne_insn_ptr = iter->data;
        }
        cur_index += 1;
    }

    if(last_bne_insn_ptr != nullptr)
        last_bne_insn_ptr->offs = true_branch_offset - last_bne_index;
}

void TU_TBWriter::handle_rels(std::shared_ptr<TB> tb_ptr, AOT_rel* rel_buf, AOT_TB* target_tb, AOT_Header* hdr)
{
    bool any_valid = true;
    for(auto valid: tb_ptr->rels_valid)
        any_valid |= valid;
    if(!any_valid)
    {
        target_tb->rel_start_index = 1;
        target_tb->rel_end_index = 0;
        return;
    }

    target_tb->rel_start_index = TU_rel_cur_index;
    for(u_int32_t i=0; i<tb_ptr->rels.size(); i++)
    {
        if(tb_ptr->rels_valid[i])
        {
            if(TU_rel_cur_index == 0)
                hdr->reltable_offset = ((uintptr_t)rel_buf - (uintptr_t)hdr);

            memcpy((void*)rel_buf, (void*)&tb_ptr->rels[i], sizeof(AOT_rel));
            TU_rel_cur_index += 1;
            rel_buf += 1; 
        }
    }
    target_tb->rel_end_index = TU_rel_cur_index - 1;
}

void fix_offset_of_beqtype(std::shared_ptr<TB> tb_ptr, AOT_TB* target_tb)
{
    assert(tb_ptr->tbtype == TU_BEQ_TYPE);
    assert(tb_ptr->false_branch_offset > tb_ptr->true_branch_offset);
    
    int32_t i = tb_ptr->true_branch_offset;
    ListNode<LoongArchInsInfo>* cur_insn = tb_ptr->dis_insns.get(i);
    LoongArchInsInfo* beq_insn = nullptr;
    u_int32_t beq_index = 0;
    while(i >= 0)
    {
        if(cur_insn->data->opc == OPC_BEQ)
        {
            beq_insn = cur_insn->data;
            beq_index = i;
            break;
        }
        i -= 1;
        cur_insn = cur_insn->prev;
    }
    assert(beq_insn != nullptr);
    
    beq_insn->offs = tb_ptr->false_branch_offset - beq_index;
    target_tb->tu_jmp[0] = tb_ptr->false_branch_offset*4;
    target_tb->tu_jmp[1] = beq_index*4;

}

static void write_tb_helper_function(std::shared_ptr<TB> tb_ptr, AOT_TB* target_tb)
{
        const AOT_TB* aot_tb = tb_ptr->origin_aot_tb;
        memcpy(target_tb, aot_tb, sizeof(AOT_TB));
        target_tb->tb_cache_offset = 0;
        target_tb->tb_cache_size = 0;
        target_tb->rel_start_index = 0;
        target_tb->rel_end_index = 0;
        target_tb->tu_jmp[0] = TB_JMP_RESET_OFFSET_INVALID;
        target_tb->tu_jmp[1] = TB_JMP_RESET_OFFSET_INVALID;
        target_tb->jmp_reset_offsets[0] = TB_JMP_RESET_OFFSET_INVALID;
        target_tb->jmp_reset_offsets[1] = TB_JMP_RESET_OFFSET_INVALID;
        if(tb_ptr->tbtype == NORMAL_BNE_TYPE)
        {
            fix_offset_of_bne(tb_ptr);
        }
        switch(tb_ptr->tbtype)
        {
            case NORMAL_BNE_TYPE:
            case NORMAL_B_TRUE_TYPE:
            case NORMAL_B_FALSE_TYPE:
            case NONE_B_TYPE:
            {
                target_tb->jmp_reset_offsets[0] = aot_tb->jmp_reset_offsets[0];
                target_tb->jmp_reset_offsets[1] = aot_tb->jmp_reset_offsets[1];
                if(aot_tb->jmp_reset_offsets[0] != TB_JMP_RESET_OFFSET_INVALID)
                    target_tb->jmp_target_arg[0] = tb_ptr->false_branch_offset << 2;
                if(aot_tb->jmp_reset_offsets[1] != TB_JMP_RESET_OFFSET_INVALID)
                    target_tb->jmp_target_arg[1] = tb_ptr->true_branch_offset << 2;
                break;
            }
            case TU_BEQ_TYPE:
                   fix_offset_of_beqtype(tb_ptr, target_tb);
                   break;
            case TU_B_TRUE_TYPE:
            case TU_B_FALSE_TYPE:
            case TU_BNE_TYPE:
                {
                    if(aot_tb->tu_jmp[0] != TB_JMP_RESET_OFFSET_INVALID)
                        target_tb->tu_jmp[0] = tb_ptr->false_branch_offset << 2;
                    if(aot_tb->tu_jmp[1] != TB_JMP_RESET_OFFSET_INVALID)
                        target_tb->tu_jmp[1] = tb_ptr->true_branch_offset << 2;
                    break;
                }
            case UNKNOW_TB_TYPE:
                std::cerr<<"unknow tb type found in write_tb_helper_function pc:0x"<<std::hex<<tb_ptr->x86_addr<<std::endl;
                exit(1);
        }
}

void TU_TBWriter::handle_assemble(std::shared_ptr<TB> tb_ptr, u_int32_t* insn_ptr, AOT_TB* target_tb, AOT_Header* hdr)
{
        auto exist = TU_TBWriter::x86addr_to_codecache_offset.find(tb_ptr->x86_addr);
        if(exist != x86addr_to_codecache_offset.end())
        {
            std::cerr<<"x86_addr 0x"<<tb_ptr->x86_addr<<" already been dumped before"<<std::endl;
            exit(1);
        }
        x86addr_to_codecache_offset[tb_ptr->x86_addr] = (u_int64_t) insn_ptr;

        target_tb->tb_cache_offset = (uintptr_t)insn_ptr - (uintptr_t)hdr;
        target_tb->tb_cache_size = tb_ptr->dis_insns.total_size() * 4;
        for(auto iter = tb_ptr->dis_insns.begin(); iter != tb_ptr->dis_insns.end(); iter = iter->next)
        {
            u_int32_t insn = AssmbleVistor::tocode(iter->data);
            *insn_ptr = insn;
            insn_ptr += 1;
        }
}

bool tb_cmp(const std::shared_ptr<TB>& a, const std::shared_ptr<TB>& b)
{
    if(a->x86_addr < b->x86_addr)
            return true;
    return false;
}

void checking(std::vector<std::shared_ptr<TB>>& tbs)
{
    for(int i=0; i<tbs.size()-1; i++)
    {
        auto tb = tbs[i];
        auto tb_next = tbs[i+1];
        if(tb->tbtype == TU_BNE_TYPE || tb->tbtype == TU_B_FALSE_TYPE || tb->tbtype == TU_B_TRUE_TYPE)
        {
            if(tb->false_branch->x86_addr != tb_next->x86_addr)
            {
                if(tb_next->false_branch->x86_addr == tbs[i+2]->x86_addr)
                {
                    LoongArchInsInfo* fix_branch_insn = new LoongArchInsInfo();
                    memset(fix_branch_insn, 0, sizeof(LoongArchInsInfo));
                    fix_branch_insn->opc = OPC_B;
                    fix_branch_insn->offs = tb_next->dis_insns.total_size();
                    tb->dis_insns.insert_at_tail(fix_branch_insn);
                }
                else
                {
                    std::cerr<<"warning in dump aot checking";
                    std::cerr<<std::hex<<tb->x86_addr<<" next_x86 should be "<<std::hex<<tb->false_branch->x86_addr;
                    std::cerr<<"but dumping "<<std::hex<<tb_next->x86_addr<<" -> "<<tb_next->false_branch->x86_addr<<std::hex<<std::endl;
                }
            }
        }
    }
}

//void TU_TBWriter::handle_assemble(std::shared_ptr<TB> tb_ptr, u_int32_t* insn_ptr, AOT_TB* target_tb, AOT_Header* hdr)
void TU_TBWriter::write_to_buf(AOT_Header* hdr, AOT_Segment* seg, AOT_TB* tb_buf, AOT_rel* rel_buf, char* code_buf, std::vector<std::shared_ptr<TB>>& tbs)
{
    std::sort(tbs.begin(), tbs.end(), tb_cmp);
    checking(tbs);
    
    AOT_TB* cur = tb_buf;
    u_int32_t* insn_ptr = (u_int32_t*) code_buf;
    
    for(auto tb_ptr : tbs)
    {
        write_tb_helper_function(tb_ptr, cur);
        handle_rels(tb_ptr, rel_buf, cur, hdr);
        handle_assemble(tb_ptr, insn_ptr, cur, hdr);      
        cur += 1;
        insn_ptr += tb_ptr->dis_insns.total_size();
        rel_buf += tb_ptr->how_many_rel();
    }

    hdr->reltable_num = TU_rel_cur_index;
    handle_linking(tbs, (u_int32_t*)code_buf);
}

#define B_INSN_OFFSET_UPPER_BOUND ((1 << 25) - 1)
#define B_INSN_OFFSET_LOWER_BOUND (-1 * (1 << 25) )
#define BNE_BEQ_INSN_OFFSET_UPPER_BOUND ((1 << 15) - 1)
#define BNE_BEQ_INSN_OFFSET_LOWER_BOUND (-1 * (1 << 15))
void TU_TBWriter::__linking(std::shared_ptr<TB> tb, u_int32_t* branch_insn_ptr, u_int32_t* target_code_cache)
{
    LoongArchInsInfo tmp;
    memset(&tmp, 0, sizeof(tmp));
    int64_t offset = ((int64_t)target_code_cache - (int64_t)branch_insn_ptr)>>2;

    u_int32_t insn = *branch_insn_ptr;
    decode(&tmp, insn);
    switch(tmp.opc)
    {
        case OPC_B:
        {
           assert(B_INSN_OFFSET_LOWER_BOUND <= offset && offset <= B_INSN_OFFSET_UPPER_BOUND);
           tmp.offs = offset;
           break;
        }
        case OPC_BNE:
        case OPC_BEQ:
        {
            assert(BNE_BEQ_INSN_OFFSET_LOWER_BOUND <= offset && offset <= BNE_BEQ_INSN_OFFSET_UPPER_BOUND);
            tmp.offs = offset;
            break;
        }
        default:
        {
            std::cerr<<"unknow branch type found when linking x86_addr:0x"<<std::hex<<tb->x86_addr<<std::endl;
            exit(1);
        }
    }
    insn = encode(&tmp);
    *branch_insn_ptr = insn;
}

void TU_TBWriter::handle_linking(std::vector<std::shared_ptr<TB>>& tbs, u_int32_t* code_buf)
{
    for(auto tb_ptr: tbs)
    {
       if(tb_ptr->tbtype == NORMAL_BNE_TYPE || tb_ptr->tbtype == NORMAL_B_FALSE_TYPE || tb_ptr->tbtype == NORMAL_B_TRUE_TYPE || tb_ptr->tbtype == NONE_B_TYPE)
       {
               u_int64_t cur_code_cache = TU_TBWriter::x86addr_to_codecache_offset[tb_ptr->x86_addr];
               if(tb_ptr->tbtype == NORMAL_BNE_TYPE)
               {
                    if(tb_ptr->false_branch != nullptr)
                    {
                        assert(tb_ptr->false_branch_offset != TB_JMP_RESET_OFFSET_INVALID);
                        u_int32_t* branch_insn_ptr = (u_int32_t*)(cur_code_cache) + tb_ptr->false_branch_offset;
                        u_int32_t* target_code_cache = (u_int32_t*)TU_TBWriter::x86addr_to_codecache_offset[tb_ptr->false_branch->x86_addr];
                        __linking(tb_ptr, branch_insn_ptr, target_code_cache);
                        
                    }
                    if(tb_ptr->true_branch != nullptr)
                    {
                        assert(tb_ptr->true_branch_offset != TB_JMP_RESET_OFFSET_INVALID);
                        u_int32_t* branch_insn_ptr = (u_int32_t*)(cur_code_cache) + tb_ptr->true_branch_offset;
                        u_int32_t* target_code_cache = (u_int32_t*)TU_TBWriter::x86addr_to_codecache_offset[tb_ptr->true_branch->x86_addr];
                        __linking(tb_ptr, branch_insn_ptr, target_code_cache);
                    }
               }
               else if(tb_ptr->tbtype == NORMAL_B_TRUE_TYPE)
               {
                    if(tb_ptr->true_branch != nullptr)
                    {
                        assert(tb_ptr->true_branch_offset != TB_JMP_RESET_OFFSET_INVALID);
                        u_int32_t* branch_insn_ptr = (u_int32_t*)(cur_code_cache) + tb_ptr->true_branch_offset;
                        u_int32_t* target_code_cache = (u_int32_t*)TU_TBWriter::x86addr_to_codecache_offset[tb_ptr->true_branch->x86_addr];
                        __linking(tb_ptr, branch_insn_ptr, target_code_cache);
                    }
               }
               else if(tb_ptr->tbtype == NORMAL_B_FALSE_TYPE)
               {
                    if(tb_ptr->true_branch != nullptr)
                    {
                        assert(tb_ptr->false_branch_offset != TB_JMP_RESET_OFFSET_INVALID);
                        u_int32_t* branch_insn_ptr = (u_int32_t*)(cur_code_cache) + tb_ptr->false_branch_offset;
                        u_int32_t* target_code_cache = (u_int32_t*)TU_TBWriter::x86addr_to_codecache_offset[tb_ptr->true_branch->x86_addr];
                        __linking(tb_ptr, branch_insn_ptr, target_code_cache);
                    }
               }
       }
       else if(tb_ptr->tbtype == TU_BNE_TYPE || tb_ptr->tbtype == TU_BEQ_TYPE || tb_ptr->tbtype == TU_B_FALSE_TYPE || tb_ptr->tbtype == TU_B_TRUE_TYPE)
       {
               u_int64_t cur_code_cache = TU_TBWriter::x86addr_to_codecache_offset[tb_ptr->x86_addr];
               if(tb_ptr->true_branch_offset != TB_JMP_RESET_OFFSET_INVALID && tb_ptr->true_branch != nullptr)
               {
                    u_int32_t* branch_insn_ptr = (u_int32_t*)(cur_code_cache) + tb_ptr->true_branch_offset;
                    u_int32_t* target_code_cache = (u_int32_t*)TU_TBWriter::x86addr_to_codecache_offset[tb_ptr->true_branch->x86_addr];
                    __linking(tb_ptr, branch_insn_ptr, target_code_cache); 
               }
               if(tb_ptr->false_branch_offset != TB_JMP_RESET_OFFSET_INVALID && tb_ptr->false_branch != nullptr)
               {
                    u_int32_t* branch_insn_ptr = (u_int32_t*)(cur_code_cache) + tb_ptr->false_branch_offset;
                    u_int32_t* target_code_cache = (u_int32_t*)TU_TBWriter::x86addr_to_codecache_offset[tb_ptr->false_branch->x86_addr];
                    __linking(tb_ptr, branch_insn_ptr, target_code_cache); 
               }
       }
       else
       {
            std::cerr<<"found UNKNOW TB TYPE in handle_linking 0x"<<std::hex<<tb_ptr->x86_addr<<std::endl;
            exit(1);
       }
    }
}
