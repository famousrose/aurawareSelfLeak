#include "Menu.hpp"
#define NOMINMAX
#include <Windows.h>
#include <chrono>

#include "valve_sdk/csgostructs.hpp"
#include "helpers/input.hpp"
#include "options.hpp"
#include "ui.hpp"
#include "droid.hpp"
#include "VFunc.h"
#include "options.hpp"
#include <functional>
#include <experimental/filesystem>
#include <algorithm>
#include <mftransform.h>
#include <cctype>
#include <Windows.h>
#include <chrono>
int selected_Weapon = 0;

const char* weaponNamesForCombobox[] = {
	"Desert Eagle",
	"Dual Berettas",
	"Five-SeveN",
	"Glock-18",
	"AK-47",
	"AUG",
	"AWP",
	"FAMAS",
	"G3SG1",
	"Galil AR",
	"M249",
	"M4A4",
	"MAC-10",
	"P90",
	"UMP-45",
	"XM1014",
	"PP-Bizon",
	"MAG-7",
	"Negev",
	"Sawed.Off",
	"Tec-9",
	"P2000",
	"MP7",
	"MP9",
	"Nova",
	"P250",
	"SCAR-20",
	"SG 553",
	"SSG 08",
	"M4A1-S",
	"USP-S",
	"CZ75-Auto" ,
	"R8 Revolver",
	"Knife"
};

const char* typeoflag[] = {
	" Factor",
	" Adaptive",
	" Break"
};

const char* ChamBTtyp[] = {
	" All Ticks",
	" Last Tick",
	" 3 Ticks"
};
const char* knifeNames[] =
{
	" Default",
	" Bayonet",
	" Flip",
	" Gut",
	" Karambit",
	" M9 Bayonet",
	" Huntsman",
	" Falchion",
	" Bowie",
	" Butterfly",
	"But Plugs"
};
#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui_internal.h"
#include "imgui/directx9/imgui_impl_dx9.h"
void RainbowMeme(int x, int y, int width, int height, float flSpeed)
{
	ImDrawList* window_draw_list = ImGui::GetWindowDrawList();

	static float flRainbow;

	flRainbow += flSpeed;

	if (flRainbow > 1.f) flRainbow = 0.f;

	for (int i = 0; i < width; i++)
	{
		float hue = (1.f / float(width)) * i;

		hue -= flRainbow;

		if (hue < 0.f) hue += 1.f;

		Color color = Color::FromHSB(hue, 1.f, 1.f);

		window_draw_list->AddRectFilled(ImVec2(x + i, y), ImVec2(width, height), ImColor(color.r(), color.g(), color.b(), color.a()));
	}
}
void DrawSpecialText(std::string text, std::string textBefore, bool sameLine = false, bool defaultColor = false)
{
	const auto style = &ImGui::GetStyle();
	if (defaultColor) ImGui::Text(text.c_str());
	else ImGui::TextColored(ImVec4(1.00f, 0.00f, 0.19f, 1.f), text.c_str());
	if (sameLine) ImGui::SameLine(style->WindowPadding.x + ImGui::CalcTextSize((textBefore + text).c_str()).x);
}
const char *opt_Chams[] = { " Textured", " Flat ", " WireFrame", " Textured XQZ","Flat XQZ ","WireFrame XQZ" };
static char* sidebar_tabs[] = {
    "ESP",
    "AIM",
    "MISC",
};
const char* TrigType[] = 
{
	" Rifle",
	" Pistol",
	" Sniper"
};
const char* Hitsounds[] =
{
	" Off",
	" Skeet",
	" Normal",
	" Bubble"
};
const char* Hitboxx[] =
{
	"",
	"   Head",
	"   Neck",
	"   Chest",
	"   Stomach",
	"   Closest"

};
static char* SkyColor[] = {
	" Default" ,
	" Purple" ,
	" Red" ,
	" Blue" ,
	" Green"
};
const char* Ltype[] = {

	" Aim",
	" Backtrack",
	" Trigger"

};
const char* skyboxchanger[] =
{
	"Off",
	"Ymaja",
	"Himalaya",
	"Mijtm",
	"Jungle",
	"New Jok",
	"Light",
	"Night",
	"Storm Light"
};
const char* TypeOfVis[] =
{
	" ESP" ,
	" Glow",
	" Chams"
};
const char* GlowTypeARR[] = {
	" Normal"," Skinny"," Pulsating"
};

const char* BoxTypeARR[] = {
	" Normal"," Corner"
};


