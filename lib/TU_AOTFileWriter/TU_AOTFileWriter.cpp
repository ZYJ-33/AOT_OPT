#include "TU_AOTFileWriter/TU_AOTFilerWriter.hpp"
#include "TU_AOTFileWriter/TU_SegmentWriter.hpp"

void TU_AOTFileWriter::write_to_buf(char* buf, AOT_File& file)
{
    AOT_Header* hdr = (AOT_Header*)buf;
    char* buf_next = (char*)(hdr + 1);
    TU_SegmentsWriter::write_to_buf(buf_next, file.get_segments(), hdr);    
}
