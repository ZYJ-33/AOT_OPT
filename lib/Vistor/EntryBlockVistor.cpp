#include "Vistor/EntryBlockVistor.hpp"

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

