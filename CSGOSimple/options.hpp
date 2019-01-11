#pragma once
#include <string>
#include "valve_sdk/Misc/Color.hpp"
#include "MainInclude.hpp"
#include<list>
#define OPTION(type, var, val) type var = val
bool Save(std::string file_name);
bool Load(std::string file_name);
void CreateConfig(std::string name);
void Delete(std::string name);
std::vector<std::string> GetConfigs();
enum Knives_t
{
	KNIFE_DEFAULT,
	KNIFE_BAYONET,
	KNIFE_FLIP,
	KNIFE_GUT,
	KNIFE_KARAMBIT,
	KNIFE_M9BAYONET,
	KNIFE_HUNTSMAN,
	KNIFE_FALCHION,
	KNIFE_BOWIE,
	KNIFE_BUTTERFLY,
	KNIFE_PUSHDAGGER
};
class Config
{
public:
	bool yourteamchams = false;
	bool theirchams = false;
	bool chams_player_regular = false;
	bool watermark = false;
	bool EndRound = false;
	bool EyePosESP = false;
	int knifemodel;
	float esp_player_chams_color_t_not_visible[4] = { 1.f, 1.f, 0.0f, 1.f };
	float esp_player_chams_color_ct_not_visible[4] = { 0.15f, 0.7f, 1.f, 1.0f };
	float esp_player_chams_color_t[4] = { 1.f, 0.f, 0.3f, 1.f };
	float esp_player_chams_color_mine[4] = { 1.f, 0.f, 0.3f, 1.f };
	float esp_player_chams_color_you[4] = { 0.f, 0.f, 0.9f, 1.f };
	float esp_player_chams_color_ct[4] = { 0.f, 0.2f, 1.f, 1.f };
	float esp_player_chams_color_t_visibleBT[4] = { 1.f, 1.f, 0.0f, 1.f };
	float esp_player_chams_color_ct_visibleBT[4] = { 0.15f, 0.7f, 1.f, 1.0f };
	float esp_player_chams_color_tBT[4] = { 1.f, 0.f, 0.3f, 1.f };
	float esp_player_chams_color_ctBT[4] = { 0.f, 0.2f, 1.f, 1.f };
	//////////////////////////////////////////////////////////////
	float color_chams_player_a_visible[4] = { 1.f, 1.f, 0.0f, 1.f };
	float color_chams_player_e_visible[4] = { 1.f, 0.f, 0.0f, 1.f };
	float color_chams_player_a_occluded[4] = { 0.f, 1.f, 0.0f, 1.f };
	float color_chams_player_e_occluded[4] = { 0.f, 0.f, 1.0f, 1.f };

	int iTab;
	OPTION(float, FakeLatencyMode, 0.1f, 1.f);
	int GlowType;
	int BoxType;
	bool gray_world = false;
	float fakelatency_amt;
	bool BacktrackExploitChk;
	int BTType;
	int GrenadeBox;
	int lag_type;
	bool Fakelat = false;
	bool FakelagOnground = false;
	int FakelagMode = 0;
	int FakelagAmount = 0.f;
	bool FakelagShoot = false;
	//
	//AIM
	//
	OPTION(int, esp_player_chams_type, 0);
	OPTION(bool, misc_backtrack, false);
 	OPTION(float, visuals_lagcomp_duration, 0.5, 5);
	OPTION(bool, lagcomp_hitboxes, false);
	OPTION(float, ChamsAlphaBT, 0.1,1);
	float ChamsAlpha = 0.9;
	OPTION(bool, LegitAA, false);
	OPTION(bool, esp_hitbox, false);
	float RecoilCrosshairSize = 2;
	
	float HitboxDuration;
	int backtrackticks;
	int TrigTypeCombo;
	int HitboxAlphaDuration;
	struct
	{
		bool Enable;
		int	 Key;
		bool Head;
		bool Arms;
		bool Chest;
		bool Stomach;
		bool Legs;

