#include "Vistor/DisassembleVistor.hpp"
#include "Vistor/DisassemblePrintVistor.hpp"
#include "Vistor/EntryBlockVistor.hpp"
#include "Vistor/RemoveNonVistor.hpp"
#include "Vistor/RemoveTailVistor.hpp"
#include "Vistor/AssembleVistor.hpp"
#include "Vistor/AddRelsVistor.hpp"
#include "Vistor/FixBranchVistor.hpp"
#include "Vistor/Lu12i_Ori_Lu32i_Vistor.hpp"
#include "Vistor/Lu12i_Ori_Vistor.hpp"
#include "Vistor/St_Addi_Vistor.hpp"
#include "Vistor/Ld_Addi_Vistor.hpp"
#include "AOTFileWriter/AOTFilerWriter.hpp"
#include <cstdio>
#include <cassert>
#include <sys/stat.h>

extern std::map<u_int64_t, std::shared_ptr<TB>> x86AddrToTb;

void test_disassemble_then_assemble(AOT_File& file)
{
    DisassmbleVistor dis;
    AssmbleVistor as;
    std::vector<LoongArchInsInfo*> insns;

    for(auto seg_ptr: file.get_segments())
    {
        for(auto tb_ptr :seg_ptr->_tbs)
        {
            u_int32_t* code = (u_int32_t*)tb_ptr->code;   
            u_int32_t size = tb_ptr->code_size >> 2;
            dis.toinsns(code, size, insns);
            u_int32_t* code_after_dis_then_as = as.tocode(insns);
            for(u_int32_t i = 0; i<size; i++)
            {
                if(code_after_dis_then_as[i] != code[i] 
                  && !(insns[i]->opc == OPC_INVALID))
                {
                        std::cout<<"something wrong in dis then as "<<std::endl;
                        std::cout<<"origin: "<<code[i]<<std::endl;
                        std::cout<<"after dis then as: "<<code_after_dis_then_as[i]<<std::endl;
                        exit(1);
                }
                if(insns[i]->opc == OPC_INVALID)
                        std::cout<<"find a invalid insn: " <<code[i]<<std::endl;
            }
            insns.clear();
        }
    }
    
}

size_t get_file_size(const char* f)
{
    struct stat s;
    stat(f, &s);
    return s.st_size;
}

void test_write_to_file(const char* opened_file, const char* write_file)
{
    DisassmbleVistor dis;
    DisassmblePrinterVistor dis_print;
    RemoveTailVistor rm_tail;
    RemoveNonVistor remove_none;
    TB_AddRels_Vistor add_rels_vistor;
    FixBranchVistor fix_b;
    Lu12i_Ori_Lu32i_Vistor first_opt;
    Lu12i_Ori_Vistor second_opt;
    St_Addi_Vistor thrid_opt;
    Ld_Addi_Vistor fouth_opt;

    FILE* file = std::fopen(opened_file, "r+");
    FILE* writefile = std::fopen(write_file, "w+");

    AOT_File aot_file(file);
    for(auto seg_ptr: aot_file.get_segments())
    {
        seg_ptr->settle_all_tb(aot_file.get_rels());
        add_rels_vistor.start(*seg_ptr, &aot_file.get_rels());
        dis.start(*seg_ptr);    

        dis_print.start(*seg_ptr);
        //first_opt.start(*seg_ptr);
        //second_opt.start(*seg_ptr);
        //thrid_opt.start(*seg_ptr);
        fouth_opt.start(*seg_ptr);
        fix_b.start(*seg_ptr);
        std::cout<<"========================================\n";
        dis_print.start(*seg_ptr);
    }

    u_int32_t file_sz = aot_file.how_many_bytes();
    char* buf = new char[file_sz];
    AOTFileWriter::write_to_buf(buf, aot_file);
    fwrite(buf, file_sz, 1, writefile);

    fclose(file);
    fclose(writefile);
}

void test_read_from_file(const char* opened_file)
{
    x86AddrToTb.clear();
    DisassmbleVistor dis;
    TB_AddRels_Vistor add_rels_vistor;
    DisassmblePrinterVistor dis_print;
    Lu12i_Ori_Vistor peep_hole_opt;

    FILE* file = std::fopen(opened_file, "r+");
    AOT_File aot_file(file);
    for(auto seg_ptr: aot_file.get_segments())
    {
        seg_ptr->settle_all_tb(aot_file.get_rels());
        add_rels_vistor.start(*seg_ptr, &aot_file.get_rels());
        dis.start(*seg_ptr);    
        
        peep_hole_opt.start(*seg_ptr);
        dis_print.start(*seg_ptr);
    }
}

int main(int argc, char** argv)
{
    const char* opened_file = "./hello_static.aot";
    const char* write_file = "./write.aot";

    test_write_to_file(opened_file, write_file);
    //test_read_from_file(opened_file);
}
/*
int main(int argc, char** argv)
{
    DisassmbleVistor dis;
    DisassmblePrinterVistor dis_print;
    EntryBlockVistor entrys;
    RemoveNonVistor remove_non;
    RemoveTailVistor remove_tail;
    AssmbleVistor ass;
    TB_AddRels_Vistor add_rels_vistor;

    const char* opened_file = "./hello_static.aot";
    std::cout << "opening "<< opened_file;
    size_t file_size = get_file_size(opened_file);
    FILE* file = std::fopen(opened_file, "r+");
    AOT_File aot_file(file);
    //test_disassemble_then_assemble(aot_file);
    
    
    for(auto seg_ptr: aot_file.get_segments())
    {
        seg_ptr->settle_all_tb(aot_file.get_rels());
        add_rels_vistor.start(*seg_ptr, &aot_file.get_rels());
        dis.start(*seg_ptr);         
        remove_tail.start(*seg_ptr);
        entrys.start(*seg_ptr);
        remove_non.start(*seg_ptr);
        
        for (auto entry: e.entrys)
        {
            p.start(*seg_ptr, entry, &aot_file.get_rels());
        }
        ass.start(*seg_ptr, v, p);
    }
    size_t test_size = aot_file.how_many_bytes();
    std::cout<<aot_file.size_in_file <<" "<<file_size<<" " <<test_size<<std::endl;
    assert(test_size == file_size);
}
*/
