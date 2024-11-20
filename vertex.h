#pragma once
#include <vector>
#include "geometry.h"

class Vertex
{
    Vec3f pos;
    Vec2f uv;
    Vec3f color;
    Vec3f normal;
};


class Mesh
{
	std::vector<Vertex> vertices;
};