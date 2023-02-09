#ifndef __VISTOR
#define __VISTOR
#include "AOT_class/aot_class.hpp"
#include<set>


class TB_AddRels_Vistor: TB_Vistor
{
    const std::vector<AOT_rel>* rels;
    public:
        void visit(TB& tb);
        void start(Segment& seg, const std::vector<AOT_rel>* rels);
};

class EntryBlockVistor: TB_Vistor
{
        public:
            std::vector<u_int64_t> entrys;
            void visit(TB& tb);
            void start(Segment& seg);
};

class DisassmbleVistor: TB_Vistor
{
    public:
            void visit(TB& tb);
            void start(Segment& seg);
};

class DisassmblePrinterVistor: TB_Vistor
{
    public:
            u_int64_t seg_start;
            const std::vector<AOT_rel>* rels;
            void visit(TB& tb);
            void start(Segment& seg, const std::vector<AOT_rel>* rels);
            void start(Segment& seg, u_int64_t entry, const std::vector<AOT_rel>* rels);
    private:
            void print_one_insn(LoongArchInsInfo& insn);
};

class AssmbleVistor: TB_Vistor
{
    public:
        std::vector<u_int32_t*> codes;
        void visit(TB& tb);
        void start(Segment& seg);
        void start(Segment& seg, DisassmbleVistor& dis, DisassmblePrinterVistor& disprint);
};

class testVistor: TB_Vistor
{
    public:
            void visit(TB& tb);
            void start(Segment& seg);
};
#endif
