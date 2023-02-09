#include "Vistor/util.hpp"

void print_one_insn(LoongArchInsInfo& insn)
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

void print_all_insn(LinkList<LoongArchInsInfo>& insns)
{
    for(auto iter=insns.begin(); iter != insns.end(); )
    {
        print_one_insn(*(iter->data));
        iter = iter->next;
    }
}
