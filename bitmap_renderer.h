#pragma once
#ifndef RENDERER_H
#define RENDERER_H

#include <utility>
#include<vector>
#include<map>
#include <functional>
#include "vector.h"
#include "bitmap.h"

struct ShaderContext
{
    std::map<int, float> varying_float;
    std::map<int, Vec2f> varying_vec2f;
    std::map<int, Vec3f> varying_vec3f;
    std::map<int, Vec4f> varying_vec4f;
};

// ������ɫ������Ϊ�� C++ ��д�����贫�� attribute������ 0-2 �Ķ������
// ��ɫ������ֱ������������Ŷ�ȡ��Ӧ���ݼ��ɣ������Ҫ����һ������ pos
// ���� varying ���õ� output �����Ⱦ����ֵ�󴫵ݸ� PS 
typedef std::function<Vec4f(int index, ShaderContext& output)> VertexShader;


// ������ɫ�������� ShaderContext����Ҫ���� Vec4f ���͵���ɫ
// ��������ÿ����� input ����ֵ�����ǰ����������� output ��ֵ�õ�
typedef std::function<Vec4f(ShaderContext& input)> PixelShader;

class Renderer
{
public:


    inline virtual ~Renderer() { Reset(); }

    inline Renderer()
    {
        _frame_buffer = nullptr;
        _depth_buffer = nullptr;
        _render_frame = false;
        _render_pixel = true;
    }

    inline Renderer(int width, int height)
    {
        _frame_buffer = nullptr;
        _depth_buffer = nullptr;
        _render_frame = false;
        _render_pixel = true;
        Init(width, height);
    }

    inline void Reset()
    {
        _vertex_shader = nullptr;
        _pixel_shader = nullptr;
        delete _frame_buffer;
        _frame_buffer = nullptr;
        if (_depth_buffer)
        {
            for (int j = 0; j < _fb_height; j++)
            {
                if (_depth_buffer[j])delete _depth_buffer[j];
                _depth_buffer[j] = nullptr;
            }
            delete[]_depth_buffer;
            _depth_buffer = nullptr;
        }
        _color_fg = 0xffffffff;
        _color_bg = 0xff191970;
    }

    inline void Init(int widht, int height)
    {
        Reset();
        _fb_width = widht;
        _fb_height = height;
        _frame_buffer = new Bitmap(widht, height);
        _depth_buffer = new float* [height];
        for (int j = 0; j < height; j++)
        {
            _depth_buffer[j] = new float[widht];
        }
        clear();
    }

    inline void clear()
    {
        if (_frame_buffer) {
            _frame_buffer->Fill(_color_bg);
        }
        if (_depth_buffer) {
            for (int j = 0; j < _fb_height; j++)
            {
                for (int i = 0; i < _fb_width; i++)
                {
                    _depth_buffer[j][i] = 0.0f;
                }
            }
        }
    }

    inline void SetVertexShader(VertexShader vs)
    {
        _vertex_shader = std::move(vs);
    }

    inline void SetPixelShader(PixelShader ps)
    {
        _pixel_shader = std::move(ps);
    }

    inline void SaveFile(const char* filename)
    {
        if (_frame_buffer) _frame_buffer->SaveFile(filename);
    }

    // ���ñ���/ǰ��ɫ
    inline void SetBGColor(uint32_t color) { _color_bg = color; }
    inline void SetFGColor(uint32_t color) { _color_fg = color; }



    // �ж�һ�����ǲ��������ε����ϱ� (Top-Left Edge)
    inline bool IsTopLeft(const Vec2i& a, const Vec2i& b) {
        return ((a.y == b.y) && (a.x < b.x)) || (a.y > b.y);
    }

    // FrameBuffer �ﻭ��
    inline void DrawLine(int x1, int y1, int x2, int y2) {
        if (_frame_buffer) _frame_buffer->DrawLine(x1, y1, x2, y2, _color_fg);
    }


    // FrameBuffer �ﻭ��
    inline void SetPixel(int x, int y, uint32_t cc) { if (_frame_buffer) _frame_buffer->SetPixel(x, y, cc); }
    inline void SetPixel(int x, int y, const Vec4f& cc) { SetPixel(x, y, vector_to_color(cc)); }
    inline void SetPixel(int x, int y, const Vec3f& cc) { SetPixel(x, y, vector_to_color(cc)); }

    // ������Ⱦ״̬���Ƿ���ʾ�߿�ͼ���Ƿ����������
    inline void SetRenderState(bool frame, bool pixel) {
        _render_frame = frame;
        _render_pixel = pixel;
    }

