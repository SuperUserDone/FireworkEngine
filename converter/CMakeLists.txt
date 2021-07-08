cmake_minimum_required(VERSION 3.1.0)

set(CONVERTER_SRC "convert.cpp")

add_executable(fw-conv ${CONVERTER_SRC})

target_link_libraries(fw-conv firework-engine assimp)
