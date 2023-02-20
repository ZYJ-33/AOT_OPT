#ifndef CONST_REG_SET
#define CONST_REG_SET
#include <cstdlib>
#include <cstring>

class ConstRegSet
{
    public:
           int size;
           int64_t regs[32];
           bool is_const[32];
           ConstRegSet();
           void set(u_int8_t which, int64_t imm);
           bool isconst(u_int8_t which);
           int64_t get(u_int8_t which);
           void kill(u_int8_t which);
};

#endif
