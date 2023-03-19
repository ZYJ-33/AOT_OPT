#include "TU_AOTFileWriter/TU_AOTFilerWriter.hpp"
#include "TU_AOTFileWriter/TU_SegmentWriter.hpp"

u_int64_t TU_AOTFileWriter::write_to_buf(char* buf, AOT_File& file)
{
    AOT_Header* hdr = (AOT_Header*)buf;
    char* buf_next = (char*)(hdr + 1);
    u_int64_t code_end = TU_SegmentsWriter::write_to_buf(buf_next, file.get_segments(), hdr);
    return code_end - (u_int64_t)hdr;
}
