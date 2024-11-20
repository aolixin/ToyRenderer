#include <windows.h>
#include "geometry.h"


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

#if 0

int main()
{
	// test vec4f and mat4x4f
	Vec4f v1(1.0f, 2.0f, 3.0f, 4.0f);
	Vec4f v2(5.0f, 6.0f, 7.0f, 8.0f);

	Vec4f v3 = v1 + v2;
	Vec4f v4 = v1 - v2;

	Mat4x4f m1 = {v1,v2,v3,v4};
	std::cout << m1 << std::endl;
	Mat4x4f m2;
	Mat4x4f m3 = m1 * m2;

	return 0;
}

#endif
