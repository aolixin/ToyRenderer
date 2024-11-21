#pragma once

#include <algorithm>

#include "geometry.h"

class Camera
{
public:
	Camera(Vec3f pos, Vec3f target, Vec3f up) :
		pos(pos), _posTemp(pos), target(target), up(up)
	{
	}

	virtual ~Camera() noexcept
	{
	}

	void setPos(const Vec3f& pos)
	{
		this->pos = pos;
	}

	void setPerspectiveForLH(float fov, float aspect, float nearZ, float farZ)
	{
		_fov = fov;
		_aspect = aspect;
		_nearZ = nearZ;
		_farZ = farZ;
	}

public:
	// 世界坐标系转到投影平面
	Mat4x4f worldToProjection;

	Vec3f pos;
	Vec3f _posTemp;
	Vec3f target;
	Vec3f up;
	float _fov;
	float _aspect;
	float _nearZ;
	float _farZ;
	float _curXRand = 0.0f;
	float _curYRand = 0.0f;

	// 世界坐标系转观察坐标系
	Mat4x4f _worldToView;
	// 观察坐标系转投影坐标系
	Mat4x4f _viewToProjection;

	// 环绕
	void circle(short xMove, short yMove)
	{
		// 鼠标移动像素与弧度的比例固定
		float circleLen = 100.f;

		// 1 计算绕y轴的旋转
		float radY = xMove / circleLen;
		Mat4x4f mScaleY = {
			(float)cos(radY), 0.0f, -(float)sin(radY), 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			(float)sin(radY), 0.0f, (float)cos(radY), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};

		// 2 计算绕x轴 这里需要设定一个最大角度
		float radX = yMove / circleLen;
		float maxRad = 3.1415926f * 0.6f;
		_curXRand += radX;
		if (_curXRand < -maxRad)
		{
			_curXRand = -maxRad;
			radX = 0.0f;
		}
		if (_curXRand > maxRad)
		{
			_curXRand = maxRad;
			radX = 0.0f;
		}

		Mat4x4f mScaleX = {
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, (float)cos(radX), (float)sin(radX), 0.0f,
			0.0f, -(float)sin(radX), (float)cos(radX), 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		};


		auto tmp = pos.xyz1() * mScaleX;
		pos = tmp.xyz();
		tmp = pos.xyz1() * mScaleY;
		pos = tmp.xyz();
	}

	// 缩放
	void zoom(short wParam)
	{
		float t = 0.9f;
		if (wParam > 0) t = 1.1f;
		pos.x *= t;
		pos.y *= t;
		pos.z *= t;
	}

	void reset()
	{
		pos = _posTemp;
		_curXRand = 0.0f;
	}

	void onMouseMessage(UINT message, WPARAM wParam, LPARAM lParam)
	{
		static bool isPressed = false;
		static LPARAM lpCur = 0;
		switch (message)
		{
		case WM_MOUSEWHEEL:
			zoom(GET_WHEEL_DELTA_WPARAM(wParam));
			break;
		case WM_LBUTTONDOWN:
			isPressed = true;
			lpCur = lParam;
			break;
		case WM_LBUTTONUP:
			isPressed = false;
			break;
		case WM_MOUSEMOVE:
			if (wParam & MK_LBUTTON)
			{
				short xMove = LOWORD(lParam) - LOWORD(lpCur);
				short yMove = HIWORD(lParam) - HIWORD(lpCur);
				lpCur = lParam;
				circle(xMove, -yMove);
			}
			break;
		case WM_KEYDOWN:
			if (wParam == VK_SPACE)
				reset();
			else if (wParam == VK_F1)
			{
				//if (g_renderMode == RenderMode::RENDER_COLOR) g_renderMode = RenderMode::RENDER_WIREFRAME;
				//else g_renderMode = RenderMode::RENDER_COLOR;
			}
			break;
		default:
			break;
		}
	}

	void onCameraMove(WPARAM wParam)
	{
		float cameraSpeed = 0.1f;
		Vec3f camFront = vector_normalize(target - pos);
		
		switch (wParam)
		{
		case 'w':
		case 'W':
			pos += cameraSpeed * camFront;
			break;
		case 's':
		case 'S':
			pos -= cameraSpeed * camFront;
			break;
		case 'a':
		case 'A':
			pos += vector_cross(camFront, up) * cameraSpeed;
			break;
		case 'd':
		case 'D':
			pos -= vector_cross(camFront, up) * cameraSpeed;
			break;

		default:
			break;
		}
	}
};
