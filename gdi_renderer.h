#pragma once

#include <windows.h>
#include <stdlib.h>
#include <tchar.h>
#include <memory>

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

	unsigned int bgColor = ((123 << 16) | (195 << 8) | 221);

public:
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
};

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