#pragma once

#include <iostream>
#include "geometry.h"
#include "tgaimage.h"
#include "vertex.h"

constexpr float PI = 3.1415926;

Mesh create_sphere(float radius, int slices, int stacks)
{
	std::vector<Vertex> vertices;
	std::vector<int> indices;

	// Calculate the angle step for longitude and latitude
	float lonStep = 2 * PI / slices;
	float latStep = PI / stacks;

	// Generate the vertices
	for (int lat = 0; lat <= stacks; ++lat)
	{
		for (int lon = 0; lon <= slices; ++lon)
		{
			// Calculate the position, normal, and texture coordinates of each vertex
			float theta = lat * latStep;
			float phi = lon * lonStep;

			float x = radius * sin(theta) * cos(phi);
			float y = radius * cos(theta);
			float z = radius * sin(theta) * sin(phi);

			float u = static_cast<float>(lon) / static_cast<float>(slices);
			float v = static_cast<float>(lat) / static_cast<float>(stacks);

			Vec3f pos(x, y, z);
			Vec2f uv({u, v});
			Vec3f color(1.0f, 1.0f, 1.0f);
			Vec3f normal(x, y, z);
			vector_normalize(normal);

			vertices.emplace_back(pos, uv, color, normal);
		}
	}

	// Generate the indices
	for (int lat = 0; lat < stacks; ++lat)
	{
		for (int lon = 0; lon < slices; ++lon)
		{
			int topLeft = lat * (slices + 1) + lon;
			int topRight = topLeft + 1;
			int bottomLeft = (lat + 1) * (slices + 1) + lon;
			int bottomRight = bottomLeft + 1;

			indices.push_back(topLeft);
			indices.push_back(bottomLeft);
			indices.push_back(topRight);

			indices.push_back(topRight);
			indices.push_back(bottomLeft);
			indices.push_back(bottomRight);
		}
	}

	return Mesh(vertices, indices);
}


void show_str(const HWND& hWnd, const std::string& fps_str, int x, int y)
{
	HDC hdc = GetDC(hWnd);
	SetTextColor(hdc, RGB(255, 0, 0));
	TextOutA(hdc, x, y, fps_str.c_str(), fps_str.length());
	ReleaseDC(hWnd, hdc);
}

void load_texture(std::string filename, const std::string suffix, TGA::TGAImage& img)
{
	size_t dot = filename.find_last_of(".");
	if (dot == std::string::npos) return;
	std::string texfile = filename.substr(0, dot) + suffix;
	std::cerr << "texture file " << texfile << " loading " << (img.read_tga_file(texfile.c_str()) ? "ok" : "failed") <<
		std::endl;
}