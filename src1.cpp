#if 0
#include <windows.h>
#include "gdi_renderer.h"
#include "geometry.h"
#include "shader_instance.h"


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);



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


	ShaderInput shader_input;
	shader_input.mat_model = matrix_set_identity();
	shader_input.mat_view = matrix_set_lookat({ 2, 3, -4 }, { 0, 0, 0 }, { 0, 1, 0 });
	shader_input.mat_proj = matrix_set_perspective(3.1415926f * 0.5f, 800 / 600.0, 1.0, 500.0f);
	shader_input.mat_mvp = shader_input.mat_model * shader_input.mat_view * shader_input.mat_proj;

	shader_input.mat_model_it = matrix_invert(shader_input.mat_model).Transpose();

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
			// shader input
			render.shader_input = shader_input;
			// draw call
			render.drawCall(cubeMesh, vert_normal, frag_normal);
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
		// ÊäÈë×Ö·û
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
