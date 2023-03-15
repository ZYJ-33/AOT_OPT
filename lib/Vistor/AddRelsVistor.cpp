#include "Vistor/AddRelsVistor.hpp"

void TB_AddRels_Vistor::visit(TB& tb)
{
    int32_t start_index = tb.origin_aot_tb->rel_start_index; 
    int32_t end_index = tb.origin_aot_tb->rel_end_index;
    if(end_index < start_index)
        return;
    tb.rels.reserve(end_index - start_index + 1);
    tb.rels_valid.reserve(end_index - start_index + 1);
    while(start_index <= end_index)
    {
        tb.rels.push_back((*rels)[start_index++]);
        tb.rels_valid.push_back(true);
    }
}

void TB_AddRels_Vistor::start(Segment& seg, const std::vector<AOT_rel>* rels)
{
    this->rels = rels;
    for(auto tb_ptr : seg._tbs)
        visit(*tb_ptr);
}

