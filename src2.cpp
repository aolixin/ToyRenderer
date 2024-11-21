#if 1
#include <windows.h>
#include "gdi_renderer.h"
#include "geometry.h"
#include "shader_instance.h"
#include "util.h"
#include "camera.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;

Camera camera({ 0, 0, -3 }, { 0, 0, 0 }, { 0, 1, 0 });

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
		WIDTH, HEIGHT,
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

	// show window
	ShowWindow(hWnd, nShowCmd);
	UpdateWindow(hWnd);

	float radius = 1.0f;
	int slices = 36;
	int stacks = 18;

	Mesh sphere_mesh = createSphere(radius, slices, stacks);


	// renderer init
	Render render(WIDTH, HEIGHT);
	render.initRenderer(hWnd);

	//ShaderInput shader_input;

	// shader input
	//render.shader_input = shader_input;

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

			auto& shader_input_ref = render.shader_input;

			// sphere
			shader_input_ref.mat_model = matrix_set_identity();
			shader_input_ref.mat_model_it = matrix_invert(shader_input_ref.mat_model).Transpose();
			shader_input_ref.mat_view = matrix_set_lookat(camera.pos, camera.target, camera.up);
			shader_input_ref.mat_proj = matrix_set_perspective(3.1415926f * 0.5f, (float)WIDTH / HEIGHT, 1.0, 500.0f);
			shader_input_ref.mat_mvp = shader_input_ref.mat_model * shader_input_ref.mat_view * shader_input_ref.mat_proj;
			render.drawCall(sphere_mesh, vert_gouraud, frag_gouraud);

			// cube
			//shader_input_ref.mat_model = matrix_set_translate(0,0,5);
			//shader_input_ref.mat_model_it = matrix_invert(shader_input_ref.mat_model).Transpose();
			//shader_input_ref.mat_view = matrix_set_lookat(camera.pos, camera.target, camera.up);
			//shader_input_ref.mat_proj = matrix_set_perspective(3.1415926f * 0.5f, (float)WIDTH / HEIGHT, 1.0, 500.0f);
			//shader_input_ref.mat_mvp = shader_input_ref.mat_model * shader_input_ref.mat_view * shader_input_ref.mat_proj;
			//render.drawCall(cube_mesh, vert_normal, frag_normal);

			//// cube
			//shader_input_ref.mat_model = matrix_set_translate(0, 0, 9);
			//shader_input_ref.mat_model_it = matrix_invert(shader_input_ref.mat_model).Transpose();
			//shader_input_ref.mat_view = matrix_set_lookat(camera.pos, camera.target, camera.up);
			//shader_input_ref.mat_proj = matrix_set_perspective(3.1415926f * 0.5f, (float)WIDTH / HEIGHT, 1.0, 500.0f);
			//shader_input_ref.mat_mvp = shader_input_ref.mat_model * shader_input_ref.mat_view * shader_input_ref.mat_proj;
			//render.drawCall(cube_mesh, vert_normal, frag_normal);

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
	case WM_MOUSEWHEEL:
	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_KEYDOWN:
		//camera.onMouseMessage(message, wParam, lParam);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CHAR:
		camera.onCameraMove(wParam);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}

	return 0;
}

#endif
