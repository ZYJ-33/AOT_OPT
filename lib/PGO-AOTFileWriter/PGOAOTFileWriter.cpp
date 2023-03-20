#include "PGO-AOTFileWriter/PGOAOTFileWriter.hpp"
#include "PGO-AOTFileWriter/PGOSegmentWriter.hpp"

void PGOAOTFileWriter::write_to_buf(char* buf, AOT_File& file)
{
    AOT_Header* hdr = (AOT_Header*)buf;
    char* buf_next = (char*)(hdr + 1);
    PGOSegmentsWriter::write_to_buf(buf_next, file.get_segments(), hdr);    
}
