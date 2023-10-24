#include <Windows.h>
#include <dwmapi.h>
#include <d3d11.h>

#include "extras.h"
 

#include <imgui/imgui.h>
#include <imgui/imgui_impl_dx11.h>
#include <imgui/imgui_impl_win32.h>



bool showMenu = false; // Initially, the menu is hidden

bool showEspPage = true;
bool showAimbotPage = false;
bool showMiscPage = false;
bool showInfoPage = false;
bool showSettingsPage = false;
bool enableESP = false;
bool enableAimbot = false;
bool teamESP = false;
ImVec4 boneColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
ImVec4 tracerColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
ImVec4 nameColor = ImVec4(1.0f, 0.0f, 0.0f, 1.0f);
bool showBones = false;
bool showNames = false;
bool showTracers = false;
float floatValue = 50.0f;
float smoothValue = 2.0f;
Vector2 screenCenter(3440.0f / 2, 1440.0f / 2);
std::vector<Vector2> headPositions; // Your list of head positions
Vector2 closestHead;

float screenWidth = 3440.0f;
float screenHeight = 1440.0f;
float centerX = screenWidth / 2.0f;
float centerY = screenHeight / 2.0f;

float Distance(const Vector2& a, const Vector2& b) {
	float dx = a.x - b.x;
	float dy = a.y - b.y;
	return std::sqrt(dx * dx + dy * dy);
}

Vector2 FindClosestToCenter(const std::vector<Vector2>& positions, float centerX, float centerY) {
	if (positions.empty()) {
		// No positions to compare, return an invalid value
		return { -1.0f, -1.0f };
	}

	Vector2 closestPosition = positions[0];
	float closestDistance = Distance(closestPosition, { centerX, centerY });

	for (size_t i = 1; i < positions.size(); ++i) {
		float distance = Distance(positions[i], { centerX, centerY });

		if (distance < closestDistance) {
			closestPosition = positions[i];
			closestDistance = distance;
		}
	}

	return closestPosition;
}

// mouse_event(MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE, targetX, targetY, 0, 0);

void ToggleImGuiMenu() {
	showMenu = !showMenu;
}

