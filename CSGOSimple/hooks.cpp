#include "hooks.hpp"
#include <intrin.h>  

#include "menu.hpp"
#include "options.hpp"
#include "helpers/input.hpp"
#include "helpers/utils.hpp"
#include "features/bhop.hpp"
#include "features/chams.hpp"
#include "features/visuals.hpp"
#include "features/glow.hpp"
#include "features/visuals.hpp"
#include "LagComp.h"
#include "options.hpp"
#include "valve_sdk\sdk.hpp"
#include "LagComp.h"
#include "features\GrenadeTra.hpp"
#include "features\NightMode.hpp"`
#include "features\Trig.hpp"
#include "features\LBOT.hpp"
#include "../CSGOSimple/features/AA.hpp"
#include "helpers\VMT.h"
#include <Psapi.h>
#pragma intrinsic(_ReturnAddress)  
ConVar* r_DrawSpecificStaticProp;
#define FLOW_OUTGOING	0		
#define FLOW_INCOMING	1
#define MAX_FLOWS		2	
extern unsigned long esp_font;
namespace Global
{
	char my_documents_folder[MAX_PATH];
	QAngle visualAngles;
	float hitmarkerAlpha;
	CUserCmd *userCMD;
	bool bSendPacket;
	int chokedticks;
}
namespace Hooks
{
	vfunc_hook hlclient_hook;
	vfunc_hook direct3d_hook;
	vfunc_hook vguipanel_hook;
	vfunc_hook vguisurf_hook;
	vfunc_hook clientmode_hook;
	vfunc_hook mdlrender_hook;
	vfunc_hook render_view;
	vfunc_hook sv_cheats;
	vfunc_hook gameevents_hook;
	RecvProp* sequence_hook;
	void Initialize()
	{
		hlclient_hook.setup(g_CHLClient, "client_panorama.dll");
		direct3d_hook.setup(g_D3DDevice9, "shaderapidx9.dll");
		vguipanel_hook.setup(g_VGuiPanel);
		vguisurf_hook.setup(g_VGuiSurface);
		gameevents_hook.setup(g_GameEvents);
		render_view.setup(g_RenderView);
		clientmode_hook.setup(g_ClientMode, "client_panorama.dll");
		ConVar* sv_cheats_con = g_CVar->FindVar("sv_cheats");
		sv_cheats.setup(sv_cheats_con);
		render_view.hook_index(index::SceneEnd, hkSceneEnd);
		direct3d_hook.hook_index(index::EndScene, hkEndScene);
		direct3d_hook.hook_index(index::Reset, hkReset);

		hlclient_hook.hook_index(index::FrameStageNotify, hkFrameStageNotify);
		hlclient_hook.hook_index(index::CreateMove, hkCreateMove_Proxy);

		vguipanel_hook.hook_index(index::PaintTraverse, hkPaintTraverse);



		vguisurf_hook.hook_index(index::PlaySound, hkPlaySound);
		vguisurf_hook.hook_index(67, hkLockCursor);
		mdlrender_hook.setup(g_MdlRender, "client_panorama.dll");
		mdlrender_hook.hook_index(index::DrawModelExecute, hkDrawModelExecute);
		clientmode_hook.hook_index(index::DoPostScreenSpaceEffects, hkDoPostScreenEffects);
		clientmode_hook.hook_index(index::OverrideView, hkOverrideView);
		sv_cheats.hook_index(index::SvCheatsGetBool, hkSvCheatsGetBool);
		Visuals::CreateFonts();
		std::cout << "Done" << std::endl;
	}
	//--------------------------------------------------------------------------------
	void Shutdown()
	{
		hlclient_hook.unhook_all();
		direct3d_hook.unhook_all();
		vguipanel_hook.unhook_all();
		vguisurf_hook.unhook_all();
		mdlrender_hook.unhook_all();
		clientmode_hook.unhook_all();
		gameevents_hook.unhook_all();
		render_view.unhook_all();

		Glow::Get().Shutdown();

		Visuals::DestroyFonts();
	}
	//--------------------------------------------------------------------------------
	static auto random_sequence(const int low, const int high) -> int
	{
		return rand() % (high - low + 1) + low;
	}

