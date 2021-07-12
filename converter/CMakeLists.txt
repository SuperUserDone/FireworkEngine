cmake_minimum_required(VERSION 3.1.0)

set(CONVERTER_SRC "convert.cpp")

if(NOT MSVC)
  add_executable(fw-conv ${CONVERTER_SRC})
  target_link_libraries(fw-conv fmt::fmt assimp::assimp CapnProto::kj
                        CapnProto::capnp)

  if(CMAKE_BUILD_TYPE STREQUAL Release)
    add_custom_command(
      TARGET fw-conv
      POST_BUILD
      COMMAND ${CMAKE_STRIP} -v -s fw-conv)
  endif()
endif()
