#ifndef PGOTB_WRITER
#define PGOTB_WRITER
#include "AOT_class/aot_class.hpp"

class PGOTBWriter
{
    public:
            static void write_to_buf(AOT_Header* hdr, AOT_Segment* seg, AOT_TB* tb_buf, AOT_rel* rel_buf, char* code_buf, std::vector<std::shared_ptr<TB>>& tbs);

    private:
            static void handle_assemble(std::shared_ptr<TB> tb_ptr, u_int32_t* insn_ptr, AOT_TB* target_tb, AOT_Header* hdr);

            static void handle_rels(std::shared_ptr<TB> tb_ptr, AOT_rel* rel_buf, AOT_TB* target_tb, AOT_Header* hdr);
};


#endif
