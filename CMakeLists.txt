cmake_minimum_required(VERSION 3.30)
project(projeto_itp C)

set(CMAKE_C_STANDARD 99)

include_directories(include)

add_executable(Extractor src/EAN-8_code_extractor.c
        include/funcs.h)
add_executable(Generator src/EAN-8_code_generator.c
        include/funcs.h)
add_executable(ID src/generatorNoValidation.c)
target_link_libraries(ID m)
