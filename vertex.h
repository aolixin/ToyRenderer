#pragma once
#include <vector>
#include "geometry.h"

class Vertex
{
public:
	Vec3f pos;
	Vec2f uv;
	Vec3f color;
	Vec3f normal;
	// 列表初始化
	Vertex()
	{
	}

	Vertex(const Vec3f& pos, const Vec2f& uv, const Vec3f& color, const Vec3f& normal) : pos(pos), uv(uv), color(color),
		normal(normal)
	{
	}
};


class Mesh
{
public:
	std::vector<Vertex> vertices;
	std::vector<int> indices;

	Mesh()
	{
	}

	Mesh(const std::vector<Vertex>& vertices, const std::vector<int>& indices) : vertices(vertices), indices(indices)
	{
	}
};