static ConVar* cl_mouseenable = nullptr;


constexpr static float get_sidebar_item_width() { return 150.0f; }
constexpr static float get_sidebar_item_height() { return  50.0f; }
 

//std::vector<std::string> ConfigList;
//ypedef void(*LPSEARCHFUNC)(LPCTSTR lpszFileName);

namespace ImGuiEx
{
inline bool ColorEdit4(const char* label, Color* v, bool show_alpha = true)
{
    auto clr = ImVec4
	{
		    v->r() / 255.0f,
            v->g() / 255.0f,
            v->b() / 255.0f,
            v->a() / 255.0f
	};



        if(ImGui::ColorEdit4(label, &clr.x, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_AlphaBar)) {
            v->SetColor(clr.x, clr.y, clr.z, clr.w);
            return true;
        }
        return false;
    }
    inline bool ColorEdit3(const char* label, Color* v)
    {
        return ColorEdit4(label, v, false);
    }

}

template<size_t N>
void render_tabs(char* (&names)[N], int& activetab, float w, float h, bool sameline)
{
    bool values[N] = { false };

    values[activetab] = true;

    for(auto i = 0; i < N; ++i) {
        if(ImGui::ToggleButton(names[i], &values[i], ImVec2{ w, h })) {
            activetab = i;
        }
        if(sameline && i < N - 1)
            ImGui::SameLine();
    }
}

ImVec2 get_sidebar_size()
{
    constexpr float padding = 10.0f;
    constexpr auto size_w = padding * 0.5f + get_sidebar_item_width();
    constexpr auto size_h = padding * 0.5f + (sizeof(sidebar_tabs) / sizeof(char*)) * get_sidebar_item_height();

    return ImVec2{ size_w, ImMax(325.0f, size_h) };
}

int get_fps()
{
    using namespace std::chrono;
    static int count = 0;
    static auto last = high_resolution_clock::now();
    auto now = high_resolution_clock::now();
    static int fps = 0;

    count++;

    if(duration_cast<milliseconds>(now - last).count() > 1000) {
        fps = count;
        count = 0;
        last = now;
    }

    return fps;
}
static int weapon_index = 7;

void RenderCurrentWeaponButton()
{
	if (!g_EngineClient->IsConnected() || !g_LocalPlayer || !g_LocalPlayer->IsAlive())
	{
		return;
	}
	auto weapon = g_LocalPlayer->m_hActiveWeapon();
	if (!weapon || !weapon->IsWeapon()) {
		return;
	}
	if (ImGui::Button("Current")) {
		weapon_index = weapon->m_Item().m_iItemDefinitionIndex();
	}
}
ImGuiID Colorpicker_Close = 0;
__inline void CloseLeftoverPicker() { if (Colorpicker_Close) ImGui::ClosePopup(Colorpicker_Close); }
void ColorPickerBox(const char* picker_idname, float col_ct[], float col_t[], float col_ct_invis[], float col_t_invis[], bool alpha = true)
{
	ImGui::SameLine();
	bool switch_entity_teams = false;
	bool switch_color_vis = false;
	bool open_popup = ImGui::ColorButtonFloat(picker_idname, switch_entity_teams ? (switch_color_vis ? col_t : col_t_invis) : (switch_color_vis ? col_ct : col_ct_invis), ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_NoTooltip, ImVec2(13, 0));
	if (open_popup) {
		ImGui::OpenPopup(picker_idname);
		Colorpicker_Close = ImGui::GetID(picker_idname);
	}

	if (ImGui::BeginPopup(picker_idname))
	{
		const char* button_name0 = switch_entity_teams ? "Terrorists" : "Counter-Terrorists";
		if (ImGui::Button(button_name0, ImVec2(-1, 0)))
			switch_entity_teams = !switch_entity_teams;

		const char* button_name1 = switch_color_vis ? "Invisible" : "Visible";
		if (ImGui::Button(button_name1, ImVec2(-1, 0)))
			switch_color_vis = !switch_color_vis;

		std::string id_new = picker_idname;
		id_new += "##pickeritself_";

		ImGui::ColorPicker4(id_new.c_str(), switch_entity_teams ? (switch_color_vis ? col_t : col_t_invis) : (switch_color_vis ? col_ct : col_ct_invis), ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoSidePreview | ImGuiColorEditFlags_NoSmallPreview | ImGuiColorEditFlags_PickerHueBar | (alpha ? ImGuiColorEditFlags_AlphaPreview | ImGuiColorEditFlags_AlphaBar : 0));
		ImGui::EndPopup();
	}
}
bool ImGui::ColorButtonFloat(const char* desc_id, const float col[], ImGuiColorEditFlags flags, ImVec2 size)
{
	return ImGui::ColorButton(desc_id, ImColor(col[0], col[1], col[2], col[3]), flags, size);
}

