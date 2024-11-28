#pragma once

#include "shader.h"
#include "color.h"

// 定义属性和 varying 中的纹理坐标 key
constexpr int VARYING_TEXUV = 0;
constexpr int VARYING_COLOR = 1;
constexpr int VARYING_LIGHT = 2;

constexpr int VARYING_UV = 3;
constexpr int VARYING_EYE = 4; // 眼睛相对顶点的位置
constexpr int VARYING_NORMAL = 4; // 眼睛相对顶点的位置


Mesh cube_mesh({
	{
		{{1, -1, -1,}, {0, 0}, blue_color, {}},
		{{-1, -1, -1,}, {0, 1}, white_color, {}},
		{{-1, 1, -1,}, {1, 1}, white_color, {}},
		{{1, 1, -1,}, {1, 0}, white_color, {}},
		{{1, -1, 1,}, {0, 0}, white_color, {}},
		{{-1, -1, 1,}, {0, 1}, white_color, {}},
		{{-1, 1, 1,}, {1, 1}, red_color, {}},
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


Mesh plane_mesh = {
	{
		{{1, -2, -1,}, {1, 1}, white_color, {}},
		{{-1, -2, -1,}, {0, 1}, white_color, {}},
		{{-1, -2, 1,}, {0, 0}, white_color, {}},
		{{1, -2, 1,}, {1, 0}, white_color, {}}
	},
	{
		0, 2, 1, 0, 3, 2
	}
};

Mesh plane_mesh2 = {
	{
		{{1, -1, 1,}, {1, 1}, white_color, {}},
		{{-1, -1, 1,}, {0, 1}, white_color, {}},
		{{-1, 1, 1,}, {0, 0}, white_color, {}},
		{{1, 1, 1,}, {1, 0}, white_color, {}}
	},
	{
		0, 2, 1, 0, 3, 2
	}
};
