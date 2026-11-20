cmake_minimum_required (VERSION 3.18)

option(BUILD_EXSAMPLES "build exsamples" ON)

message(STATUS "================================")
message(STATUS "CMAKE_SOURCE_DIR      = ${CMAKE_SOURCE_DIR}")
message(STATUS "CMAKE_BINARY_DIR      = ${CMAKE_BINARY_DIR}")
message(STATUS "CMAKE_CXX_COMPILER_ID = ${CMAKE_CXX_COMPILER_ID}")
if(CMAKE_SIZEOF_VOID_P EQUAL 8)
    MESSAGE("64 bits compiler detected")
    SET(EX_PLATFORM 64)
    SET(EX_PLATFORM_NAME "x64")
else()
    MESSAGE("32 bits compiler detected")
    SET(EX_PLATFORM 32)
    SET(EX_PLATFORM_NAME "x86")
endif()
message(STATUS "================================")

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)
set(LIB_DIR ${CMAKE_SOURCE_DIR}/lib)

set(UI_DIR ${CMAKE_SOURCE_DIR}/minimgui)

set(BIN_DIR ${CMAKE_BINARY_DIR})

# include
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${INCLUDE_DIR}
    ${UI_DIR}
)

if(MSVC)
    link_directories(
        ${LIB_DIR}/windows/msvc/${EX_PLATFORM_NAME}
        ${CMAKE_BINARY_DIR}
    )
else()
    link_directories(
        ${LIB_DIR}/windows/gcc/${EX_PLATFORM_NAME}
        ${CMAKE_BINARY_DIR}
    )
endif()

#
# minimgui
#

project(minimgui)

file(GLOB UI_HEADERS ${UI_DIR}/*.h)
file(GLOB UI_SOURCES ${UI_DIR}/*.cpp)
add_library(minimgui
    ${UI_HEADERS}
    ${UI_SOURCES}
)

#set_target_properties(minimgui PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${BIN_DIR}/lib/${EX_PLATFORM_NAME})

# win32

project(minimgui-win32)

set(win32_headers ${UI_DIR}/backends/ui_impl_win32.h)
set(win32_sources ${UI_DIR}/backends/ui_impl_win32.cpp)
add_library(minimgui-win32
    ${win32_headers}
    ${win32_sources}
)

# glfw

project(minimgui-glfw)

set(glfw_headers ${UI_DIR}/backends/ui_impl_glfw.h)
set(glfw_sources ${UI_DIR}/backends/ui_impl_glfw.cpp)
add_library(minimgui-glfw
    ${glfw_headers}
    ${glfw_sources}
)

# gdi

project(minimgui-gdi)

set(gdi_headers ${UI_DIR}/backends/ui_impl_gdi.h)
set(gdi_sources ${UI_DIR}/backends/ui_impl_gdi.cpp)
add_library(minimgui-gdi
    ${gdi_headers}
    ${gdi_sources}
)

# gdiplus

project(minimgui-gdiplus)

set(gdiplus_headers ${UI_DIR}/backends/ui_impl_gdiplus.h)
set(gdiplus_sources ${UI_DIR}/backends/ui_impl_gdiplus.cpp)
add_library(minimgui-gdiplus
    ${gdiplus_headers}
    ${gdiplus_sources}
)

# gl2

project(minimgui-gl2)

set(gl2_headers ${UI_DIR}/backends/ui_impl_gl2.h)
set(gl2_sources ${UI_DIR}/backends/ui_impl_gl2.cpp)
add_library(minimgui-gl2
    ${gl2_headers}
    ${gl2_sources}
)

#
# exsamples
#

if (BUILD_EXSAMPLES)
    add_subdirectory(exsamples)
    file(COPY ${CMAKE_SOURCE_DIR}/exsamples/assets DESTINATION ${BIN_DIR})
endif()
