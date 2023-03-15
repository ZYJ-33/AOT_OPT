#ifndef TB_WRITER
#define TB_WRITER
#include "AOT_class/aot_class.hpp"

class TU_TBWriter
{
    public:
            static void write_to_buf(AOT_Header* hdr, AOT_Segment* seg, AOT_TB* tb_buf, AOT_rel* rel_buf, char* code_buf, std::vector<std::shared_ptr<TB>>& tbs);

    private:
            static std::map<u_int64_t, u_int64_t> x86addr_to_codecache_offset;
            static void handle_assemble(std::shared_ptr<TB> tb_ptr, u_int32_t* insn_ptr, AOT_TB* target_tb, AOT_Header* hdr);
            static void handle_rels(std::shared_ptr<TB> tb_ptr, AOT_rel* rel_buf, AOT_TB* target_tb, AOT_Header* hdr);
            static void handle_linking(std::vector<std::shared_ptr<TB>>& tbs, u_int32_t* code_cache);
            static void __linking(std::shared_ptr<TB> tb , u_int32_t* branch_insn_ptr, u_int32_t* target_codecache);
};


#endif
