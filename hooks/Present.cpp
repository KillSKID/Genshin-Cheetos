#ifndef _WIN64
#error Has to be Build in x64
#endif

#include <pch.h>
#include <base.h>
#include "../configs/Configs.h"
#include "../hooks/FontData.h"

extern Config cfg;

int AnimateAlpha = 0;
float h = 0;
bool ShowAnimation = false;

ImVec4 HSVtoRGB(float H, float S, float V) {
	if (H > 360 || H < 0 || S>100 || S < 0 || V>100 || V < 0) {
		H = 360;
	}
	float s = S / 100;
	float v = V / 100;
	float C = s * v;
	float X = C * (1.f - fabs(fmod(H / 60.0, 2.f) - 1.f));
	float m = v - C;
	float r, g, b;

	if (H >= 0.f && H < 60.f) {
		r = C, g = X, b = 0;
	}
	else if (H >= 60.f && H < 120.f) {
		r = X, g = C, b = 0;
	}
	else if (H >= 120.f && H < 180.f) {
		r = 0, g = C, b = X;
	}
	else if (H >= 180.f && H < 240.f) {
		r = 0, g = X, b = C;
	}
	else if (H >= 240.f && H < 300.f) {
		r = X, g = 0, b = C;
	}
	else {
		r = C, g = 0, b = X;
	}
	float R = (r + m);
	float G = (g + m);
	float B = (b + m);

	return ImVec4(R, G, B, 1.f);
}