		//rifles
		bool Rifles_Enable;
		int Rifles_Key;
		bool Rifles_Head;
		bool Rifles_Arms;
		bool Rifles_Chest;
		bool Rifles_Stomach;
		bool Rifles_Legs;
		//pistols
		bool Pistols_Enable;
		int Pistols_Key;
		bool Pistols_Head;
		bool Pistols_Arms;
		bool Pistols_Chest;
		bool Pistols_Stomach;
		bool Pistols_Legs;
		//snipers
		bool Snipers_Enable;
		int Snipers_Key;
		bool Snipers_Head;
		bool Snipers_Arms;
		bool Snipers_Chest;
		bool Snipers_Stomach;
		bool Snipers_Legs;

	}Trigger;
	OPTION(bool, esp_no_smoke, false);
	OPTION(bool, legit_enabled, false);
	OPTION(int, aimkey, 0);
	OPTION(bool, legit_rcs, false);
	OPTION(int, LegitType,NULL);
	OPTION(bool, enable_legitbot, false);
	//pistols
	OPTION(int, hitbox_pistols, 0);
	OPTION(bool, aim_LegitBotPistols, false);
	OPTION(float, legit_fov_pistols, 1.f);
	OPTION(float, legit_smooth_pistols, 1.f);
	OPTION(float, legit_rcsmin_pistols, 0.f);
	OPTION(float, legit_rcsmax_pistols, 0.f);
	// rifles
	OPTION(int, hitbox_rifles, 0);
	OPTION(bool, aim_LegitBotRifles, false);
	OPTION(float, legit_smooth_rifles, 1.f);
	OPTION(float, legit_fov_rifles, 1.f);
	OPTION(float, legit_rcsmin_rifles, 0.f);
	OPTION(float, legit_rcsmax_rifles, 0.f);
	//snipers
	OPTION(int, hitbox_snipers, 0);
	OPTION(bool, aim_LegitBotSnipers, false);
	OPTION(float, legit_smooth_Snipers, 1.f);
	OPTION(float, legit_fov_Snipers, 1.f);
	OPTION(float, legit_rcsmin_Snipers, 0.f);
	OPTION(float, legit_rcsmax_Snipers, 0.f);
	OPTION(float, hitchance_amount, 0.f);
    // 
    // ESP
    // 
    OPTION(bool, esp_enabled, false);
    OPTION(bool, esp_enemies_only, false);
    OPTION(bool, esp_player_boxes, false);
    OPTION(bool, esp_player_names, false);
	//OPTION(bool, boneESP         , false);
    OPTION(bool, esp_player_health, false);
    OPTION(bool, esp_player_armour, false);
	OPTION(bool, esp_player_Skel, false);
    OPTION(bool, esp_player_weapons, false);
	OPTION(bool, KevESP, false);
	OPTION(bool, BombInfo, false);
	OPTION(bool, RecoilCrosshair, false);
	OPTION(bool, esp_player_Flash,false);
	OPTION(bool, GrenadeESP, false);
	OPTION(bool, HeadEsp, false); 
    OPTION(bool, esp_player_snaplines, false);
    OPTION(bool, esp_crosshair, false);
    OPTION(bool, esp_dropped_weapons, false);
    OPTION(bool, esp_defuse_kit, false);
    OPTION(bool, esp_planted_c4, false);


    // 
    // GLOW
    // 
    OPTION(bool, glow_enabled, false);
    OPTION(bool, glow_enemies_only, false);
    OPTION(bool, glow_players, false);
    OPTION(bool, glow_chickens, false);
    OPTION(bool, glow_c4_carrier, false);
    OPTION(bool, glow_planted_c4, false);
    OPTION(bool, glow_defuse_kits, false);
    OPTION(bool, glow_weapons, false);

    //
    // CHAMS
    //
	OPTION(bool, chams_wep_enabled, false);
	OPTION(bool, backtrackchams1tick, false); 
	OPTION(bool, chams_wep_wireframe, false);
	OPTION(bool, chams_wep_flat, false);
	OPTION(bool, chams_wep_ignorez, false);
	OPTION(bool, chams_wep_glass, false);
    OPTION(bool, chams_Metal_enabled, false);
	OPTION(bool, chams_player_enabled, false);
    OPTION(bool, chams_player_enemies_only, false);
    OPTION(bool, chams_player_wireframe, false);
    OPTION(bool, chams_player_flat, false);
    OPTION(bool, chams_player_ignorez, false);
    OPTION(bool, chams_player_glass, false);
    OPTION(bool, chams_arms_enabled, false);
    OPTION(bool, chams_arms_wireframe, false);
    OPTION(bool, chams_arms_flat, false);
    OPTION(bool, chams_arms_ignorez, false);
    OPTION(bool, chams_arms_glass, false);
	bool chams_player_regular_IgnoreZ = false;
	bool chams_player_flat_IgnoreZ = false;
	bool chams_player_wireframe_IgnoreZ = false;

