cmake_minimum_required(VERSION 3.15)
project(chico)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

set(SOURCES
        cia.cc
        cia.h
        config.cc
        config.h
        cpu.cc
        cpu.h
        emulator.cc
        emulator.h
        frame_buffer.cc
        frame_buffer.h
        keyboard.cc
        keyboard.h
        logging.cc
        logging.h
        machine.cc
        machine.h
        main.cc
        sid.cc
        sid.h
        vic_ii.cc
        vic_ii.h
        bus.h bus.cc cia_1.h cia_2.h cia_1.cc cia_2.cc)

add_executable(chico ${SOURCES})
target_link_libraries(chico ${SDL2_LIBRARY})
