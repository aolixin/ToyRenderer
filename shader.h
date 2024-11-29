#pragma once

#include <functional>
#include <map>
#include "geometry.h"

class ShaderContext
{
public:
	std::map<int, float> varying_float;
	std::map<int, Vec2f> varying_vec2f;
	std::map<int, Vec3f> varying_vec3f;
	std::map<int, Vec4f> varying_vec4f;
	bool inside = true;

	void Reset()
	{
		varying_float.clear();
		varying_vec2f.clear();
		varying_vec3f.clear();
		varying_vec4f.clear();
		inside = true;
	}
};


class VertexAttribute
{
public:
	ShaderContext context;
	Vec4f pos;
	Vec2f spf;
	Vec2i spi;
};


typedef std::function<Vec4f(int index, ShaderContext& output)> VertexShader;

typedef std::function<Vec4f(ShaderContext& vert_input)> FragmentShader;
