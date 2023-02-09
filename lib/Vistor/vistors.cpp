#include "Vistor/vistors.hpp"
#include<cstdlib>
#include<cstring>

extern std::map<u_int64_t, std::shared_ptr<TB>> x86AddrToTb;

void TB::visit(TB_Vistor& vistor)
{
    vistor.visit(*this);
}

static void print_one_insn(LoongArchInsInfo& insn)
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

static void print_all_insn(LinkList<LoongArchInsInfo>& insns)
{
    for(auto iter=insns.begin(); iter != insns.end(); )
    {
        print_one_insn(*(iter->data));
        iter = iter->next;
    }
}

void AssmbleVistor::start(Segment& seg, DisassmbleVistor& dis, DisassmblePrinterVistor& disprint)
{
    codes.reserve(seg._tbs.size());
    for(auto tb_ptr: seg._tbs)
    {
         (*tb_ptr).visit(*this);
         TB* new_tb = new TB();
         new_tb->code = (char*)codes[codes.size()-1];
         new_tb->code_size = tb_ptr->dis_insns.total_size()<<2;
         dis.visit(*new_tb);
         std::cout<<"origin x86_pc: 0x"<<std::hex<<tb_ptr->x86_addr<<std::endl;
         print_all_insn(tb_ptr->dis_insns);
         std::cout<<"after as "<<std::endl;
         print_all_insn(new_tb->dis_insns);
    }
}

void RemoveTailVistor::remove_tail_of(TB& tb, uint which)
{
    assert(which == 0 || which == 1);
    int32_t i = tb.origin_aot_tb->jmp_target_arg[which]>>2;
    int32_t total_insn_count = tb.code_size >> 2;

    ListNode<LoongArchInsInfo>* insn = tb.dis_insns.get(i);
    assert(insn->data->opc == OPC_B);
    int32_t off = insn->data->offs;
    
    if(( which ? tb.true_branch : tb.false_branch) != nullptr)
    {
        ListNode<LoongArchInsInfo>* go = insn->next;
        i += 1;
        int remain = 10;
        while(remain > 0)
        {
            go = tb.delete_ith_insn_alongwith_rel(go, i);
            remain -= 1;
        }
    }
    else
    {
        return;
    }
}




void EntryBlockVistor::start(Segment& seg)
{
    for(auto tb_ptr : seg._tbs)
        visit(*tb_ptr);
}

void EntryBlockVistor::visit(TB& tb)
{
    if(tb.parents.size() == 0)
            entrys.push_back(tb.x86_addr);
}



