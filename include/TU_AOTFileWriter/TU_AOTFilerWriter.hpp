#ifndef TU_AOT_FILE_WRITER
#define TU_AOT_FILE_WRITER
#include<cstdlib>
#include "AOT_class/aot_class.hpp"
class TU_AOTFileWriter
{
    public:
           static u_int64_t write_to_buf(char* buf, AOT_File& file);
};
#endif
