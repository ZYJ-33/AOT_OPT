#ifndef AOT_FILE_WRITER
#define AOT_FILE_WRITER
#include<cstdlib>
#include "AOT_class/aot_class.hpp"

class AOTFileWriter
{
    public:
           static void write_to_buf(char* buf, AOT_File& file);
};


#endif
