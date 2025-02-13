#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx9.h"
#include "imgui/imgui_impl_win32.h"
#include <dwmapi.h>
#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dwmapi.lib")
#include <cstdlib>

IDirect3D9Ex* p_object = NULL;
IDirect3DDevice9Ex* p_device = NULL;
D3DPRESENT_PARAMETERS p_params = { NULL };
MSG messager = { NULL };
HWND my_wnd = NULL;
HWND game_wnd = NULL;
DWORD processID;
HRESULT directx_init()
{
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_object))) exit(3);
	ZeroMemory(&p_params, sizeof(p_params));
	p_params.Windowed = TRUE;
	p_params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	p_params.hDeviceWindow = my_wnd;
	p_params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	p_params.BackBufferFormat = D3DFMT_A8R8G8B8;
	p_params.BackBufferWidth = settings::width;
	p_params.BackBufferHeight = settings::height;
	p_params.EnableAutoDepthStencil = TRUE;
	p_params.AutoDepthStencilFormat = D3DFMT_D16;
	p_params.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	if (FAILED(p_object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, my_wnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &p_params, 0, &p_device)))
	{
		p_object->Release();
		exit(4);
	}
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(my_wnd);
	ImGui_ImplDX9_Init(p_device);
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.IniFilename = 0;
	ImGuiStyle* style = &ImGui::GetStyle();
	style->WindowBorderSize = 1;
	style->FrameBorderSize = 1;
	style->WindowTitleAlign = { 0.5f, 0.5f };
	style->Colors[ImGuiCol_BorderShadow] = ImColor(0, 0, 0, 0);
	style->Colors[ImGuiCol_Border] = ImColor(80, 10, 171, 250);  
	style->Colors[ImGuiCol_TitleBg] = ImColor(50, 0, 75, 250);  
	style->Colors[ImGuiCol_TitleBgActive] = ImColor(80, 10, 171, 250);  
	style->Colors[ImGuiCol_WindowBg] = ImColor(38, 0, 75, 250);                    //your window colors for the menu 
	style->Colors[ImGuiCol_FrameBg] = ImColor(50, 0, 100, 245);
	style->Colors[ImGuiCol_FrameBgActive] = ImColor(80, 10, 171, 245);
	style->Colors[ImGuiCol_FrameBgHovered] = ImColor(100, 0, 180, 245);
	style->Colors[ImGuiCol_Button] = ImColor(80, 0, 140, 245);
	style->Colors[ImGuiCol_ButtonActive] = ImColor(100, 0, 180, 245);
	style->Colors[ImGuiCol_ButtonHovered] = ImColor(120, 0, 200, 245);
	style->Colors[ImGuiCol_SliderGrab] = ImColor(120, 0, 180, 250);
	style->Colors[ImGuiCol_SliderGrabActive] = ImColor(150, 0, 200, 250);
	style->Colors[ImGuiCol_CheckMark] = ImColor(160, 0, 220, 250);
	style->Colors[ImGuiCol_Header] = ImColor(120, 0, 180, 250);
	style->Colors[ImGuiCol_HeaderActive] = ImColor(140, 0, 200, 250);
	style->Colors[ImGuiCol_HeaderHovered] = ImColor(160, 0, 220, 250);
	style->Colors[ImGuiCol_PopupBg] = ImColor(38, 0, 75, 250);
	p_object->Release();
	return S_OK;
}


