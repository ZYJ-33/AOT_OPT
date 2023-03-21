#include"Basic/aot.hpp"
#include"AOT_class/aot_class.hpp"
#include"Basic/util.hpp"
#include"Disassmbler/disassmbler.hpp"
#include<cstring>
#include<cstdlib>
#include<climits>
#include<cassert>
#include<cstdio>
#include"Vistor/TB_Vistor.hpp"

std::map<u_int64_t, std::shared_ptr<TB>> x86AddrToTb;

TB::TB()
{
}

void TB::visit(TB_Vistor& tb)
{
    tb.visit(*this);
}

bool TB::is_entry_block()
{
    return parents.size() == 0;
}

u_int32_t TB::true_branch_exec_count()
{
    if(this->true_branch != nullptr)
            return 1;
    return 0;
}

u_int32_t TB::false_branch_exec_count()
{
    if(this->false_branch != nullptr)
             return 1;
    return 0;
}

void TB::convert_from_bne_to_beq()
{
     assert(condi_branch_offset != TB_JMP_RESET_OFFSET_INVALID
            && true_branch_offset != TB_JMP_RESET_OFFSET_INVALID
            && false_branch_offset != TB_JMP_RESET_OFFSET_INVALID);

     ListNode<LoongArchInsInfo>* condi_jmp = dis_insns.get(condi_branch_offset);
     LoongArchInsInfo* bne_insn = condi_jmp->data;
     assert(bne_insn->opc == OPC_BNE);
     LoongArchInsInfo* beq_insn = bne_insn;

     ListNode<LoongArchInsInfo>* true_branch_insn = dis_insns.get(true_branch_offset);
     ListNode<LoongArchInsInfo>* false_branch_insn = dis_insns.get(false_branch_offset);
     
     assert(true_branch_insn->data->opc == OPC_B && false_branch_insn->data->opc == OPC_B);
     ListNode<LoongArchInsInfo>* lower = false_branch_offset < true_branch_offset? false_branch_insn: true_branch_insn;
     ListNode<LoongArchInsInfo>* higher = false_branch_offset > true_branch_offset? false_branch_insn: true_branch_insn;
     
     ListNode<LoongArchInsInfo>* middle_part = lower->next;
     ListNode<LoongArchInsInfo>* tail_part = higher->next;

     middle_part->prev = higher;
     tail_part->prev = lower;

     lower->next = tail_part;
     higher->next = middle_part;

     u_int16_t tmp = true_branch_offset;
     true_branch_offset = false_branch_offset;
     false_branch_offset = tmp;
     
     beq_insn->opc = OPC_BEQ;
     beq_insn->offs = false_branch_offset - condi_branch_offset;
}

std::shared_ptr<TB> TB::max_exec_branch()
{
    auto true_count = this->true_branch_exec_count();
    auto false_count = this->false_branch_exec_count();
    if(true_count > false_count)
            return true_branch;
    return false_branch;
}

std::shared_ptr<TB> TB::other_branch(std::shared_ptr<TB> branch)
{
    assert(branch != nullptr);
    assert(branch == true_branch || branch == false_branch);
    if(branch == true_branch)
            return false_branch;
    return true_branch;
}

bool TB::in_scope_of_rel(u_int32_t start, u_int32_t count)
{
    u_int32_t end_index = start + count - 1;
    for(u_int32_t i=0; i<rels.size(); i++)
    {
        if(rels_valid[i])
        {
            u_int32_t rel_start_index = rels[i].tc_offset/4;
            u_int32_t rel_end_index = rel_start_index + rels[i].rel_slots_num - 1;
            if(!(end_index < rel_start_index || rel_end_index < start))
                    return true;
        }
    }
    return false;
}

void __add_b_insn(TB& tb, LoongArchInsInfo* node, u_int32_t index)
{
    if(node->opc == OPC_B)
            tb.b_insns.emplace_back(node, index);
    else if(node->opc == OPC_BNE)
            tb.bne_insns.emplace_back(node, index);
    else if(node->opc == OPC_BEQZ || node->opc == OPC_BNEZ || node->opc == OPC_BCEQZ || node->opc == OPC_BCNEZ
            || node->opc == OPC_JIRL || node->opc == OPC_BL || node->opc == OPC_BEQ || node->opc == OPC_BLT
            || node->opc == OPC_BGE || node->opc == OPC_BLTU || node->opc == OPC_BGEU)
            tb.other_b_insns.emplace_back(node, index);
}

