cmake_minimum_required(VERSION 3.24)

project(demos)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

######################################################################
# Shared engine library
######################################################################
add_library(engine STATIC
    olcPGEX_MiniAudio.cpp
    olcPixelGameEngine.cpp
)

target_include_directories(engine PUBLIC
    ${CMAKE_SOURCE_DIR}
    "${CMAKE_BINARY_DIR}/_deps/miniaudio"
    "${CMAKE_BINARY_DIR}/_deps/olcPixelGameEngine"
)

######################################################################
# macOS
######################################################################
if (APPLE)
    set(OpenGL_GL_PREFERENCE LEGACY)
    find_package(OpenGL REQUIRED)
    find_package(GLUT    REQUIRED)
    find_package(Threads REQUIRED)
    find_package(PNG     REQUIRED)
    find_library(CARBON_LIBRARY Carbon REQUIRED)

    target_link_libraries(engine PRIVATE
        OpenGL::GL
        ${GLUT_LIBRARIES}
        Threads::Threads
        PNG::PNG
        ${CARBON_LIBRARY}
    )
endif()

######################################################################
# Windows (MinGW)
######################################################################
if (WIN32 AND MINGW)
    set(OpenGL_GL_PREFERENCE LEGACY)
    find_package(OpenGL REQUIRED)

    target_link_libraries(engine PRIVATE
        OpenGL::GL
        gdiplus shlwapi dwmapi stdc++fs
    )
endif()

######################################################################
# Windows (MSVC)
######################################################################
if (WIN32 AND MSVC)
    set(OpenGL_GL_PREFERENCE LEGACY)
    find_package(OpenGL REQUIRED)

    target_link_libraries(engine PRIVATE OpenGL::GL gdiplus shlwapi dwmapi)
endif()

######################################################################
# Linux
######################################################################
if (UNIX AND NOT APPLE AND NOT EMSCRIPTEN)
    set(OpenGL_GL_PREFERENCE LEGACY)
    find_package(OpenGL REQUIRED)
    find_package(X11    REQUIRED)
    find_package(Threads REQUIRED)
    find_package(PNG    REQUIRED)

    target_link_libraries(engine PRIVATE
        OpenGL::GL
        X11::X11
        Threads::Threads
        PNG::PNG
        stdc++fs dl
    )
endif()

function(add_common_settings target)

    # Debug definitions (all platforms)
    target_compile_definitions(${target} PRIVATE
        $<$<CONFIG:Debug>:MA_DEBUG_OUTPUT>
        $<$<CONFIG:Debug>:DEBUG=1>
    )

    ######################################################################
    # Emscripten
    ######################################################################
    if (EMSCRIPTEN)
        execute_process(COMMAND
            "${EMSCRIPTEN_ROOT_PATH}/embuilder${EMCC_SUFFIX}" build libpng zlib)

        target_link_options(${target} PRIVATE
            -sALLOW_MEMORY_GROWTH=1
            -sSTACK_SIZE=131072
            -sEXPORTED_RUNTIME_METHODS=HEAPF32
            -sMAX_WEBGL_VERSION=2
            -sMIN_WEBGL_VERSION=2
            -sUSE_LIBPNG=1
            -sLLD_REPORT_UNDEFINED
            --shell-file "${CMAKE_CURRENT_SOURCE_DIR}/shell.html"
            --preload-file "${CMAKE_CURRENT_SOURCE_DIR}/assets@assets"
        )
    else()
        add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory
                "${CMAKE_CURRENT_SOURCE_DIR}/assets"
                "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR}/assets"
        )
    endif()

    target_link_libraries(${target} PRIVATE engine)

endfunction()

######################################################################
# Targets
######################################################################
if (EMSCRIPTEN)
    set(CMAKE_EXECUTABLE_SUFFIX .html)
endif()

foreach(demo IN ITEMS demo demo_synthesis demo_waveform demo_spatialization)
    add_executable(${demo} ${demo}.cpp)
    add_common_settings(${demo})
endforeach()
