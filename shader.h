#pragma once

#include <functional>
#include <map>
#include "geometry.h"
#include "vertex.h"

class ShaderContext
{
public:
	std::map<int, float> varying_float;
	std::map<int, Vec2f> varying_vec2f;
	std::map<int, Vec3f> varying_vec3f;
	std::map<int, Vec4f> varying_vec4f;
};

//class ShaderInput
//{
//public:
//	Vec3f light_dir = {2, 0, 2};
//	Vec3f light_color = {0.8, 0.2f, 0.2f};
//	Vec3f eye_pos;
//
//	Mat4x4f mat_model;
//	Mat4x4f mat_view;
//	Mat4x4f mat_proj;
//	Mat4x4f mat_mvp;
//
//	Mat4x4f mat_model_it;
//};

class VertexAttribute
{
public:
	ShaderContext context;
	//float rhw;
	Vec4f pos;
	Vec2f spf;
	Vec2i spi;
};


typedef std::function<Vec4f(int index, const Mesh& mesh, const ShaderInput& input, ShaderContext& output)> VertexShader;


typedef std::function<Vec4f(const ShaderInput& input, ShaderContext& vert_input)> FragmentShader;