void create_overlay()
{
	WNDCLASSEXA wcex = {
		sizeof(WNDCLASSEXA),
		0,
		DefWindowProcA,
		0,
		0,
		0,
		LoadIcon(0, IDI_APPLICATION),
		LoadCursor(0, IDC_ARROW),
		0,
		0,
		"SofMainFNBase",
		LoadIcon(0, IDI_APPLICATION)
	};
	ATOM rce = RegisterClassExA(&wcex);
	RECT rect;
	GetWindowRect(GetDesktopWindow(), &rect);
	my_wnd = gui::create_window_in_band(0, rce, L"SofMainFNBase", WS_POPUP, rect.left, rect.top, rect.right, rect.bottom, 0, 0, wcex.hInstance, 0, gui::ZBID_UIACCESS);
	SetWindowLong(my_wnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
	SetLayeredWindowAttributes(my_wnd, RGB(0, 0, 0), 255, LWA_ALPHA);
	MARGINS margin = { -1 };
	DwmExtendFrameIntoClientArea(my_wnd, &margin);
	ShowWindow(my_wnd, SW_SHOW);
	UpdateWindow(my_wnd);
}

void aimbot(uintptr_t target_mesh)
{
	if (!target_mesh) return;
	if (!is_visible(target_mesh)) return;

	Vector3 head3d = get_entity_bone(target_mesh, 110);
	Vector2 head2d = project_world_to_screen(head3d);
	Vector2 target{};

	if (head2d.x != 0)
	{
		
		float diff_x = head2d.x - settings::screen_center_x;

		
		target.x = diff_x / settings::aimbot::smoothness;

		
		target.x += (rand() % 3 - 1) * 0.02f;  
	}

	if (head2d.y != 0)
	{
		
		float diff_y = head2d.y - settings::screen_center_y;

		
		target.y = diff_y / settings::aimbot::smoothness;

		
		target.y += (rand() % 3 - 1) * 0.02f; 
	}

	
	if (target.x + settings::screen_center_x > settings::screen_center_x * 2) target.x = 0;
	if (target.x + settings::screen_center_x < 0) target.x = 0;
	if (target.y + settings::screen_center_y > settings::screen_center_y * 2) target.y = 0;
	if (target.y + settings::screen_center_y < 0) target.y = 0;

	
	input::move_mouse(target.x, target.y);
}
void draw_cornered_box(int x, int y, int w, int h, const ImColor color, int thickness)
{
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x, y + (h / 3)), color, thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x, y), ImVec2(x + (w / 3), y), color, thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w - (w / 3), y), ImVec2(x + w, y), color, thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w, y), ImVec2(x + w, y + (h / 3)), color, thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x, y + h - (h / 3)), ImVec2(x, y + h), color, thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x, y + h), ImVec2(x + (w / 3), y + h), color, thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w - (w / 3), y + h), ImVec2(x + w, y + h), color, thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(x + w, y + h - (h / 3)), ImVec2(x + w, y + h), color, thickness);
}


void Box(int X, int Y, int W, int H, const ImColor color, int thickness)
{
	ImGui::GetForegroundDrawList()->AddLine(ImVec2{ (float)X, (float)Y }, ImVec2{ (float)(X + W), (float)Y }, color, thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2{ (float)(X + W), (float)Y }, ImVec2{ (float)(X + W), (float)(Y + H) }, color, thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2{ (float)X, (float)(Y + H) }, ImVec2{ (float)(X + W), (float)(Y + H) }, color, thickness);
	ImGui::GetForegroundDrawList()->AddLine(ImVec2{ (float)X, (float)Y }, ImVec2{ (float)X, (float)(Y + H) }, color, thickness);
}

void draw_filled_rect(int x, int y, int w, int h, const ImColor color)
{
	ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), color, 0, 0);
}

void draw_line(Vector2 target, const ImColor color)
{
	ImGui::GetForegroundDrawList()->AddLine(ImVec2(settings::screen_center_x, settings::height), ImVec2(target.x, target.y), color, 0.1f);
}

void draw_distance(Vector2 location, float distance, const ImColor color)
{
	char dist[64];
	sprintf_s(dist, "%.fm", distance);
	ImVec2 text_size = ImGui::CalcTextSize(dist);
	ImGui::GetForegroundDrawList()->AddText(ImVec2(location.x - text_size.x / 2, location.y - text_size.y / 2), color, dist);
}

#include <chrono>

using namespace std::chrono;

