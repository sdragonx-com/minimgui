cmake_minimum_required (VERSION 3.18)

set(PROJECT_NAME ui_gl2)
project(${PROJECT_NAME})
file(GLOB SOURCES *.cpp *.h)

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${SRC_DIR}/glad/gl4.6.c
)

#target_include_directories(${PROJECT_NAME}
#    ${INCLUDE_DIR}
#    ${UI_DIR}
#)

target_link_libraries(${PROJECT_NAME} PRIVATE
    minimgui
    minimgui-gl2
    minimgui-glfw
    glfw3
)

if(MSVC)
    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR})
    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})
    set_target_properties(${PROJECT_NAME} PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
else()
    set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BIN_DIR})
endif()

