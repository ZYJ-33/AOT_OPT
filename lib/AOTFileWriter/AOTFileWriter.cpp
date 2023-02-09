#include "AOTFileWriter/AOTFilerWriter.hpp"
#include "AOTFileWriter/SegmentWriter.hpp"

void AOTFileWriter::write_to_buf(char* buf, AOT_File& file)
{
    AOT_Header* hdr = (AOT_Header*)buf;
    char* buf_next = (char*)(hdr + 1);
    SegmentsWriter::write_to_buf(buf_next, file.get_segments(), hdr);    
}
