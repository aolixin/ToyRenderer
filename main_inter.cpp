#if true
#include <windows.h>
#include <chrono>

#include "gdi_renderer.h"
#include "geometry.h"
#include "shader_instance.h"
#include "util.h"
#include "camera.h"
#include "model.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

//constexpr int WIDTH = 800;
//constexpr int HEIGHT = 600;

constexpr int WIDTH = 1080;
constexpr int HEIGHT = 720;

constexpr bool MSAA_ENABLE = false;
constexpr int mip_level = 4;

//Camera camera({2, 3, 4}, {0, 0, 0}, {0, 1, 0});
Camera camera({0, 0, -3}, {0, 0, 0}, {0, 1, 0});

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
	wcex.lpszClassName = L"ToyRender";
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
		L"ToyRender",
		L"ToyRender",
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


	// fps calculate
	std::chrono::steady_clock::time_point frame_start;
	std::chrono::steady_clock::time_point frame_end;

	int frame_count = 0;

	// shader attribute
	Mat4x4f mat_model;
	Mat4x4f mat_model_it;
	Mat4x4f mat_view;
	Mat4x4f mat_proj;
	Mat4x4f mat_mvp;
	constexpr float fovy = 3.1415926f * 0.5f;
	constexpr float aspect = (float)WIDTH / HEIGHT;
	constexpr float zn = 1.0f;
	constexpr float zf = 500.0f;
	Vec3f light_dir = {-1, -1, 1};

	TGA::TGAImage tga_image;

	std::string file_path = "res/awesomeface.tga";
	std::string file_path2 = "res/container.tga";

	if (!tga_image.read_tga_file(file_path))return 0;

	tga_image.CreateMipMap(mip_level);

	auto vert_tex = [&](int index, ShaderContext& output) -> Vec4f
	{
		Vec4f pos = plane_mesh2.vertices[index].pos.xyz1() * mat_mvp;
		output.varying_vec2f[VARYING_TEXUV] = plane_mesh2.vertices[index].uv;

		return pos;
	};

	int sample_mip_level = 0;

	auto frag_tex = [&](ShaderContext& vert_input) -> Vec4f
	{
		//return white_color.xyz1();
		Vec2f uv = vert_input.varying_vec2f[VARYING_TEXUV];
		return vector_from_color(tga_image.sample_mipmap(uv,sample_mip_level));
	};

	// renderer init
	Render render(WIDTH, HEIGHT);
	render.initRenderer(hWnd);
	render.msaa_enable(MSAA_ENABLE);

	// create frame buffer
	int frame_buffer_id = render.create_frame_buffer(WIDTH, HEIGHT);
	int depth_buffer_id = render.create_depth_buffer(WIDTH, HEIGHT);

	render.set_frame_buffer(frame_buffer_id);
	render.set_depth_buffer(depth_buffer_id);

	// main loop
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
			frame_start = std::chrono::steady_clock::now();
			// clear buffer
			render.clearBuffer();

			mat_view = matrix_set_lookat(camera.pos, camera.target, camera.up);
			mat_proj = matrix_set_perspective(fovy, aspect, zn, zf);


			// show mipmap
			float scale = 4;
			float dis = 0;
			for (int i = 0; i < mip_level; i++)
			{
				sample_mip_level = i;
				mat_model = matrix_set_scale(scale, scale, 1) * matrix_set_translate(dis, 0, 0);
				mat_model_it = matrix_invert(mat_model).Transpose();
				mat_mvp = mat_model * mat_view * mat_proj;
				render.drawCall(plane_mesh2, vert_tex, frag_tex);
				dis += scale;
				scale /= 2;
				dis += scale;

			}

			// draw plane

			//sample_mip_level = 1;
			//mat_model = matrix_set_scale(scale, scale, 1);
			//mat_model = matrix_set_identity();
			//mat_model_it = matrix_invert(mat_model).Transpose();
			//mat_mvp = mat_model * mat_view * mat_proj;
			//render.drawCall(plane_mesh2, vert_tex, frag_tex);

			// Resolve msaa
			if (MSAA_ENABLE)
				render.ResolvePixel();

			// swap buffer
			render.update(hWnd);

			// show fps
			frame_end = std::chrono::steady_clock::now();
			std::chrono::duration<double> duration = frame_end - frame_start;
			double fps = 1.0 / duration.count();
			std::string fps_str = "FPS: " + std::to_string(static_cast<int>(fps));
			show_str(hWnd, fps_str, WIDTH - 100, 20);
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
		camera.onMouseMessage(message, wParam, lParam);
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
