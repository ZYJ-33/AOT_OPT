#ifndef PGO_SEGMENT_WRITER
#define PGO_SEGMENT_WRITER
#include "PGO-AOTFileWriter/PGOAOTFileWriter.hpp"
#include "PGO-AOTFileWriter/PGOTBWriter.hpp"
#include "AOT_class/aot_class.hpp"

class PGOSegmentsWriter
{
    public:
            static void write_to_buf(char* buf, std::vector<Segment*>& seg, AOT_Header* hdr); 
};

#endif
