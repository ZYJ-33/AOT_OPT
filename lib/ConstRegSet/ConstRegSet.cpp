#include "ConstRegSet/ConstRegSet.hpp"
#include<iostream>

ConstRegSet::ConstRegSet()
{
    memset(regs, 0, sizeof(int32_t)*32);
    memset(is_const,0,sizeof(bool)*32);
    size = 0;
}

void ConstRegSet::kill(u_int8_t which)
{
    if(is_const[which])
    {
        is_const[which] = false;
        size -= 1;
    }
}

bool ConstRegSet::isconst(u_int8_t which)
{
    return is_const[which];
}

void ConstRegSet::set(u_int8_t which, int64_t imm)
{
    if(which == 0)
            return;
    else
    {
            regs[which] = imm;
            if(!is_const[which])
            {
                is_const[which] = true;
                size += 1;
            }
    }
}

int64_t ConstRegSet::get(u_int8_t which)
{
    if(is_const[which])
        return regs[which];
    std::cerr<<"try to get a non const reg\n";
    exit(1);
}
