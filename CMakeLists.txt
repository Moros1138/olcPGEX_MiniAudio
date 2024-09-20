# require version 3.10 or higher
cmake_minimum_required(VERSION 3.10)

project(olcPGEX_MiniAudio)

option(UPDATE_GIT_SUBMODULES "Update Git submodules" ON)

if (UPDATE_GIT_SUBMODULES)
    message(STATUS "Updating Git submodules...")
    execute_process(
        COMMAND git submodule update --init --recursive
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        RESULT_VARIABLE result
    )
    
    if(NOT result EQUAL 0)
        message(FATAL_ERROR "Failed to update Git submodules")
    endif()
endif()

# Set C++ Standards
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

function(add_common_settings target)

    ######################################################################
    # MacOS
    ######################################################################
    if(APPLE)
        
        # OpenGL
        set(OpenGL_GL_PREFERENCE LEGACY)
        find_package(OpenGL REQUIRED)
        include_directories(${OpenGL_INCLUDE_DIRS})
        target_link_libraries(${target} ${OpenGL_LIBRARIES} OpenGL::GL)

        # Carbon
        FIND_LIBRARY(CARBON_LIBRARY Carbon)
        target_link_libraries(${target} ${CARBON_LIBRARY})

        # GLUT
        find_package(GLUT REQUIRED)
        target_link_libraries(${target} ${GLUT_LIBRARIES})

        # Threads
        find_package(Threads REQUIRED)
        target_link_libraries(${target} Threads::Threads)
        include_directories(${Threads_INCLUDE_DIRS})
        
        find_package(PNG REQUIRED)
        target_link_libraries(${target} PNG::PNG)
        include_directories(${PNG_INCLUDE_DIRS})

        # debug mode!
        target_compile_definitions(${target} PRIVATE $<$<CONFIG:Debug>:-DMA_DEBUG_OUTPUT>)
        target_compile_definitions(${target} PRIVATE $<$<CONFIG:Debug>:-DDEBUG=1>)

    endif() # APPLE


    ######################################################################
    # Windows: MinGW
    ######################################################################
    if(WIN32 AND MINGW)
        
        # OpenGL
        set(OpenGL_GL_PREFERENCE LEGACY)
        find_package(OpenGL REQUIRED)
        include_directories(${OpenGL_INCLUDE_DIRS})
        target_link_libraries(${target} ${OpenGL_LIBRARIES} OpenGL::GL)

        if (NOT HAS_TERMINAL)
            target_link_libraries(${target} -mwindows -municode)
        endif (NOT HAS_TERMINAL)

        # GDI+
        set(GDIPLUS_LIBRARY gdiplus)
        target_link_libraries(${target} ${GDIPLUS_LIBRARY})
        
        # Shlwapi
        set(SHLWAPI_LIBRARY shlwapi)
        target_link_libraries(${target} ${SHLWAPI_LIBRARY})
        
        # Dwmapi
        set(DWMAPI_LIBRARY dwmapi)
        target_link_libraries(${target} ${DWMAPI_LIBRARY})

        # stdc++fs
        target_link_libraries(${target} stdc++fs)

        # debug mode!
        target_compile_definitions(${target} PRIVATE $<$<CONFIG:Debug>:-DMA_DEBUG_OUTPUT>)
        target_compile_definitions(${target} PRIVATE $<$<CONFIG:Debug>:-DDEBUG=1>)
        
    endif()

    ######################################################################
    # Windows: Visual Studio / MSVC
    ######################################################################
    if(WIN32 AND MSVC)
        
        # OpenGL
        set(OpenGL_GL_PREFERENCE LEGACY)
        find_package(OpenGL REQUIRED)
        include_directories(${OpenGL_INCLUDE_DIRS})
        target_link_libraries(${target} ${OpenGL_LIBRARIES} OpenGL::GL)

        # set the startup project to the target executable instead of ALL_BUILD
        set_property(
            DIRECTORY
            ${CMAKE_CURRENT_SOURCE_DIR}
            PROPERTY
            VS_STARTUP_PROJECT
            ${target}
        )
        
        # set working directory for Visual Studio Debugger
        set_target_properties(
            ${target} PROPERTIES
            VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        )
        
        # set subsytem, console if HAS_TERMINAL is true. windows if not
        if (HAS_TERMINAL)
            target_link_options(${target} PRIVATE "/SUBSYSTEM:CONSOLE")
        else ()
            target_link_options(${target} PRIVATE "/SUBSYSTEM:WINDOWS")
        endif ()

        # GDI+
        set(GDIPLUS_LIBRARY gdiplus)
        target_link_libraries(${target} ${GDIPLUS_LIBRARY})
        
        # Shlwapi
        set(SHLWAPI_LIBRARY shlwapi)
        target_link_libraries(${target} ${SHLWAPI_LIBRARY})
        
        # Dwmapi
        set(DWMAPI_LIBRARY dwmapi)
        target_link_libraries(${target} ${DWMAPI_LIBRARY})

        # debug mode!
        target_compile_definitions(${target} PRIVATE $<$<CONFIG:Debug>:-DMA_DEBUG_OUTPUT>)
        target_compile_definitions(${target} PRIVATE $<$<CONFIG:Debug>:-DDEBUG=1>)
        
    endif() # Visual Studio / MSVC

    ######################################################################
    # Linux: using anything?
    ######################################################################
    if(UNIX AND NOT APPLE AND NOT EMSCRIPTEN)
        
        # OpenGL
        set(OpenGL_GL_PREFERENCE LEGACY)
        find_package(OpenGL REQUIRED)
        include_directories(${OpenGL_INCLUDE_DIRS})
        target_link_libraries(${target} ${OpenGL_LIBRARIES} OpenGL::GL)

        # X11
        find_package(X11 REQUIRED)
        target_link_libraries(${target} X11::X11)

        include_directories(${X11_INCLUDE_DIRS})

        # Threads
        find_package(Threads REQUIRED)
        target_link_libraries(${target} Threads::Threads)
        include_directories(${Threads_INCLUDE_DIRS})

        find_package(PNG REQUIRED)
        target_link_libraries(${target} PNG::PNG)
        include_directories(${PNG_INCLUDE_DIRS})

        # stdc++fs
        target_link_libraries(${target} stdc++fs)

        # dl, for miniaudio
        target_link_libraries(${target} dl)
        
        # debug mode!
        target_compile_definitions(${target} PRIVATE $<$<CONFIG:Debug>:-DMA_DEBUG_OUTPUT>)
        target_compile_definitions(${target} PRIVATE $<$<CONFIG:Debug>:-DDEBUG=1>)

    endif() # Linux

    ######################################################################
    # Emscripten
    ######################################################################
    if (EMSCRIPTEN)
        
        # build Cache: libpng, zlib
        execute_process(COMMAND "${EMSCRIPTEN_ROOT_PATH}/embuilder${EMCC_SUFFIX}" build libpng zlib)
        
        # debug mode!
        target_compile_definitions(${target} PRIVATE $<$<CONFIG:Debug>:-DMA_DEBUG_OUTPUT>)
        target_compile_definitions(${target} PRIVATE $<$<CONFIG:Debug>:-DDEBUG=1>)
        
        target_link_options(${target} PRIVATE -sALLOW_MEMORY_GROWTH=1)
        target_link_options(${target} PRIVATE -sSTACK_SIZE=131072)
        target_link_options(${target} PRIVATE -sMAX_WEBGL_VERSION=2)
        target_link_options(${target} PRIVATE -sMIN_WEBGL_VERSION=2)
        target_link_options(${target} PRIVATE -sUSE_LIBPNG=1)
        target_link_options(${target} PRIVATE -sLLD_REPORT_UNDEFINED)
        target_link_options(${target} PRIVATE --shell-file "${CMAKE_CURRENT_SOURCE_DIR}/shell.html")
        target_link_options(${target} PRIVATE --preload-file ${CMAKE_SOURCE_DIR}/demo/assets@assets)

    endif() # Emscripten

    include_directories(${CMAKE_SOURCE_DIR})
    include_directories(third_party/miniaudio)
    include_directories(third_party/olcPixelGameEngine)


    if (NOT EMSCRIPTEN)
        add_custom_command(
            TARGET ${target}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND}
            ARGS -E copy_directory ${CMAKE_SOURCE_DIR}/demo/assets ${CMAKE_BINARY_DIR}/assets
        )
    endif()

endfunction()

# Executable aka binary output
add_executable(
    demo
    demo/demo.cpp
    demo/common.cpp
    olcPGEX_MiniAudio.h
    third_party/olcPixelGameEngine/olcPixelGameEngine.h
    third_party/miniaudio/miniaudio.h
)

add_executable(
    demo_synthesis
    demo/demo_synthesis.cpp
    demo/common.cpp
    olcPGEX_MiniAudio.h
    third_party/olcPixelGameEngine/olcPixelGameEngine.h
    third_party/miniaudio/miniaudio.h
)

add_executable(
    demo_waveform
    demo/demo_waveform.cpp
    demo/common.cpp
    olcPGEX_MiniAudio.h
    third_party/olcPixelGameEngine/olcPixelGameEngine.h
    third_party/miniaudio/miniaudio.h
)

if (EMSCRIPTEN)
    # generate an HTML file
    set(CMAKE_EXECUTABLE_SUFFIX .html)
endif()

add_common_settings(demo)
add_common_settings(demo_synthesis)
add_common_settings(demo_waveform)