void __adjust_index_of_branchs_add(std::vector<BranchInsnAndOffset>& branch_and_offs, u_int32_t i)
{
    for(auto& branch_and_off : branch_and_offs)
    {
        if(branch_and_off.offset > i)
                branch_and_off.offset += 1;
    }
}

void adjust_index_of_branchs_when_add_insn(TB& tb, u_int32_t i)
{
    __adjust_index_of_branchs_add(tb.b_insns, i);
    __adjust_index_of_branchs_add(tb.bne_insns, i);
    __adjust_index_of_branchs_add(tb.other_b_insns, i);
}

ListNode<LoongArchInsInfo>* TB::add_insn(ListNode<LoongArchInsInfo>* prev, ListNode<LoongArchInsInfo>* inserted, u_int32_t index)
{
    u_int32_t inserted_index = index + 1;
    u_int64_t inserted_offset = inserted_index << 2;
    u_int64_t j = 0;

    for(auto& rel: rels)
    {
        if(rels_valid[j] && (rel.tc_offset <= inserted_offset && inserted_offset < rel.tc_offset + (rel.rel_slots_num<<2)))
        {
            std::cerr<<"try to add an insn to middle of rel"<<std::endl;
            exit(1);
        }
        j += 1;
    }

    adjust_index_of_branchs_when_add_insn(*this, inserted_index);
    __add_b_insn(*this, inserted->data, inserted_index);

    j = 0;
    for(auto& rel: rels)
    {
        if(rels_valid[j] && rel.tc_offset >= inserted_offset)
                rel.tc_offset += 4;
        j += 1;
    }
    
    for(int i=0; i<2; i++)
    {
        if(origin_aot_tb->jmp_reset_offsets[i] != TB_JMP_RESET_OFFSET_INVALID)
        {
            if(origin_aot_tb->jmp_target_arg[i] >= inserted_offset)
                    origin_aot_tb->jmp_target_arg[i] += 4;
        }
    }

    return dis_insns.insert_after(prev, inserted);
}

ListNode<LoongArchInsInfo>* TB::add_insn_at_tail(ListNode<LoongArchInsInfo>* inserted, u_int32_t index)
{
    return add_insn(dis_insns.end()->prev, inserted, index);
}



TB::TB(FILE* f, AOT_TB* aot_tb, u_int32_t SegBegin):origin_aot_tb(aot_tb)
{
    has_invalid_insn = false;
    true_branch = nullptr;
    false_branch = nullptr;
    x86_addr = aot_tb->offset_in_segment + SegBegin;

    auto tb = x86AddrToTb[x86_addr];
    if(tb != nullptr)
    {
        std::cerr<<"x86_addr: "<<std::hex<<x86_addr<<" already in map"<<std::endl;
        error("");
    }

    int save = ftell(f);
    code = new char[aot_tb->tb_cache_size];
    code_size = aot_tb->tb_cache_size;
    fseek(f, aot_tb->tb_cache_offset, SEEK_SET);
    fread(code, code_size, 1, f);
    size_in_file += code_size;
    false_branch_offset = TB_JMP_RESET_OFFSET_INVALID;
    true_branch_offset = TB_JMP_RESET_OFFSET_INVALID;
    condi_branch_offset = TB_JMP_RESET_OFFSET_INVALID;

    if(aot_tb->jmp_reset_offsets[0] != TB_JMP_RESET_OFFSET_INVALID)
            false_branch_offset = aot_tb->jmp_target_arg[0]/4;
    if(aot_tb->jmp_reset_offsets[1] != TB_JMP_RESET_OFFSET_INVALID)
            true_branch_offset = aot_tb->jmp_target_arg[1]/4;
    
    if(true_branch_offset != TB_JMP_RESET_OFFSET_INVALID && false_branch_offset != TB_JMP_RESET_OFFSET_INVALID)
    {
        u_int16_t cur = (true_branch_offset < false_branch_offset? true_branch_offset : false_branch_offset) - 1;
        LoongArchInsInfo tmp;
        
        while(cur >= 0)
        {
             memset(&tmp, 0, sizeof(tmp));
             u_int32_t insn = ((u_int32_t*)code)[cur];
             decode(&tmp, insn);
             if(tmp.opc == OPC_BEQ || tmp.opc == OPC_BNE)
             {
                    condi_branch_offset = cur;
                    break;
             }
             cur -= 1;
        }
        assert(condi_branch_offset != TB_JMP_RESET_OFFSET_INVALID);
    }
            
    
    fseek(f, save, SEEK_SET);
}

