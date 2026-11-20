cmake_minimum_required (VERSION 3.18)

#
# gdi
#

set(PROJECT_NAME ui_gdi)
project(${PROJECT_NAME})
file(GLOB SOURCES *.cpp *.h)

add_executable(${PROJECT_NAME}
    ${SOURCES}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE UI_BACKEND_GDIPLUS)

target_link_libraries(${PROJECT_NAME} PRIVATE
    minimgui
    minimgui-gdi
    minimgui-win32
    gdiplus
    msimg32
    gdi32
)

if(MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})
    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
    set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
else()
    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
endif()

#
# gdiplus
#

set(PROJECT_NAME ui_gdiplus)
project(${PROJECT_NAME})
file(GLOB SOURCES *.cpp *.h)

add_executable(${PROJECT_NAME}
    ${SOURCES}
)

target_compile_definitions(${PROJECT_NAME} PRIVATE UI_BACKEND_GDIPLUS)

target_link_libraries(${PROJECT_NAME} PRIVATE
    minimgui
    minimgui-gdiplus
    minimgui-win32
    gdiplus
    gdi32
)

if(MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})
    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
    set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
else()
    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
endif()

