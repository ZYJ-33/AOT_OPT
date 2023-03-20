#ifndef FILE_VISTOR
#define FILE_VISTOR
#include "AOT_class/aot_class.hpp"
class File_Vistor
{
    public:
        virtual void visit(AOT_File& file) = 0;
};
#endif
