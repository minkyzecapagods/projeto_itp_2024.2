cmake_minimum_required(VERSION 3.30)
project(projeto_itp C)

set(CMAKE_C_STANDARD 99)

include_directories(include)

add_executable(Extractor src/EAN-8_code_extractor.c)
add_executable(Generator src/EAN-8_code_generator.c)
add_executable(ID src/identifier.c)