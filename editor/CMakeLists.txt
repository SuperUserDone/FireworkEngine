cmake_minimum_required(VERSION 3.0.3)

set(EDITOR_SRC "edit.cpp" "runtime/editor_loop.cpp" "runtime/editor_ui.cpp")

add_executable(marrow-editor ${EDITOR_SRC})
target_link_libraries(marrow-editor blood-engine)
