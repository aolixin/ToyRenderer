#pragma once

#include <iostream>
#include <windows.h>
#include <stdlib.h>
#include <memory>
#include <unordered_map>

#include "vertex.h"
#include "shader.h"
#include "color.h"
#include "msaa.h"
#include "shader_instance.h"

class Render
{
private:
	const int quad_size = 2;

	int g_width = 0;
	int g_height = 0;

	HDC g_tempDC = nullptr;
	HBITMAP g_tempBm = nullptr;
	//HBITMAP g_oldBm = nullptr;
	int target_frame_buffer_id = -1;
	int target_depth_buffer_id = -1;

	VertexAttribute g_vertexAttr[3];

	std::vector<Vertex> vertices;
	std::vector<int> indices;

	std::unordered_map<int, uint32_t*> id_frame_buffer;
	std::unordered_map<int, Vec4f**> id_msaa_frame_buffer;

	std::unordered_map<int, HBITMAP> id_DC;

	std::unordered_map<int, float*> id_depth_buffer;
	std::unordered_map<int, float**> id_msaa_depth_buffer;

	std::unordered_map<int, bool**> id_coverage_mask;

	std::vector<Vec2f> quad_uv = std::vector<Vec2f>(quad_size * quad_size);
	std::vector<ShaderContext> quad_shader_context = std::vector<ShaderContext>(quad_size * quad_size);


	bool _msaa_enable = false;

public:
	//ShaderInput shader_input;
	static int id;

	Render(int w, int h)
	{
		g_width = w;
		g_height = h;
	}

	inline void initRenderer(HWND hWnd);
	inline void update(HWND hWnd);
	inline void clearBuffer();
	inline void shutDown();

	inline void drawCall(const Mesh& mesh, const VertexShader& vert, const FragmentShader& frag);
	inline void drawCall_ortho(const Mesh& mesh, const VertexShader& vert, const FragmentShader& frag);

	inline void drawPrimitive(const std::vector<int>&, const Mesh&, const VertexShader&, const FragmentShader&);
	inline void drawPrimitive_ortho(const std::vector<int>& indices2draw, const Mesh& mesh,
	                                const VertexShader& vert_shader,
	                                const FragmentShader& frag_shader);

	inline bool primitive_assembly(const std::vector<int>&, const Mesh&, const VertexShader&);

	inline void drawLineDDA(int x1, int y1, int x2, int y2, uint32_t color);
	inline void drawLineBresenham(int x1, int y1, int x2, int y2, uint32_t color);


	inline void drawPixel(int x, int y, uint32_t color);
	inline void ResolvePixel();

	inline int create_frame_buffer(int, int);
	inline int create_depth_buffer(int, int);

	inline void set_frame_buffer(int switch_id);
	inline void set_depth_buffer(int id);

	inline void msaa_enable(bool enable);

	inline bool inTriangle(const Vec2f&, const Vec2f&, const Vec2f&, const float&, const float&);
	inline void rasterization_interpolation(const Vec2f&, const Vec2f&, const Vec2f&, int, int,
	                                        ShaderContext& frag_input);
};

// 初始化 id
int Render::id = 0;


void Render::initRenderer(HWND hWnd)
{
	id_depth_buffer.clear();
	id_frame_buffer.clear();
	id_msaa_frame_buffer.clear();
	id_coverage_mask.clear();


	if (g_width == 0 || g_height == 0)return;

	HDC hDC = GetDC(hWnd);
	g_tempDC = CreateCompatibleDC(hDC);
	ReleaseDC(hWnd, hDC);
}

void Render::msaa_enable(bool enable = false)
{
	this->_msaa_enable = enable;
}