HRESULT PRESENT_CALL Base::Hooks::Present(IDXGISwapChain* thisptr, UINT SyncInterval, UINT Flags)
{
	if (Data::ShowMenu)
	{
		ShowAnimation = true;
	}
	else {
		ShowAnimation = false;
	}

	if (!Data::InitImGui)
	{
		Data::pSwapChain = thisptr;
		if (SUCCEEDED(Data::pSwapChain->GetDevice(__uuidof(ID3D11Device), (void**)&Data::pDxDevice11)))
		{
			Data::pDxDevice11->GetImmediateContext(&Data::pContext);
			DXGI_SWAP_CHAIN_DESC sd;
			Data::pSwapChain->GetDesc(&sd);
			Data::hWindow = sd.OutputWindow;
			ID3D11Texture2D* pBackBuffer;
			Data::pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
			Data::pDxDevice11->CreateRenderTargetView(pBackBuffer, NULL, &Data::pMainRenderTargetView);
			pBackBuffer->Release();
			Data::oWndProc = (WNDPROC)SetWindowLongPtr(Data::hWindow, GWLP_WNDPROC, (LONG_PTR)WndProc);

			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO();

			io.Fonts->AddFontFromMemoryCompressedTTF(verdana_compressed_data, verdana_compressed_size, 14, NULL, io.Fonts->GetGlyphRangesCyrillic());

			io.ConfigFlags = ImGuiConfigFlags_NoMouseCursorChange;

			ImGui_ImplWin32_Init(Data::hWindow);
			ImGui_ImplDX11_Init(Data::pDxDevice11, Data::pContext);
			Data::InitImGui = true;
		}
	}

	if (!Data::InitImGui)
		return Data::oPresent(thisptr, SyncInterval, Flags);

	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	auto clear_color = ImVec4(0.1f, 0.1f, 0.1f, 1.00f);

	ImGuiStyle& style = ImGui::GetStyle();

	static ImColor mainColor = ImColor(0, 122, 204, AnimateAlpha);
	static float my_color[4] = { 0.f, 0.47843137254f, 0.8f, 1.f };

	auto colors = style.Colors;
	style.Alpha = AnimateAlpha / 255.f;
	colors[ImGuiCol_WindowBg] = ImColor(15, 15, 15, AnimateAlpha);
	colors[ImGuiCol_ChildBg] = ImColor(10, 10, 10, AnimateAlpha);
	colors[ImGuiCol_Border] = ImColor(30, 30, 30, AnimateAlpha);

	colors[ImGuiCol_FrameBg] = ImColor(23, 23, 23, AnimateAlpha);
	colors[ImGuiCol_FrameBgHovered] = ImColor(40, 40, 40);
	colors[ImGuiCol_FrameBgActive] = ImColor(20, 20, 20, AnimateAlpha);
	colors[ImGuiCol_Separator] = mainColor;
	colors[ImGuiCol_SliderGrab] = mainColor;
	colors[ImGuiCol_SliderGrabActive] = mainColor;
	colors[ImGuiCol_PopupBg] = ImColor(30, 30, 30, AnimateAlpha);
	colors[ImGuiCol_ScrollbarBg] = ImColor(0, 0, 0, 0);
	colors[ImGuiCol_ScrollbarGrab] = mainColor;
	colors[ImGuiCol_ScrollbarGrabActive] = mainColor;
	colors[ImGuiCol_ScrollbarGrabHovered] = ImColor(21, 21, 21, 30);

	colors[ImGuiCol_Header] = ImColor(40, 40, 40, AnimateAlpha);
	colors[ImGuiCol_HeaderHovered] = ImColor(21, 21, 21, 30);
	colors[ImGuiCol_HeaderActive] = mainColor;
	colors[ImGuiCol_Button] = ImColor(12, 12, 12, AnimateAlpha);
	colors[ImGuiCol_ButtonHovered] = ImColor(21, 21, 21, 30);
	colors[ImGuiCol_ButtonActive] = ImColor(21, 21, 21, 200);
	colors[ImGuiCol_Text] = ImColor(255, 255, 255, AnimateAlpha);
	colors[ImGuiCol_CheckMark] = mainColor;

	if (AnimateAlpha > 0)
	{
		ImGui::SetNextWindowSize(ImVec2(650, 470));
		ImGui::Begin("Genshit", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar); {
			ImGui::BeginGroup();
			ImGui::ColorBar("unicorn", ImVec2(650.f, 2.f));
			ImGui::EndGroup();

			static int tabs = 0;
			auto d = ImGui::GetWindowDrawList();
			auto p = ImGui::GetWindowPos();
			ImGui::SetWindowFontScale(1.25f);
			ImGui::BeginGroup();
			{
				if (ImGui::Button("Visuals", ImVec2(120, 85)))
					tabs = 0;
				ImGui::NextColumn();
				if (ImGui::Button("Movement", ImVec2(120, 85)))
					tabs = 1;
				ImGui::NextColumn();
				if (ImGui::Button("Combat", ImVec2(120, 85)))
					tabs = 2;
				ImGui::NextColumn();
				if (ImGui::Button("Configs", ImVec2(120, 85)))
					tabs = 3;
				ImGui::NextColumn();
				if (ImGui::Button("Info", ImVec2(120, 85)))
					tabs = 4;
			};
			ImGui::EndGroup();

			ImGui::SetWindowFontScale(1);
			ImGui::SameLine();
			{
				ImGui::BeginGroup();
				{
					static bool unlockFPS = false;
					static int fps = 60;
					static float rainbowSpeed = 2.f;
					static bool rainbow = false;
					static bool rapaidFire = false;
					static bool chestESP = false;
					static bool levelESP = false;
					static bool healthBar = false;

					static bool waterMark = false;
					static bool arrayList = false;
					static bool position = false;

					static bool noSkillsCoolDown = false;
					static bool noQCharge = false;
					static bool infStamina = false;

					static bool freezeEnemy = false;
					static bool freezeBowCharge = false;
					static bool doubleAttack = false;
					static bool flyHack = false;

					static bool locky = false;

					static float s = 100;
					static float b = 100;

					switch (tabs)
					{
					case 0:
						ImGui::BeginGroup();
						{
							ImGui::Spacing();
							ImGui::Columns(2, nullptr, false); {
								ImGui::BeginChild("Left", ImVec2(250.f, 440.f), true, ImGuiWindowFlags_NoResize);
								ImGui::Checkbox("UnlockFPS", &unlockFPS);
								ImGui::SliderInt("FPS", &fps, 30, 240, "%d");
								ImGui::Checkbox("Chest ESP", &chestESP);
								ImGui::Checkbox("Level ESP", &levelESP);
								ImGui::Checkbox("Health Bar", &healthBar);
								ImGui::EndChild();
							}
							ImGui::NextColumn(); {
								ImGui::BeginChild("Right", ImVec2(250.f, 440.f), true, ImGuiWindowFlags_NoResize);
								ImGui::ColorEdit4("GuiColor", my_color);
								ImGui::Checkbox("Rainbow", &rainbow);

								ImGui::SliderFloat("Rb Saturation", &s, 0.f, 100.f, "%f");
								ImGui::SliderFloat("Rb Brightness", &b, 0.f, 100.f, "%f");

								ImGui::SliderFloat("Rb Speed", &rainbowSpeed, 0.f, 10.f, "%f");
								ImGui::Checkbox("Watermark", &waterMark);
								ImGui::Checkbox("ArrayList", &arrayList);
								ImGui::Checkbox("Position", &position);
								ImGui::EndChild();
							}
						}
						ImGui::EndGroup();
						break;
					case 1:
						ImGui::BeginGroup();
						{
							ImGui::Spacing();
							ImGui::Columns(2, nullptr, false); {
								ImGui::BeginChild("Left", ImVec2(250.f, 440.f), true, ImGuiWindowFlags_NoResize);
								ImGui::Checkbox("Infinite Stamina", &infStamina);

								ImGui::Checkbox("Fly Hack", &flyHack);

								ImGui::EndChild();
							}
							ImGui::NextColumn(); {
								ImGui::BeginChild("Right", ImVec2(250.f, 440.f), true, ImGuiWindowFlags_NoResize);

								ImGui::Checkbox("Lock PosY", &locky);

								ImGui::EndChild();
							}
						}
						ImGui::EndGroup();
						break;
					case 2:
						ImGui::BeginGroup();
						{
							ImGui::Spacing();
							ImGui::Columns(2, nullptr, false); {
								ImGui::BeginChild("Left", ImVec2(250.f, 440.f), true, ImGuiWindowFlags_NoResize);
								ImGui::Checkbox("RapidFire", &rapaidFire);

								ImGui::Checkbox("Double Attacks", &doubleAttack);

								ImGui::Checkbox("Freeze Enemy", &freezeEnemy);

								ImGui::Checkbox("Freeze Bow Charge", &freezeBowCharge);

								ImGui::EndChild();
							}
							ImGui::NextColumn(); {
								ImGui::BeginChild("Right", ImVec2(250.f, 440.f), true, ImGuiWindowFlags_NoResize);

								ImGui::Checkbox("No Skills Cooldown", &noSkillsCoolDown);

								ImGui::Checkbox("No Q Charge", &noQCharge);

								ImGui::EndChild();
							}
							cfg.EnableRapid_Fire = rapaidFire;
						}
						ImGui::EndGroup();
						break;
					case 3:
						ImGui::BeginGroup();
						{
							ImGui::Spacing();
							ImGui::Columns(2, nullptr, false); {
								ImGui::BeginChild("Left", ImVec2(250.f, 440.f), true, ImGuiWindowFlags_NoResize);

								ImGui::EndChild();
							}
							ImGui::NextColumn(); {
								ImGui::BeginChild("Right", ImVec2(250.f, 440.f), true, ImGuiWindowFlags_NoResize);

								ImGui::EndChild();
							}
						}
						ImGui::EndGroup();
						break;

					case 4:
						ImGui::BeginGroup();
						{
							ImGui::Spacing();
							ImGui::Text("Make by killred");
							ImGui::NextColumn();
							ImGui::Text("[Information]");
							ImGui::NextColumn();
							ImGui::Text("- Simple Gui");
							ImGui::NextColumn();
							ImGui::Text("- I will add more features in the coming future");
						}
						ImGui::EndGroup();
						break;
					}
					ImGui::EndGroup();

					cfg.Unlock_FPS = fps;
					cfg.EnableUnlock_FPS = unlockFPS;
					cfg.EnableChest_ESP = chestESP;
					cfg.EnableLevel_ESP = levelESP;
					cfg.EnableHealth_Bar = healthBar;
					cfg.EnableWaterMark = waterMark;
					cfg.EnableInfStamina = infStamina;
					cfg.EnableNo_CoolDown = noSkillsCoolDown;
					cfg.EnableNo_QCharge = noQCharge;
					cfg.Enable_Freeze_Enemy = freezeEnemy;
					cfg.Enable_Freeze_BowCharge = freezeBowCharge;
					cfg.EnableDouble_Attack = doubleAttack;
					cfg.Enable_FlyHack = flyHack;
					cfg.Enable_LockY = locky;
					cfg.rainbowSpeed = rainbowSpeed;

					cfg.position = position;
					cfg.arrayList = arrayList;

					cfg.Saturation = s;
					cfg.Brightness = b;

					if (rainbow) {
						ImVec4 rainbow = HSVtoRGB(h, s, b);
						mainColor = ImColor(rainbow.x, rainbow.y, rainbow.z, AnimateAlpha != 255 ? AnimateAlpha : my_color[3]);
					}
					else {
						mainColor = ImColor(my_color[0], my_color[1], my_color[2], AnimateAlpha != 255 ? AnimateAlpha : my_color[3]);
					}
				}
			}

			ImGui::End();
		}
	}

	//RESET COLOR
	style.Alpha = 1.f;
	colors[ImGuiCol_WindowBg] = ImColor(15, 15, 15, 255);
	colors[ImGuiCol_ChildBg] = ImColor(10, 10, 10, 255);
	colors[ImGuiCol_Border] = ImColor(30, 30, 30, 255);
	colors[ImGuiCol_FrameBg] = ImColor(23, 23, 23, 255);
	colors[ImGuiCol_FrameBgActive] = ImColor(20, 20, 20, 255);
	colors[ImGuiCol_PopupBg] = ImColor(30, 30, 30, 255);
	colors[ImGuiCol_Header] = ImColor(40, 40, 40, 255);
	colors[ImGuiCol_Button] = ImColor(12, 12, 12, 255);
	colors[ImGuiCol_Text] = ImColor(255, 255, 255, 255);
	;

	if (cfg.EnableWaterMark) {
		ImGui::SetNextWindowSize(ImVec2(216, 50));
		ImGui::SetNextWindowBgAlpha(255.0f);
		ImGui::Begin("caoniam", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar);
		ImGui::BeginChild("ccc", ImVec2(200, 35), true, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoTitleBar);
		ImGui::ColorBar("unicorn", ImVec2(200.0f, 2.f));
		ImGui::Text(" cheetos");
		ImGui::SameLine();
		ImGui::TextColored(ImVec4(0, 1.f, 0.5f, 1.f), "sense");
		ImGui::SameLine();
		ImGui::Text(" | ");
		ImGui::SameLine();
		ImGui::Text("verycool");
		ImGui::EndChild();
		ImGui::End();
	}

	if (cfg.arrayList) {
		colors[ImGuiCol_WindowBg] = ImColor(15, 15, 15, AnimateAlpha);
		colors[ImGuiCol_Border] = ImColor(30, 30, 30, AnimateAlpha);
		colors[ImGuiCol_ResizeGrip] = ImColor(40, 40, 40, AnimateAlpha);
		colors[ImGuiCol_TitleBg] = ImColor(50, 50, 50, AnimateAlpha);
		colors[ImGuiCol_TitleBgActive] = mainColor;
		ImGui::SetNextWindowSizeConstraints(ImVec2(150, 100), ImVec2(INT32_MAX, INT32_MAX));
		ImGui::Begin("caoniam2", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		ImGui::SetWindowFontScale(1.25f);
		float startH = h;
		const char* arrays[11] = {};
		int index = 0;
		if (cfg.EnableNo_CoolDown) {
			arrays[index] = "NoSkillsCooldown";
			index++;
		}
		if (cfg.Enable_Freeze_BowCharge) {
			arrays[index] = "FreezeBowCharge";
			index++;
		}
		if (cfg.EnableDouble_Attack) {
			arrays[index] = "DoubleAttack";
			index++;
		}
		if (cfg.Enable_Freeze_Enemy) {
			arrays[index] = "FreezeEnemy";
			index++;
		}
		if (cfg.EnableInfStamina) {
			arrays[index] = "InfStamina";
			index++;
		}
		if (cfg.Enable_LockY) {
			arrays[index] = "NoFallDown";
			index++;
		}
		if (cfg.EnableNo_QCharge) {
			arrays[index] = "NoQCharge";
			index++;
		}
		if (cfg.EnableHealth_Bar) {
			arrays[index] = "HealthBar";
			index++;
		}
		if (cfg.EnableChest_ESP) {
			arrays[index] = "ChestESP";
			index++;
		}
		if (cfg.EnableLevel_ESP) {
			arrays[index] = "LevelESP";
			index++;
		}
		if (cfg.Enable_FlyHack) {
			arrays[index] = R"(FlyHack)";
			index++;
		}

		for (int i = 0; i < index; i++) {
			ImGui::TextColored(HSVtoRGB(startH, cfg.Saturation, cfg.Brightness), arrays[i]);
			if (h >= 360 || startH + 10 >= 360) {
				startH = 0;
			}
			else {
				startH += 10;
			}
		}
		ImGui::End();
	}

	if (cfg.position) {
		static char coords[32];
		snprintf(coords, 32, "X %.01f, Z %.01f\n", cfg.x, cfg.z);
		ImGui::SetNextWindowSizeConstraints(ImVec2(200, 40), ImVec2(INT32_MAX, INT32_MAX));
		ImGui::Begin("caoniam3", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
		ImGui::SetWindowFontScale(1.25f);
		ImGui::Text(coords);
		ImGui::End();
	}

	ImGui::EndFrame();
	ImGui::Render();

	Data::pContext->OMSetRenderTargets(1, &Data::pMainRenderTargetView, NULL);
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	return Data::oPresent(thisptr, SyncInterval, Flags);
}