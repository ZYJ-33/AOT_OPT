#include "Vistor/CheckVistor.hpp"

void CheckVistor::visit(TB& tb)
{
    switch(tb.tbtype)
    {
        case NORMAL_BNE_TYPE:
        case NONE_B_TYPE:
        case NORMAL_B_FALSE_TYPE:
        case NORMAL_B_TRUE_TYPE:
                break;
        case TU_BEQ_TYPE:
        case TU_BNE_TYPE:
        case TU_B_FALSE_TYPE:
        case TU_B_TRUE_TYPE:
        case TU_B_TRUE_BEQ_TYPE:
            assert(tb.true_branch != nullptr && tb.false_branch != nullptr);
            break;
        case UNKNOW_TB_TYPE:
            std::cerr<<"found an unknow tb type 0x"<<std::hex<<tb.x86_addr<<std::endl;
            exit(1);
    }
}

void CheckVistor::start(Segment& seg)
{
    for(auto tb_ptr: seg._tbs)
        tb_ptr->visit(*this);
}

