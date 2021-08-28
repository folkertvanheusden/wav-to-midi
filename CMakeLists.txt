# (C) 2021 by folkert van heusden <mail@vanheusden.com>, released under Apache License v2.0
cmake_minimum_required(VERSION 3.9.4)

project(wtm3 VERSION 6.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_definitions("-D_FILE_OFFSET_BITS=64")

set(CMAKE_BUILD_TYPE Debug)

add_compile_options(-Wall -pedantic)

add_executable(wtm3
	fft.cpp
	wtm3.cpp
	)

add_subdirectory(midifile)
include_directories(midifile/include)
target_link_libraries(wtm3 midifile)

include(FindPkgConfig)

pkg_check_modules(SNDFILE REQUIRED sndfile)
target_link_libraries(wtm3 ${SNDFILE_LIBRARIES})
target_include_directories(wtm3 PUBLIC ${SNDFILE_INCLUDE_DIRS})
target_compile_options(wtm3 PUBLIC ${SNDFILE_CFLAGS_OTHER})

pkg_check_modules(FFTW3 REQUIRED fftw3)
target_link_libraries(wtm3 ${FFTW3_LIBRARIES})
target_include_directories(wtm3 PUBLIC ${FFTW3_INCLUDE_DIRS})
target_compile_options(wtm3 PUBLIC ${FFTW3_CFLAGS_OTHER})


target_compile_options(wtm3 PRIVATE "-Wall")