    //
    // MISC
    //
    OPTION(bool, misc_bhop, false);
    OPTION(bool, misc_no_hands, false);
	OPTION(bool, misc_thirdperson, false);
	OPTION(bool, misc_KnifeLeft, false);
	OPTION(bool, SniperX, false);
	OPTION(bool, Radar, false);
	OPTION(bool, Noscope, false);
	OPTION(bool, GrenadeTra, false);
	OPTION(bool, backtrackchams, false); 
	OPTION(bool, misc_hitmarker, false);
	int Hitsound = 0;
	//OPTION(bool, misc_DamageInd, false);
	//OPTION(bool, misc_bulletshow, false);
	//OPTION(int, misc_BeamType, NULL); 
	//OPTION(bool, misc_BuyBot, false);
	//OPTION(bool, misc_EventLogs, false);
	OPTION(float, misc_thirdperson_dist, 50.f);
    OPTION(int, viewmodel_fov, 68);
    OPTION(float, mat_ambient_light_r, 0.0f);
    OPTION(float, mat_ambient_light_g, 0.0f);
    OPTION(float, mat_ambient_light_b, 0.0f);
	OPTION(bool, NightMode, false);
    // 
    // COLORS
    // 
	OPTION(int,SkyColor1,NULL);
	OPTION(int, SkyBoxChanger,0);
	OPTION(int, TypeOfESP, NULL);
	OPTION(bool, ColorSkybox, false);
	float g_fMColor[4] = { 0.21f, 0.21f, 0.21f, 1.0f }; //RGBA color
	float g_fBColor[4] = { 0.64f, 0.f, 0.35f, 1.0f };
	float g_fTColor[4] = { 1.f, 1.f, 1.f, 1.0f };
    OPTION(Color, color_esp_ally_visible, Color(0, 128, 255));
    OPTION(Color, color_esp_enemy_visible, Color(255, 0, 0));
    OPTION(Color, color_esp_ally_occluded, Color(0, 128, 255));
    OPTION(Color, color_esp_enemy_occluded, Color(255, 0, 0));
	OPTION(Color, color_esp_Box, Color(255, 0, 0));
    OPTION(Color, color_esp_crosshair, Color(255, 255, 255));
	OPTION(Color, color_esp_DamgeIndicator,Color(255, 0, 0));
    OPTION(Color, color_esp_weapons, Color(128, 0, 128));
    OPTION(Color, color_esp_defuse, Color(0, 128, 255));
    OPTION(Color, color_esp_c4, Color(255, 0, 0));
	OPTION(Color, color_esp_playerweapons, Color(255, 255, 0));
	OPTION(Color, color_esp_HeadESP, Color(255, 0, 0));
	OPTION(Color, color_esp_playername, Color(255, 0, 0));

    OPTION(Color, color_glow_ally, Color(0, 128, 255));
    OPTION(Color, color_glow_enemy, Color(255, 0, 0));
    OPTION(Color, color_glow_chickens, Color(0, 128, 0));
    OPTION(Color, color_glow_c4_carrier, Color(255, 255, 0));
    OPTION(Color, color_glow_planted_c4, Color(128, 0, 128));
    OPTION(Color, color_glow_defuse, Color(255, 255, 255));
    OPTION(Color, color_glow_weapons, Color(255, 128, 0));

    OPTION(Color, color_chams_player_ally_visible, Color(0, 128, 255));
    OPTION(Color, color_chams_player_ally_occluded, Color(0, 255, 128));
    OPTION(Color, color_chams_player_enemy_visible, Color(255, 0, 0));
    OPTION(Color, color_chams_player_enemy_occluded, Color(255, 128, 0));
    OPTION(Color, color_chams_arms_visible, Color(0, 128, 255));
	OPTION(Color, color_chams_T, Color(255, 22, 22));
	OPTION(Color, color_chams_CT, Color(22, 22, 255));

    OPTION(Color, color_chams_arms_occluded, Color(0, 128, 255));
	OPTION(Color, color_chams_wep_visible, Color(0, 128, 255));
	OPTION(Color, color_chams_wep_occluded, Color(0, 128, 255));
	OPTION(Color, color_Scoped_wep,         Color(0, 0, 0));
	OPTION(Color, color_hitboxCol,          Color(1.f, 0.f, 0.f));
	OPTION(Color, color_Skel,				Color(255,255,255));
	OPTION(Color, FlashCol,                 Color(255, 255, 255));
	OPTION(Color, GrenESPCol,               Color(255, 0, 0));
};


extern Config g_Options;
extern bool   g_Unload;
extern bool   g_Save;
extern bool   g_Load;
extern bool   g_Clear;
extern bool   g_View;
