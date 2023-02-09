#ifndef SEGMENT_WRITER
#define SEGMENT_WRITER
#include "AOTFileWriter/AOTFilerWriter.hpp"
#include "AOTFileWriter/TBWriter.hpp"
#include "AOT_class/aot_class.hpp"

class SegmentsWriter
{
    public:
            static void write_to_buf(char* buf, std::vector<Segment*>& seg, AOT_Header* hdr); 
};

#endif