void Debugg()
{
	
	static auto lastTime = high_resolution_clock::now();
	static int frameCount = 0;

	
	auto currentTime = high_resolution_clock::now();
	duration<float> deltaTime = currentTime - lastTime;
	lastTime = currentTime;
	frameCount++;

	
	static float fps = 0.0f;
	if (deltaTime.count() >= 1.0f) {
		fps = frameCount / deltaTime.count();
		frameCount = 0;
	}

	
	const char* text = "baseid %p", Base = Driver::GetBase();; //this was another debugging but i dont use it 


	
	char fpsText[64];
	snprintf(fpsText, sizeof(fpsText), "FPS: %.2f", fps);

	const char* fullText = nullptr;
	char combinedText[256];
	snprintf(combinedText, sizeof(combinedText), "%s | %s", text, fpsText);
	fullText = combinedText;

	
	ImVec2 position = ImVec2(30, 30); 
	ImVec2 textSize = ImGui::CalcTextSize(fullText);

	
	ImVec2 newPosition = ImVec2(position.x, position.y + textSize.y);

	
	ImGui::SetCursorPos(newPosition);
	ImGui::Text("%s", fullText);
}

ImVec2 GetWatermarkSize()
{
	ImVec2 position = ImVec2(30, 30);
	const char* text = "not using ";


	ImVec2 textSize = ImGui::CalcTextSize(text);
	return ImVec2(position.x, position.y + textSize.y);
}