int Render::create_frame_buffer(int w, int h)
{
	if (!_msaa_enable)
	{
		uint32_t* ptr = nullptr;
		BITMAPINFO bi = {
			{
				sizeof(BITMAPINFOHEADER), g_width, -g_height, 1, 32, BI_RGB,
				(DWORD)g_width * g_height * 4, 0, 0, 0, 0
			}
		};
		HBITMAP bt = CreateDIBSection(g_tempDC, &bi, DIB_RGB_COLORS, (void**)&ptr, 0, 0);

		id_frame_buffer[id] = ptr;
		id_DC[id] = bt;
		id++;
		return id - 1;
	}
	else
	{
		// build buffer
		uint32_t* ptr = nullptr;
		BITMAPINFO bi = {
			{
				sizeof(BITMAPINFOHEADER), g_width, -g_height, 1, 32, BI_RGB,
				(DWORD)g_width * g_height * 4, 0, 0, 0, 0
			}
		};
		HBITMAP bt = CreateDIBSection(g_tempDC, &bi, DIB_RGB_COLORS, (void**)&ptr, 0, 0);

		// msaa buffer
		Vec4f** msaa_ptr = new Vec4f*[w * h];
		for (size_t i = 0; i < w * h; i++)
		{
			msaa_ptr[i] = new Vec4f[MULTISAPLE];
		}
		// coverage mask
		bool** mask = new bool*[w * h];
		for (size_t i = 0; i < w * h; i++)
		{
			mask[i] = new bool[MULTISAPLE];
		}

		// store buffer id
		id_frame_buffer[id] = ptr;
		id_DC[id] = bt;
		id_msaa_frame_buffer[id] = msaa_ptr;
		id_coverage_mask[id] = mask;

		id++;
		return id - 1;
	}
}

int Render::create_depth_buffer(int w, int h)
{
	if (!_msaa_enable)
	{
		float* depth_ptr = new float[w * h];
		id_depth_buffer[id] = depth_ptr;
		id++;
		return id - 1;
	}
	else
	{
		float* depth_ptr = new float[w * h];
		id_depth_buffer[id] = depth_ptr;

		float** msaa_ptr = new float*[w * h];
		for (size_t i = 0; i < w * h; i++)
		{
			msaa_ptr[i] = new float[MULTISAPLE];
		}
		id_depth_buffer[id] = depth_ptr;
		id_msaa_depth_buffer[id] = msaa_ptr;

		id++;
		return id - 1;
	}
}

void Render::set_frame_buffer(int switch_id)
{
	if (this->target_frame_buffer_id == switch_id)return;
	this->target_frame_buffer_id = switch_id;
	SelectObject(g_tempDC, id_DC[this->target_frame_buffer_id]);
}

void Render::set_depth_buffer(int switch_id)
{
	this->target_depth_buffer_id = switch_id;
}


void Render::drawPixel(int x, int y, uint32_t color)
{
	auto& g_frameBuff = id_frame_buffer[this->target_frame_buffer_id];
	if (x < 0 || x >= g_width || y < 0 || y >= g_height) return;

	int idx = y * g_width + x;
	g_frameBuff[idx] = color;
}

void Render::ResolvePixel()
{
	auto& g_frameBuff = id_frame_buffer[this->target_frame_buffer_id];
	auto& g_msaa_frame = id_msaa_frame_buffer[this->target_frame_buffer_id];
	auto& g_covergae_mask = id_coverage_mask[this->target_frame_buffer_id];
	Vec4f color = {0.0f, 0.0f, 0.0f, 0.0f};
	for (int cy = 0; cy < g_height; ++cy)
	{
		for (int cx = 0; cx < g_width; ++cx)
		{
			int idx = cy * g_width + cx;
			color = {0.0f, 0.0f, 0.0f, 0.0f};
			int cnt = 0;
			for (int k = 0; k < MULTISAPLE; k++)
			{
				if (g_covergae_mask[idx][k])cnt++;
				color += g_msaa_frame[idx][k];
			}
			color.a = 1.0f;

			auto co = vector_to_color(color / (float)MULTISAPLE);

			g_frameBuff[idx] = vector_to_color(color / (float)MULTISAPLE);
		}
	}
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
	auto& g_frameBuff = id_frame_buffer[this->target_frame_buffer_id];
	auto& g_depthBuff = id_depth_buffer[this->target_depth_buffer_id];
	auto& g_msaa_frameBuff = id_msaa_frame_buffer[this->target_frame_buffer_id];
	auto& g_msaa_depthBuff = id_msaa_depth_buffer[this->target_depth_buffer_id];
	auto& g_coverage_mask = id_coverage_mask[this->target_frame_buffer_id];
	for (int row = 0; row < g_height; ++row)
	{
		for (int col = 0; col < g_width; ++col)
		{
			int idx = row * g_width + col;
			// 默认背景色浅蓝 R123 G195 B221
			g_frameBuff[idx] = _msaa_enable ? 0 : bgColor;
			// 深度缓冲区 0.0f
			g_depthBuff[idx] = 0.0f;
			// msaa
			if (_msaa_enable)
			{
				for (int i = 0; i < MULTISAPLE; i++)
				{
					g_msaa_frameBuff[idx][i] = {0.0f, 0.0f, 0.0f, 0.0f};
					g_msaa_depthBuff[idx][i] = 0.0f;
					g_coverage_mask[idx][i] = false;
				}
			}
		}
	}
}

