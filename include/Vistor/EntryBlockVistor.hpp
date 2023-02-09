#ifndef ENTRY_BLOCK_VISTOR
#define ENTRY_BLOCK_VISTOR

#include "AOT_class/aot_class.hpp"
#include "Vistor/TB_Vistor.hpp"

class EntryBlockVistor: TB_Vistor
{
        public:
            std::vector<u_int64_t> entrys;
            void visit(TB& tb);
            void start(Segment& seg);
};
#endif
