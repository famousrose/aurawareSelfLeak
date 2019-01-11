#include "AA.hpp"
/*
void AA::LAA(CUserCmd *cmd, bool& bSendPacket)
{

	if (!cmd || !g_LocalPlayer || !g_LocalPlayer->IsAlive()  || (cmd->buttons & IN_USE) || (cmd->buttons & IN_ATTACK) || (cmd->buttons & IN_ATTACK2) || g_LocalPlayer->m_nMoveType()  == MOVETYPE_LADDER || (g_LocalPlayer->m_hActiveWeapon()->IsSniper()))
		return;
	if ((cmd->buttons & IN_USE))
		return;

	QAngle oldAngle = cmd->viewangles;
	float oldForward = cmd->forwardmove;
	float oldSideMove = cmd->sidemove;
	static bool Direction = false;
	static int ChokedTicks = 0;
	if (GetAsyncKeyState(VK_LEFT)) Direction = true;
	if (GetAsyncKeyState(VK_RIGHT)) Direction = false;

	if (g_Options.LegitAA && !cmd->buttons & IN_ATTACK)
	{
		if (ChokedTicks < 1) {
			bSendPacket = false;
			cmd->viewangles.yaw += Direction ? 90 : -90;

			ChokedTicks++;
		}
		else {
			bSendPacket = true;
			ChokedTicks = 0;
		}
	}
}
//-----------------------------------------------------------------------------------\\

void AA::LAAFIX(CUserCmd* cmd, bool& bSendPacket) {
	if (g_Options.LegitAA)
	return;

	static bool real = false;
	real = !real;
	QAngle angle = cmd->viewangles;
	if (real) angle -= 90.0f;
	bSendPacket = !real;
	cmd->viewangles = angle;
	Math::NormalizeAngles(cmd->viewangles);
}
*/