bool TB::pipehole_opt_available()
{
   if(origin_aot_tb->jmp_reset_offsets[0] != TB_JMP_RESET_OFFSET_INVALID 
   && origin_aot_tb->jmp_reset_offsets[1] != TB_JMP_RESET_OFFSET_INVALID)
   {
       if(b_insns.size() == 2)  
       {
                return (bne_insns.size() == 1 && other_b_insns.size() == 0
            && b_insns[0].offset == origin_aot_tb->jmp_target_arg[0]/4
            && b_insns[1].offset == origin_aot_tb->jmp_target_arg[1]/4);
       }
       else if(b_insns.size() == 4)
       {
            for(u_int32_t i=1; i<4; i+=2)
            {
                bool res = false;
                for(u_int32_t j = 0; j < rels.size(); j++)
                {
                    if(rels[j].kind == B_EPILOGUE)
                    {
                            if (b_insns[i].offset == rels[j].tc_offset/4)
                            {
                                 res = true;
                                 break;
                            }
                    }       
                }
                if(!res)
                     return false;
            }
            return (bne_insns.size() == 1 && other_b_insns.size() == 0
            && b_insns[0].offset == origin_aot_tb->jmp_target_arg[0]/4
            && b_insns[2].offset == origin_aot_tb->jmp_target_arg[1]/4);
       }
       else
            return false;
   }
   else if(origin_aot_tb->jmp_reset_offsets[0] != TB_JMP_RESET_OFFSET_INVALID)
   {
        if(b_insns.size() == 1)
        {
            return bne_insns.size() == 0 && other_b_insns.size() == 0
                && b_insns[0].offset == origin_aot_tb->jmp_target_arg[0]/4;
        }
        else if(b_insns.size() == 2)
        {
                bool res = false;
                for(u_int32_t j = 0; j < rels.size(); j++)
                {
                    if(rels[j].kind == B_EPILOGUE)
                    {
                            if (b_insns[1].offset == rels[j].tc_offset/4)
                            {
                                 res = true;
                                 break;
                            }
                    }       
                }
                if(!res)
                     return false;
                return bne_insns.size() == 0 && other_b_insns.size() == 0
                && b_insns[0].offset == origin_aot_tb->jmp_target_arg[0]/4;
        }
        else
            return false;
   }
   else if(origin_aot_tb->jmp_reset_offsets[1] != TB_JMP_RESET_OFFSET_INVALID)
   {
        if(b_insns.size() == 1)
        {
            return bne_insns.size() == 0 && other_b_insns.size() == 0
                && b_insns[0].offset == origin_aot_tb->jmp_target_arg[1]/4;
        }
        else if(b_insns.size() == 2)
        {
                bool res = false;
                for(u_int32_t j = 0; j < rels.size(); j++)
                {
                    if(rels[j].kind == B_EPILOGUE)
                    {
                            if (b_insns[1].offset == rels[j].tc_offset/4)
                            {
                                 res = true;
                                 break;
                            }
                    }       
                }
                if(!res)
                     return false;
                return bne_insns.size() == 0 && other_b_insns.size() == 0
                && b_insns[0].offset == origin_aot_tb->jmp_target_arg[1]/4;
        }
        else
            return false;
   }
   else
   {
        return b_insns.size() == 1 && bne_insns.size() == 0 && other_b_insns.size() == 0; 
   }
}

u_int32_t MyAbs(int32_t a, int32_t b)
{
    if(a<b)
            return (u_int32_t) b - a;
    return a-b;
}

