#pragma once

#define NOMINMAX // for windows.h

// std
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

#ifndef M_PI
#define M_PI 3.1415926f
#endif

// glad
#include <glad/gl4.6.h>

// glfw
#include <GLFW/glfw3.h>

// glm
#ifndef GLM_ENABLE_EXPERIMENTAL
    #define GLM_ENABLE_EXPERIMENTAL
#endif

// #include <glm/glm.hpp>
// #include <glm/gtc/matrix_transform.hpp>
// #include <glm/gtc/quaternion.hpp>
// #include <glm/gtc/type_ptr.hpp>
// #include <glm/gtx/io.hpp>
#include <minimgui/ui_std.h>

#if defined(CGL_USE_GLM98) || (defined(__BORLANDC__) && !defined(__clang__))

    #include <cgl/external/glm/glm/glm.hpp>
    #include <cgl/external/glm/glm/geometric.hpp>
    #include <cgl/external/glm/glm/trigonometric.hpp>

    #include <cgl/external/glm/glm/gtc/matrix_projection.hpp>
    #include <cgl/external/glm/glm/gtc/matrix_transform.hpp>
    #include <cgl/external/glm/glm/gtc/type_ptr.hpp>

    #include <cgl/external/glm/glm/gtx/euler_angles.hpp>
    #include <cgl/external/glm/glm/gtx/intersect.hpp>
    #include <cgl/external/glm/glm/gtx/matrix_decompose.hpp>
    #include <cgl/external/glm/glm/gtx/normal.hpp>
    #include <cgl/external/glm/glm/gtx/rotate_vector.hpp>
    #include <cgl/external/glm/glm/gtx/quaternion.hpp>

    #include <cgl/external/glm/glm/quat.hpp>

#else // CGL_USE_GLM98

    // glm 1.0.1
    #if !defined(GLM_ENABLE_EXPERIMENTAL)
        #define GLM_ENABLE_EXPERIMENTAL
    #endif

    // #define GLM_EXTERNAL_TEMPLATE
    // #define GLM_CONFIG_SWIZZLE
    // #define GLM_SILENT_WARNINGS

    #include <glm/glm.hpp>
    #include <glm/gtc/matrix_transform.hpp>
    #include <glm/gtc/quaternion.hpp>
    #include <glm/gtc/type_ptr.hpp>

    #include <glm/ext.hpp>
    #include <glm/gtx/intersect.hpp>
    #include <glm/gtx/io.hpp>
    #include <glm/gtx/matrix_decompose.hpp>
    #include <glm/gtx/normal.hpp>
    #include <glm/gtx/quaternion.hpp>
    #include <glm/gtx/rotate_vector.hpp>
    #include <glm/gtx/vector_angle.hpp>

    #ifndef GLM_API
        #define GLM_API     inline
        #define GLM_INS_API inline
    #endif

    #ifndef GLM_CLASS_TYPEDEF
        #define GLM_CLASS_TYPEDEF CGL_CLASS_TYPEDEF
    #endif

#endif // CGL_USE_GLM98

// SOIL2
#include <SOIL2/SOIL2.h>

// imgui

#if defined(__BORLANDC__) || defined(_MSC_VER)

    #pragma comment(lib, "opengl32.lib")
    #pragma comment(lib, "glu32.lib")

    #if defined(_M_AMD64) || defined(_M_X64)
        #pragma comment(lib, "x64/glew32s.lib")
        #pragma comment(lib, "x64/glfw3.lib")
        #pragma comment(lib, "x64/SOIL2.lib")
        // #pragma comment(lib, "x64/imgui.lib")
    #else
        #pragma comment(lib, "x86/glew32s.lib")
        // #pragma comment(lib, "x86/glfw3.lib")
        #pragma comment(lib, "x86/SOIL2.lib")
        // #pragma comment(lib, "x86/imgui.lib")
    #endif
#endif

typedef glm::ivec2 vec2i;
typedef glm::vec2 vec2f;
typedef glm::vec3 vec3f;
typedef glm::vec4 vec4f;

extern bool vsync;   // 是否垂直同步
extern vec2i screen; // 窗口大小
extern vec2f mouse;  // 鼠标位置
extern vec2f lastpos;
extern int mouse_button;

// 顶点
class vertex
{
public:
    vec3f position;
    vec3f normal;
    vec4f color;
    vec2f texcoord;

    vertex() : position(), normal(), color(), texcoord() { }
    vertex(const vec3f& p, const vec3f& n, const vec4f& c, const vec2f& st) : position(p), normal(n), color(c), texcoord(st) { }
};

// 绘制顶点列表
int draw_arrays(int shape, const vertex* vs, size_t pos, size_t size);

// 绘制索引的顶点列表
int draw_elements(int shape, const vertex* vs, const uint32_t* indices, size_t size);

// 绘制立方体
void draw_cube(float x1, float y1, float z1, float x2, float y2, float z2);