void RenderImGuiMenu() {

	if (showMenu)
	{
		ImGui::Begin("Cheat Menu");

		if (ImGui::BeginTabBar("MenuTabs")) {
			if (ImGui::BeginTabItem("ESP")) {
				// Render ESP-related options here
				ImGui::Checkbox("Enable ESP", &enableESP);

				if (enableESP)
				{
					
				}
				ImGui::Checkbox("Show Bones", &showBones);

				if (showBones)
				{
					ImGui::ColorEdit4("Bone Color", &boneColor.x);
				}
				ImGui::Checkbox("Show Tracers", &showTracers);

				if (showTracers)
				{
					ImGui::ColorEdit4("Tracer Color", &tracerColor.x);
				}
				ImGui::Checkbox("Show Names", &showNames);
				if (showNames) 
				{
					
				}

				ImGui::Checkbox("Disable Team ESP", &teamESP);
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Aimbot")) {
				ImGui::Checkbox("Enable Aimbot", &enableAimbot);
				
				if (enableAimbot)
				{
					ImGui::SliderFloat("Aimbot FOV", &floatValue, 10.0f, 200.0f);
					ImGui::SliderFloat("Aimbot Smoothing", &smoothValue, 1.f, 10.0f);
				}
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Misc")) {
				// Render Misc-related options here
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Info")) {
				if (ImGui::Begin("Head Positions")) {
					
					for (const Vector2& position : headPositions) {
						ImGui::Text("Head Position: (%.2f, %.2f)", position.x, position.y);
					}
					
					ImGui::End();
				}
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Settings")) {
				// Render Settings-related options here
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		ImGui::End();
	}
	
}

// Check if a key is pressed
bool IsKeyPressed(int key) {
	return (GetAsyncKeyState(key) & 0x8000) != 0;
}



extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK window_procedure(HWND window, UINT message, WPARAM w_param, LPARAM l_param) {
	if (ImGui_ImplWin32_WndProcHandler(window, message, w_param, l_param)) {
		return 0L;
	}

	if (message == WM_DESTROY) {
		PostQuitMessage(0);
		return 0L;
	}

	return DefWindowProc(window, message, w_param, l_param);
}


INT APIENTRY WinMain(HINSTANCE instance, HINSTANCE, PSTR, INT cmd_show) {

	WNDCLASSEXW wc{};
	wc.cbSize = sizeof(WNDCLASSEXW);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = window_procedure;
	wc.hInstance = instance;
	wc.lpszClassName = L"External Overlay Class";

	RegisterClassExW(&wc);

	const HWND window = CreateWindowExW(
		isWindowTransparent ? WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED : WS_EX_TOPMOST | WS_EX_LAYERED,
		wc.lpszClassName,
		L"External Overlay",
		WS_POPUP, // Use windowed style
		0, 0,
		3440, 1440, // Set the width and height to 1920x1080
		nullptr,
		nullptr,
		wc.hInstance,
		nullptr
	);

	SetLayeredWindowAttributes(window, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);

	{
		RECT client_area{};
		GetClientRect(window, &client_area);

		RECT window_area{};
		GetWindowRect(window, &window_area);

		POINT diff{};
		ClientToScreen(window, &diff);

		const MARGINS margins{
			window_area.left + (diff.x - window_area.left),
			window_area.top + (diff.y - window_area.top),
			client_area.right,
			client_area.bottom
		};

		DwmExtendFrameIntoClientArea(window, &margins);
	}

	DXGI_SWAP_CHAIN_DESC sd{};
	sd.BufferDesc.RefreshRate.Numerator = 60U;
	sd.BufferDesc.RefreshRate.Denominator = 1U;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.SampleDesc.Count = 1U;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 2U;
	sd.OutputWindow = window;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	constexpr D3D_FEATURE_LEVEL levels[2]
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_0
	};

	ID3D11Device* device{ nullptr };
	ID3D11DeviceContext* device_context{ nullptr };
	IDXGISwapChain* swap_chain{ nullptr };
	ID3D11RenderTargetView* render_target_view{ nullptr };
	D3D_FEATURE_LEVEL level{};

	D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0U,
		levels,
		2U,
		D3D11_SDK_VERSION,
		&sd,
		&swap_chain,
		&device,
		&level,
		&device_context
	);

	ID3D11Texture2D* back_buffer{ nullptr };
	swap_chain->GetBuffer(0U, IID_PPV_ARGS(&back_buffer));

	if (back_buffer)
	{
		device->CreateRenderTargetView(back_buffer, nullptr, &render_target_view);
		back_buffer->Release();
	}
	else
	{
		return 1;
	}

	ShowWindow(window, SW_SHOW);
	UpdateWindow(window);

	ImGui::CreateContext();
	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(window);
	ImGui_ImplDX11_Init(device, device_context);

	bool running = true;


	HWND GameHWND = FindWindowW(NULL, L"Counter-Strike 2");
	DWORD dwPid;
	GetWindowThreadProcessId(GameHWND, &dwPid);
	TargetProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);
	uintptr_t moduleBase = GetModuleBaseAddress(dwPid, L"client.dll");

	uintptr_t localPlayer = RPM<std::uintptr_t>(moduleBase + dwLocalPlayerController);
	const Vector3 localOrigin = RPM<Vector3>(localPlayer + m_vOldOrigin);
	const uintptr_t entity_list = RPM<uintptr_t>(moduleBase + dwEntityList);

	while (running) {
		MSG msg;

		while (PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (msg.message == WM_QUIT)
			{
				running = false;
			}
		}

		if (!running)
		{
			break;
		}

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();

		ImGui::NewFrame();


		// Create a menu window
		if (IsKeyPressed(VK_INSERT)) {
			Sleep(100);
			// Toggle the WS_EX_TRANSPARENT style
			isWindowTransparent = !isWindowTransparent;

			// Update the window style
			SetWindowLong(window, GWL_EXSTYLE, isWindowTransparent ?
				(GetWindowLong(window, GWL_EXSTYLE) | WS_EX_TRANSPARENT) :
				(GetWindowLong(window, GWL_EXSTYLE) & ~WS_EX_TRANSPARENT));

			// Redraw the window
			SetLayeredWindowAttributes(window, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);
			ToggleImGuiMenu();
		}

		if (enableAimbot)
			ImGui::GetForegroundDrawList()->AddCircle(ImVec2(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y / 2), floatValue, ImGui::ColorConvertFloat4ToU32(boneColor), 32, 1.0f);



		RenderImGuiMenu();

		headPositions.clear();

		const int localTeam = RPM<int>(localPlayer + m_iTeamNum);
		const view_matrix_t view_matrix = RPM<view_matrix_t>(moduleBase + dwViewMatrix);

		for (int i = 0; i < 32; i++)
		{


			uintptr_t list_entry = RPM<uintptr_t>(entity_list + (8 * (i & 0x7FFF) >> 9) + 16);
			if (!list_entry) continue;

			std::uint64_t player = RPM<uintptr_t>(list_entry + 120 * (i & 0x1FF));
			if (!player) continue;

			int health = RPM<int>(player + m_iPawnHealth);
			if (health <= 0) continue;

			const int playerTeam = RPM<int>(player + m_iTeamNum);
			if (teamESP && (playerTeam == localTeam)) {                   // turn bool false for team esp
				continue;
			}

			const std::uint32_t playerPawn = RPM<std::uint32_t>(player + m_hPlayerPawn);

			const uintptr_t list_entry2 = RPM<uintptr_t>(entity_list + 0x8 * ((playerPawn & 0x7FFF) >> 9) + 16);

			const uintptr_t pCSPlayerPawn = RPM<uintptr_t>(list_entry2 + 120 * (playerPawn & 0x1FF));

			std::string playerName = "Invalid Name";
			const DWORD64 playerNameAddress = RPM<DWORD64>(player + m_sSanitizedPlayerName);

			if (playerNameAddress) {
				char buf[256];
				ReadProcessMemory(TargetProcess, reinterpret_cast<LPCVOID>(playerNameAddress), buf, sizeof(buf), NULL);
				playerName = std::string(buf);
			}


			const Vector3 origin = RPM<Vector3>(pCSPlayerPawn + m_vOldOrigin);
			const Vector3 head = { origin.x, origin.y, origin.z + 75.f };

			const Vector3 screenPos = origin.world_to_screen(view_matrix);
			const Vector3 screenHead = head.world_to_screen(view_matrix);

			const float height = screenPos.y - screenHead.y;
			const float width = height / 2.4f;
			
			uint64_t gamescene = RPM<uint64_t>(pCSPlayerPawn + m_pGameSceneNode);
			uint64_t bonearray =RPM<uint64_t>(gamescene + 0x1E0);

			
			if (enableAimbot)
			{
				Vector3 head = RPM<Vector3>(bonearray + 6 * 32);
				Vector3 Ahead;
				if (!wrld_to_screen(head, Ahead, view_matrix))
					continue;
				float distance = Distance(Vector2(Ahead.x, Ahead.y), screenCenter);
				// Check if the distance is within the desired radius
				if (distance <= floatValue) {
					// Add the head position to the filtered list
					headPositions.push_back(Vector2(Ahead.x, Ahead.y));
				}
			}

			if (showBones)
			{

				Vector3 head = RPM<Vector3>(bonearray + 6 * 32);
				Vector3 cou = RPM<Vector3>(bonearray + 5 * 32);
				Vector3 shoulderR = RPM<Vector3>(bonearray + 8 * 32);
				Vector3 shoulderL = RPM<Vector3>(bonearray + 13 * 32);
				Vector3 brasR = RPM<Vector3>(bonearray + 9 * 32);
				Vector3 brasL = RPM<Vector3>(bonearray + 14 * 32);
				Vector3 handR = RPM<Vector3>(bonearray + 11 * 32);
				Vector3 handL = RPM<Vector3>(bonearray + 16 * 32);
				Vector3 cock = RPM<Vector3>(bonearray + 0 * 32);
				Vector3 kneesR = RPM<Vector3>(bonearray + 23 * 32);
				Vector3 kneesL = RPM<Vector3>(bonearray + 26 * 32);
				Vector3 feetR = RPM<Vector3>(bonearray + 24 * 32);
				Vector3 feetL = RPM<Vector3>(bonearray + 27 * 32);

				Vector3 Ahead;
				Vector3 Acou;
				Vector3 AshoulderR;
				Vector3 AshoulderL;
				Vector3 AbrasR;
				Vector3 AbrasL;
				Vector3 AhandR;
				Vector3 AhandL;
				Vector3 Acock;
				Vector3 AkneesR;
				Vector3 AkneesL;
				Vector3 AfeetR;
				Vector3 AfeetL;

				if (!wrld_to_screen(head, Ahead, view_matrix))
					continue;
				if (!wrld_to_screen(cou, Acou, view_matrix))
					continue;
				if (!wrld_to_screen(shoulderR, AshoulderR, view_matrix))
					continue;
				if (!wrld_to_screen(shoulderL, AshoulderL, view_matrix))
					continue;
				if (!wrld_to_screen(brasR, AbrasR, view_matrix))
					continue;
				if (!wrld_to_screen(brasL, AbrasL, view_matrix))
					continue;
				if (!wrld_to_screen(handL, AhandL, view_matrix))
					continue;
				if (!wrld_to_screen(handR, AhandR, view_matrix))
					continue;
				if (!wrld_to_screen(cock, Acock, view_matrix))
					continue;
				if (!wrld_to_screen(kneesR, AkneesR, view_matrix))
					continue;
				if (!wrld_to_screen(kneesL, AkneesL, view_matrix))
					continue;
				if (!wrld_to_screen(feetR, AfeetR, view_matrix))
					continue;
				if (!wrld_to_screen(feetL, AfeetL, view_matrix))
					continue;

				ImGui::GetForegroundDrawList()->AddCircle(ImVec2(Ahead.x, Ahead.y), 3, ImGui::ColorConvertFloat4ToU32(boneColor), 32, 1.0f);
				ImGui::GetForegroundDrawList()->AddLine(ImVec2(Acou.x, Acou.y), ImVec2(Ahead.x, Ahead.y), ImGui::ColorConvertFloat4ToU32(boneColor));
				ImGui::GetForegroundDrawList()->AddLine(ImVec2(Acou.x, Acou.y), ImVec2(AshoulderR.x, AshoulderR.y), ImGui::ColorConvertFloat4ToU32(boneColor));
				ImGui::GetForegroundDrawList()->AddLine(ImVec2(Acou.x, Acou.y), ImVec2(AshoulderL.x, AshoulderL.y), ImGui::ColorConvertFloat4ToU32(boneColor));
				ImGui::GetForegroundDrawList()->AddLine(ImVec2(AbrasL.x, AbrasL.y), ImVec2(AshoulderL.x, AshoulderL.y), ImGui::ColorConvertFloat4ToU32(boneColor));
				ImGui::GetForegroundDrawList()->AddLine(ImVec2(AbrasR.x, AbrasR.y), ImVec2(AshoulderR.x, AshoulderR.y), ImGui::ColorConvertFloat4ToU32(boneColor));
				ImGui::GetForegroundDrawList()->AddLine(ImVec2(AbrasR.x, AbrasR.y), ImVec2(AhandR.x, AhandR.y), ImGui::ColorConvertFloat4ToU32(boneColor));
				ImGui::GetForegroundDrawList()->AddLine(ImVec2(AbrasL.x, AbrasL.y), ImVec2(AhandL.x, AhandL.y), ImGui::ColorConvertFloat4ToU32(boneColor));
				ImGui::GetForegroundDrawList()->AddLine(ImVec2(Acou.x, Acou.y), ImVec2(Acock.x, Acock.y), ImGui::ColorConvertFloat4ToU32(boneColor));
				ImGui::GetForegroundDrawList()->AddLine(ImVec2(AkneesR.x, AkneesR.y), ImVec2(Acock.x, Acock.y), ImGui::ColorConvertFloat4ToU32(boneColor));
				ImGui::GetForegroundDrawList()->AddLine(ImVec2(AkneesL.x, AkneesL.y), ImVec2(Acock.x, Acock.y), ImGui::ColorConvertFloat4ToU32(boneColor));
				ImGui::GetForegroundDrawList()->AddLine(ImVec2(AkneesL.x, AkneesL.y), ImVec2(AfeetL.x, AfeetL.y), ImGui::ColorConvertFloat4ToU32(boneColor));
				ImGui::GetForegroundDrawList()->AddLine(ImVec2(AkneesR.x, AkneesR.y), ImVec2(AfeetR.x, AfeetR.y), ImGui::ColorConvertFloat4ToU32(boneColor));	
			}
			

			if (screenPos.z >= 0.01f) {
				// Condition to determine the color based on the team
				ImU32 borderColor = localTeam == playerTeam ? IM_COL32(75, 175, 75, 255) : IM_COL32(175, 75, 75, 255);

				// Convert screen coordinates to ImGui coordinates
				ImVec2 startPos(screenHead.x - width / 2, screenHead.y);
				ImVec2 endPos(startPos.x + width, startPos.y + height);

				// Calculate the dimensions and color based on playerHealth
				float boxX = screenHead.x - (width / 2 + 5);
				float boxY = screenHead.y + (height * (100 - health) / 100);
				float boxWidth = 2;
				float boxHeight = height - (height * (100 - health) / 100);

				ImU32 healthColor = IM_COL32(255 - health, 55 + health * 2, 75, 255);



				if (enableESP)
				{
					// Draw the border box using ImGui
					ImGui::GetForegroundDrawList()->AddRect(startPos, endPos, borderColor, 0.0f, 0, 1.0f);

					// Draw the healthca
					ImGui::GetForegroundDrawList()->AddRect(
						ImVec2(boxX, boxY),
						ImVec2(boxX + boxWidth, boxY + boxHeight),
						healthColor, 0.0f, 0, 1.0f
					);
				}

				if (showNames) {
					// Draw text above the player
					ImVec2 textPos(screenHead.x - width / 2, screenHead.y - 20.0f); // Adjust the Y-coordinate as needed
					ImU32 textColor = IM_COL32(255, 255, 255, 255); // Text color
					ImGui::GetForegroundDrawList()->AddText(textPos, textColor, playerName.c_str());
				}
				if (showTracers)
				{
					ImVec2 lineStart(ImGui::GetIO().DisplaySize.x / 2, ImGui::GetIO().DisplaySize.y); // Start of the line at the bottom of the screen
					ImVec2 lineEnd(endPos.x / 2, endPos.y); // End of the line at the bottom of the box

					ImGui::GetForegroundDrawList()->AddLine(lineStart, lineEnd, ImGui::ColorConvertFloat4ToU32(tracerColor), 1.0f); // Draw the line
				}

				



			}

		}

		if (enableAimbot)
		{
			Vector2 closest = FindClosestToCenter(headPositions, centerX, centerY);
			
			// Move the mouse cursor to the target point
			if (IsKeyPressed(VK_MENU)) {
				if (closest.x != -1.0f && closest.y != -1.0f) {
					ImGui::GetForegroundDrawList()->AddCircle(ImVec2(closest.x, closest.y), 3, IM_COL32(0, 0, 255, 255), 32, 1.0f);

					POINT cursorPos;
					GetCursorPos(&cursorPos);

					// Calculate the relative movement
					int deltaX = closest.x - cursorPos.x;
					int deltaY = closest.y - cursorPos.y;

					// Move the mouse cursor relatively
					mouse_event(MOUSEEVENTF_MOVE, deltaX / smoothValue, deltaY / smoothValue, 0, 0);
				}
			}
		}

		ImGui::Render();
		ImDrawData* drawData = ImGui::GetDrawData();

		constexpr float color[4]{ 0.f, 0.f, 0.f, 0.f };
		device_context->OMSetRenderTargets(1U, &render_target_view, nullptr);
		device_context->ClearRenderTargetView(render_target_view, color);

		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		swap_chain->Present(1U, 0U);

	}

	// more clean up
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();

	ImGui::DestroyContext();

	if (swap_chain)
	{
		swap_chain->Release();
	}

	if (device_context)
	{
		device_context->Release();
	}

	if (device)
	{
		device->Release();
	}

	if (render_target_view)
	{
		render_target_view->Release();
	}

	DestroyWindow(window);

	UnregisterClassW(wc.lpszClassName, wc.hInstance);


	return 0;
}