    inline bool DrawPrimitive()
    {
        if (_frame_buffer == nullptr || _vertex_shader == nullptr)
            return false;

        for (int k = 0; k < 3; k++)
        {
            Vertex& vertex = _vertex[k];
            vertex.context.varying_float.clear();
            vertex.context.varying_vec2f.clear();
            vertex.context.varying_vec3f.clear();
            vertex.context.varying_vec4f.clear();

            // vertex_shader
            vertex.pos = _vertex_shader(k, vertex.context);

            float w = Abs(vertex.pos.w);

            // �ü�
            if (w == 0.0f)return false;
            if (vertex.pos.z<-w || vertex.pos.z > w)return false;
            if (vertex.pos.x<-w || vertex.pos.x>w)return false;
            if (vertex.pos.y<-w || vertex.pos.y>w)return false;

            vertex.rhw = 1.0f / w;
            // ͸�ӳ���ת���� ndc ����
            vertex.pos *= vertex.rhw;

            // ������Ļ����
            vertex.spf.x = (vertex.pos.x + 1.0f) * (float)_fb_width * 0.5f;
            vertex.spf.y = (vertex.pos.y + 1.0f) * (float)_fb_height * 0.5f;

            // ��ȡ��������
            vertex.spi.x = (int)(vertex.spf.x + 0.5f);
            vertex.spi.y = (int)(vertex.spf.y + 0.5f);

            if (k == 0)
            {
                _min_x = _max_x = Between(0, _fb_width - 1, vertex.spi.x);
                _min_y = _max_y = Between(0, _fb_height - 1, vertex.spi.y);
            }
            else
            {
                _min_x = Between(0, _fb_width - 1, Min(_min_x, vertex.spi.x));
                _max_x = Between(0, _fb_width - 1, Max(_max_x, vertex.spi.x));
                _min_y = Between(0, _fb_height - 1, Min(_min_y, vertex.spi.y));
                _max_y = Between(0, _fb_height - 1, Max(_max_y, vertex.spi.y));
            }
        }


        if (_render_frame)
        {
            DrawLine(_vertex[0].spi.x, _vertex[0].spi.y, _vertex[1].spi.x, _vertex[1].spi.y);
            DrawLine(_vertex[1].spi.x, _vertex[1].spi.y, _vertex[2].spi.x, _vertex[2].spi.y);
            DrawLine(_vertex[2].spi.x, _vertex[2].spi.y, _vertex[0].spi.x, _vertex[0].spi.y);
        }

        if (!_render_pixel) return false;

        Vec4f v01 = _vertex[1].pos - _vertex[0].pos;
        Vec4f v02 = _vertex[2].pos - _vertex[0].pos;
        Vec4f normal = vector_cross(v01, v02);

        Vertex* vtx[3] = { &_vertex[0],&_vertex[1],&_vertex[2] };

        if (normal.z > 0.0f)
        {
            vtx[1] = &_vertex[2];
            vtx[2] = &_vertex[1];
        }
        else if (normal.z == 0.0f)
        {
            return false;
        }


        // ���������˵�λ��
        Vec2i p0 = vtx[0]->spi;
        Vec2i p1 = vtx[1]->spi;
        Vec2i p2 = vtx[2]->spi;

        // ���������Ϊ����˳�
        float s = Abs(vector_cross(p1 - p0, p2 - p0));
        if (s <= 0) return false;

        // ���������ʱ�����������ı��ϵĵ���Ҫ�������ҷ����·����ϵĵ㲻����
        // ���ж��Ƿ��� TopLeft���жϳ��������·� Edge Equation һ�����
        bool TopLeft01 = IsTopLeft(p0, p1);
        bool TopLeft12 = IsTopLeft(p1, p2);
        bool TopLeft20 = IsTopLeft(p2, p0);

        // ������������Ӿ��ε����е�
        for (int cy = _min_y; cy <= _max_y; cy++) {
            for (int cx = _min_x; cx <= _max_x; cx++) {
                Vec2f px = { (float)cx + 0.5f, (float)cy + 0.5f };

                // Edge Equation
                // ʹ���������⸡����ͬʱ��Ϊ������ϵ�����Է���ȡ��
                int E01 = (cx - p0.x) * (p1.y - p0.y) - (cy - p0.y) * (p1.x - p0.x);
                int E12 = (cx - p1.x) * (p2.y - p1.y) - (cy - p1.y) * (p2.x - p1.x);
                int E20 = (cx - p2.x) * (p0.y - p2.y) - (cy - p2.y) * (p0.x - p2.x);


                // ��������ϱߣ��� E >= 0 �жϺϷ���������±߾��� E > 0 �жϺϷ�
                // ����ͨ������һ����� 1 ������ < 0 �� <= 0 ��һ��ʽ�ӱ��
                if (E01 < (TopLeft01 ? 0 : 1)) continue;   // �ڵ�һ���ߺ���
                if (E12 < (TopLeft12 ? 0 : 1)) continue;   // �ڵڶ����ߺ���
                if (E20 < (TopLeft20 ? 0 : 1)) continue;   // �ڵ������ߺ���

                // �����˵㵽��ǰ���ʸ��
                Vec2f s0 = vtx[0]->spf - px;
                Vec2f s1 = vtx[1]->spf - px;
                Vec2f s2 = vtx[2]->spf - px;

                // ��������ϵ�������ڲ������������ a / b / c
                float a = Abs(vector_cross(s1, s2));    // �������� Px-P1-P2 ���
                float b = Abs(vector_cross(s2, s0));    // �������� Px-P2-P0 ���
                float c = Abs(vector_cross(s0, s1));    // �������� Px-P0-P1 ���
                float s = a + b + c;                    // �������� P0-P1-P2 ���

                if (s == 0.0f) continue;

                // ������������Ա�֤��a + b + c = 1������������ֵϵ��
                a = a * (1.0f / s);
                b = b * (1.0f / s);
                c = c * (1.0f / s);

                // ���㵱ǰ��� 1/w���� 1/w ����Ļ�ռ�����Թ�ϵ����ֱ�����Ĳ�ֵ
                float rhw = vtx[0]->rhw * a + vtx[1]->rhw * b + vtx[2]->rhw * c;

                // ������Ȳ���
                if (rhw < _depth_buffer[cy][cx]) continue;
                _depth_buffer[cy][cx] = rhw;   // ��¼ 1/w ����Ȼ���

                // ��ԭ��ǰ���ص� w
                float w = 1.0f / ((rhw != 0.0f) ? rhw : 1.0f);

                // �����ֵϵ��
                float c0 = vtx[0]->rhw * a * w;
                float c1 = vtx[1]->rhw * b * w;
                float c2 = vtx[2]->rhw * c * w;

                // ׼��Ϊ��ǰ���صĸ��� varying ���в�ֵ
                ShaderContext input;

                ShaderContext& i0 = vtx[0]->context;
                ShaderContext& i1 = vtx[1]->context;
                ShaderContext& i2 = vtx[2]->context;


                for (auto const& it : i0.varying_float)
                {
                    int key = it.first;
                    float f0 = i0.varying_float[key];
                    float f1 = i1.varying_float[key];
                    float f2 = i2.varying_float[key];

                    input.varying_float[key] = c0 * f0 + c1 * f1 + c2 * f2;
                }

                for (auto const& it : i0.varying_vec2f) {
                    int key = it.first;
                    const Vec2f& f0 = i0.varying_vec2f[key];
                    const Vec2f& f1 = i1.varying_vec2f[key];
                    const Vec2f& f2 = i2.varying_vec2f[key];
                    input.varying_vec2f[key] = c0 * f0 + c1 * f1 + c2 * f2;
                }

                for (auto const& it : i0.varying_vec3f) {
                    int key = it.first;
                    const Vec3f& f0 = i0.varying_vec3f[key];
                    const Vec3f& f1 = i1.varying_vec3f[key];
                    const Vec3f& f2 = i2.varying_vec3f[key];
                    input.varying_vec3f[key] = c0 * f0 + c1 * f1 + c2 * f2;
                }

                for (auto const& it : i0.varying_vec4f) {
                    int key = it.first;
                    const Vec4f& f0 = i0.varying_vec4f[key];
                    const Vec4f& f1 = i1.varying_vec4f[key];
                    const Vec4f& f2 = i2.varying_vec4f[key];
                    input.varying_vec4f[key] = c0 * f0 + c1 * f1 + c2 * f2;
                }

                // ִ��������ɫ��
                Vec4f color = { 0.0f, 0.0f, 0.0f, 0.0f };

                if (_pixel_shader != nullptr) {
                    color = _pixel_shader(input);
                }

                // ���Ƶ� framebuffer �ϣ�������Լ��жϣ���� PS ���ص���ɫ alpha ����
                // С�ڵ�������������ƣ����������Ļ�Ҫ��ǰ��ĸ�����Ȼ���Ĵ���Ų������
                // ֻ����Ҫ��Ⱦ��ʱ��Ÿ�����ȡ�
                _frame_buffer->SetPixel(cx, cy, color);
            }
        }
        // �����߿��ٻ�һ�α��⸲��
        if (_render_frame) {
            DrawLine(_vertex[0].spi.x, _vertex[0].spi.y, _vertex[1].spi.x, _vertex[1].spi.y);
            DrawLine(_vertex[0].spi.x, _vertex[0].spi.y, _vertex[2].spi.x, _vertex[2].spi.y);
            DrawLine(_vertex[2].spi.x, _vertex[2].spi.y, _vertex[1].spi.x, _vertex[1].spi.y);
        }

        return true;
    }



protected:

    struct Vertex
    {
        ShaderContext context;
        float rhw;
        Vec4f pos;
        Vec2f spf;
        Vec2i spi;
    };

protected:
    Bitmap* _frame_buffer;
    float** _depth_buffer;
    int _fb_width{};
    int _fb_height{};
    uint32_t _color_fg{};
    uint32_t _color_bg{};

    Vertex _vertex[3];

    int _min_x{};
    int _max_x{};
    int _min_y{};
    int _max_y{};

    bool _render_frame;
    bool _render_pixel;

    VertexShader _vertex_shader;
    PixelShader _pixel_shader;
};

#endif // RENDERER_H