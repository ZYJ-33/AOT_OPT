#include "Vistor/DisassemblePrintVistor.hpp"
#include <set>
extern int insn2format[];
extern void(*print_func_table[])(std::ostream&, LoongArchInsInfo*);

extern std::map<u_int64_t, std::shared_ptr<TB>> x86AddrToTb;

void DisassmblePrinterVistor::start(Segment& seg)
{
    this->seg_start = seg.seg->info.seg_begin;
    for(auto tb_ptr: seg._tbs)
         (*tb_ptr).visit(*this);
}

void DisassmblePrinterVistor::start(Segment& seg, u_int64_t entry)
{
    std::vector<std::shared_ptr<TB>> queue;
    std::shared_ptr<TB> cur = x86AddrToTb[entry];
    queue.push_back(cur);
    
    int cur_head = 0;
    std::set<u_int64_t> seen;
    seen.insert(cur->x86_addr);

    while(cur_head < queue.size())
    {
        int count = queue.size()-cur_head;
        while(count > 0)
        {
            std::shared_ptr<TB> head = queue[cur_head];
            head->visit(*this);
            if(head->true_branch != nullptr && seen.find(head->true_branch->x86_addr) == seen.end())
            {
                    queue.push_back(head->true_branch);
                    seen.insert(head->true_branch->x86_addr);
            }
            if(head->false_branch != nullptr && seen.find(head->false_branch->x86_addr) == seen.end())
            {
                    queue.push_back(head->false_branch);
                    seen.insert(head->false_branch->x86_addr);
            }
            cur_head += 1;
            count -= 1;
        }
    }
}

void DisassmblePrinterVistor::print_one_insn(LoongArchInsInfo& insn)
{
      if(insn.opc != OPC_INVALID)
      {
           int format = insn2format[insn.opc];
           auto print_func = print_func_table[format];
           (*print_func)(std::cout, &insn);
      }
      else
          std::cout<<"Invalid insn\n";
}

void DisassmblePrinterVistor::visit(TB& tb)
{
   std::cout<<"print disassmble at x86_pc: 0x"<<std::hex<<tb.x86_addr<<std::endl;
   u_int32_t rel_cur = 0;
   u_int64_t i = 0;
  
   while(rel_cur < tb.rels_valid.size() && tb.rels_valid[rel_cur] == false)
           rel_cur++;

   for(auto iter = tb.dis_insns.begin(); iter != tb.dis_insns.end();)
   {
        if(rel_cur < tb.rels_valid.size() && i == ( (tb.rels[rel_cur].tc_offset) / 4) )
        {
            int count = tb.rels[rel_cur].rel_slots_num;
            std::cout<<rel_kind2name[tb.rels[rel_cur].kind]<<std::endl;
            std::cout<<"{"<<std::endl;
            while(count > 0)
            {
                print_one_insn(*(iter->data));
                iter = iter->next;
                i+=1;
                count -= 1;
            } 
            std::cout<<"}"<<std::endl;
            rel_cur += 1;
            while(rel_cur < tb.rels_valid.size() && tb.rels_valid[rel_cur] == false)
                rel_cur++;
        }
        else if(tb.origin_aot_tb->jmp_reset_offsets[0] != TB_JMP_RESET_OFFSET_INVALID 
               && i == (tb.origin_aot_tb->jmp_target_arg[0] / 4))
        {
            std::cout<<"false_branch"<<std::endl;
            std::cout<<"{"<<std::endl;
            if(tb.false_branch != nullptr)
                std::cout<<std::hex<<tb.false_branch->x86_addr<<" extra_addend:"<<std::dec<<tb.false_branch->x86_addr - this->seg_start<<std::endl;
            else
                std::cout<<"next tb not exist yet"<<std::endl;
            print_one_insn(*iter->data); 
            i += 1;
            iter = iter->next;
            std::cout<<"}"<<std::endl;
        }
        else if(tb.origin_aot_tb->jmp_reset_offsets[1] != TB_JMP_RESET_OFFSET_INVALID
               && i == (tb.origin_aot_tb->jmp_target_arg[1] / 4))
        {
            std::cout<<"true_branch"<<std::endl;
            std::cout<<"{"<<std::endl;
            if(tb.true_branch != nullptr)
                std::cout<<std::hex<<tb.true_branch->x86_addr<<" extra_addend:"<<std::dec<<tb.true_branch->x86_addr - this->seg_start<<std::endl;
            else
                std::cout<<"next tb not exist yet"<<std::endl;
            print_one_insn(*iter->data); 
            i += 1;
            iter = iter->next;
            std::cout<<"}"<<std::endl;
        }
        else
        {
            print_one_insn(*iter->data); 
            i += 1;
            iter = iter->next;
        }
   }
}

