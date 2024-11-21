#pragma once
#include <functional>
#include <map>
#include "geometry.h"

struct ShaderContext
{
    std::map<int, float> varying_float;
    std::map<int, Vec2f> varying_vec2f;
    std::map<int, Vec3f> varying_vec3f;
    std::map<int, Vec4f> varying_vec4f;
};

struct VertexAttribute
{
    ShaderContext context;
    float rhw;
    Vec4f pos;
    Vec2f spf;
    Vec2i spi;
};

// 顶点着色器：因为是 C++ 编写，无需传递 attribute，传个 0-2 的顶点序号
// 着色器函数直接在外层根据序号读取相应数据即可，最后需要返回一个坐标 pos
// 各项 varying 设置到 output 里，由渲染器插值后传递给 PS

typedef std::function<Vec4f(int index, ShaderContext& output)> VertexShader;


// 像素着色器：输入 ShaderContext，需要返回 Vec4f 类型的颜色
// 三角形内每个点的 input 具体值会根据前面三个顶点的 output 插值得到

typedef std::function<Vec4f(ShaderContext& input)> FragmentShader;

