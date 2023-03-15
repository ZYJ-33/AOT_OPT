#ifndef SEGMENT_WRITER
#define SEGMENT_WRITER
#include "TU_AOTFileWriter/TU_AOTFilerWriter.hpp"
#include "TU_AOTFileWriter/TU_TBWriter.hpp"
#include "AOT_class/aot_class.hpp"

class TU_SegmentsWriter
{
    public:
            static void write_to_buf(char* buf, std::vector<Segment*>& seg, AOT_Header* hdr); 
};

#endif
