#pragma once

#include <windows.h>
#include <stdlib.h>
#include <tchar.h>
#include <memory>
#include "vertex.h"
#include "shader.h"

class Render
{
private:
	int g_width = 0;
	int g_height = 0;

	HDC g_tempDC = nullptr;
	HBITMAP g_tempBm = nullptr;
	HBITMAP g_oldBm = nullptr;
	uint32_t* g_frameBuff = nullptr;
	std::shared_ptr<float[]> g_depthBuff = nullptr;

	VertexAttribute g_vertexAttr[3];

	std::vector<Vertex> vertices;
	std::vector<int> indices;

public:
	static uint32_t bgColor;

	ShaderInput shader_input;


	Render(int w, int h)
	{
		g_width = w;
		g_height = h;
	}

	// 初始化渲染器 屏幕长宽 屏幕缓冲
	void inline initRenderer(HWND hWnd);
	// 每帧绘制
	void inline update(HWND hWnd);
	// 清理屏幕缓冲
	void inline clearBuffer();
	void inline shutDown();

	void inline drawCall(const Mesh& mesh, const VertexShader& vert, const FragmentShader& frag);

	void inline drawCube(Mesh& mesh, const VertexShader& vert, const FragmentShader& frag);
	void inline drawPlane(int lt, int rt, int rb, int lb);
	void inline drawPrimitive(const std::vector<int>&, const Mesh&, const VertexShader&, const FragmentShader&);

	void inline drawLineDDA(int x1, int y1, int x2, int y2, uint32_t color);
	void inline drawLineBresenham(int x1, int y1, int x2, int y2, uint32_t color);


	void inline drawPixel(int x, int y, uint32_t color);

	bool inline IsTopLeft(const Vec2i& a, const Vec2i& b);
};

uint32_t Render::bgColor = ((123 << 16) | (195 << 8) | 221);

void Render::initRenderer(HWND hWnd)
{
	if (g_width == 0 || g_height == 0)return;
	// 1. 创建一个屏幕缓冲
	// 1.1 创建一个与当前设备兼容的DC
	HDC hDC = GetDC(hWnd);
	g_tempDC = CreateCompatibleDC(hDC);
	ReleaseDC(hWnd, hDC);
	// 1.2 创建该DC的bitmap缓冲  32位色
	BITMAPINFO bi = {
		{
			sizeof(BITMAPINFOHEADER), g_width, -g_height, 1, 32, BI_RGB,
			(DWORD)g_width * g_height * 4, 0, 0, 0, 0
		}
	};
	g_tempBm = CreateDIBSection(g_tempDC, &bi, DIB_RGB_COLORS, (void**)&g_frameBuff, 0, 0);
	// 1.3 选择该bitmap到dc中
	g_oldBm = (HBITMAP)SelectObject(g_tempDC, g_tempBm);
	// 1.4 创建深度缓冲区
	g_depthBuff.reset(new float[g_width * g_height]);

	// 清理屏幕缓冲
	clearBuffer();
}

// 交换缓冲区
void Render::update(HWND hWnd)
{
	// present frameBuffer to screen
	HDC hDC = GetDC(hWnd);
	BitBlt(hDC, 0, 0, g_width, g_height, g_tempDC, 0, 0, SRCCOPY);
	ReleaseDC(hWnd, hDC);
}

void Render::clearBuffer()
{
	for (int row = 0; row < g_height; ++row)
	{
		for (int col = 0; col < g_width; ++col)
		{
			int idx = row * g_width + col;
			// 默认背景色浅蓝 R123 G195 B221
			g_frameBuff[idx] = bgColor;
			// 深度缓冲区 1.0f
			g_depthBuff[idx] = 0.0f;
		}
	}
}

void Render::shutDown()
{
	if (g_tempDC)
	{
		if (g_oldBm)
		{
			SelectObject(g_tempDC, g_oldBm);
			g_oldBm = nullptr;
		}
		DeleteDC(g_tempDC);
		g_tempDC = nullptr;
	}

	if (g_tempBm)
	{
		DeleteObject(g_tempBm);
		g_tempBm = nullptr;
	}
}

void Render::drawCall(const Mesh& mesh, const VertexShader& vert, const FragmentShader& frag)
{
	this->vertices = mesh.vertices;
	this->indices = mesh.indices;

	// 遍历三角形
	for (int i = 0; i < mesh.indices.size(); i += 3)
	{
		auto i0 = mesh.indices[i];
		auto i1 = mesh.indices[i + 1];
		auto i2 = mesh.indices[i + 2];
		drawPrimitive({i0, i1, i2}, mesh, vert, frag);
	}
}

void Render::drawCube(Mesh& mesh, const VertexShader& vert, const FragmentShader& frag)
{
}


void Render::drawPlane(int lt, int rt, int rb, int lb)
{
}