const AOT_rel& TB::find_closest_load_target_addr_rel_entry(u_int32_t branch_insn_offset, const std::vector<AOT_rel>& all_rels) const
{
    const AOT_rel* closest = nullptr;
    for(int i=origin_aot_tb->rel_start_index; i<=origin_aot_tb->rel_end_index; i++)
    {
        const AOT_rel& rel = all_rels[i];
        if(rel.kind == LOAD_CALL_TARGET && rel.tc_offset > branch_insn_offset)
        {
            if(closest == nullptr)
                    closest = &rel;
            else
            {
                if(closest->tc_offset > rel.tc_offset)
                    closest = &rel;
            }
        }
    }
    return *closest;
}

u_int32_t TB::how_many_rel()
{
    int count = 0;
    for(auto valid: rels_valid)
            if(valid)
                    count += 1;
    return count;
}

u_int32_t TB::how_many_code()
{
    return dis_insns.total_size()*4;
}

ListNode<LoongArchInsInfo>* TB::delete_ith_rel(u_int64_t i)
{
    if(rels_valid[i])
    {
        AOT_rel& rel = rels[i];
        u_int64_t ith_insn = rel.tc_offset >> 2;
        u_int64_t count = rel.rel_slots_num;
        ListNode<LoongArchInsInfo>* insn = dis_insns.get(ith_insn);
        while(count > 0)
        {
            insn = delete_ith_insn_alongwith_rel(insn, ith_insn);
            count -= 1;
        }
        return insn;
    }
    else
        return nullptr;
}

void __erase_b_and_index(std::vector<BranchInsnAndOffset>& b_and_index, u_int32_t removed_index)
{
    for(auto iter = b_and_index.begin(); iter != b_and_index.end(); iter++)
    {
        if(iter->offset == removed_index)
        {
            b_and_index.erase(iter);
            break;
        }
    }
}

void __delete_b_insn(TB& tb, LoongArchInsInfo* node, u_int32_t i)
{
        if(node->opc == OPC_B)
            __erase_b_and_index(tb.b_insns, i);    
        else if(node->opc == OPC_BNE)
            __erase_b_and_index(tb.bne_insns, i);
        else if(node->opc == OPC_BEQZ || node->opc == OPC_BNEZ || node->opc == OPC_BCEQZ || node->opc == OPC_BCNEZ
            || node->opc == OPC_JIRL || node->opc == OPC_BL || node->opc == OPC_BEQ || node->opc == OPC_BLT
            || node->opc == OPC_BGE || node->opc == OPC_BLTU || node->opc == OPC_BGEU)
            __erase_b_and_index(tb.other_b_insns, i);
}

void __adjust_index_of_branchs(std::vector<BranchInsnAndOffset>& branch_and_offs, u_int32_t i)
{
    for(auto& branch_and_off : branch_and_offs)
    {
        if(branch_and_off.offset > i)
                branch_and_off.offset -= 1;
    }
}

void adjust_index_of_branchs_when_delete_insn(TB& tb, u_int32_t i)
{
      __adjust_index_of_branchs(tb.b_insns, i);
      __adjust_index_of_branchs(tb.bne_insns, i);
      __adjust_index_of_branchs(tb.other_b_insns, i);
}

ListNode<LoongArchInsInfo>* TB::delete_ith_insn_alongwith_rel(ListNode<LoongArchInsInfo>* node, u_int64_t i)
{
    u_int64_t offset_of_ith_insn = i << 2;

    u_int64_t j = 0;
    for(auto& rel: rels)
    {
         if (rels_valid[j] && (rel.tc_offset <= offset_of_ith_insn &&  offset_of_ith_insn < rel.tc_offset + (rel.rel_slots_num<<2)))
         {
              rels_valid[j] = false;
              break;
         }
         j += 1;
    }
    __delete_b_insn(*this, node->data, i);
    adjust_index_of_branchs_when_delete_insn(*this, i);

    j = 0;
    for(auto& rel: rels)
    {
        if(rels_valid[j] && offset_of_ith_insn < rel.tc_offset)
                rel.tc_offset -= 4;
        j += 1;
    }

    if(true_branch_offset != TB_JMP_RESET_OFFSET_INVALID && true_branch_offset > i)
            true_branch_offset -= 1;
    if(false_branch_offset != TB_JMP_RESET_OFFSET_INVALID && false_branch_offset > i)
            false_branch_offset -= 1;
    if(condi_branch_offset != TB_JMP_RESET_OFFSET_INVALID && condi_branch_offset > i)
            condi_branch_offset -= 1;

    return dis_insns.remove(node);
}


