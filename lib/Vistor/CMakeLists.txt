add_library(vistor
            STATIC
            PeepHoleOPTVistor.cpp FixBranchVistor.cpp AddRelsVistor.cpp AssembleVistor.cpp DisassemblePrintVistor.cpp DisassembleVistor.cpp EntryBlockVistor.cpp RemoveNonVistor.cpp RemoveTailVistor.cpp TB_Vistor.cpp util.cpp)
            
target_link_libraries(vistor aot_class)
target_link_libraries(vistor disassembler)
