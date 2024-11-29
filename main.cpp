#if false
#include <windows.h>
#include <chrono>

#include "gdi_renderer.h"
#include "geometry.h"
#include "shader_instance.h"
#include "util.h"
#include "camera.h"
#include "model.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

constexpr int WIDTH = 800;
constexpr int HEIGHT = 600;

constexpr bool MSAA_ENABLE = false;

//Camera camera({2, 3, 4}, {0, 0, 0}, {0, 1, 0});
Camera camera({ 0, 0, 3 }, { 0, 0, 0 }, { 0, 1, 0 });

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance,
	_In_ LPSTR lpCmdLine, _In_ int nShowCmd)
{
	WNDCLASSEX wcex = { 0 };
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

	// build mesh

	Model model("res/diablo3_pose/diablo3_pose.obj");
	Mesh mesh;

	int face_num = model.nfaces();
	mesh.vertices.reserve(face_num * 3);
	mesh.indices.reserve(face_num * 3);

	for (int i = 0; i < face_num; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			Vertex v;
			v.pos = model.vert(i, j);
			v.normal = model.normal(i, j);
			v.uv = model.uv(i, j);
			mesh.vertices.push_back(v);
		}
		// .obj 三角形是顺时针, 构建mesh时改成逆时针
		mesh.indices.push_back(i * 3);
		mesh.indices.push_back(i * 3 + 2);
		mesh.indices.push_back(i * 3 + 1);
	}

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
	Vec3f light_dir = { -1, -1, 1 };

	// shader definition
	auto vert_gouraud_tex = [&](int index, ShaderContext& output) -> Vec4f
		{
			Vec4f pos = mesh.vertices[index].pos.xyz1() * mat_mvp;
			Vec3f pos_world = (mesh.vertices[index].pos.xyz1() * mat_model).xyz();
			Vec3f eye_dir = camera.pos - pos_world;
			output.varying_vec2f[VARYING_TEXUV] = mesh.vertices[index].uv;
			output.varying_vec3f[VARYING_EYE] = eye_dir;
			return pos;
		};

	auto frag_gouraud_tex = [&](ShaderContext& input) -> Vec4f
		{
			Vec2f uv = input.varying_vec2f[VARYING_TEXUV];

			Vec3f eye_dir = input.varying_vec3f[VARYING_EYE];

			Vec3f l = vector_normalize(light_dir);

			Vec3f n = (model.normal(uv).xyz1() * mat_model_it).xyz();

			float s = model.Specular(uv);

			Vec3f r = vector_normalize(n * vector_dot(n, l) * 2.0f - l);

			float p = Saturate(vector_dot(r, eye_dir));
			float spec = Saturate(pow(p, s * 20) * 0.05);

			float intense = Saturate(vector_dot(n, l)) + 0.2f + spec;
			Vec4f color = model.Diffuse(uv);
			return color * intense;
		};

	auto vert_normal_plane = [&](int index, ShaderContext& output) -> Vec4f
		{
			Vec4f pos = plane_mesh.vertices[index].pos.xyz1() * mat_mvp;
			output.varying_vec2f[VARYING_TEXUV] = plane_mesh.vertices[index].uv;
			output.varying_vec4f[VARYING_COLOR] = plane_mesh.vertices[index].color.xyz1();
			Vec3f normal = plane_mesh.vertices[index].normal;

			return pos;
		};

	auto frag_normal_plane = [&](ShaderContext& vert_input) -> Vec4f
		{
			return vert_input.varying_vec4f[VARYING_COLOR];
		};

	auto vert_normal_cube = [&](int index, ShaderContext& output) -> Vec4f
		{
			Vec4f pos = cube_mesh.vertices[index].pos.xyz1() * mat_mvp;
			output.varying_vec2f[VARYING_TEXUV] = cube_mesh.vertices[index].uv;
			output.varying_vec4f[VARYING_COLOR] = cube_mesh.vertices[index].color.xyz1();
			Vec3f normal = cube_mesh.vertices[index].normal;

			return pos;
		};

	auto frag_normal_cube = [&](ShaderContext& vert_input) -> Vec4f
		{
			return vert_input.varying_vec4f[VARYING_COLOR];
		};

	auto vert_tex = [&](int index, ShaderContext& output) -> Vec4f
		{
			Vec4f pos = plane_mesh.vertices[index].pos.xyz1() * mat_mvp;
			output.varying_vec2f[VARYING_TEXUV] = cube_mesh.vertices[index].uv;

			return pos;
		};

	auto frag_tex = [&](ShaderContext& vert_input) -> Vec4f
		{
			Vec2f uv = vert_input.varying_vec2f[VARYING_TEXUV];
			return model.normal(uv).xyz1();
		};

	// renderer init
	Render render(WIDTH, HEIGHT);
	render.init_renderer(hWnd);
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
			render.clear_buffer();

			mat_view = matrix_set_lookat(camera.pos, camera.target, camera.up);
			mat_proj = matrix_set_perspective(fovy, aspect, zn, zf);

			// draw plane
			//mat_model = matrix_set_scale(5,1,5);
			//mat_model_it = matrix_invert(mat_model).Transpose();
			//mat_mvp = mat_model * mat_view * mat_proj;
			//render.drawcall(plane_mesh, vert_normal, frag_normal);


			// draw person
			mat_model = matrix_set_identity();
			mat_model_it = matrix_invert(mat_model).Transpose();
			mat_mvp = mat_model * mat_view * mat_proj;
			render.drawcall(mesh, vert_gouraud_tex, frag_gouraud_tex);

			//mat_model = matrix_set_identity();
			//mat_model_it = matrix_invert(mat_model).Transpose();
			//mat_mvp = mat_model * mat_view * mat_proj;
			//render.drawcall(cube_mesh, vert_normal_cube, frag_normal_cube);

			// Resolve msaa
			if (MSAA_ENABLE)
				render.resolve_pixel();

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

	render.shutdown();
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