ListNode<LoongArchInsInfo>* TB::delete_ith_insn(ListNode<LoongArchInsInfo>* node, u_int64_t i)
{
    u_int64_t offset_of_ith_insn = i << 2;

    u_int64_t j = 0;
    for(auto& rel: rels)
    {
         if (rels_valid[j] && (rel.tc_offset <= offset_of_ith_insn &&  offset_of_ith_insn < rel.tc_offset + (rel.rel_slots_num<<2)))
         {
                std::cerr << " try to delete insn in rel entry: " << i <<"th insn"<< " ignore for now "<<std::endl;
                return node;
         }
         j += 1;
    }
    __delete_b_insn(*this, node->data, i);
    adjust_index_of_branchs_when_delete_insn(*this, i);

    j = 0;
    for(auto& rel: rels)
    {
        if(rels_valid[j] && (offset_of_ith_insn < rel.tc_offset))
                rel.tc_offset -= 4;
        j += 1;
    }

    if(true_branch_offset != TB_JMP_RESET_OFFSET_INVALID && true_branch_offset > i)
            true_branch_offset -= 1;
    if(false_branch_offset != TB_JMP_RESET_OFFSET_INVALID && false_branch_offset > i)
            false_branch_offset -= 1;
    if(condi_branch_offset != TB_JMP_RESET_OFFSET_INVALID && condi_branch_offset > i)
            condi_branch_offset -= 1;

    return dis_insns.remove(node);
}

void SetNextTB(std::shared_ptr<TB> tb ,std::shared_ptr<TB> branch, bool is_true_branch)
{
    if(is_true_branch)
    {
        assert(tb->true_branch == nullptr);
        tb->true_branch = branch;
    }
    else
    {
        assert(tb->false_branch == nullptr);
        tb->false_branch = branch;
    }
    branch->parents.push_back(tb);
}

Segment::Segment(FILE* f, const AOT_Segment* seg):seg(seg), size_in_file(0)
{
    int save = ftell(f);

    char lib_name_buf[PATH_MAX];
    fseek(f, seg->lib_name_offset, SEEK_SET);
    fread(lib_name_buf, PATH_MAX, 1, f);
    size_in_file += PATH_MAX;
    lib_name = lib_name_buf;
    file_offset = seg->info.file_offset;
    
    fseek(f, seg->tb_offset, SEEK_SET);
    AOT_TB tmp;
    tbs.reserve(seg->tb_nums);
    _tbs.reserve(seg->tb_nums);
    for(int i=0; i<seg->tb_nums; i++)
    {
        fread(&tmp, sizeof(AOT_TB), 1, f);
        tbs.push_back(tmp);
        auto tb =  std::make_shared<TB>(f, &tbs[i], (seg->info).seg_begin);
        size_in_file += sizeof(AOT_TB);
        size_in_file += tb->size_in_file;
        x86AddrToTb[(seg->info).seg_begin + tbs[i].offset_in_segment] = tb;
        _tbs.push_back(tb);
    }
    fseek(f, save, SEEK_SET);
}

u_int32_t Segment::how_many_tbs()
{
    return _tbs.size();
}

u_int32_t Segment::how_many_rel()
{
    u_int32_t total = 0;
    for(auto tb_ptr: _tbs)
        total += tb_ptr->how_many_rel();
    return total;
}

u_int32_t Segment::how_many_code()
{
    u_int32_t total = 0;
    for(auto tb_ptr: _tbs)
            total += tb_ptr->how_many_code();
    return total;
}