void Menu::Initialize()
{
    _visible = true;

    cl_mouseenable = g_CVar->FindVar("cl_mouseenable");
	ImGui::CreateContext();

    ImGui_ImplDX9_Init(InputSys::Get().GetMainWindow(), g_D3DDevice9);


}

void Menu::Shutdown()
{
    ImGui_ImplDX9_Shutdown();
    cl_mouseenable->SetValue(true);
}

void Menu::OnDeviceLost()
{
    ImGui_ImplDX9_InvalidateDeviceObjects();
}

void Menu::OnDeviceReset()
{
    ImGui_ImplDX9_CreateDeviceObjects();
}

void Menu::Render()
{
	ImGuiStyle &style = ImGui::GetStyle();
	if (!_visible)
		return;
	auto bColor = g_Options.g_fBColor;
	auto mColor = g_Options.g_fMColor;
	auto tColor = g_Options.g_fTColor;
	//int(enemyColor[0] * 255.0f), int(enemyColor[1] * 255.0f), int(enemyColor[2] * 255.0f), 255

	ImColor mainColor = ImColor(int(255), int(54), int(54), 255);
	ImColor bodyColor = ImColor(int(25), int(25), int(24), 170);
	ImColor fontColor = ImColor(int(255), int(255), int(255), 255);

	ImVec4 mainColorHovered = ImVec4(mainColor.Value.x + 0.1f, mainColor.Value.y + 0.1f, mainColor.Value.z + 0.1f, mainColor.Value.w);
	ImVec4 mainColorActive = ImVec4(mainColor.Value.x + 0.2f, mainColor.Value.y + 0.2f, mainColor.Value.z + 0.2f, mainColor.Value.w);
	ImVec4 menubarColor = ImVec4(bodyColor.Value.x, bodyColor.Value.y, bodyColor.Value.z, bodyColor.Value.w - 0.8f);
	ImVec4 frameBgColor = ImVec4(bodyColor.Value.x, bodyColor.Value.y, bodyColor.Value.z, bodyColor.Value.w + .1f);
	ImVec4 tooltipBgColor = ImVec4(bodyColor.Value.x, bodyColor.Value.y, bodyColor.Value.z, bodyColor.Value.w + .05f);

	style.Alpha = 0.7f;
	style.WindowPadding = ImVec2(8, 8);
	style.WindowMinSize = ImVec2(32, 32);
	style.WindowRounding = 0.0f;
	style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
	//style.ChildWindowRounding = 0.0f;
	style.FramePadding = ImVec2(0, 0);
	style.FrameRounding = 2.0f;
	style.ItemSpacing = ImVec2(4, 2);
	style.ItemInnerSpacing = ImVec2(4, 4);
	style.TouchExtraPadding = ImVec2(0, 0);
	style.IndentSpacing = 21.0f;
	style.ColumnsMinSpacing = 3.0f;
	style.ScrollbarSize = 12.f;
	style.ScrollbarRounding = 0.0f;
	style.GrabMinSize = 1.0f;
	style.GrabRounding = 0.0f;
	style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
	style.DisplayWindowPadding = ImVec2(22, 22);
	style.DisplaySafeAreaPadding = ImVec2(4, 4);
	style.AntiAliasedLines = true;
	style.CurveTessellationTol = 1.25f;
	style.Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style.Colors[ImGuiCol_WindowBg] = bodyColor;
	style.Colors[ImGuiCol_ChildWindowBg] = ImVec4(.0f, .0f, .0f, .0f);
	style.Colors[ImGuiCol_PopupBg] = tooltipBgColor;
	style.Colors[ImGuiCol_Border] = mainColorActive;
	style.Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	style.Colors[ImGuiCol_FrameBg] = frameBgColor;
	style.Colors[ImGuiCol_FrameBgHovered] = mainColorHovered;
	style.Colors[ImGuiCol_FrameBgActive] = mainColorActive;
	style.Colors[ImGuiCol_TitleBg] = mainColor; 
	style.Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(1.00f, 0.98f, 0.95f, 0.75f);
	style.Colors[ImGuiCol_TitleBgActive] = mainColor;
	style.Colors[ImGuiCol_MenuBarBg] = menubarColor;
	style.Colors[ImGuiCol_ScrollbarBg] = ImVec4(frameBgColor.x + .05f, frameBgColor.y + .05f, frameBgColor.z + .05f, frameBgColor.w);
	style.Colors[ImGuiCol_ScrollbarGrab] = mainColor;
	style.Colors[ImGuiCol_ScrollbarGrabHovered] = mainColorHovered;
	style.Colors[ImGuiCol_ScrollbarGrabActive] = mainColorActive;
	style.Colors[ImGuiCol_ComboBg] = frameBgColor;
	style.Colors[ImGuiCol_CheckMark] = mainColor;
	style.Colors[ImGuiCol_SliderGrab] = mainColorHovered;
	style.Colors[ImGuiCol_SliderGrabActive] = mainColorActive;
	style.Colors[ImGuiCol_Button] = mainColorActive;
	style.Colors[ImGuiCol_ButtonHovered] = mainColorHovered;
	style.Colors[ImGuiCol_ButtonActive] = ImVec4(1.00f, 0.20f, 0.20f, 0.90f);
	style.Colors[ImGuiCol_Header] = mainColor;
	style.Colors[ImGuiCol_HeaderHovered] = mainColorHovered;
	style.Colors[ImGuiCol_HeaderActive] = mainColorActive;

	style.Colors[ImGuiCol_Column] = mainColor;
	style.Colors[ImGuiCol_ColumnHovered] = mainColorHovered;
	style.Colors[ImGuiCol_ColumnActive] = mainColorActive;

	style.Colors[ImGuiCol_ResizeGrip] = mainColor;
	style.Colors[ImGuiCol_ResizeGripHovered] = mainColorHovered;
	style.Colors[ImGuiCol_ResizeGripActive] = mainColorActive;
	style.Colors[ImGuiCol_CloseButton] = mainColor;
	style.Colors[ImGuiCol_CloseButtonHovered] = mainColorHovered;
	style.Colors[ImGuiCol_CloseButtonActive] = mainColorActive;
	style.Colors[ImGuiCol_PlotLines] = mainColor;
	style.Colors[ImGuiCol_PlotLinesHovered] = mainColorHovered;
	style.Colors[ImGuiCol_PlotHistogram] = mainColor;
	style.Colors[ImGuiCol_PlotHistogramHovered] = mainColorHovered;
	style.Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	style.Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);

	ImGui_ImplDX9_NewFrame();
	ImGui::GetIO().MouseDrawCursor = _visible;

	const auto sidebar_size = get_sidebar_size();
	static int active_sidebar_tab = 0;


	ImGui::GetIO().MouseDrawCursor = _visible;
	float flSpeed = 0.0010f;
	float group_w = ImGui::GetCurrentWindow()->Size.x - style.WindowPadding.x * 2;

	if (_visible)
	{
		

		ImGui::Begin("auraware", &_visible, ImVec2(550, 400), 0.5f, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_NoSavedSettings);
		{

			if (ImGui::Button("Legit", ImVec2(103, 50))) g_Options.iTab = 1;
			ImGui::SameLine();
			if (ImGui::Button("Rage", ImVec2(103, 50))) g_Options.iTab = 2;
			ImGui::SameLine();
			if (ImGui::Button("Visuals", ImVec2(103, 50))) g_Options.iTab = 3;
			ImGui::SameLine();
			if (ImGui::Button("Misc", ImVec2(103, 50))) g_Options.iTab = 4;
			ImGui::SameLine();
			if (ImGui::Button("Skins", ImVec2(103, 50))) g_Options.iTab = 5;


			ImGui::BeginGroupBox("mainchild",ImVec2(550, 400));
			{

				if (g_Options.iTab == 1) // legit
				{
					ImGui::Combo("##legittype", &g_Options.LegitType, Ltype, ARRAYSIZE(Ltype));
					switch (g_Options.LegitType)
					{
					case 0:
						ImGui::BeginChild("Legit_1", ImVec2(0, 0), true);
						{
							//ImGui::Text("Legitbot");
							//ImGui::Separator();
							//ImGui::Checkbox("Enable Aim", &g_Options.aim_LegitBotRifles);
							//ImGui::Hotkey("Key##0", &g_Options.aimkey, ImVec2(0, 0));
						//	ImGui::Combo("Hitbox", &g_Options.hitbox_rifles, Hitboxx, ARRAYSIZE(Hitboxx));
							//ImGui::SliderFloat("Smooth##0", &g_Options.legit_smooth_rifles, 1.00f, 100.00f, "%.2f");
							//ImGui::SliderFloat("FOV##0", &g_Options.legit_fov_rifles, 0.00f, 30.00f, "%.2f");
						//	ImGui::SliderFloat("Min RCS##0", &g_Options.legit_rcsmin_rifles, 1.00f, 100.00f, "%.2f");
							//ImGui::SliderFloat("Max RCS##0", &g_Options.legit_rcsmax_rifles, 1.00f, 100.00f, "%.2f");
						}
						ImGui::EndChild();
						break;
					case 1:
						ImGui::BeginChild("Backtracking", ImVec2(250, 300), true);
						{
							ImGui::Text("Backtracking");
							ImGui::Separator();
							ImGui::Checkbox("BackTracking", &g_Options.misc_backtrack); ImGui::SameLine(); ImGui::SliderFloat("Alpha", &g_Options.ChamsAlphaBT, 0.1f, 1.f, "%.1f Alpha");
							ImGui::SliderInt("Ticks", &g_Options.backtrackticks, 1, 12, "%.0f");
							ImGui::Checkbox("Backtrack Chams", &g_Options.backtrackchams); ImGui::SameLine(); ColorPickerBox("##Picker_chams_players", g_Options.esp_player_chams_color_ctBT, g_Options.esp_player_chams_color_tBT, g_Options.esp_player_chams_color_ctBT, g_Options.esp_player_chams_color_tBT, false);
							ImGui::Combo("Chams Type",&g_Options.BTType, ChamBTtyp, ARRAYSIZE(ChamBTtyp));
							//ImGui::Checkbox("Fake Lag ", &g_Options.Fakelat);
							//ImGui::Checkbox("idek", &g_Options.BacktrackExploitChk);
							//ImGui::SliderFloat("Ammount", &g_Options.fakelatency_amt, 0.0f, 1000.f,"%0.0f");
							//ImGui::SliderFloat("Duration", &g_Options.visuals_lagcomp_duration, 0, 12);
							ImGui::Checkbox("Lag Comp Hitboxes", &g_Options.esp_hitbox);
							ImGui::SliderFloat("##duration", &g_Options.HitboxDuration, 1.f, 10.0f, "Duration: %.2f /secs"); ImGui::SameLine(); ImGuiEx::ColorEdit3("##Hitbox", (&g_Options.color_hitboxCol));
							ImGui::SliderInt("##AlphaDuration", &g_Options.HitboxAlphaDuration, 0, 100, "Alpha %.2f");

						}
						ImGui::EndChild();
						break;
					case 2:
						ImGui::BeginChild("Trigger", ImVec2(300, 300), true);
						{
							//ImGui::Text("Misc");
							//ImGui::Separator();
							//ImGui::Combo("##Trigger Type", &g_Options.TrigTypeCombo, TrigType, ARRAYSIZE(TrigType));

							//switch (g_Options.TrigTypeCombo)
							//{
							///case 0:
							//	ImGui::BeginChild("Rifle");
							//	{
									//ImGui::Checkbox("Active", &g_Options.Trigger.Rifles_Enable);
									//ImGui::Hotkey("Key##TriggerKey", &g_Options.Trigger.Rifles_Key, ImVec2(0, 0));
									//ImGui::Selectable("Head", &g_Options.Trigger.Rifles_Head);
									//ImGui::Selectable("Arms", &g_Options.Trigger.Rifles_Arms);
									//ImGui::Selectable("Chest", &g_Options.Trigger.Rifles_Chest);
									//ImGui::Selectable("Stomach", &g_Options.Trigger.Rifles_Stomach);
									//ImGui::Selectable("Legs", &g_Options.Trigger.Rifles_Legs);

								//}
								//ImGui::EndChild();
								//break;
							//case 1:
								//ImGui::BeginChild("Pistol");
								//{
									//ImGui::Checkbox("Active", &g_Options.Trigger.Pistols_Enable);
									//ImGui::Hotkey("Key##TriggerKey", &g_Options.Trigger.Pistols_Key, ImVec2(0, 0));

									//ImGui::Selectable("Head", &g_Options.Trigger.Pistols_Head);
									//ImGui::Selectable("Arms", &g_Options.Trigger.Pistols_Arms);
									//ImGui::Selectable("Chest", &g_Options.Trigger.Pistols_Chest);
									//ImGui::Selectable("Stomach", &g_Options.Trigger.Pistols_Stomach);
									//ImGui::Selectable("Legs", &g_Options.Trigger.Pistols_Legs);

									//ImGui::Separator();
									//ImGui::Text("AA");
									//ImGui::Checkbox("Legit AA", &g_Options.LegitAA);
								//}
								//ImGui::EndChild();
							//	break;

							//case 2:
								//ImGui::BeginChild("Sniper");
								//{
									//ImGui::Checkbox("Active", &g_Options.Trigger.Snipers_Enable);
									//ImGui::Hotkey("Key##TriggerKey", &g_Options.Trigger.Snipers_Key, ImVec2(0, 0));
									//ImGui::Selectable("Head", &g_Options.Trigger.Snipers_Head);
									//ImGui::Selectable("Arms", &g_Options.Trigger.Snipers_Arms);
									//ImGui::Selectable("Chest", &g_Options.Trigger.Snipers_Chest);
									//ImGui::Selectable("Stomach", &g_Options.Trigger.Snipers_Stomach);
									//ImGui::Selectable("Legs", &g_Options.Trigger.Snipers_Legs);

								//}
								//ImGui::EndChild();
								
							//}
							break;

						}
						ImGui::EndChild();


					}

					
				
				}
				else if (g_Options.iTab == 2) // rage
				{
					ImGui::Text("Being Coded ");
				}
				else if (g_Options.iTab == 3) // visuals
				{
					ImGui::BeginChild("Visuals", ImVec2(500, 350), true );
					{
						ImGui::Combo("Type", &g_Options.TypeOfESP, TypeOfVis, ARRAYSIZE(TypeOfVis));

						switch (g_Options.TypeOfESP)
						{
						case 0:
							//ImGui::PushItemWidth(128);
							ImGui::BeginChild("ESP MAIN", ImVec2(220, 300), true);
							{
								ImGui::Text("ESP");
								ImGui::Separator();
								ImGui::Checkbox("Enabled", &g_Options.esp_enabled); //ImGui::SameLine(); ImGuiEx::ColorEdit3("Enemies Visible", &g_Options.color_esp_enemy_visible);
								ImGui::Checkbox("Team check", &g_Options.esp_enemies_only);
								ImGui::Checkbox("Boxes", &g_Options.esp_player_boxes); ImGui::SameLine(); ImGuiEx::ColorEdit3("##vis", &g_Options.color_esp_Box);
								ImGui::Combo("Box Type ", &g_Options.BoxType, BoxTypeARR, ARRAYSIZE(BoxTypeARR));
								ImGui::Checkbox("Eye Pos ESP", &g_Options.EyePosESP);
								ImGui::Checkbox("Health", &g_Options.esp_player_health);
								ImGui::Checkbox("Armour", &g_Options.esp_player_armour);
								ImGui::Checkbox("Skeleton", &g_Options.esp_player_Skel); ImGui::SameLine(); ImGuiEx::ColorEdit3("##Skeleton",&g_Options.color_Skel);
								ImGui::Checkbox("KevESP", &g_Options.KevESP);
								ImGui::Checkbox("Dropped Weapons", &g_Options.esp_dropped_weapons); ImGui::SameLine(); ImGuiEx::ColorEdit3("##Weapons",&g_Options.color_esp_weapons);
								ImGui::Checkbox("Weapon", &g_Options.esp_player_weapons); ImGui::SameLine(); ImGuiEx::ColorEdit3("##Weaponsplayer", &g_Options.color_esp_playerweapons);
								ImGui::Checkbox("Names", &g_Options.esp_player_names); ImGui::SameLine(); ImGuiEx::ColorEdit3("##playername", &g_Options.color_esp_playername);

							}
							
							ImGui::EndChild();

							ImGui::SameLine();

							ImGui::BeginChild("ESP Misc", ImVec2(220, 300), true);
							{
								ImGui::Text("Misc");
								ImGui::Separator();
								ImGui::Checkbox("Hitmarker", &g_Options.misc_hitmarker);
								ImGui::Checkbox("Bomb Info", &g_Options.BombInfo);
								ImGui::Checkbox("Recoil", &g_Options.RecoilCrosshair); ImGui::SameLine(); ImGuiEx::ColorEdit3("##Crosshairrecoil", &g_Options.color_esp_crosshair); 
								ImGui::InputFloat("Size",&g_Options.RecoilCrosshairSize,NULL,NULL,1);
								ImGui::Checkbox("Radar ESP", &g_Options.Radar);  
								ImGui::Checkbox("No Scope", &g_Options.Noscope);
								ImGui::Checkbox("Grenade Esp ", &g_Options.GrenadeESP);
								ImGui::Checkbox("Grenade Trad", &g_Options.GrenadeTra);
								ImGui::Checkbox("Flash ESP", &g_Options.esp_player_Flash); ImGui::SameLine(); ImGuiEx::ColorEdit3("##Flash ESP",&g_Options.FlashCol);
								ImGui::Checkbox("No Smoke ", &g_Options.esp_no_smoke);
								ImGui::Checkbox("Head ESP", &g_Options.HeadEsp); ImGui::SameLine(); ImGuiEx::ColorEdit3("##Head ESP",&g_Options.color_esp_HeadESP);
								ImGui::Checkbox("Snaplines", &g_Options.esp_player_snaplines);
								ImGui::Checkbox("Defuse Kit", &g_Options.esp_defuse_kit); ImGui::SameLine(); ImGuiEx::ColorEdit3("##Defuse Kit",&g_Options.color_esp_defuse);
								ImGui::Checkbox("Planted C4", &g_Options.esp_planted_c4); ImGui::SameLine(); ImGuiEx::ColorEdit3("##Planted C4",&g_Options.color_esp_c4);
								ImGui::Combo("HitSounds", &g_Options.Hitsound, Hitsounds, ARRAYSIZE(Hitsounds));

							}
							ImGui::EndChild();
							break;
							ImGui::SameLine();
						case 1:

							ImGui::BeginChild("Glow", ImVec2(220, 300), true);
							{
								ImGui::Text("Glow ESP");
								ImGui::Separator();
								ImGui::Checkbox("Enabled", &g_Options.glow_enabled); ImGui::SameLine();
								ImGui::Checkbox("Team check", &g_Options.glow_enemies_only);
								ImGui::Combo("Glow Type", &g_Options.GlowType, GlowTypeARR, ARRAYSIZE(GlowTypeARR));
								ImGui::Checkbox("Players", &g_Options.glow_players); ImGui::SameLine(); ImGuiEx::ColorEdit3("E", &g_Options.color_glow_enemy); ImGui::SameLine(); ImGuiEx::ColorEdit3("T", &g_Options.color_glow_ally);
								ImGui::Checkbox("Chickens", &g_Options.glow_chickens); ImGui::SameLine(); ImGuiEx::ColorEdit3("##Chickens",&g_Options.color_glow_chickens);
								ImGui::Checkbox("C4 Carrier", &g_Options.glow_c4_carrier); ImGui::SameLine(); ImGuiEx::ColorEdit3("##C4 Carrier",&g_Options.color_glow_c4_carrier);
								ImGui::Checkbox("Planted C4", &g_Options.glow_planted_c4); ImGui::SameLine(); ImGuiEx::ColorEdit3("##C4",&g_Options.color_glow_planted_c4);
								ImGui::Checkbox("Defuse Kits", &g_Options.glow_defuse_kits); ImGui::SameLine(); ImGuiEx::ColorEdit3("##Defuse Kit",&g_Options.color_glow_defuse);
								ImGui::Checkbox("Weapons", &g_Options.glow_weapons); ImGui::SameLine(); ImGuiEx::ColorEdit3("##Weapons",&g_Options.color_glow_weapons);
								
							}
							ImGui::EndChild();
							break;
							ImGui::SameLine();
						case 2:
							
							 
							
								ImGui::BeginChild("Chams",ImVec2(500,300));
								{
									ImGui::BeginChild("Chams", ImVec2(166, 200));
									{
										if (ImGui::CollapsingHeader("Chams"))
										{
											ImGui::Checkbox("Enabled", &g_Options.chams_player_enabled); ImGui::SameLine(); ImGui::Checkbox("E", &g_Options.theirchams); ImGui::SameLine(); ColorPickerBox("##E", g_Options.esp_player_chams_color_you, g_Options.esp_player_chams_color_you, g_Options.esp_player_chams_color_you, g_Options.esp_player_chams_color_you, false); ImGui::SameLine(); ImGui::Checkbox("F", &g_Options.yourteamchams); ImGui::SameLine(); ColorPickerBox("##T", g_Options.esp_player_chams_color_mine, g_Options.esp_player_chams_color_mine, g_Options.esp_player_chams_color_mine, g_Options.esp_player_chams_color_mine, false);
											ImGui::SliderFloat("Chams Alpha", &g_Options.ChamsAlpha, 0.1f, 1.f);
											//ImGui::Checkbox("Team Check", &g_Options.chams_player_enemies_only); ImGui::SameLine(); ImGui::SliderFloat("Chams Alpha", &g_Options.ChamsAlpha, 0.1f, 1.f);
											ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Reg", &g_Options.chams_player_regular);
											ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Flat", &g_Options.chams_player_flat);
											ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Wire", &g_Options.chams_player_wireframe);
											ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("RegZ", &g_Options.chams_player_regular_IgnoreZ);
											ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("FlatZ", &g_Options.chams_player_flat_IgnoreZ);
											ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("WireZ", &g_Options.chams_player_wireframe_IgnoreZ);


										}
									}
									ImGui::EndChild(); 
									ImGui::SameLine();
									ImGui::BeginChild("Chams2", ImVec2(166, 200));
									{
										if (ImGui::CollapsingHeader("Chams Arms"))
										{
											ImGui::Checkbox("Enabled Arms", &g_Options.chams_arms_enabled); ImGui::SameLine(); ImGuiEx::ColorEdit3("##Arms", &g_Options.color_chams_arms_visible);
											ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Wireframe Arms", &g_Options.chams_arms_wireframe); //ImGui::SameLine(); ImGuiEx::ColorEdit4("!Vis", &g_Options.color_chams_arms_occluded);
											ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Flat Arms ", &g_Options.chams_arms_flat);
											ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Ignore-Z Arms", &g_Options.chams_arms_ignorez);
											ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Glass Arms", &g_Options.chams_arms_glass);


										}
									}
									ImGui::EndChild();
									ImGui::SameLine();
									ImGui::BeginChild("Chams3", ImVec2(166, 200));
									{
										if (ImGui::CollapsingHeader("Chams Weapons"))
										{
											ImGui::Checkbox("Chams Weapon", &g_Options.chams_wep_enabled); ImGui::SameLine(); ImGuiEx::ColorEdit3("##Colour", &g_Options.color_chams_wep_visible);
											ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Wireframe Weapon", &g_Options.chams_wep_wireframe);
											ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Flat Weapon", &g_Options.chams_wep_flat);
											ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Ignore-Z Weapon ", &g_Options.chams_wep_ignorez);
											ImGui::Bullet(); ImGui::SameLine(); ImGui::Selectable("Glass Weapon", &g_Options.chams_wep_glass);


										}
									}
									ImGui::EndChild();
								}		//ImGui::Columns(1, nullptr, false);
								ImGui::EndChild();
								break;
						}
					}
					ImGui::EndChild();
				}
				else if (g_Options.iTab == 4) // misc tab
				{
					ImGui::BeginChild("Misc", ImVec2(250, 300), true);
					{	
						ImGui::Text("Misc");
						ImGui::Separator();
						ImGui::Checkbox("Bunny hop", &g_Options.misc_bhop);
						ImGui::Checkbox("Third Person", &g_Options.misc_thirdperson);
						ImGui::SliderFloat("FOV", &g_Options.misc_thirdperson_dist, 0.f, 150.f);
						ImGui::Checkbox("Knife Left", &g_Options.misc_KnifeLeft);
						ImGui::SliderInt("FOV ", &g_Options.viewmodel_fov, 68, 120);
						//ImGui::Checkbox("No hands", &g_Options.misc_no_hands);
						ImGui::Checkbox(("NightMode"), &g_Options.NightMode);
					}

					ImGui::EndChild();

					ImGui::SameLine();

					ImGui::BeginChild("empty", ImVec2(250, 300), true);
					{
						//ImGui::Text("idk lol");
					}
					ImGui::EndChild();
				}
				else if (g_Options.iTab == 5) // colors tab
				{
					ImGui::BeginChild("SkinChanger", ImVec2(250, 300), true);
					{
							ImGui::BeginChild("##Skins_part_1", ImVec2(250, 80), true);
							{
								ImGui::Text("Still in Progress");
								ImGui::Text("Knife Model Changer");
								ImGui::Separator();
								ImGui::Combo("Knifes", &g_Options.knifemodel, knifeNames, ARRAYSIZE(knifeNames));
							}
							ImGui::EndChild();
					}
					ImGui::EndChild();
				}

			}

			ImGui::EndGroupBox();
		}

	} ImGui::End();




    //ImGui::PopStyle();

    ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

void Menu::Show()
{
    _visible = true;
    cl_mouseenable->SetValue(false);
}

void Menu::Hide()
{
    _visible = false;
    cl_mouseenable->SetValue(true);
}

void Menu::Toggle()
{
	cl_mouseenable->SetValue(_visible);
    _visible = !_visible;
}
