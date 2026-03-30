cmake_minimum_required(VERSION 3.24)
project(olcPGEX_MiniAudio)
   
# Pull in miniaudio v0.11.25
set(MINIAUDIO_HEADER "${CMAKE_BINARY_DIR}/_deps/miniaudio/miniaudio.h")
if(NOT EXISTS "${MINIAUDIO_HEADER}")
    message(STATUS "Downloading miniaudio.h...")
    file(DOWNLOAD
        "https://raw.githubusercontent.com/mackron/miniaudio/0.11.25/miniaudio.h"
        "${MINIAUDIO_HEADER}"
        STATUS download_status
    )
    list(GET download_status 0 status_code)
    if(NOT status_code EQUAL 0)
        message(FATAL_ERROR "Failed to download miniaudio.h")
    endif()
endif()

# Pull in olcPixelGameEngine v2.28
set(PGE_HEADER "${CMAKE_BINARY_DIR}/_deps/olcPixelGameEngine/olcPixelGameEngine.h")
if(NOT EXISTS "${PGE_HEADER}")
    message(STATUS "Downloading olcPixelGameEngine.h...")
    file(DOWNLOAD
        "https://raw.githubusercontent.com/OneLoneCoder/olcPixelGameEngine/v2.28/olcPixelGameEngine.h"
        "${PGE_HEADER}"
        STATUS download_status
    )
    list(GET download_status 0 status_code)
    if(NOT status_code EQUAL 0)
        message(FATAL_ERROR "Failed to download olcPixelGameEngine.h")
    endif()
endif()

add_subdirectory(demos)
