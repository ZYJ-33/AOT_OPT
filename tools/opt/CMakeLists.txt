add_executable(opt driver.cpp)
target_link_libraries(opt aot_class)
target_link_libraries(opt vistor)
target_link_libraries(opt AOTFileWriter)
target_link_libraries(opt ConstRegSet)

