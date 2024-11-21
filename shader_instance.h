#pragma once

#include "shader.h"

// 定义属性和 varying 中的纹理坐标 key
constexpr int VARYING_TEXUV = 0;
constexpr int VARYING_COLOR = 1;
constexpr int VARYING_LIGHT = 2;

const Vec3f red_color = {1.0f, 0.0f, 0.0f};
const Vec3f green_color = {0.0f, 1.0f, 0.0f};
const Vec3f blue_color = {0.0f, 0.0f, 1.0f};
const Vec3f white_color = {1.0f, 1.0f, 1.0f};

Mesh cube_mesh({
	{
		{{1, -1, -1,}, {0, 0}, green_color, {}},
		{{-1, -1, -1,}, {0, 1}, white_color, {}},
		{{-1, 1, -1,}, {1, 1}, white_color, {}},
		{{1, 1, -1,}, {1, 0}, white_color, {}},
		{{1, -1, 1,}, {0, 0}, white_color, {}},
		{{-1, -1, 1,}, {0, 1}, white_color, {}},
		{{-1, 1, 1,}, {1, 1}, blue_color, {}},
		{{1, 1, 1,}, {1, 0}, white_color, {}}
	},
	{
		0, 2, 1, 0, 3, 2,
		1, 2, 6, 1, 6, 5,
		0, 7, 3, 0, 4, 7,
		2, 3, 7, 2, 7, 6,
		0, 1, 5, 0, 5, 4,
		4, 5, 6, 4, 6, 7
	}
});

auto vert_normal = [&](int index, const Mesh& mesh, const ShaderInput& input, ShaderContext& output) -> Vec4f
{
	const auto& mat_mvp = input.mat_mvp;
	Vec4f pos = mesh.vertices[index].pos.xyz1() * mat_mvp;
	output.varying_vec2f[VARYING_TEXUV] = mesh.vertices[index].uv;
	output.varying_vec4f[VARYING_COLOR] = mesh.vertices[index].color.xyz1();
	Vec3f normal = mesh.vertices[index].normal;

	return pos;
};

auto frag_normal = [&](ShaderContext& input) -> Vec4f
{
	return input.varying_vec4f[VARYING_COLOR];
};


auto vert_gouraud = [&](int index, const Mesh& mesh, const ShaderInput& input, ShaderContext& output) -> Vec4f
{
	const auto& mat_mvp = input.mat_mvp;
	Vec4f pos = mesh.vertices[index].pos.xyz1() * mat_mvp;
	output.varying_vec2f[VARYING_TEXUV] = mesh.vertices[index].uv;
	output.varying_vec4f[VARYING_COLOR] = mesh.vertices[index].color.xyz1();

	Vec3f normal = mesh.vertices[index].normal;
	normal = (normal.xyz1() * input.mat_model_it).xyz();

	float intense = vector_dot(normal, vector_normalize(input.light_dir));
	intense = Max(0.0f, intense) + 0.1f;

	output.varying_float[VARYING_LIGHT] = Min(1.0f, intense);

	return pos;
};


auto frag_gouraud = [&](ShaderContext& input) -> Vec4f
{
	auto& color = input.varying_vec4f[VARYING_COLOR];
	auto& light = input.varying_float[VARYING_LIGHT];

	return color * light;
};
