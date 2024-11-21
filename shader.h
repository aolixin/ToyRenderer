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

// ������ɫ������Ϊ�� C++ ��д�����贫�� attribute������ 0-2 �Ķ������
// ��ɫ������ֱ������������Ŷ�ȡ��Ӧ���ݼ��ɣ������Ҫ����һ������ pos
// ���� varying ���õ� output �����Ⱦ����ֵ�󴫵ݸ� PS

typedef std::function<Vec4f(int index, ShaderContext& output)> VertexShader;


// ������ɫ�������� ShaderContext����Ҫ���� Vec4f ���͵���ɫ
// ��������ÿ����� input ����ֵ�����ǰ����������� output ��ֵ�õ�

typedef std::function<Vec4f(ShaderContext& input)> FragmentShader;

