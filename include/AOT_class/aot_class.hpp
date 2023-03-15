#ifndef __AOT_CLASS
#define __AOT_CLASS
#include "Basic/aot.hpp"
#include <cstdlib>
#include <memory>
#include <map>
#include "Disassmbler/disassmbler.hpp"
#include "LinkList/linklist.hpp"

class TB;
class TB_Vistor;

class BranchInsnAndOffset
{
    public:
    LoongArchInsInfo* branch_insn;
    u_int32_t offset;
    BranchInsnAndOffset(LoongArchInsInfo* branch_insn, u_int32_t offset): branch_insn(branch_insn), offset(offset)
    {}
};

class TB
{
    public:
        bool has_invalid_insn;
        u_int32_t size_in_file;
        char* code;
        u_int32_t code_size;
        bool pipehole_available;
        TBtype tbtype;
        std::shared_ptr<TB> true_branch;
        std::shared_ptr<TB> false_branch;
        u_int16_t false_branch_offset;
        u_int16_t true_branch_offset;
        std::vector<std::shared_ptr<TB>> parents;
        std::vector<AOT_rel> rels;
        std::vector<bool> rels_valid; 
        LinkList<LoongArchInsInfo> dis_insns;
        std::vector<BranchInsnAndOffset> b_insns;
        std::vector<BranchInsnAndOffset> bne_insns;
        std::vector<BranchInsnAndOffset> other_b_insns;
        
        bool in_scope_of_rel(u_int32_t start, u_int32_t count);
        ListNode<LoongArchInsInfo>* delete_ith_insn(ListNode<LoongArchInsInfo>* node, u_int64_t i);
        ListNode<LoongArchInsInfo>* delete_ith_insn_alongwith_rel(ListNode<LoongArchInsInfo>* node, u_int64_t i);
        ListNode<LoongArchInsInfo>* add_insn(ListNode<LoongArchInsInfo>* prev, ListNode<LoongArchInsInfo>* inserted, u_int32_t index);
        ListNode<LoongArchInsInfo>* add_insn_at_tail(ListNode<LoongArchInsInfo>* inserted, u_int32_t index);
        ListNode<LoongArchInsInfo>* delete_ith_rel(u_int64_t i);
        void adjust_branch_insn_offset_when_delete(u_int32_t index_of_deleted);

        AOT_TB* origin_aot_tb;
        u_int64_t x86_addr;
        TB(FILE* f, AOT_TB* aot_tb, u_int32_t SegBegin);
        TB();
        void visit(TB_Vistor& vistor);
        const AOT_rel& find_closest_load_target_addr_rel_entry(u_int32_t branch_insn_offset, const std::vector<AOT_rel>& rels) const;
        u_int32_t how_many_bytes();
        u_int32_t how_many_rel();
        u_int32_t how_many_code();
        bool pipehole_opt_available();
};

class Segment
{
    public:
        /*lib_name and file_offset are primary key to identify a segment in origin x86 elf file */
        u_int32_t size_in_file;
        std::string lib_name;
        target_ulong file_offset;
        std::vector<AOT_TB> tbs;
        std::vector<std::shared_ptr<TB>> _tbs;
        const AOT_Segment* seg;
        Segment(FILE* f, const AOT_Segment* seg);
        void settle_all_tb();
        void TB_Link(std::shared_ptr<TB> tb, bool is_true_branch, u_int64_t x86_addr);
        u_int32_t how_many_bytes();
        u_int32_t how_many_tbs();
        u_int32_t how_many_rel();
        u_int32_t how_many_code();
};

class AOT_File
{
    public:
            u_int32_t size_in_file;
            AOT_Header hdr;
            std::vector<AOT_rel> rels;
            std::vector<AOT_Segment> segments;
            std::vector<Segment*> _segs;
            AOT_File(FILE* aot_file);

            std::vector<AOT_Segment>& get_AOTsegments();
            std::vector<Segment*>& get_segments();
            std::vector<AOT_rel>& get_rels();
            AOT_Header& get_AOTheader();
            u_int32_t how_many_bytes();
};
#endif
