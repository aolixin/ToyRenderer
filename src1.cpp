#include <windows.h>
#include "gdi_renderer.h"
#include "camera.h"
#include "geometry.h"

// 定义属性和 varying 中的纹理坐标 key
const int VARYING_TEXUV = 0;
const int VARYING_COLOR = 1;
const int VARYING_LIGHT = 2;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

#if 1

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
                   _In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	WNDCLASSEX wcex = {0};
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszClassName = L"simpleSoftRender";
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE((WORD)IDI_APPLICATION));
	if (!RegisterClassEx(&wcex))
	{
		MessageBox(NULL,
		           L"Call to RegisterClassEx failed!",
		           L"Win32 Guided Tour",
		           NULL);

		return 1;
	}

	// 1.2 create window
	HWND hWnd = CreateWindow(
		L"simpleSoftRender",
		L"simpleSoftRender",
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT,
		800, 600,
		NULL,
		NULL,
		hInstance,
		NULL
	);
	if (!hWnd)
	{
		MessageBox(NULL,
		           L"Call to CreateWindow failed!",
		           L"Win32 Guided Tour",
		           NULL);

		return 1;
	}

	// 1.3 show window
	ShowWindow(hWnd, nShowCmd);
	UpdateWindow(hWnd);

	// renderer init
	Render render(800, 600);
	render.initRenderer(hWnd);

	Vec3f red_color = {1.0f, 0.0f, 0.0f};
	Vec3f green_color = {0.0f, 1.0f, 0.0f};
	Vec3f blue_color = {0.0f, 0.0f, 1.0f};
	Vec3f white_color = {1.0f, 1.0f, 1.0f};

	//Mesh cubeMesh({
	//	{
	//		{{1, -1, -1,}, {0, 0}, {1.0f, 1.0f, 0.2f}, {}},
	//		{{-1, -1, -1,}, {0, 1}, {0.2f, 1.0f, 1.0f}, {}},
	//		{{-1, 1, -1,}, {1, 1}, {1.0f, 0.3f, 0.3f}, {}},
	//		{{1, 1, -1,}, {1, 0}, {0.2f, 1.0f, 0.3f}, {}},
	//		{{1, -1, 1,}, {0, 0}, {1.0f, 0.2f, 0.2f}, {}},
	//		{{-1, -1, 1,}, {0, 1}, {0.2f, 1.0f, 0.2f}, {}},
	//		{{-1, 1, 1,}, {1, 1}, {0.2f, 0.2f, 1.0f}, {}},
	//		{{1, 1, 1,}, {1, 0}, {1.0f, 0.2f, 1.0f}, {}}
	//	},
	//	{
	//		0, 2, 1, 0, 3, 2,
	//		1, 2, 6, 1, 6, 5,
	//		0, 7, 3, 0, 4, 7,
	//		2, 3, 7, 2, 7, 6,
	//		0, 1, 5, 0, 5, 4,
	//		4, 5, 6, 4, 6, 7
	//	}
	//});
	Mesh cubeMesh({
		{
			{{1, -1, -1,}, {0, 0}, green_color, {}},
			{{-1, -1, -1,}, {0, 1}, white_color, {}},
			{{-1, 1, -1,}, {1, 1}, white_color, {}},
			{{1, 1, -1,}, {1, 0}, white_color, {}},
			{{1, -1, 1,}, {0, 0}, white_color, {}},
			{{-1, -1, 1,}, {0, 1}, white_color, {}},
			{{-1, 1, 1,}, {1, 1}, blue_color, {}},
			{{1, 1, 1,}, {1, 0}, white_color, {}}
		},
		{
			0, 2, 1, 0, 3, 2,
			1, 2, 6, 1, 6, 5,
			0, 7, 3, 0, 4, 7,
			2, 3, 7, 2, 7, 6,
			0, 1, 5, 0, 5, 4,
			4, 5, 6, 4, 6, 7
		}
	});

	// 定义变换矩阵：模型变换，摄像机变换，透视变换
	//Mat4x4f mat_model = matrix_set_rotate(-1, -0.5, 1, 1); // 模型变换，旋转一定角度
	Mat4x4f mat_model = matrix_set_identity();
	Mat4x4f mat_view = matrix_set_lookat({2, 3, -4}, {0, 0, 0}, {0, 1, 0}); // 摄像机方位
	Mat4x4f mat_proj = matrix_set_perspective(3.1415926f * 0.5f, 800 / 600.0, 1.0, 500.0f);
	//mat_proj = matrix_set_identity();
	Mat4x4f mat_mvp = mat_model * mat_view * mat_proj; // 综合变换矩阵

	auto vertShader = [&](int index, ShaderContext& output) -> Vec4f
	{
		Vec4f pos = cubeMesh.vertices[index].pos.xyz1() * mat_mvp;
		output.varying_vec2f[VARYING_TEXUV] = cubeMesh.vertices[index].uv;
		output.varying_vec4f[VARYING_COLOR] = cubeMesh.vertices[index].color.xyz1();
		//// 法向需要经过 model 矩阵的逆矩阵转置的矩阵变换，从模型坐标系转换
		//// 到世界坐标系，光照需要在世界坐标系内进行运算
		Vec3f normal = cubeMesh.vertices[index].normal;

		//normal = (normal.xyz1() * mat_model_it).xyz();

		//// 计算光照强度
		//float intense = vector_dot(normal, vector_normalize(light_dir));
		//// 避免越界同时加入一个常量环境光 0.1
		//intense = Max(0.0f, intense) + 0.1;

		//output.varying_float[VARYING_LIGHT] = Min(1.0f, intense);

		return pos;
	};

	auto fragShader = [&](ShaderContext& input) -> Vec4f
	{
		return input.varying_vec4f[VARYING_COLOR];
		return {1.0f, 1.0f, 1.0f, 1.0f};
	};

	// 1.4 start message loop
	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// clear buffer
			render.clearBuffer();
			// draw call
			//render.drawCall();
			render.drawCall(cubeMesh, vertShader, fragShader);
			// swap buffer
			render.update(hWnd);
		}
	}

	render.shutDown();
	return static_cast<int>(msg.wParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CHAR:
		// 输入字符
		if (wParam == 'q' || wParam == 'Q')
		{
			MessageBox(hWnd, L"Q key pressed", L"Keyboard Input", MB_OK);
		}
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}

#endif