	//-------------------------------------------------

	long __stdcall hkEndScene(IDirect3DDevice9* device)
	{
		auto oEndScene = direct3d_hook.get_original<EndScene>(index::EndScene);

		static auto viewmodel_fov = g_CVar->FindVar("viewmodel_fov");
		static auto mat_ambient_light_r = g_CVar->FindVar("mat_ambient_light_r");
		static auto mat_ambient_light_g = g_CVar->FindVar("mat_ambient_light_g");
		static auto mat_ambient_light_b = g_CVar->FindVar("mat_ambient_light_b");
		static auto crosshair_cvar = g_CVar->FindVar("crosshair");
		static auto LeftKnife = g_CVar->FindVar("cl_righthand");

		viewmodel_fov->m_fnChangeCallbacks.m_Size = 0;
		viewmodel_fov->SetValue(g_Options.viewmodel_fov);
		mat_ambient_light_r->SetValue(g_Options.mat_ambient_light_r);
		mat_ambient_light_g->SetValue(g_Options.mat_ambient_light_g);
		mat_ambient_light_b->SetValue(g_Options.mat_ambient_light_b);
		DWORD dwOld_D3DRS_COLORWRITEENABLE;

		device->GetRenderState(D3DRS_COLORWRITEENABLE, &dwOld_D3DRS_COLORWRITEENABLE);
		device->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);

		Menu::Get().Render();

		device->SetRenderState(D3DRS_COLORWRITEENABLE, dwOld_D3DRS_COLORWRITEENABLE);

