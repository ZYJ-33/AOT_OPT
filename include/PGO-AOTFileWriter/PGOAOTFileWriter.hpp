#ifndef PGO_AOT_FILE_WRITER
#define PGO_AOT_FILE_WRITER
#include<cstdlib>
#include "AOT_class/aot_class.hpp"
class PGOAOTFileWriter
{
    public:
           static void write_to_buf(char* buf, AOT_File& file);
};
#endif
