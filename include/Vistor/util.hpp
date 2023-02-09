#ifndef VISTOR_UTIL
#define VISTOR_UTIL
#include "Disassmbler/disassmbler.hpp"
#include "LinkList/linklist.hpp"
void print_one_insn(LoongArchInsInfo& insn);
void print_all_insn(LinkList<LoongArchInsInfo>& insns);
#endif