void Render::drawPrimitive(const std::vector<int>& indices2draw, const Mesh& mesh, const VertexShader& vert_shader,
                           const FragmentShader& frag_shader)
{
	int _min_x = 0, _max_x = 0, _min_y = 0, _max_y = 0;

	// 1. 顶点着色器
	for (int k = 0; k < 3; ++k)
	{
		int idx = indices2draw[k];
		auto& vert_attri = g_vertexAttr[k];
		vert_attri.context.varying_float.clear();
		vert_attri.context.varying_vec2f.clear();
		vert_attri.context.varying_vec3f.clear();
		vert_attri.context.varying_vec4f.clear();

		// 顶点着色器
		vert_attri.pos = vert_shader(idx, mesh, shader_input, vert_attri.context);

		float w = Abs(vert_attri.pos.w);
		if (w == 0.0f)return;

		if (vert_attri.pos.z < 0 || vert_attri.pos.z > w)return;
		//if (vert_attri.pos.x < -w || vert_attri.pos.x > w)return;
		//if (vert_attri.pos.y < -w || vert_attri.pos.y > w)return;

		// 齐次除法
		//vert_attri.rhw = 1.0f / w;
		float rhw = 1.0f / w;
		vert_attri.pos *= rhw;

		// 映射到 [ 0,1 ]
		vert_attri.spf.x = (vert_attri.pos.x + 1.0f) * 0.5f;
		vert_attri.spf.y = (vert_attri.pos.y + 1.0f) * 0.5f;

		// 映射到 [0,width], [0,height]
		vert_attri.spf.x *= g_width;
		vert_attri.spf.y *= g_height;

		// 翻转一下
		vert_attri.spf.x = g_width - vert_attri.spf.x;
		vert_attri.spf.y = g_height - vert_attri.spf.y;

		vert_attri.spi.x = (int)(vert_attri.spf.x + 0.5f);
		vert_attri.spi.y = (int)(vert_attri.spf.y + 0.5f);

		if (k == 0)
		{
			_min_x = _max_x = Between(0, g_width - 1, vert_attri.spi.x);
			_min_y = _max_y = Between(0, g_height - 1, vert_attri.spi.y);
		}
		else
		{
			_min_x = Between(0, g_width - 1, Min(_min_x, vert_attri.spi.x));
			_max_x = Between(0, g_width - 1, Max(_max_x, vert_attri.spi.x));
			_min_y = Between(0, g_height - 1, Min(_min_y, vert_attri.spi.y));
			_max_y = Between(0, g_height - 1, Max(_max_y, vert_attri.spi.y));
		}
	}

	// 背面剔除
	Vec4f v01 = g_vertexAttr[1].pos - g_vertexAttr[0].pos;
	Vec4f v02 = g_vertexAttr[2].pos - g_vertexAttr[0].pos;
	Vec4f normal = vector_cross(v01, v02);
	if (normal.z >= 0)return;


	// 2. 光栅化

	const Vec2i& p0 = g_vertexAttr[0].spi;
	const Vec2i& p1 = g_vertexAttr[1].spi;
	const Vec2i& p2 = g_vertexAttr[2].spi;

	const Vec2f& f0 = g_vertexAttr[0].spf;
	const Vec2f& f1 = g_vertexAttr[1].spf;
	const Vec2f& f2 = g_vertexAttr[2].spf;

	for (int cy = _min_y; cy <= _max_y; cy++)
	{
		for (int cx = _min_x; cx <= _max_x; cx++)
		{
			// 判断是不是在三角形内部

			int res01 = (p0.x - cx) * (p1.y - cy) - (p1.x - cx) * (p0.y - cy);
			int res12 = (p1.x - cx) * (p2.y - cy) - (p2.x - cx) * (p1.y - cy);
			int res20 = (p2.x - cx) * (p0.y - cy) - (p0.x - cx) * (p2.y - cy);
			if (res01 >= 0 || res12 >= 0 || res20 >= 0)continue;

			Vec2f cxy = {(float)cx + 0.5f, (float)cy + 0.5f};

			Vec2f s0 = f0 - cxy;
			Vec2f s1 = f1 - cxy;
			Vec2f s2 = f2 - cxy;

			float a = Abs(vector_cross(s1, s2));
			float b = Abs(vector_cross(s2, s0));
			float c = Abs(vector_cross(s0, s1));

			float s = a + b + c;
			if (s == 0.0f)continue;

			a = a * (1.0f / s);
			b = b * (1.0f / s);
			c = c * (1.0f / s);

			// 计算当前点的 1/w，因 1/w 和屏幕空间呈线性关系，故直接重心插值
			float rhw = g_vertexAttr[0].pos.z * a + g_vertexAttr[1].pos.z * b + g_vertexAttr[2].pos.z * c;

			float tmp = g_depthBuff[cy * g_width + cx];

			// 进行深度测试
			if (rhw < g_depthBuff[cy * g_width + cx]) continue;
			g_depthBuff[cy * g_width + cx] = rhw; // 记录 1/w 到深度缓存

			// 还原当前像素的 w
			float w = 1.0f / ((rhw != 0.0f) ? rhw : 1.0f);

			// 计算插值系数
			float c0 = g_vertexAttr[0].pos.z * a * w;
			float c1 = g_vertexAttr[1].pos.z * b * w;
			float c2 = g_vertexAttr[2].pos.z * c * w;

			ShaderContext frag_input;
			ShaderContext& con0 = g_vertexAttr[0].context;
			ShaderContext& con1 = g_vertexAttr[1].context;
			ShaderContext& con2 = g_vertexAttr[2].context;

			for (const auto& it : con0.varying_float)
			{
				int key = it.first;
				float f0 = con0.varying_float[key];
				float f1 = con1.varying_float[key];
				float f2 = con2.varying_float[key];
				frag_input.varying_float[key] = c0 * f0 + c1 * f1 + c2 * f2;
			}

			for (const auto& it : con0.varying_vec2f)
			{
				int key = it.first;
				const auto& f0 = con0.varying_vec2f[key];
				const auto& f1 = con1.varying_vec2f[key];
				const auto& f2 = con2.varying_vec2f[key];
				frag_input.varying_vec2f[key] = c0 * f0 + c1 * f1 + c2 * f2;
			}

			for (const auto& it : con0.varying_vec3f)
			{
				int key = it.first;
				const auto& f0 = con0.varying_vec3f[key];
				const auto& f1 = con1.varying_vec3f[key];
				const auto& f2 = con2.varying_vec3f[key];
				frag_input.varying_vec3f[key] = c0 * f0 + c1 * f1 + c2 * f2;
			}

			for (const auto& it : con0.varying_vec4f)
			{
				int key = it.first;
				const auto& f0 = con0.varying_vec4f[key];
				const auto& f1 = con1.varying_vec4f[key];
				const auto& f2 = con2.varying_vec4f[key];
				frag_input.varying_vec4f[key] = c0 * f0 + c1 * f1 + c2 * f2;
			}

			Vec4f color = {0.0f, 0.0f, 0.0f, 0.0f};
			color = frag_shader(frag_input);

			auto color32 = vector_to_color(color);
			drawPixel(cx, cy, color32);
		}
	}

	// 3. 绘制三角形边框
	drawLineBresenham(g_vertexAttr[0].spi.x, g_vertexAttr[0].spi.y, g_vertexAttr[1].spi.x, g_vertexAttr[1].spi.y,
	                  0x000000);
	drawLineBresenham(g_vertexAttr[1].spi.x, g_vertexAttr[1].spi.y, g_vertexAttr[2].spi.x, g_vertexAttr[2].spi.y,
	                  0x000000);
	drawLineBresenham(g_vertexAttr[2].spi.x, g_vertexAttr[2].spi.y, g_vertexAttr[0].spi.x, g_vertexAttr[0].spi.y,
	                  0x000000);
}

