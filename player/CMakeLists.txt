cmake_minimum_required(VERSION 3.0.3)

set(PLAYER_SRC "player.cpp")

add_executable(firework-player ${PLAYER_SRC})

target_link_libraries(firework-player firework-engine)
