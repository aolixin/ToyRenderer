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
	unsigned int* g_frameBuff = nullptr;
	std::shared_ptr<float[]> g_depthBuff = nullptr;



public:

	static unsigned int bgColor;

	Render(int w, int h) {
		g_width = w;
		g_height = h;
	}



	// 初始化渲染器 屏幕长宽 屏幕缓冲
	void initRenderer(HWND hWnd);
	// 每帧绘制
	void update(HWND hWnd);
	// 清理屏幕缓冲
	void clearBuffer();
	void shutDown();

	void drawCall(const Mesh& mesh, const VertexShader& vert, FragmentShader& frag);

	void inline drawCube();
	void inline drawPlane(int lt, int rt, int rb, int lb);
	void inline drawPrimitive(const Vertex& a, const Vertex& b, const Vertex& c);

	void inline drawLineDDA(int x1, int y1, int x2, int y2, uint32_t color);
	void inline drawLineBresenham(int x1, int y1, int x2, int y2, uint32_t color);



	void inline drawPixel(int x, int y, uint32_t color);
};

unsigned int Render::bgColor = ((123 << 16) | (195 << 8) | 221);

void Render::initRenderer(HWND hWnd)
{
	if (g_width == 0 || g_height == 0)return;
	// 1. 创建一个屏幕缓冲
	// 1.1 创建一个与当前设备兼容的DC
	HDC hDC = GetDC(hWnd);
	g_tempDC = CreateCompatibleDC(hDC);
	ReleaseDC(hWnd, hDC);
	// 1.2 创建该DC的bitmap缓冲  32位色
	BITMAPINFO bi = { { sizeof(BITMAPINFOHEADER), g_width, -g_height, 1, 32, BI_RGB,
		(DWORD)g_width * g_height * 4, 0, 0, 0, 0 } };
	g_tempBm = CreateDIBSection(g_tempDC, &bi, DIB_RGB_COLORS, (void**)&g_frameBuff, 0, 0);
	// 1.3 选择该bitmap到dc中
	g_oldBm = (HBITMAP)SelectObject(g_tempDC, g_tempBm);
	// 1.4 创建深度缓冲区
	g_depthBuff.reset(new float[g_width * g_height]);

	// 清理屏幕缓冲
	clearBuffer();

}

void Render::update(HWND hWnd)
{
	// 1. clear frameBuffer
	clearBuffer();

	// 2. draw line
	drawLineBresenham(100, 100, 200, 200, 0xffffff);

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
			g_depthBuff[idx] = 1.0f;
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

void Render::drawCall(const Mesh& mesh, const VertexShader& vert, FragmentShader& frag)
{

}

void Render::drawCube()
{
	drawPlane(0, 1, 2, 3);  // 正面
	drawPlane(1, 5, 6, 2);  // 右面
	drawPlane(4, 0, 3, 7);  // 左面
	drawPlane(4, 5, 1, 0);  // 上面
	drawPlane(3, 2, 6, 7);  // 下面
	drawPlane(5, 4, 7, 6);  // 后面
}

void Render::drawPlane(int lt, int rt, int rb, int lb)
{
	// drawPrimitive(vertexes[lt], vertexes[rt], vertexes[rb]);
	// drawPrimitive(vertexes[lt], vertexes[rb], vertexes[lb]);
}

void Render::drawPrimitive(const Vertex& a, const Vertex& b, const Vertex& c)
{

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
