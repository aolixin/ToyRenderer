#pragma once

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

	Vec3f pos;
	Vec3f _posTemp;
	Vec3f target;
	Vec3f up;

	// »·ÈÆ
	void circle(short xMove, short yMove)
	{
		Vec3f front = vector_normalize(target - pos);
		Vec3f right = vector_normalize(vector_cross(front, up));
		Vec3f deltaX = right * (float)xMove * 0.01f;
		target -= deltaX;

		Vec3f deltaY = up * (float)yMove * 0.01f;
		target += deltaY;
	}


	void reset()
	{
		pos = _posTemp;
	}

	void onMouseMessage(UINT message, WPARAM wParam, LPARAM lParam)
	{
		static bool isPressed = false;
		static LPARAM lpCur = 0;
		switch (message)
		{
		case WM_MOUSEWHEEL:
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
