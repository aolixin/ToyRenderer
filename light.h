#pragma once
#include "camera.h"

class DirectionalLight
{
public:
	Vec3f direction;
	Vec3f color;
	float intensity;

	Vec3f center;
	float width;
	float height;
	float near;
	float far;

	DirectionalLight(const Vec3f& dir, const Vec3f& color, float intensity) : direction(dir), color(color), intensity(intensity)
	{
		
	}

	void buildAABB(const Camera& cam)
	{
		near = 1.0f;
		far = 10.0f;
		width = 10.0f;
		height = 10.0f;
		center = Vec3f(5, 5, -5);
	}
};
