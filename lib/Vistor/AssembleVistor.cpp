#include "Vistor/AssembleVistor.hpp"
#include "Vistor/util.hpp"

void AssmbleVistor::visit(TB& tb)
{
    u_int32_t* code = new u_int32_t[tb.dis_insns.total_size()];
    u_int32_t cur = 0;
    for(auto iter=tb.dis_insns.begin(); iter!=tb.dis_insns.end();iter=iter->next, cur+=1)
        code[cur] = encode(iter->data);
    codes.push_back(code);
}

u_int32_t AssmbleVistor::tocode(LoongArchInsInfo* insn)
{
    return encode(insn);
}

u_int32_t* AssmbleVistor::tocode(std::vector<LoongArchInsInfo*>& insns)
{
    u_int32_t* code = new u_int32_t[insns.size()];
    u_int32_t cur = 0;
    for(auto iter=insns.begin(); iter!=insns.end(); iter++, cur+=1)
        code[cur] = encode(&(**iter));
    return code;
}

void AssmbleVistor::start(Segment& seg)
{
    codes.reserve(seg._tbs.size());
    for(auto tb_ptr: seg._tbs)
            (*tb_ptr).visit(*this);
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


    }
}