		return oEndScene(device);
	}


	//--------------------------------------------------------------------------------
	long __stdcall hkReset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* pPresentationParameters)
	{
		auto oReset = direct3d_hook.get_original<Reset>(index::Reset);
		Visuals::DestroyFonts();
		Menu::Get().OnDeviceLost();

		auto hr = oReset(device, pPresentationParameters);

		if (hr >= 0) {
			Menu::Get().OnDeviceReset();
			Visuals::CreateFonts();
		}

		return hr;
	}
	


	static bool hooked = false;
	void __stdcall hkCreateMove(int sequence_number , float input_sample_frametime , bool active , bool& bSendPacket)
	{
		static auto oCreateMove = hlclient_hook.get_original<CreateMove>(index::CreateMove);
		oCreateMove(g_CHLClient , sequence_number, input_sample_frametime, active);
		C_BaseCombatWeapon* weapon = g_LocalPlayer->m_hActiveWeapon();
		auto verified = g_Input->GetVerifiedCmd(sequence_number);
		auto pLocal = (C_BasePlayer*)g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer());
		auto cmd = g_Input->GetUserCmd2(sequence_number);

		if (!cmd || !cmd->command_number || !pLocal)
			return;
		

		TimeWarp::Get().StoreRecords(cmd, pLocal);
		TimeWarp::Get().DoBackTrack(cmd, pLocal);
		CCSGrenadeHint::Get().Tick(cmd->buttons);

		if (g_Options.misc_bhop)
			BunnyHop::OnCreateMove(cmd);

		verified->m_cmd = *cmd;
		verified->m_crc = cmd->GetChecksum();
		//knife left
		if (g_Options.misc_KnifeLeft)
			Visuals::Misc::KnifeLeft();
	}
	//--------------------------------------------------------------------------------


	__declspec(naked) void __stdcall hkCreateMove_Proxy(int sequence_number, float input_sample_frametime, bool active)
	{
		__asm
		{
			push ebp
			mov  ebp, esp
			push ebx
			lea  ecx, [esp]
			push ecx
			push dword ptr[active]
			push dword ptr[input_sample_frametime]
			push dword ptr[sequence_number]
			call Hooks::hkCreateMove
			pop  ebx
			pop  ebp
			retn 0Ch
		}

	}

	//--------------------------------------------------------------------------------
	void __stdcall hkPaintTraverse(unsigned int panel, bool forceRepaint, bool allowForce)
	{
		if (g_Options.esp_enabled && g_Options.Noscope && strcmp("HudZoom", g_VGuiPanel->GetName(panel)) == 0)
			return;
		static auto panelId = vgui::VPANEL{ 0 };
		static auto oPaintTraverse = vguipanel_hook.get_original<PaintTraverse>(index::PaintTraverse);
		oPaintTraverse(g_VGuiPanel, panel, forceRepaint, allowForce);

		if (!panelId) {
			const auto panelName = g_VGuiPanel->GetName(panel);
			if (!strcmp(panelName, "FocusOverlayPanel")) {
				panelId = panel;
			}
		}
		else if (panelId == panel)
		{
			if (g_EngineClient->IsInGame() && !g_EngineClient->IsTakingScreenshot()) {

				if (!g_LocalPlayer)
					return;

				if (g_Options.esp_enabled)
				{
					Visuals::Misc::Noscope();
					for (auto i = 1; i <= g_EntityList->GetHighestEntityIndex(); ++i)
					{
						auto entity = C_BasePlayer::GetPlayerByIndex(i);

						if (!entity)
							continue;

						if (entity == g_LocalPlayer)
							continue;

						if (i < 65 && !entity->IsDormant() && entity->IsAlive()) {
							// Begin will calculate player screen coordinate, bounding box, etc
							// If it returns false it means the player is not inside the screen
							// or is an ally (and team check is enabled)
							if (Visuals::Player::Begin(entity)) {
								if (g_Options.esp_player_snaplines) Visuals::Player::RenderSnapline();
								if (g_Options.esp_player_boxes)     Visuals::Player::RenderBox();
								if (g_Options.esp_player_weapons)   Visuals::Player::RenderWeapon();
								if (g_Options.esp_player_names)     Visuals::Player::RenderName();
								if (g_Options.esp_player_health)    Visuals::Player::RenderHealth();
								if (g_Options.esp_player_armour)    Visuals::Player::RenderArmour();
								if (g_Options.esp_player_Flash)     Visuals::Misc::Flash(entity);
								if (g_Options.HeadEsp)              Visuals::Player::HeadEsp(entity);
								if (g_Options.SniperX)				Visuals::Misc::SniperX(entity);
								if (g_Options.KevESP)				Visuals::Player::KevESP(entity);
								if (g_Options.esp_player_Skel)      Visuals::Misc::Skel(entity);
								if (g_Options.EyePosESP)            Visuals::Misc::EyePos(entity);

								//if (g_Options.boneESP)              Visuals::Player::BoneESP(entity);
							}
						}
						else if (g_Options.esp_dropped_weapons && entity->IsWeapon()) {
							Visuals::Misc::RenderWeapon((C_BaseCombatWeapon*)entity);
						}
						else if (g_Options.esp_defuse_kit && entity->IsDefuseKit()) {
							Visuals::Misc::RenderDefuseKit(entity);
						}
						else if (entity->IsPlantedC4())
						{
							if (g_Options.esp_planted_c4)
								Visuals::Misc::RenderPlantedC4((C_BaseEntity*)entity);

							if (g_Options.BombInfo)
								Visuals::Misc::RenderC4Info((C_PlantedC4*)entity);
						}
					}
					if (g_Options.RecoilCrosshair)
						Visuals::Misc::RecoilCrosshair();

					//if (g_Options.esp_crosshair)
						//Visuals::Misc::RenderCrosshair();



					if (g_Options.Radar)
						Visuals::Misc::Radar();

					HitMarkerEvent::Get().Paint();
					CCSGrenadeHint::Get().Paint();

					if (g_Options.Noscope)
						Visuals::Misc::Noscope();
				}

			}

		}
	}

	IDirect3DStateBlock9* pixel_state = NULL; IDirect3DVertexDeclaration9* vertDec; IDirect3DVertexShader9* vertShader;
	DWORD dwOld_D3DRS_COLORWRITEENABLE;

	void SaveState(IDirect3DDevice9 * pDevice)
	{
		pDevice->GetRenderState(D3DRS_COLORWRITEENABLE, &dwOld_D3DRS_COLORWRITEENABLE);
		//	pDevice->CreateStateBlock(D3DSBT_PIXELSTATE, &pixel_state); // This seam not to be needed anymore because valve fixed their shit
		pDevice->GetVertexDeclaration(&vertDec);
		pDevice->GetVertexShader(&vertShader);
		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0xffffffff);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, false);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_ADDRESSW, D3DTADDRESS_WRAP);
		pDevice->SetSamplerState(NULL, D3DSAMP_SRGBTEXTURE, NULL);
	}

	void RestoreState(IDirect3DDevice9 * pDevice) // not restoring everything. Because its not needed.
	{
		pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, dwOld_D3DRS_COLORWRITEENABLE);
		pDevice->SetRenderState(D3DRS_SRGBWRITEENABLE, true);
		//pixel_state->Apply(); 
		//pixel_state->Release();
		pDevice->SetVertexDeclaration(vertDec);
		pDevice->SetVertexShader(vertShader);
	}
	//--------------------------------------------------------------------------------

	void __stdcall hkPlaySound(const char* name)
	{
		static auto oPlaySound = vguisurf_hook.get_original<PlaySound>(index::PlaySound);

		oPlaySound(g_VGuiSurface, name);

		// Auto Accept
		if (strstr(name, "UI/competitive_accept_beep.wav")) {
			static auto fnAccept =
				(void(*)())Utils::PatternScan("client_panorama.dll", "55 8B EC 83 E4 F8 8B 4D 08 BA ? ? ? ? E8 ? ? ? ? 85 C0 75 12");

			fnAccept();

			//This will flash the CSGO window on the taskbar
			//so we know a game was found (you cant hear the beep sometimes cause it auto-accepts too fast)
			FLASHWINFO fi;
			fi.cbSize = sizeof(FLASHWINFO);
			fi.hwnd = InputSys::Get().GetMainWindow();
			fi.dwFlags = FLASHW_ALL | FLASHW_TIMERNOFG;
			fi.uCount = 0;
			fi.dwTimeout = 0;
			FlashWindowEx(&fi);
		}
	}
	
	//--------------------------------------------------------------------------------
	int __stdcall hkDoPostScreenEffects(int a1)
	{
		auto oDoPostScreenEffects = clientmode_hook.get_original<DoPostScreenEffects>(index::DoPostScreenSpaceEffects);

		if (g_LocalPlayer && g_Options.glow_enabled)
			Glow::Get().Run();

		return oDoPostScreenEffects(g_ClientMode, a1);
	}
	//--------------------------------------------------------------------------------\\

	void __fastcall hkSceneEnd(void * thisptr, void * edx)
	{

		if (!g_LocalPlayer)
			return hkSceneEnd(thisptr, edx);

		if (g_Options.chams_player_enabled)
		{
			constexpr float color_normal[4] = { 255, 255, 255, 255 };

			IMaterial * mat = nullptr;
			IMaterial * regular = g_MatSystem->FindMaterial("simple_regular", TEXTURE_GROUP_MODEL);
			IMaterial * regular_IgnoreZ = g_MatSystem->FindMaterial("chams_ignorez", TEXTURE_GROUP_MODEL);
			IMaterial * flat = g_MatSystem->FindMaterial("debug/debugdrawflat", TEXTURE_GROUP_MODEL);
			IMaterial * flat_IgnoreZ = g_MatSystem->FindMaterial("flat_ignorez", TEXTURE_GROUP_MODEL);
			IMaterial * wire = g_MatSystem->FindMaterial("chams_wire", TEXTURE_GROUP_MODEL);
			IMaterial * wire_IgnoreZ = g_MatSystem->FindMaterial("chams_wireignore", TEXTURE_GROUP_MODEL); 

			if (!regular || regular->IsErrorMaterial())
				return;
			//static  IMaterial* wire_frame = CreateMaterial(false, true, false);
			//static  IMaterial* ignoreZ = CreateMaterial(true, false, false);
			//static  IMaterial* ignoreZ = CreateMaterial(false, false,true);
			for (int i = 1; i < g_GlobalVars->maxClients; ++i) {
				auto ent = static_cast<C_BasePlayer*>(g_EntityList->GetClientEntity(i));
				if (ent && ent->IsAlive() && !ent->IsDormant()) {

					//const char *opt_Chams[] = { " Textured", " Flat ", " WireFrame", " Textured XQZ","Flat XQZ ","WireFrame XQZ" };
					const auto enemy = ent->m_iTeamNum() != g_LocalPlayer->m_iTeamNum();
					const auto front = enemy ? g_Options.esp_player_chams_color_t : g_Options.esp_player_chams_color_ct;
					const auto back = enemy ? g_Options.esp_player_chams_color_t_not_visible : g_Options.esp_player_chams_color_ct_not_visible;
					if (g_Options.theirchams && ent->m_iTeamNum() != g_LocalPlayer->m_iTeamNum())
					{
						if (g_Options.chams_player_regular)
						{
							regular->IncrementReferenceCount();

							g_RenderView->SetColorModulation(g_Options.esp_player_chams_color_you);
							g_RenderView->SetBlend(g_Options.ChamsAlpha);
							g_MdlRender->ForcedMaterialOverride(regular);
							ent->DrawModel(0x1, 255);
							g_MdlRender->ForcedMaterialOverride(nullptr);
						}
						if (g_Options.chams_player_regular_IgnoreZ)
						{
							regular_IgnoreZ->IncrementReferenceCount();
							//regular_IgnoreZ->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);

							g_RenderView->SetBlend(g_Options.ChamsAlpha);
							g_RenderView->SetColorModulation(g_Options.esp_player_chams_color_you);
							g_MdlRender->ForcedMaterialOverride(regular_IgnoreZ);
							ent->DrawModel(0x1, 255);
							g_MdlRender->ForcedMaterialOverride(nullptr);
						}
						if (g_Options.chams_player_flat)
						{
							flat->IncrementReferenceCount();

							g_RenderView->SetBlend(g_Options.ChamsAlpha);
							g_RenderView->SetColorModulation(g_Options.esp_player_chams_color_you);
							g_MdlRender->ForcedMaterialOverride(flat);
							ent->DrawModel(0x1, 255);
							g_MdlRender->ForcedMaterialOverride(nullptr);
						}
						if (g_Options.chams_player_flat_IgnoreZ)
						{
							flat_IgnoreZ->IncrementReferenceCount();

							g_RenderView->SetBlend(g_Options.ChamsAlpha);
							g_RenderView->SetColorModulation(g_Options.esp_player_chams_color_you);
							g_MdlRender->ForcedMaterialOverride(flat_IgnoreZ);
							ent->DrawModel(0x1, 255);
							g_MdlRender->ForcedMaterialOverride(nullptr);
						}
						if (g_Options.chams_player_wireframe)
						{
							wire->IncrementReferenceCount();
							wire->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, true);

							g_RenderView->SetBlend(g_Options.ChamsAlpha);
							g_RenderView->SetColorModulation(g_Options.esp_player_chams_color_you);
							g_MdlRender->ForcedMaterialOverride(wire);
							ent->DrawModel(0x1, 255);
							g_MdlRender->ForcedMaterialOverride(nullptr);
						}
						if (g_Options.chams_player_wireframe_IgnoreZ)
						{
							wire_IgnoreZ->IncrementReferenceCount();
							wire_IgnoreZ->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, true);
							wire_IgnoreZ->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
							g_RenderView->SetBlend(g_Options.ChamsAlpha);
							g_RenderView->SetColorModulation(g_Options.esp_player_chams_color_you);
							g_MdlRender->ForcedMaterialOverride(wire_IgnoreZ);
							ent->DrawModel(0x1, 255);
							g_MdlRender->ForcedMaterialOverride(nullptr);
						}
					}
					////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

					if (g_Options.yourteamchams && ent->m_iTeamNum() == g_LocalPlayer->m_iTeamNum())
					{
						if (g_Options.chams_player_regular)
						{
							regular->IncrementReferenceCount();

							g_RenderView->SetColorModulation(g_Options.esp_player_chams_color_mine);
							g_RenderView->SetBlend(g_Options.ChamsAlpha);
							g_MdlRender->ForcedMaterialOverride(regular);
							ent->DrawModel(0x1, 255);
							g_MdlRender->ForcedMaterialOverride(nullptr);
						}
						if (g_Options.chams_player_regular_IgnoreZ)
						{
							regular_IgnoreZ->IncrementReferenceCount();
							//regular_IgnoreZ->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);

							g_RenderView->SetBlend(g_Options.ChamsAlpha);
							g_RenderView->SetColorModulation(g_Options.esp_player_chams_color_mine);
							g_MdlRender->ForcedMaterialOverride(regular_IgnoreZ);
							ent->DrawModel(0x1, 255);
							g_MdlRender->ForcedMaterialOverride(nullptr);
						}
						if (g_Options.chams_player_flat)
						{
							flat->IncrementReferenceCount();

							g_RenderView->SetBlend(g_Options.ChamsAlpha);
							g_RenderView->SetColorModulation(g_Options.esp_player_chams_color_mine);
							g_MdlRender->ForcedMaterialOverride(flat);
							ent->DrawModel(0x1, 255);
							g_MdlRender->ForcedMaterialOverride(nullptr);
						}
						if (g_Options.chams_player_flat_IgnoreZ)
						{
							flat_IgnoreZ->IncrementReferenceCount();

							g_RenderView->SetBlend(g_Options.ChamsAlpha);
							g_RenderView->SetColorModulation(g_Options.esp_player_chams_color_mine);
							g_MdlRender->ForcedMaterialOverride(flat_IgnoreZ);
							ent->DrawModel(0x1, 255);
							g_MdlRender->ForcedMaterialOverride(nullptr);
						}
						if (g_Options.chams_player_wireframe)
						{
							wire->IncrementReferenceCount();
							wire->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, true);

							g_RenderView->SetBlend(g_Options.ChamsAlpha);
							g_RenderView->SetColorModulation(g_Options.esp_player_chams_color_mine);
							g_MdlRender->ForcedMaterialOverride(wire);
							ent->DrawModel(0x1, 255);
							g_MdlRender->ForcedMaterialOverride(nullptr);
						}
						if (g_Options.chams_player_wireframe_IgnoreZ)
						{
							wire_IgnoreZ->IncrementReferenceCount();
							wire_IgnoreZ->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, true);
							wire_IgnoreZ->SetMaterialVarFlag(MATERIAL_VAR_IGNOREZ, true);
							g_RenderView->SetBlend(g_Options.ChamsAlpha);
							g_RenderView->SetColorModulation(g_Options.esp_player_chams_color_mine);
							g_MdlRender->ForcedMaterialOverride(wire_IgnoreZ);
							ent->DrawModel(0x1, 255);
							g_MdlRender->ForcedMaterialOverride(nullptr);
						}




						
					}
					
				
					
				}
			}
		}
	if (g_Options.glow_enabled)
	{
			Glow::Get().Run();
	}
	}


	inline bool ApplyCustomModel(C_BaseAttributableItem* pWeapon, const char* vMdl)
	{
		auto pLocal = (C_BasePlayer*)g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer());

		// Get the view model of this weapon.
		C_BaseViewModel* pViewModel = pLocal->m_hViewModel().Get();

		if (!pViewModel)
			return false;

		// Get the weapon belonging to this view model.
		auto hViewModelWeapon = pViewModel->m_hWeapon();
		C_BaseAttributableItem* pViewModelWeapon = (C_BaseAttributableItem*)g_EntityList->GetClientEntityFromHandle(hViewModelWeapon);

		if (pViewModelWeapon != pWeapon)
			return false;

		// Check if an override exists for this view model.
		int nViewModelIndex = pViewModel->m_nModelIndex();

		// Set the replacement model.
		pViewModel->m_nModelIndex() = g_MdlInfo->GetModelIndex(vMdl);

		return true;
	}
	//--------------------------------------------------------------------------------
	void __stdcall hkFrameStageNotify(ClientFrameStage_t stage)
	{
		static auto ofunc = hlclient_hook.get_original<FrameStageNotify>(index::FrameStageNotify);
		C_BasePlayer* me = C_BasePlayer::GetPlayerByIndex(g_EngineClient->GetLocalPlayer());
		auto pLocal = (C_BasePlayer*)g_EntityList->GetClientEntity(g_EngineClient->GetLocalPlayer());
		if (g_EngineClient->IsInGame() && g_EngineClient->IsConnected() && stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START)
		{

			auto hWeapons = pLocal->m_hMyWeapons();
			// skin changer
			if (hWeapons)
			{
				static char* KnifeCT = "models/weapons/v_knife_ct.mdl";
				static char* KnifeT = "models/weapons/v_knife_t.mdl";
				static char* Bayonet = "models/weapons/v_knife_bayonet.mdl";
				static char* Butterfly = "models/weapons/v_knife_butterfly.mdl";
				static char* Flip = "models/weapons/v_knife_flip.mdl";
				static char* Gut = "models/weapons/v_knife_gut.mdl";
				static char* Karambit = "models/weapons/v_knife_karam.mdl";
				static char* M9Bayonet = "models/weapons/v_knife_m9_bay.mdl";
				static char* Huntsman = "models/weapons/v_knife_tactical.mdl";
				static char* Falchion = "models/weapons/v_knife_falchion_advanced.mdl";
				static char* Dagger = "models/weapons/v_knife_push.mdl";
				static char* Bowie = "models/weapons/v_knife_survival_bowie.mdl";

				// go through all weapons
				for (int i = 0; hWeapons[i]; i++)
				{
					// Get the weapon entity from the handle.
					auto pWeapon = (C_BaseAttributableItem*)g_EntityList->GetClientEntityFromHandle(hWeapons[i]);
					int pWeaponType = ((C_BaseCombatWeapon*)g_EntityList->GetClientEntityFromHandle(hWeapons[i]))->GetCSWeaponData()->WeaponType;

					if (!pWeapon)
						continue;

					// Knife changer if holding a knife
					if (pWeaponType == WEAPONTYPE_KNIFE)
					{
						// model indexes
						switch (g_Options.knifemodel)
						{
						case KNIFE_DEFAULT:
							if (pLocal->m_iTeamNum() == 0) pWeapon->m_nModelIndex() = g_MdlInfo->GetModelIndex(KnifeT);
							if (pLocal->m_iTeamNum() == 1) pWeapon->m_nModelIndex() = g_MdlInfo->GetModelIndex(KnifeCT);
							break;
						case KNIFE_BAYONET:
							pWeapon->m_nModelIndex() = g_MdlInfo->GetModelIndex(Bayonet);
							break;
						case KNIFE_FLIP:
							pWeapon->m_nModelIndex() = g_MdlInfo->GetModelIndex(Flip);
							break;
						case KNIFE_GUT:
							pWeapon->m_nModelIndex() = g_MdlInfo->GetModelIndex(Gut);
							break;
						case KNIFE_KARAMBIT:
							pWeapon->m_nModelIndex() = g_MdlInfo->GetModelIndex(Karambit);
							break;
						case KNIFE_M9BAYONET:
							pWeapon->m_nModelIndex() = g_MdlInfo->GetModelIndex(M9Bayonet);
							break;
						case KNIFE_HUNTSMAN:
							pWeapon->m_nModelIndex() = g_MdlInfo->GetModelIndex(Huntsman);
							break;
						case KNIFE_FALCHION:
							pWeapon->m_nModelIndex() = g_MdlInfo->GetModelIndex(Falchion);
							break;
						case KNIFE_BOWIE:
							pWeapon->m_nModelIndex() = g_MdlInfo->GetModelIndex(Bowie);
							break;
						case KNIFE_BUTTERFLY:
							pWeapon->m_nModelIndex() = g_MdlInfo->GetModelIndex(Butterfly);
							break;
						case KNIFE_PUSHDAGGER:
							pWeapon->m_nModelIndex() = g_MdlInfo->GetModelIndex(Dagger);
							break;
						}

						// changine knife model
						switch (g_Options.knifemodel)
						{
						case KNIFE_DEFAULT:
							if (pLocal->m_iTeamNum() == 0) ApplyCustomModel(pWeapon, KnifeT);
							if (pLocal->m_iTeamNum() == 1) ApplyCustomModel(pWeapon, KnifeCT);
							break;
						case KNIFE_BAYONET:
							ApplyCustomModel(pWeapon, Bayonet);
							break;
						case KNIFE_FLIP:
							ApplyCustomModel(pWeapon, Flip);
							break;
						case KNIFE_GUT:
							ApplyCustomModel(pWeapon, Gut);
							break;
						case KNIFE_KARAMBIT:
							ApplyCustomModel(pWeapon, Karambit);
							break;
						case KNIFE_M9BAYONET:
							ApplyCustomModel(pWeapon, M9Bayonet);
							break;
						case KNIFE_HUNTSMAN:
							ApplyCustomModel(pWeapon, Huntsman);
							break;
						case KNIFE_FALCHION:
							ApplyCustomModel(pWeapon, Falchion);
							break;
						case KNIFE_BOWIE:
							ApplyCustomModel(pWeapon, Bowie);
							break;
						case KNIFE_BUTTERFLY:
							ApplyCustomModel(pWeapon, Butterfly);
							break;
						case KNIFE_PUSHDAGGER:
							ApplyCustomModel(pWeapon, Dagger);
							break;
						}

					}
				}
			}



		}
		ofunc(g_CHLClient, stage);

	}
	//--------------------------------------------------------------------------------
	void __stdcall hkOverrideView(CViewSetup* vsView)
	{
		static auto ofunc = clientmode_hook.get_original<OverrideView>(index::OverrideView);

		if (g_EngineClient->IsInGame() && vsView) {
				//grenade_prediction::Get().View(vsView);
			Visuals::Misc::ThirdPerson();
			if (g_EngineClient->IsInGame() && vsView) {
				CCSGrenadeHint::Get().View();
			}

		} 
		ofunc(g_ClientMode, vsView);

	}
	//--------------------------------------------------------------------------------
	void __stdcall hkDrawModelExecute(IMatRenderContext* ctx, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* pCustomBoneToWorld)
	{
		static auto ofunc = mdlrender_hook.get_original<DrawModelExecute>(index::DrawModelExecute);
		//chams are here
		Chams::Get().OnDrawModelExecute(ctx, state, info, pCustomBoneToWorld);;
		ofunc(g_MdlRender, ctx, state, info, pCustomBoneToWorld);

		if (ofunc && Chams::Get().Do_BTChams(g_MdlRender, ctx, state, info))
			g_MdlRender->ForcedMaterialOverride(nullptr, 0);

	}

	auto dwCAM_Think = Utils::PatternScan(GetModuleHandleW(L"client_panorama.dll"), "85 C0 75 30 38 86");
	typedef bool(__thiscall *svc_get_bool_t)(PVOID);
	bool __fastcall hkSvCheatsGetBool(PVOID pConVar, void* edx)
	{
		static auto ofunc = sv_cheats.get_original<svc_get_bool_t>(13);
		if (!ofunc)
			return false;

		if (reinterpret_cast<DWORD>(_ReturnAddress()) == reinterpret_cast<DWORD>(dwCAM_Think))
			return true;
		return ofunc(pConVar);
	}

	void __fastcall hkLockCursor(ISurface* thisptr, void* edx)
	{
		static auto oLockCursor = Hooks::vguisurf_hook.get_original<LockCursor_t>(67);

		if (!Menu::Get().IsVisible())
			return oLockCursor(thisptr, edx);

		g_VGuiSurface->UnlockCursor();
	}
}