void Segment::TB_Link(std::shared_ptr<TB> tb, bool is_true_branch)
{
   /*
   u_int64_t x86_addr = -1;
   get_addr(&x86_addr, is_true_branch? 1:0, *tb, rels);
   if (x86_addr == -1)
   {
        if(is_true_branch)
        {
            std::cerr<<"can't find true branch in tb , pc: "<<std::hex<<(seg->info).seg_begin + tb->origin_aot_tb->offset_in_segment;
            exit(1);
        }
        else
        {
            std::cerr<<"can't find false branch in tb , pc: "<<std::hex<<(seg->info).seg_begin + tb->origin_aot_tb->offset_in_segment;
            exit(1);
        }
   }
   */

   u_int64_t x86_addr = tb->origin_aot_tb->x86_offset[is_true_branch? 1:0] + seg->info.seg_begin;
   std::shared_ptr<TB> next_tb = x86AddrToTb[x86_addr];
   if(next_tb != nullptr)
        SetNextTB(tb, next_tb, is_true_branch);
}

void Segment::settle_all_tb()
{
    for(auto tb: _tbs)
    {
        const AOT_TB& origin_tb = *tb->origin_aot_tb;
        int true_branch = 1;
        int false_branch = 0;
        u_int64_t seg_begin = (seg->info).seg_begin;

        if(origin_tb.jmp_reset_offsets[true_branch] != TB_JMP_RESET_OFFSET_INVALID 
           && origin_tb.jmp_reset_offsets[false_branch] != TB_JMP_RESET_OFFSET_INVALID)
        {
            TB_Link(tb, true);
            TB_Link(tb, false);
        }
        else if(origin_tb.jmp_reset_offsets[true_branch] != TB_JMP_RESET_OFFSET_INVALID)
            TB_Link(tb, true);
        else if(origin_tb.jmp_reset_offsets[false_branch] != TB_JMP_RESET_OFFSET_INVALID)
            TB_Link(tb, false);
        else
                std::cerr<<"TB with pc: "<<origin_tb.offset_in_segment + seg_begin << "did not have outgoing branch"<<std::endl;
        
    }
}

AOT_File::AOT_File(FILE* f):size_in_file(0)
{
    fseek(f, 0, SEEK_SET);
    fread(&hdr, sizeof(AOT_Header), 1, f);
    size_in_file += sizeof(AOT_Header);
    fseek(f, hdr.segtab_offset, SEEK_SET);
    AOT_Segment seg_tmp;
    segments.reserve(hdr.segnum);
    _segs.reserve(hdr.segnum);
    for(int i=0; i<hdr.segnum; i++)
    {
        fread(&seg_tmp, sizeof(AOT_Segment), 1, f);
        segments.push_back(seg_tmp);
        _segs.push_back(new Segment(f, &segments[segments.size()-1]));
        size_in_file += sizeof(AOT_Segment);
        size_in_file += _segs[_segs.size()-1]->size_in_file;
    }

    fseek(f, hdr.reltable_offset, SEEK_SET);
    u_int32_t cur_p = ftell(f);
    AOT_rel rel_tmp;
    rels.reserve(hdr.reltable_num);
    for(int i=0; i<hdr.reltable_num; i++)
    {
        fread(&rel_tmp, sizeof(AOT_rel), 1, f);
        rels.push_back(rel_tmp);
    }
    size_in_file += sizeof(AOT_rel) * hdr.reltable_num;
}

u_int32_t AOT_File::how_many_bytes()
{
    u_int32_t total = sizeof(AOT_Header);
    for(auto seg_ptr : _segs)
            total += seg_ptr->how_many_bytes();
    return total;
}

u_int32_t Segment::how_many_bytes()
{
    u_int32_t total = sizeof(AOT_Segment);
    total += PATH_MAX;
    for(auto tb_ptr: _tbs)
        total += tb_ptr->how_many_bytes();
    return total;
}

u_int32_t TB::how_many_bytes()
{
    u_int32_t total = sizeof(AOT_TB);
    u_int32_t count = 0;

    total += dis_insns.total_size()*4;

    for(bool rel_valid: rels_valid)
    {
        if(rel_valid)
        {
                total += sizeof(AOT_rel);
                count += 1;
        }
    }
   return total; 
}

std::vector<AOT_rel>& AOT_File::get_rels()
{
    return rels;
}

std::vector<Segment*>& AOT_File::get_segments()
{
    return _segs;
}

std::vector<AOT_Segment>& AOT_File::get_AOTsegments()
{
    return segments;
}
