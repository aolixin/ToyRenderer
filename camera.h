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
		Vec3f front = vector_normalize(target - pos);
		Vec3f right = vector_normalize(vector_cross(front, up));
		Vec3f deltaX = right * (float)xMove * 0.01f;
		target -= deltaX;

		Vec3f deltaY = up * (float)yMove * 0.01f;
		target += deltaY;
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
		float cameraSpeed = 0.5f;
		Vec3f camFront = vector_normalize(target - pos);
		
		switch (wParam)
		{
		case 'w':
		case 'W':
			pos += cameraSpeed * camFront;
			target += cameraSpeed * camFront;
			break;
		case 's':
		case 'S':
			pos -= cameraSpeed * camFront;
			target -= cameraSpeed * camFront;
			break;
		case 'a':
		case 'A':
			pos += vector_cross(camFront, up) * cameraSpeed;
			target += vector_cross(camFront, up) * cameraSpeed;
			break;
		case 'd':
		case 'D':
			pos -= vector_cross(camFront, up) * cameraSpeed;
			target -= vector_cross(camFront, up) * cameraSpeed;
			break;

		default:
			break;
		}
	}
};
