#include"Basic/aot.hpp"
#include"AOT_class/aot_class.hpp"
#include"Basic/util.hpp"
#include<cstring>
#include<cstdlib>
#include<climits>
#include<cassert>
#include<cstdio>
#include "Vistor/TB_Vistor.hpp"

std::map<u_int64_t, std::shared_ptr<TB>> x86AddrToTb;

TB::TB()
{
}

void TB::visit(TB_Vistor& tb)
{
    tb.visit(*this);
}

TB::TB(FILE* f, AOT_TB* aot_tb, u_int32_t SegBegin):origin_aot_tb(aot_tb)
{
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
    
    fseek(f, save, SEEK_SET);
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
        ListNode<LoongArchInsInfo>* insn = dis_insns.get(i);
        while(count > 0)
        {
            insn = delete_ith_insn_alongwith_rel(insn, i);
            count -= 1;
        }
        return insn;
    }
    else
        return nullptr;
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

    j = 0;
    for(auto& rel: rels)
    {
        if(rels_valid[j] && offset_of_ith_insn < rel.tc_offset)
                rel.tc_offset -= 4;
        j += 1;
    }

    for(int i=0; i<2; i++)
    {
        if(origin_aot_tb->jmp_reset_offsets[i] != TB_JMP_RESET_OFFSET_INVALID)
        {
            if(origin_aot_tb->jmp_target_arg[i] > offset_of_ith_insn)
                    origin_aot_tb->jmp_target_arg[i] -= 4;
        }
    }
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

    j = 0;
    for(auto& rel: rels)
    {
        if(rels_valid[j] && (offset_of_ith_insn < rel.tc_offset))
                rel.tc_offset -= 4;
        j += 1;
    }
    for(int i=0; i<2; i++)
    {
        if(origin_aot_tb->jmp_reset_offsets[i] != TB_JMP_RESET_OFFSET_INVALID)
        {
            if(origin_aot_tb->jmp_target_arg[i] > offset_of_ith_insn)
                    origin_aot_tb->jmp_target_arg[i] -= 4;
        }
    }
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

void Segment::get_addr(u_int64_t* res, int which, const TB& tb, const std::vector<AOT_rel>& rels)
{
    u_int64_t branch_insn_offset = (tb.origin_aot_tb)->jmp_target_arg[which];
    u_int32_t* branch_insn_ptr = (u_int32_t*)(tb.code+branch_insn_offset);

    const AOT_rel& rel = tb.find_closest_load_target_addr_rel_entry(branch_insn_offset, rels);
    if(rel.kind == LOAD_CALL_TARGET)
        *res = rel.extra_addend + (seg->info).seg_begin;

    /*
    for(int i=tb.origin_aot_tb->rel_start_index; i<=tb.origin_aot_tb->rel_end_index; i++)
    {
        const AOT_rel& rel = rels[i];
        if(rel.kind == LOAD_CALL_TARGET
            && rel.tc_offset == branch_insn_offset + 20)
         //   && ((*branch_insn_ptr) & 0x1f) == 21)
         {
                *res = rel.extra_addend + (seg->info).seg_begin;
                return;
         }
    }
    */
}

void Segment::TB_Link(std::shared_ptr<TB> tb, bool is_true_branch, const std::vector<AOT_rel>& rels)
{
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
   std::shared_ptr<TB> next_tb = x86AddrToTb[x86_addr];
   if(next_tb == nullptr)
   {
        std::cerr<<"warning： can't find tb relate to x86_addr: "<<std::hex<<x86_addr<<std::endl;
       // exit(1);
   }
   else
        SetNextTB(tb, next_tb, is_true_branch);
}

void Segment::settle_all_tb(const std::vector<AOT_rel>& rels)
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
            TB_Link(tb, true, rels);
            TB_Link(tb, false, rels);
        }
        else if(origin_tb.jmp_reset_offsets[true_branch] != TB_JMP_RESET_OFFSET_INVALID)
            TB_Link(tb, true, rels);
        else if(origin_tb.jmp_reset_offsets[false_branch] != TB_JMP_RESET_OFFSET_INVALID)
            TB_Link(tb, false, rels);
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
