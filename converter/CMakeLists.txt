cmake_minimum_required(VERSION 3.1.0)

set(CONVERTER_SRC "convert.cpp")

if(NOT MSVC)
  add_executable(fw-conv ${CONVERTER_SRC})
  target_link_libraries(fw-conv assimp::assimp CapnProto::kj CapnProto::capnp)
endif()