void Render::shutDown()
{
	// 释放资源
	if (g_tempDC)
	{
		DeleteDC(g_tempDC);
		g_tempDC = nullptr;
	}

	if (g_tempBm)
	{
		DeleteObject(g_tempBm);
		g_tempBm = nullptr;
	}

	for (auto& it : id_frame_buffer)
	{
		DeleteObject(it.second);
	}

	for (auto& it : id_depth_buffer)
	{
		delete[] it.second;
	}

	id_frame_buffer.clear();
	id_depth_buffer.clear();
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

void Render::drawPrimitive(const std::vector<int>& indices2draw, const Mesh& mesh, const VertexShader& vert_shader,
                           const FragmentShader& frag_shader)
{
	// 逐顶点操作 和 图元装配
	bool pass = primitive_assembly(indices2draw, mesh, vert_shader);
	if (!pass)return;

	int _min_x = 0, _max_x = 0, _min_y = 0, _max_y = 0;
	_min_x = g_width + 2;
	_max_x = -1;
	_min_y = g_height + 2;
	_max_y = -1;

	for (auto& vert_attr : g_vertexAttr)
	{
		_min_x = Between(0, g_width - 1, Min(_min_x, vert_attr.spi.x));
		_max_x = Between(0, g_width - 1, Max(_max_x, vert_attr.spi.x));
		_min_y = Between(0, g_height - 1, Min(_min_y, vert_attr.spi.y));
		_max_y = Between(0, g_height - 1, Max(_max_y, vert_attr.spi.y));
	}

	// 光栅化
	//const Vec2i& p0 = g_vertexAttr[0].spi;
	//const Vec2i& p1 = g_vertexAttr[1].spi;
	//const Vec2i& p2 = g_vertexAttr[2].spi;

	const Vec2f& f0 = g_vertexAttr[0].spf;
	const Vec2f& f1 = g_vertexAttr[1].spf;
	const Vec2f& f2 = g_vertexAttr[2].spf;

	for (int quad_y = _min_y; quad_y <= _max_y; quad_y += quad_size)
	{
		for (int quad_x = _min_x; quad_x <= _max_x; quad_x += quad_size)
		{
			// 插值
			rasterization_interpolation(f0, f1, f2, quad_x, quad_y, quad_shader_context[0]);
			rasterization_interpolation(f0, f1, f2, quad_x + 1, quad_y, quad_shader_context[1]);
			rasterization_interpolation(f0, f1, f2, quad_x, quad_y + 1, quad_shader_context[2]);
			rasterization_interpolation(f0, f1, f2, quad_x + 1, quad_y + 1, quad_shader_context[3]);

			bool inside = quad_shader_context[0].inside ||
				quad_shader_context[1].inside ||
				quad_shader_context[2].inside ||
				quad_shader_context[3].inside;
			if (!inside)continue;

			// quad uv
			quad_uv[0] = quad_shader_context[0].varying_vec2f[VARYING_TEXUV];
			quad_uv[1] = quad_shader_context[1].varying_vec2f[VARYING_TEXUV];
			quad_uv[2] = quad_shader_context[2].varying_vec2f[VARYING_TEXUV];
			quad_uv[3] = quad_shader_context[3].varying_vec2f[VARYING_TEXUV];

			float du = quad_uv[1].u - quad_uv[0].u;
			float dv = quad_uv[2].u - quad_uv[0].u;
			// frag
			for (int i = 0; i < quad_size * quad_size; i++)
			{
				auto& frag_input = quad_shader_context[i];
				if (!frag_input.inside)continue;

				//frag_input.varying_vec2f[VARYING_DUDV] = Vec2f{du, dv};
				auto color = frag_shader(frag_input);
				drawPixel(quad_x + i % quad_size, quad_y + i / quad_size, vector_to_color(color));
			}
		}
	}


	// 3. 绘制三角形边框
	//drawLineBresenham(g_vertexAttr[0].spi.x, g_vertexAttr[0].spi.y, g_vertexAttr[1].spi.x, g_vertexAttr[1].spi.y,
	//                  0x000000);
	//drawLineBresenham(g_vertexAttr[1].spi.x, g_vertexAttr[1].spi.y, g_vertexAttr[2].spi.x, g_vertexAttr[2].spi.y,
	//                  0x000000);
	//drawLineBresenham(g_vertexAttr[2].spi.x, g_vertexAttr[2].spi.y, g_vertexAttr[0].spi.x, g_vertexAttr[0].spi.y,
	//                  0x000000);
}

bool Render::primitive_assembly(const std::vector<int>& indices2draw, const Mesh& mesh, const VertexShader& vert_shader)
{
	// 顶点着色器
	for (int k = 0; k < 3; ++k)
	{
		int idx = indices2draw[k];
		auto& vert_attri = g_vertexAttr[k];
		vert_attri.context.varying_float.clear();
		vert_attri.context.varying_vec2f.clear();
		vert_attri.context.varying_vec3f.clear();
		vert_attri.context.varying_vec4f.clear();

		vert_attri.pos = vert_shader(idx, vert_attri.context);

		float w = Abs(vert_attri.pos.w);
		if (w == 0.0f)return false;

		if (vert_attri.pos.z < 0 || vert_attri.pos.z > w)return false;
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
	}

	// 背面剔除
	Vec4f v01 = g_vertexAttr[1].pos - g_vertexAttr[0].pos;
	Vec4f v02 = g_vertexAttr[2].pos - g_vertexAttr[0].pos;
	Vec4f normal = vector_cross(v01, v02);
	if (normal.z >= 0)return false;

	return true;
}

void Render::rasterization_interpolation(const Vec2f& f0, const Vec2f& f1, const Vec2f& f2, int cx, int cy,
                                         ShaderContext& frag_input)
{
	frag_input.Reset();

	auto& target_depth = id_depth_buffer[this->target_depth_buffer_id];

	if (!inTriangle(f0, f1, f2, cx + 0.5f, cy + 0.5f))
	{
		frag_input.inside = false;
	}

	Vec2f cxy = {(float)cx + 0.5f, (float)cy + 0.5f};

	Vec2f s0 = f0 - cxy;
	Vec2f s1 = f1 - cxy;
	Vec2f s2 = f2 - cxy;

	float a = Abs(vector_cross(s1, s2));
	float b = Abs(vector_cross(s2, s0));
	float c = Abs(vector_cross(s0, s1));

	float s = a + b + c;
	if (s == 0.0f)return;

	a = a * (1.0f / s);
	b = b * (1.0f / s);
	c = c * (1.0f / s);

	// 计算当前点的 1/w，因 1/w 和屏幕空间呈线性关系，故直接重心插值
	float rhw = g_vertexAttr[0].pos.z * a + g_vertexAttr[1].pos.z * b + g_vertexAttr[2].pos.z * c;

	if (cx < 0 || cx >= g_width || cy < 0 || cy >= g_height)
	{
		frag_input.inside = false;
	}
	else
	{
		if (rhw < target_depth[cy * g_width + cx])
		{
			frag_input.inside = false;
		}
		else if (frag_input.inside)
		{
			target_depth[cy * g_width + cx] = rhw;
		}
	}


	// 还原当前像素的 w
	float w = 1.0f / ((rhw != 0.0f) ? rhw : 1.0f);

	// 计算插值系数
	float c0 = g_vertexAttr[0].pos.z * a * w;
	float c1 = g_vertexAttr[1].pos.z * b * w;
	float c2 = g_vertexAttr[2].pos.z * c * w;


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
}


void Render::drawCall_ortho(const Mesh& mesh, const VertexShader& vert, const FragmentShader& frag)
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

void Render::drawPrimitive_ortho(const std::vector<int>& indices2draw, const Mesh& mesh,
                                 const VertexShader& vert_shader,
                                 const FragmentShader& frag_shader)
{
	int _min_x = 0, _max_x = 0, _min_y = 0, _max_y = 0;

	// 顶点着色器
	for (int k = 0; k < 3; ++k)
	{
		int idx = indices2draw[k];
		auto& vert_attri = g_vertexAttr[k];
		vert_attri.context.varying_float.clear();
		vert_attri.context.varying_vec2f.clear();
		vert_attri.context.varying_vec3f.clear();
		vert_attri.context.varying_vec4f.clear();

		vert_attri.pos = vert_shader(idx, vert_attri.context);

		float w = Abs(vert_attri.pos.w);
		if (w == 0.0f)return;

		if (vert_attri.pos.z < 0 || vert_attri.pos.z > w)return;
		//if (vert_attri.pos.x < -w || vert_attri.pos.x > w)return;
		//if (vert_attri.pos.y < -w || vert_attri.pos.y > w)return;

		// 齐次除法
		//vert_attri.rhw = 1.0f / w;
		//float rhw = 1.0f / w;
		//vert_attri.pos *= rhw;

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


	// 光栅化

	const Vec2i& p0 = g_vertexAttr[0].spi;
	const Vec2i& p1 = g_vertexAttr[1].spi;
	const Vec2i& p2 = g_vertexAttr[2].spi;

	const Vec2f& f0 = g_vertexAttr[0].spf;
	const Vec2f& f1 = g_vertexAttr[1].spf;
	const Vec2f& f2 = g_vertexAttr[2].spf;

	auto& g_frameBuff = id_frame_buffer[this->target_frame_buffer_id];
	auto& g_depthBuff = id_depth_buffer[this->target_depth_buffer_id];

	for (int cy = _min_y; cy <= _max_y; cy++)
	{
		for (int cx = _min_x; cx <= _max_x; cx++)
		{
			// 判断是不是在三角形内部

			int res01 = (p0.x - cx) * (p1.y - cy) - (p1.x - cx) * (p0.y - cy);
			int res12 = (p1.x - cx) * (p2.y - cy) - (p2.x - cx) * (p1.y - cy);
			int res20 = (p2.x - cx) * (p0.y - cy) - (p0.x - cx) * (p2.y - cy);
			if (res01 > 0 || res12 > 0 || res20 > 0)continue;

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

			// 深度测试
			if (rhw < g_depthBuff[cy * g_width + cx]) continue;
			g_depthBuff[cy * g_width + cx] = rhw;

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
	//drawLineBresenham(g_vertexAttr[0].spi.x, g_vertexAttr[0].spi.y, g_vertexAttr[1].spi.x, g_vertexAttr[1].spi.y,
	//                  0x000000);
	//drawLineBresenham(g_vertexAttr[1].spi.x, g_vertexAttr[1].spi.y, g_vertexAttr[2].spi.x, g_vertexAttr[2].spi.y,
	//                  0x000000);
	//drawLineBresenham(g_vertexAttr[2].spi.x, g_vertexAttr[2].spi.y, g_vertexAttr[0].spi.x, g_vertexAttr[0].spi.y,
	//                  0x000000);
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


bool Render::inTriangle(const Vec2f& f0, const Vec2f& f1, const Vec2f& f2, const float& x, const float& y)
{
	float res01 = (f0.x - x) * (f1.y - y) - (f1.x - x) * (f0.y - y);
	float res12 = (f1.x - x) * (f2.y - y) - (f2.x - x) * (f1.y - y);
	float res20 = (f2.x - x) * (f0.y - y) - (f0.x - x) * (f2.y - y);
	if (res01 > 0 || res12 > 0 || res20 > 0)return false;
	return true;
}
