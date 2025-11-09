cmake_minimum_required(VERSION "4.1.2")


project("test_cmake")

include_directories("D:\\CodeRelated\\CPP_Compiler\\Libraries\\Finished\\SFML\\include")
link_directories("D:\\CodeRelated\\CPP_Compiler\\Libraries\\Finished\\SFML\\lib")

add_executable(test_cmake main.cpp)

target_link_libraries(test_cmake sfml-graphics-s sfml-window-s sfml-system-s)