void game_loop()
{
	cache::uworld = read<uintptr_t>(Base + UWORLD);
	cache::game_instance = read<uintptr_t>(cache::uworld + GAME_INSTANCE);
	cache::local_players = read<uintptr_t>(read<uintptr_t>(cache::game_instance + LOCAL_PLAYERS));
	cache::player_controller = read<uintptr_t>(cache::local_players + PLAYER_CONTROLLER);
	cache::local_pawn = read<uintptr_t>(cache::player_controller + LOCAL_PAWN);
	if (cache::local_pawn != 0)
	{
		cache::root_component = read<uintptr_t>(cache::local_pawn + ROOT_COMPONENT);
		cache::relative_location = read<Vector3>(cache::root_component + RELATIVE_LOCATION);
		cache::player_state = read<uintptr_t>(cache::local_pawn + PLAYER_STATE);
		cache::my_team_id = read<int>(cache::player_state + TEAM_INDEX);
	}
	cache::game_state = read<uintptr_t>(cache::uworld + GAME_STATE);
	cache::player_array = read<uintptr_t>(cache::game_state + PLAYER_ARRAY);
	cache::player_count = read<int>(cache::game_state + (PLAYER_ARRAY + sizeof(uintptr_t)));
	cache::closest_distance = FLT_MAX;
	cache::closest_mesh = NULL;
	for (int i = 0; i < cache::player_count; i++)
	{
		uintptr_t player_state = read<uintptr_t>(cache::player_array + (i * sizeof(uintptr_t)));
		if (!player_state) continue;
		int player_team_id = read<int>(player_state + TEAM_INDEX);
		if (player_team_id == cache::my_team_id) continue;
		uintptr_t pawn_private = read<uintptr_t>(player_state + PAWN_PRIVATE);
		if (!pawn_private) continue;
		if (pawn_private == cache::local_pawn) continue;
		uintptr_t mesh = read<uintptr_t>(pawn_private + MESH);
		if (!mesh) continue;
		Vector3 head3d = get_entity_bone(mesh, 110);
		Vector2 head2d = project_world_to_screen(head3d);
		Vector3 bottom3d = get_entity_bone(mesh, 0);
		Vector2 bottom2d = project_world_to_screen(bottom3d);
		float box_height = abs(head2d.y - bottom2d.y);
		float box_width = box_height * 0.50f;
		float distance = cache::relative_location.distance(bottom3d) / 100;
		if (settings::visuals::enable)
		{
			if (settings::visuals::Box)
			{
				if (is_visible(mesh))
				{
					Box(head2d.x - (box_width / 2), head2d.y, box_width, box_height, ImColor(250, 250, 250, 250), 1);
				}
				else
				{
					Box(head2d.x - (box_width / 2), head2d.y, box_width, box_height, ImColor(250, 0, 0, 250), 1);
				}
				if (settings::visuals::fill_box) draw_filled_rect(head2d.x - (box_width / 2), head2d.y, box_width, box_height, ImColor(0, 0, 0, 50));
			}
		}
		if (settings::visuals::enable)
		{
			if (settings::visuals::CorneredBox)
			{
				if (is_visible(mesh))
				{
					draw_cornered_box(head2d.x - (box_width / 2), head2d.y, box_width, box_height, ImColor(250, 250, 250, 250), 1);
				}
				else
				{
					draw_cornered_box(head2d.x - (box_width / 2), head2d.y, box_width, box_height, ImColor(250, 0, 0, 250), 1);
				}
				if (settings::visuals::fill_box) draw_filled_rect(head2d.x - (box_width / 2), head2d.y, box_width, box_height, ImColor(0, 0, 0, 50));
			}
			if (settings::visuals::niggerskel)
			{
				Vector2 bonePositions[16];
				const int boneIndices[] = { 110, 3, 66, 9, 38, 10, 39, 11, 40, 78, 71, 79, 72, 75, 82, 67 };
				for (int i = 0; i < 16; ++i)
				{
					bonePositions[i] = project_world_to_screen(get_entity_bone(mesh, boneIndices[i]));
				}
				std::pair<int, int> bonePairs[] =
				{
					{1, 2}, {3, 4}, {4, 3}, {5, 3}, {6, 4}, {5, 7}, {6, 8},
					{10, 1}, {9, 1}, {12, 10}, {11, 9}, {13, 12}, {14, 11}, {2, 15}, { 15, 0 }
				};

				ImGui::GetBackgroundDrawList()->PushClipRectFullScreen();
				for (const auto& pair : bonePairs)
				{
					ImVec2 start(bonePositions[pair.first].x, bonePositions[pair.first].y);
					ImVec2 end(bonePositions[pair.second].x, bonePositions[pair.second].y);

					if (pair.first == 110)
					{
						end.y += 20;
					}

					if (is_visible(mesh))
					{
						ImGui::GetBackgroundDrawList()->AddLine(start, end, ImColor(255, 0, 255, 250), 1);
					}
					else
					{
						ImGui::GetBackgroundDrawList()->AddLine(start, end, ImColor(255, 165, 0, 250), 1);
					}

				}
				ImGui::GetBackgroundDrawList()->PopClipRect();
			}
			if (settings::visuals::line)
			{
				if (is_visible(mesh))
				{
					draw_line(bottom2d, ImColor(250, 250, 250, 250));
				}
				else
				{
					draw_line(bottom2d, ImColor(250, 0, 0, 250));
				}
			}
			if (settings::visuals::distance)
			{
				draw_distance(bottom2d, distance, ImColor(250, 250, 250, 250));
			}
		}
		double dx = head2d.x - settings::screen_center_x;
		double dy = head2d.y - settings::screen_center_y;
		float dist = sqrtf(dx * dx + dy * dy);
		if (dist <= settings::aimbot::fov && dist < cache::closest_distance)
		{
			cache::closest_distance = dist;
			cache::closest_mesh = mesh;
		}
	}
	
	if (settings::aimbot::enable)
	{
		if (GetAsyncKeyState(settings::aimbot::current_key)) aimbot(cache::closest_mesh);
	}
#include <cmath>  

	static float rotation_angle = 0.0f;  
	static float color_time = 0.0f;     

	if (crosshair::Crosshair)
	{
		ImDrawList* draw_list = ImGui::GetForegroundDrawList();
		ImVec2 screen_size = ImGui::GetIO().DisplaySize;
		ImVec2 center = ImVec2(screen_size.x / 2, screen_size.y / 2);

		float crosshair_length = crosshair::crosshair_length;
		float arm_length = crosshair_length / 2;
		float thickness = crosshair::crosshair_thickness;

		
		float r = (sin(color_time) + 1.0f) / 2.0f;  
		float g = (sin(color_time + 2.0f) + 1.0f) / 2.0f;
		float b = (sin(color_time + 4.0f) + 1.0f) / 2.0f;

		ImVec4 crosshairColor = ImVec4(r, g, b, 1.0f);
		ImU32 crosshairColorU32 = IM_COL32((int)(r * 255), (int)(g * 255), (int)(b * 255), 255);

		
		auto RotatePoint = [&](ImVec2 point, float angle) -> ImVec2 {
			float s = sin(angle);
			float c = cos(angle);

			float xnew = (point.x - center.x) * c - (point.y - center.y) * s + center.x;
			float ynew = (point.x - center.x) * s + (point.y - center.y) * c + center.y;

			return ImVec2(xnew, ynew);
			};

		
		ImVec2 horizontal_start = RotatePoint(ImVec2(center.x - crosshair_length, center.y), rotation_angle);
		ImVec2 horizontal_end = RotatePoint(ImVec2(center.x + crosshair_length, center.y), rotation_angle);
		ImVec2 vertical_start = RotatePoint(ImVec2(center.x, center.y - crosshair_length), rotation_angle);
		ImVec2 vertical_end = RotatePoint(ImVec2(center.x, center.y + crosshair_length), rotation_angle);

		draw_list->AddLine(horizontal_start, horizontal_end, crosshairColorU32, thickness);
		draw_list->AddLine(vertical_start, vertical_end, crosshairColorU32, thickness);

		
		ImVec2 tl1 = RotatePoint(ImVec2(center.x - crosshair_length, center.y), rotation_angle);
		ImVec2 tl2 = RotatePoint(ImVec2(center.x - crosshair_length, center.y - arm_length), rotation_angle);
		ImVec2 tl3 = RotatePoint(ImVec2(center.x - crosshair_length + arm_length, center.y - arm_length), rotation_angle);

		ImVec2 tr1 = RotatePoint(ImVec2(center.x, center.y - crosshair_length), rotation_angle);
		ImVec2 tr2 = RotatePoint(ImVec2(center.x + arm_length, center.y - crosshair_length), rotation_angle);
		ImVec2 tr3 = RotatePoint(ImVec2(center.x + arm_length, center.y - crosshair_length + arm_length), rotation_angle);

		ImVec2 br1 = RotatePoint(ImVec2(center.x + crosshair_length, center.y), rotation_angle);
		ImVec2 br2 = RotatePoint(ImVec2(center.x + crosshair_length, center.y + arm_length), rotation_angle);
		ImVec2 br3 = RotatePoint(ImVec2(center.x + crosshair_length - arm_length, center.y + arm_length), rotation_angle);

		ImVec2 bl1 = RotatePoint(ImVec2(center.x, center.y + crosshair_length), rotation_angle);
		ImVec2 bl2 = RotatePoint(ImVec2(center.x - arm_length, center.y + crosshair_length), rotation_angle);
		ImVec2 bl3 = RotatePoint(ImVec2(center.x - arm_length, center.y + crosshair_length - arm_length), rotation_angle);

		
		draw_list->AddLine(tl1, tl2, crosshairColorU32, thickness);
		draw_list->AddLine(tl2, tl3, crosshairColorU32, thickness);

		draw_list->AddLine(tr1, tr2, crosshairColorU32, thickness);
		draw_list->AddLine(tr2, tr3, crosshairColorU32, thickness);

		draw_list->AddLine(br1, br2, crosshairColorU32, thickness);
		draw_list->AddLine(br2, br3, crosshairColorU32, thickness);

		draw_list->AddLine(bl1, bl2, crosshairColorU32, thickness);
		draw_list->AddLine(bl2, bl3, crosshairColorU32, thickness);

		
		rotation_angle += 0.02f;   
		color_time += 0.01f;       
	}


	if (settings::Features::Watermark)
	{
		int FPS = ImGui::GetIO().Framerate;

		float Time = ImGui::GetTime();
		int r = (int)(sin(Time * 2.0f) * 127.5f + 127.5f);
		int g = (int)(sin(Time * 2.0f + 2.0f) * 127.5f + 127.5f);
		int b = (int)(sin(Time * 2.0f + 4.0f) * 127.5f + 127.5f);

		const char* text = "Sofmain - Landen1337";
		ImVec2 textSize = ImGui::CalcTextSize(text);
		float textX = 15 + (250 - textSize.x) * 0.5f;
		float textY = 15 + (37 - textSize.y) * 0.5f;

		ImDrawList* drawList = ImGui::GetBackgroundDrawList();
		ImVec4 bgColor = ImVec4(0.0f, 0.0f, 0.0f, 0.8f);
		drawList->AddRectFilled(ImVec2(15, 15), ImVec2(265, 52), ImColor(bgColor), 10.0f);
		drawList->AddRect(ImVec2(15, 15), ImVec2(265, 52), ImColor(r, g, b, 255), 10.0f, 0, 2.0f);
		drawList->AddText(ImVec2(textX + 1, textY + 1), ImColor(0, 0, 0, 180), text);
		drawList->AddText(ImVec2(textX, textY), ImColor(r, g, b, 255), text);
	}
	if (settings::Features::Debug) {


		Debugg();

	}
}


