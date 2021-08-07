cmake_minimum_required(VERSION 3.20)
project(lftp-ctrl C)

set(CMAKE_C_STANDARD 99)

include_directories(
        .
        ./include
)

set(SOURCES
        main.c
        src/ctrl.c
        src/repl_loop.c
        src/repl_history.c
        )

add_executable(${PROJECT_NAME} ${SOURCES})