void Render::drawLineDDA(int x1, int y1, int x2, int y2, uint32_t color)
{
	double dx = x2 - x1;
	double dy = y2 - y1;
	double k = dy / dx;

	for (int x = x1; x < x2; x++)
	{
		int y = k * (x - x1) + y1;
		drawPixel(x, y, color);
	}
}


void Render::drawLineBresenham(int x1, int y1, int x2, int y2, uint32_t color)
{
	if (x1 == x2 && y1 == y2)
	{
		drawPixel(x1, y1, color);
	}
	else if (x1 == x2)
	{
		if (y1 > y2) std::swap(y1, y2);
		for (int y = y1; y <= y2; ++y)
			drawPixel(x1, y, color);
	}
	else if (y1 == y2)
	{
		if (x1 > x2) std::swap(x1, x2);
		for (int x = x1; x <= x2; ++x)
			drawPixel(x, y1, color);
	}
	else
	{
		// Bresenham
		int diff = 0;
		int dx = std::abs(x1 - x2);
		int dy = std::abs(y1 - y2);
		if (dx > dy)
		{
			if (x1 > x2) std::swap(x1, x2), std::swap(y1, y2);
			for (int x = x1, y = y1; x < x2; ++x)
			{
				drawPixel(x, y, color);
				diff += dy;
				if (diff >= dx)
				{
					diff -= dx;
					y += (y1 < y2) ? 1 : -1;
				}
			}
			drawPixel(x2, y2, color);
		}
		else
		{
			if (y1 > y2) std::swap(x1, x2), std::swap(y1, y2);
			for (int y = y1, x = x1; y < y2; ++y)
			{
				drawPixel(x, y, color);
				diff += dx;
				if (diff >= dy)
				{
					diff -= dy;
					x += (x1 < x2) ? 1 : -1;
				}
			}
			drawPixel(x2, y2, color);
		}
	}
}

void Render::drawPixel(int x, int y, uint32_t color)
{
	if (x < 0 || x >= g_width || y < 0 || y >= g_height) return;

	int idx = y * g_width + x;
	g_frameBuff[idx] = color;
}


// 判断一条边是不是三角形的左上边 (Top-Left Edge)
bool Render::IsTopLeft(const Vec2i& a, const Vec2i& b)
{
	return ((a.y == b.y) && (a.x < b.x)) || (a.y > b.y);
}