void render_menu()
{
	switch (settings::aimbot::current_aimkey)
	{
	case 0:
		settings::aimbot::current_key = VK_LBUTTON;
	case 1:
		settings::aimbot::current_key = VK_RBUTTON;
	}
	if (settings::aimbot::show_fov)
	{
		float Time = ImGui::GetTime();
		int r = (int)(sin(Time * 2.0f) * 127.5f + 127.5f);
		int g = (int)(sin(Time * 2.0f + 2.0f) * 127.5f + 127.5f);
		int b = (int)(sin(Time * 2.0f + 4.0f) * 127.5f + 127.5f);
		ImColor Dynamic(r, g, b);

		// Increase the thickness value to make the circle thicker (e.g., 3.0f)
		ImGui::GetForegroundDrawList()->AddCircle(ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), settings::aimbot::fov, Dynamic, 100, 3.0f);
	}
	if (GetAsyncKeyState(VK_INSERT) & 1) settings::show_menu = !settings::show_menu;
	if (settings::show_menu)	
	{
		ImGui::SetNextWindowSize({ 620, 350 });
		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 18.0f);
		ImGui::Begin("SofMainFNBase", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);

		
		ImVec2 windowPos = ImGui::GetWindowPos();
		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
		{
			ImVec2 mouseDelta = ImGui::GetIO().MouseDelta;
			ImGui::SetWindowPos(ImVec2(windowPos.x + mouseDelta.x, windowPos.y + mouseDelta.y));
		}

		
		
		ImVec2 windowSize = ImGui::GetWindowSize();
		ImVec2 textSize = ImGui::CalcTextSize("SofMain - Landen1337 | .gg/visualstudio");

		
		ImVec2 textPos = ImVec2((windowSize.x - textSize.x) / 2, 10); 

		
		ImGui::SetCursorPos(textPos);

		
		ImGui::Text("SofMain - Landen1337 | .gg/visualstudio");


		ImGui::Separator();

		
		ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 12.0f);            //for rounded buttons/ for menu 
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, { 14, 7 }); 

		ImGui::PushStyleColor(ImGuiCol_Button, { 0.85f, 0.3f, 1.0f, 1.0f });  
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, { 0.75f, 0.1f, 1.0f, 1.0f });  
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, { 0.65f, 0.0f, 0.9f, 1.0f });  


		const float buttonSpacing = 15.0f;
		const float buttonWidth = 190.0f;
		const ImVec2 buttonSize = { 100, 30 };
		const float startX = (ImGui::GetWindowSize().x - (buttonWidth * 3 + buttonSpacing * 2)) / 2;

		
		static float currentTabPos = 0.0f;
		static float animationStartTime = 0.0f;
		const float animationDuration = 0.2f;

		float targetPos = settings::tab * (buttonWidth + buttonSpacing);
		if (ImGui::IsItemClicked()) animationStartTime = ImGui::GetTime();

		float elapsedTime = ImGui::GetTime() - animationStartTime;
		float t = elapsedTime / animationDuration;
		t = (t > 1.0f) ? 1.0f : t;

		
		currentTabPos = (1.0f - t) * currentTabPos + t * targetPos;

		
		ImGui::SetCursorPos({ startX, 50 });
		if (ImGui::Button("Aimbot", buttonSize)) settings::tab = 0;
		ImGui::SameLine(0.0f, buttonSpacing);
		if (ImGui::Button("Visuals", buttonSize)) settings::tab = 1;
		ImGui::SameLine(0.0f, buttonSpacing);
		if (ImGui::Button("Misc", buttonSize)) settings::tab = 2;

		
		ImGui::GetWindowDrawList()->AddRectFilled(
			{ startX + currentTabPos, 90 },
			{ startX + currentTabPos + buttonWidth, 95 },
			ImGui::GetColorU32({ 0.85f, 0.0f, 1.0f, 0.8f })
		);

		
		switch (settings::tab)
		{
		case 0:
			ImGui::TextColored({ 0.8f, 0.4f, 1.0f, 1.0f }, "Aimbot Settings");
			ImGui::Separator();
			ImGui::Spacing();

			if (ImGui::CollapsingHeader("Enable & Visibility"))
			{
				ImGui::Checkbox("Enable Aimbot", &settings::aimbot::enable);
				ImGui::Checkbox("Show FOV Circle", &settings::aimbot::show_fov);
			}

			if (ImGui::CollapsingHeader("Aimbot Configuration"))
			{
				ImGui::Columns(2, NULL, false);
				ImGui::Text("Field of View");
				ImGui::NextColumn();
				ImGui::SliderFloat("##Fov", &settings::aimbot::fov, 50, 300, "%.2f°");
				ImGui::Columns(1);

				ImGui::Columns(2, NULL, false);
				ImGui::Text("Aim Smoothness");
				ImGui::NextColumn();
				ImGui::SliderFloat("##Smoothness", &settings::aimbot::smoothness, 1, 10, "%.2f");
				ImGui::Columns(1);
			}

			if (ImGui::CollapsingHeader("Aim Key"))
			{
				ImGui::Combo("Select Aim Key", &settings::aimbot::current_aimkey,
					settings::aimbot::aimkey, IM_ARRAYSIZE(settings::aimbot::aimkey));
			}

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			if (ImGui::Button("Reset Aimbot Settings", { 200, 30 }))
			{
				settings::aimbot::fov = 90;
				settings::aimbot::smoothness = 5;
			}
			break;

		case 1: 
			ImGui::TextColored({ 0.8f, 0.3f, 1.0f, 1.0f }, "Visuals Settings");
			ImGui::Separator();
			ImGui::Spacing();

			if (ImGui::CollapsingHeader("ESP Settings"))
			{
				ImGui::Checkbox("Enable ESP", &settings::visuals::enable);
				ImGui::Checkbox("Skeleton", &settings::visuals::niggerskel);
				ImGui::Checkbox("Fill Box", &settings::visuals::fill_box);
				ImGui::Checkbox("Regular Box", &settings::visuals::Box);
				ImGui::Checkbox("Draw Lines", &settings::visuals::line);
				ImGui::Checkbox("Show Distance", &settings::visuals::distance);
			}

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			if (ImGui::Button("Reset Visuals Settings", { 200, 30 }))
			{
				settings::visuals::enable = false;
				settings::visuals::niggerskel = false;
				settings::visuals::fill_box = false;
				settings::visuals::Box = false;
				settings::visuals::line = false;
				settings::visuals::distance = false;
			}
			break;

		case 2: 
			ImGui::TextColored({ 0.0f, 0.8f, 1.0f, 1.0f }, "Misc");
			ImGui::Separator();
			ImGui::Spacing();

			if (ImGui::Button("Unload", ImVec2(150, 35))) {
				ImGui::OpenPopup("Unload Confirmation");
			}
			if (ImGui::BeginPopupModal("Unload Confirmation", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
				ImGui::Text("Are you sure you want to exit?");
				ImGui::Spacing();

				if (ImGui::Button("Yes", ImVec2(100, 30))) {
					exit(0);
				}
				ImGui::SameLine();
				if (ImGui::Button("Cancel", ImVec2(100, 30))) {
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
			if (ImGui::Button("Debug mode", ImVec2(150, 35))) {
				ImGui::OpenPopup("Debug mode");
			} 
			if (ImGui::BeginPopupModal("Debug mode", NULL, ImGuiWindowFlags_AlwaysAutoResize)) {
				ImGui::Text("Base Address: %p\n", (void*)Base);
				ImGui::Spacing();
				ImGui::Spacing();
				ImGui::SameLine();
				if (ImGui::Button("close", ImVec2(100, 30))) {
					ImGui::CloseCurrentPopup();
				}
				ImGui::EndPopup();
			}
			
			break;
		}
		ImGui::End();

	}
}

HWND get_process_wnd(uint32_t pid)
{
	std::pair<HWND, uint32_t> params = { 0, pid };
	BOOL bresult = EnumWindows([](HWND hwnd, LPARAM lparam) -> BOOL
	{
		auto pparams = (std::pair<HWND, uint32_t>*)(lparam);
		uint32_t processid = 0;
		if (GetWindowThreadProcessId(hwnd, reinterpret_cast<LPDWORD>(&processid)) && processid == pparams->second)
		{
			SetLastError((uint32_t)-1);
			pparams->first = hwnd;
			return FALSE;
		}
		return TRUE;
	}, (LPARAM)&params);
	if (!bresult && GetLastError() == -1 && params.first) return params.first;
	return 0;
}

WPARAM render_loop()
{
	static RECT old_rc;
	ZeroMemory(&messager, sizeof(MSG));
	while (messager.message != WM_QUIT)
	{
		if (PeekMessage(&messager, my_wnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&messager);
			DispatchMessage(&messager);
		}
		if (game_wnd == NULL) exit(0);
		HWND active_wnd = GetForegroundWindow();
		if (active_wnd == game_wnd)
		{
			HWND target_wnd = GetWindow(active_wnd, GW_HWNDPREV);
			SetWindowPos(my_wnd, target_wnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		else
		{
			game_wnd = get_process_wnd(processID);
			Sleep(250);
		}
		RECT rc;
		POINT xy;
		ZeroMemory(&rc, sizeof(RECT));
		ZeroMemory(&xy, sizeof(POINT));
		rc.left = xy.x;
		rc.top = xy.y;
		ImGuiIO& io = ImGui::GetIO();
		io.DeltaTime = 1.0f / 60.0f;
		POINT p;
		GetCursorPos(&p);
		io.MousePos.x = p.x - xy.x;
		io.MousePos.y = p.y - xy.y;
		if (GetAsyncKeyState(0x1))
		{
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].x = io.MousePos.y;
		}
		else
		{
			io.MouseDown[0] = false;
		}
		if (rc.left != old_rc.left || rc.right != old_rc.right || rc.top != old_rc.top || rc.bottom != old_rc.bottom)
		{
			old_rc = rc;
			settings::width = rc.right;
			settings::height = rc.bottom;
			p_params.BackBufferWidth = settings::width;
			p_params.BackBufferHeight = settings::height;
			SetWindowPos(my_wnd, (HWND)0, xy.x, xy.y, settings::width, settings::height, SWP_NOREDRAW);
			p_device->Reset(&p_params);
		}
		ImGui_ImplDX9_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		game_loop();
		render_menu();
		ImGui::EndFrame();
		p_device->SetRenderState(D3DRS_ZENABLE, false);
		p_device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
		p_device->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
		p_device->Clear(0, 0, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
		if (p_device->BeginScene() >= 0)
		{
			ImGui::Render();
			ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
			p_device->EndScene();
		}
		HRESULT result = p_device->Present(0, 0, 0, 0);
		if (result == D3DERR_DEVICELOST && p_device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			p_device->Reset(&p_params);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
	}
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	if (p_device != 0)
	{
		p_device->EndScene();
		p_device->Release();
	}
	if (p_object != 0) p_object->Release();
	DestroyWindow(my_wnd);
	return messager.wParam;
}
