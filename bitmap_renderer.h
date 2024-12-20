//#pragma once
//
//#ifndef RENDERER_H
//#define RENDERER_H
//
//#include <utility>
//#include<vector>
//#include<map>
//#include <functional>
//#include "geometry.h"
//#include "bitmap.h"
//
//struct ShaderContext
//{
//	std::map<int, float> varying_float;
//	std::map<int, Vec2f> varying_vec2f;
//	std::map<int, Vec3f> varying_vec3f;
//	std::map<int, Vec4f> varying_vec4f;
//};
//
//// 顶点着色器：因为是 C++ 编写，无需传递 attribute，传个 0-2 的顶点序号
//// 着色器函数直接在外层根据序号读取相应数据即可，最后需要返回一个坐标 pos
//// 各项 varying 设置到 output 里，由渲染器插值后传递给 PS 
//typedef std::function<Vec4f(int index, ShaderContext& output)> VertexShader;
//
//
//// 像素着色器：输入 ShaderContext，需要返回 Vec4f 类型的颜色
//// 三角形内每个点的 input 具体值会根据前面三个顶点的 output 插值得到
//typedef std::function<Vec4f(ShaderContext& input)> PixelShader;
//
//class Renderer
//{
//public:
//	inline virtual ~Renderer() { Reset(); }
//
//	inline Renderer()
//	{
//		_frame_buffer = nullptr;
//		_depth_buffer = nullptr;
//		_render_frame = false;
//		_render_pixel = true;
//	}
//
//	inline Renderer(int width, int height)
//	{
//		_frame_buffer = nullptr;
//		_depth_buffer = nullptr;
//		_render_frame = false;
//		_render_pixel = true;
//		Init(width, height);
//	}
//
//	inline void Reset()
//	{
//		_vertex_shader = nullptr;
//		_pixel_shader = nullptr;
//		delete _frame_buffer;
//		_frame_buffer = nullptr;
//		if (_depth_buffer)
//		{
//			for (int j = 0; j < _fb_height; j++)
//			{
//				if (_depth_buffer[j])delete _depth_buffer[j];
//				_depth_buffer[j] = nullptr;
//			}
//			delete[]_depth_buffer;
//			_depth_buffer = nullptr;
//		}
//		_color_fg = 0xffffffff;
//		_color_bg = 0xff191970;
//	}
//
//	inline void Init(int widht, int height)
//	{
//		Reset();
//		_fb_width = widht;
//		_fb_height = height;
//		_frame_buffer = new Bitmap(widht, height);
//		_depth_buffer = new float*[height];
//		for (int j = 0; j < height; j++)
//		{
//			_depth_buffer[j] = new float[widht];
//		}
//		clear();
//	}
//
//	inline void clear()
//	{
//		if (_frame_buffer)
//		{
//			_frame_buffer->Fill(_color_bg);
//		}
//		if (_depth_buffer)
//		{
//			for (int j = 0; j < _fb_height; j++)
//			{
//				for (int i = 0; i < _fb_width; i++)
//				{
//					_depth_buffer[j][i] = 0.0f;
//				}
//			}
//		}
//	}
//
//	inline void SetVertexShader(VertexShader vs)
//	{
//		_vertex_shader = std::move(vs);
//	}
//
//	inline void SetPixelShader(PixelShader ps)
//	{
//		_pixel_shader = std::move(ps);
//	}
//
//	inline void SaveFile(const char* filename)
//	{
//		if (_frame_buffer) _frame_buffer->SaveFile(filename);
//	}
//
//	// 设置背景/前景色
//	inline void SetBGColor(uint32_t color) { _color_bg = color; }
//	inline void SetFGColor(uint32_t color) { _color_fg = color; }
//
//
//	// 判断一条边是不是三角形的左上边 (Top-Left Edge)
//	inline bool IsTopLeft(const Vec2i& a, const Vec2i& b)
//	{
//		return ((a.y == b.y) && (a.x < b.x)) || (a.y > b.y);
//	}
//
//	// FrameBuffer 里画线
//	inline void DrawLine(int x1, int y1, int x2, int y2)
//	{
//		if (_frame_buffer) _frame_buffer->DrawLine(x1, y1, x2, y2, _color_fg);
//	}
//
//
//	// FrameBuffer 里画点
//	inline void SetPixel(int x, int y, uint32_t cc) { if (_frame_buffer) _frame_buffer->SetPixel(x, y, cc); }
//	inline void SetPixel(int x, int y, const Vec4f& cc) { SetPixel(x, y, vector_to_color(cc)); }
//	inline void SetPixel(int x, int y, const Vec3f& cc) { SetPixel(x, y, vector_to_color(cc)); }
//
//	// 设置渲染状态，是否显示线框图，是否填充三角形
//	inline void SetRenderState(bool frame, bool pixel)
//	{
//		_render_frame = frame;
//		_render_pixel = pixel;
//	}
//
//	inline bool DrawPrimitive()
//	{
//		if (_frame_buffer == nullptr || _vertex_shader == nullptr)
//			return false;
//
//		for (int k = 0; k < 3; k++)
//		{
//			Vertex& vertex = _vertex[k];
//			vertex.context.varying_float.clear();
//			vertex.context.varying_vec2f.clear();
//			vertex.context.varying_vec3f.clear();
//			vertex.context.varying_vec4f.clear();
//
//			// vertex_shader
//			vertex.pos = _vertex_shader(k, vertex.context);
//
//			float w = Abs(vertex.pos.w);
//
//			// 裁剪
//			if (w == 0.0f)return false;
//			if (vertex.pos.z < -w || vertex.pos.z > w)return false;
//			if (vertex.pos.x < -w || vertex.pos.x > w)return false;
//			if (vertex.pos.y < -w || vertex.pos.y > w)return false;
//
//			vertex.rhw = 1.0f / w;
//			// 透视除法转换到 ndc 坐标
//			vertex.pos *= vertex.rhw;
//
//			// 计算屏幕坐标
//			vertex.spf.x = (vertex.pos.x + 1.0f) * (float)_fb_width * 0.5f;
//			vertex.spf.y = (vertex.pos.y + 1.0f) * (float)_fb_height * 0.5f;
//
//			// 获取像素坐标
//			vertex.spi.x = (int)(vertex.spf.x + 0.5f);
//			vertex.spi.y = (int)(vertex.spf.y + 0.5f);
//
//			if (k == 0)
//			{
//				_min_x = _max_x = Between(0, _fb_width - 1, vertex.spi.x);
//				_min_y = _max_y = Between(0, _fb_height - 1, vertex.spi.y);
//			}
//			else
//			{
//				_min_x = Between(0, _fb_width - 1, Min(_min_x, vertex.spi.x));
//				_max_x = Between(0, _fb_width - 1, Max(_max_x, vertex.spi.x));
//				_min_y = Between(0, _fb_height - 1, Min(_min_y, vertex.spi.y));
//				_max_y = Between(0, _fb_height - 1, Max(_max_y, vertex.spi.y));
//			}
//		}
//
//
//		if (_render_frame)
//		{
//			DrawLine(_vertex[0].spi.x, _vertex[0].spi.y, _vertex[1].spi.x, _vertex[1].spi.y);
//			DrawLine(_vertex[1].spi.x, _vertex[1].spi.y, _vertex[2].spi.x, _vertex[2].spi.y);
//			DrawLine(_vertex[2].spi.x, _vertex[2].spi.y, _vertex[0].spi.x, _vertex[0].spi.y);
//		}
//
//		if (!_render_pixel) return false;
//
//		Vec4f v01 = _vertex[1].pos - _vertex[0].pos;
//		Vec4f v02 = _vertex[2].pos - _vertex[0].pos;
//		Vec4f normal = vector_cross(v01, v02);
//
//		Vertex* vtx[3] = {&_vertex[0], &_vertex[1], &_vertex[2]};
//
//		if (normal.z > 0.0f)
//		{
//			vtx[1] = &_vertex[2];
//			vtx[2] = &_vertex[1];
//		}
//		else if (normal.z == 0.0f)
//		{
//			return false;
//		}
//
//
//		// 保存三个端点位置
//		Vec2i p0 = vtx[0]->spi;
//		Vec2i p1 = vtx[1]->spi;
//		Vec2i p2 = vtx[2]->spi;
//
//		// 计算面积，为零就退出
//		float s = Abs(vector_cross(p1 - p0, p2 - p0));
//		if (s <= 0) return false;
//
//		// 三角形填充时，左面和上面的边上的点需要包括，右方和下方边上的点不包括
//		// 先判断是否是 TopLeft，判断出来后会和下方 Edge Equation 一起决策
//		bool TopLeft01 = IsTopLeft(p0, p1);
//		bool TopLeft12 = IsTopLeft(p1, p2);
//		bool TopLeft20 = IsTopLeft(p2, p0);
//
//		// 迭代三角形外接矩形的所有点
//		for (int cy = _min_y; cy <= _max_y; cy++)
//		{
//			for (int cx = _min_x; cx <= _max_x; cx++)
//			{
//				Vec2f px = {(float)cx + 0.5f, (float)cy + 0.5f};
//
//				// Edge Equation
//				// 使用整数避免浮点误差，同时因为是左手系，所以符号取反
//				int E01 = (cx - p0.x) * (p1.y - p0.y) - (cy - p0.y) * (p1.x - p0.x);
//				int E12 = (cx - p1.x) * (p2.y - p1.y) - (cy - p1.y) * (p2.x - p1.x);
//				int E20 = (cx - p2.x) * (p0.y - p2.y) - (cy - p2.y) * (p0.x - p2.x);
//
//
//				// 如果是左上边，用 E >= 0 判断合法，如果右下边就用 E > 0 判断合法
//				// 这里通过引入一个误差 1 ，来将 < 0 和 <= 0 用一个式子表达
//				if (E01 < (TopLeft01 ? 0 : 1)) continue; // 在第一条边后面
//				if (E12 < (TopLeft12 ? 0 : 1)) continue; // 在第二条边后面
//				if (E20 < (TopLeft20 ? 0 : 1)) continue; // 在第三条边后面
//
//				// 三个端点到当前点的矢量
//				Vec2f s0 = vtx[0]->spf - px;
//				Vec2f s1 = vtx[1]->spf - px;
//				Vec2f s2 = vtx[2]->spf - px;
//
//				// 重心坐标系：计算内部子三角形面积 a / b / c
//				float a = Abs(vector_cross(s1, s2)); // 子三角形 Px-P1-P2 面积
//				float b = Abs(vector_cross(s2, s0)); // 子三角形 Px-P2-P0 面积
//				float c = Abs(vector_cross(s0, s1)); // 子三角形 Px-P0-P1 面积
//				float s = a + b + c; // 大三角形 P0-P1-P2 面积
//
//				if (s == 0.0f) continue;
//
//				// 除以总面积，以保证：a + b + c = 1，方便用作插值系数
//				a = a * (1.0f / s);
//				b = b * (1.0f / s);
//				c = c * (1.0f / s);
//
//				// 计算当前点的 1/w，因 1/w 和屏幕空间呈线性关系，故直接重心插值
//				float rhw = vtx[0]->rhw * a + vtx[1]->rhw * b + vtx[2]->rhw * c;
//
//				// 进行深度测试
//				if (rhw < _depth_buffer[cy][cx]) continue;
//				_depth_buffer[cy][cx] = rhw; // 记录 1/w 到深度缓存
//
//				// 还原当前像素的 w
//				float w = 1.0f / ((rhw != 0.0f) ? rhw : 1.0f);
//
//				// 计算插值系数
//				float c0 = vtx[0]->rhw * a * w;
//				float c1 = vtx[1]->rhw * b * w;
//				float c2 = vtx[2]->rhw * c * w;
//
//				// 准备为当前像素的各项 varying 进行插值
//				ShaderContext input;
//
//				ShaderContext& i0 = vtx[0]->context;
//				ShaderContext& i1 = vtx[1]->context;
//				ShaderContext& i2 = vtx[2]->context;
//
//
//				for (auto const& it : i0.varying_float)
//				{
//					int key = it.first;
//					float f0 = i0.varying_float[key];
//					float f1 = i1.varying_float[key];
//					float f2 = i2.varying_float[key];
//
//					input.varying_float[key] = c0 * f0 + c1 * f1 + c2 * f2;
//				}
//
//				for (auto const& it : i0.varying_vec2f)
//				{
//					int key = it.first;
//					const Vec2f& f0 = i0.varying_vec2f[key];
//					const Vec2f& f1 = i1.varying_vec2f[key];
//					const Vec2f& f2 = i2.varying_vec2f[key];
//					input.varying_vec2f[key] = c0 * f0 + c1 * f1 + c2 * f2;
//				}
//
//				for (auto const& it : i0.varying_vec3f)
//				{
//					int key = it.first;
//					const Vec3f& f0 = i0.varying_vec3f[key];
//					const Vec3f& f1 = i1.varying_vec3f[key];
//					const Vec3f& f2 = i2.varying_vec3f[key];
//					input.varying_vec3f[key] = c0 * f0 + c1 * f1 + c2 * f2;
//				}
//
//				for (auto const& it : i0.varying_vec4f)
//				{
//					int key = it.first;
//					const Vec4f& f0 = i0.varying_vec4f[key];
//					const Vec4f& f1 = i1.varying_vec4f[key];
//					const Vec4f& f2 = i2.varying_vec4f[key];
//					input.varying_vec4f[key] = c0 * f0 + c1 * f1 + c2 * f2;
//				}
//
//				// 执行像素着色器
//				Vec4f color = {0.0f, 0.0f, 0.0f, 0.0f};
//
//				if (_pixel_shader != nullptr)
//				{
//					color = _pixel_shader(input);
//				}
//
//				// 绘制到 framebuffer 上，这里可以加判断，如果 PS 返回的颜色 alpha 分量
//				// 小于等于零则放弃绘制，不过这样的话要把前面的更新深度缓存的代码挪下来，
//				// 只有需要渲染的时候才更新深度。
//				_frame_buffer->SetPixel(cx, cy, color);
//			}
//		}
//		// 绘制线框，再画一次避免覆盖
//		if (_render_frame)
//		{
//			DrawLine(_vertex[0].spi.x, _vertex[0].spi.y, _vertex[1].spi.x, _vertex[1].spi.y);
//			DrawLine(_vertex[0].spi.x, _vertex[0].spi.y, _vertex[2].spi.x, _vertex[2].spi.y);
//			DrawLine(_vertex[2].spi.x, _vertex[2].spi.y, _vertex[1].spi.x, _vertex[1].spi.y);
//		}
//
//		return true;
//	}
//
//protected:
//	struct Vertex
//	{
//		ShaderContext context;
//		float rhw;
//		Vec4f pos;
//		Vec2f spf;
//		Vec2i spi;
//	};
//
//protected:
//	Bitmap* _frame_buffer;
//	float** _depth_buffer;
//	int _fb_width{};
//	int _fb_height{};
//	uint32_t _color_fg{};
//	uint32_t _color_bg{};
//
//	Vertex _vertex[3];
//
//	int _min_x{};
//	int _max_x{};
//	int _min_y{};
//	int _max_y{};
//
//	bool _render_frame;
//	bool _render_pixel;
//
//	VertexShader _vertex_shader;
//	PixelShader _pixel_shader;
//};
//
//#endif // RENDERER_H
