#include "Vistor/AddRelsVistor.hpp"

void TB_AddRels_Vistor::visit(TB& tb)
{
    u_int64_t start_index = tb.origin_aot_tb->rel_start_index; 
    u_int64_t end_index = tb.origin_aot_tb->rel_end_index;
    tb.rels.reserve(end_index - start_index + 1);

    while(start_index <= end_index)
        tb.rels.emplace_back((*rels)[start_index++]);
}

void TB_AddRels_Vistor::start(Segment& seg, const std::vector<AOT_rel>* rels)
{
    this->rels = rels;
    for(auto tb_ptr : seg._tbs)
        visit(*tb_ptr);
}

