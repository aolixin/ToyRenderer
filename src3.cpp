#if 0
#include <windows.h>
#include "gdi_renderer.h"
#include "geometry.h"
#include "shader_instance.h"
#include "util.h"
#include "camera.h"
#include "model.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);


constexpr int WIDTH = 1920;
constexpr int HEIGHT = 1080;

Camera camera({0, 0, 3}, {0, 0, 0}, {0, 1, 0});

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

	// ����mesh
	Model model("diablo3_pose.obj");
	Mesh mesh;

	int face_num = model.nfaces();
	mesh.vertices.reserve(face_num * 3);
	mesh.indices.reserve(face_num * 3);

	for (int i = 0; i < face_num; i++)
	{
		// ����������������룬�� VS ��ȡ
		for (int j = 0; j < 3; j++)
		{
			Vertex v;
			v.pos = model.vert(i, j);
			v.normal = model.normal(i, j);
			v.uv = model.uv(i, j);
			mesh.vertices.push_back(v);
		}
		// .obj ��������˳ʱ��, ����meshʱ�ĳ���ʱ��
		mesh.indices.push_back(i * 3);
		mesh.indices.push_back(i * 3 + 2);
		mesh.indices.push_back(i * 3 + 1);
	}

	auto vert_gouraud_tex = [&](int index, const Mesh& mesh, const ShaderInput& input, ShaderContext& output) -> Vec4f
	{
		const auto& mat_mvp = input.mat_mvp;
		Vec4f pos = mesh.vertices[index].pos.xyz1() * mat_mvp;
		Vec3f pos_world = (mesh.vertices[index].pos.xyz1() * input.mat_model).xyz();
		// ����ģ�Ͷ��㵽�۾��ķ���
		Vec3f eye_dir = input.eye_pos - pos_world;
		output.varying_vec2f[VARYING_UV] = mesh.vertices[index].uv;
		output.varying_vec3f[VARYING_EYE] = eye_dir;

		return pos;
	};


	auto frag_gouraud_tex = [&](const ShaderInput& input, ShaderContext& vert_input) -> Vec4f
	{
		Vec3f light_dir = {2, 0, 2};
		Vec2f uv = vert_input.varying_vec2f[VARYING_UV];
		// ģ���ϵ�ǰ�㵽�۾��ķ���
		Vec3f eye_dir = vert_input.varying_vec3f[VARYING_EYE];
		// ��һ�����շ���
		Vec3f l = vector_normalize(light_dir);
		// ������ͼȡ������ת��Ϊ��������ϵ
		Vec3f n = (model.normal(uv).xyz1() * input.mat_model_it).xyz();
		// ��ģ����ȡ����ǰ��ĸ߹����
		float s = model.Specular(uv);
		// ���㷴�����
		Vec3f r = vector_normalize(n * vector_dot(n, l) * 2.0f - l);
		// ����߹�
		float p = Saturate(vector_dot(r, eye_dir));
		float spec = Saturate(pow(p, s * 20) * 0.05);
		// �ۺϹ���ǿ��
		float intense = Saturate(vector_dot(n, l)) + 0.2f + spec;
		Vec4f color = model.diffuse(uv);
		return color * intense;
	};

	// renderer init
	Render render(WIDTH, HEIGHT);
	render.initRenderer(hWnd);


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
			shader_input_ref.eye_pos = camera.pos;
			shader_input_ref.mat_model = matrix_set_identity();
			shader_input_ref.mat_model_it = matrix_invert(shader_input_ref.mat_model).Transpose();
			shader_input_ref.mat_view = matrix_set_lookat(camera.pos, camera.target, camera.up);
			shader_input_ref.mat_proj = matrix_set_perspective(3.1415926f * 0.5f, (float)WIDTH / HEIGHT, 1.0, 500.0f);
			shader_input_ref.mat_mvp = shader_input_ref.mat_model * shader_input_ref.mat_view * shader_input_ref.
				mat_proj;
			render.drawCall(mesh, vert_gouraud_tex, frag_gouraud_tex);


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
