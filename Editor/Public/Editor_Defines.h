#pragma once

#include "../Default/framework.h"
#include <process.h>
namespace Editor
{
	const unsigned int			g_iWinSizeX = 1920;
	const unsigned int			g_iWinSizeY = 1080;

	const unsigned int			g_iWinSizeX_Imgui = 800;
	const unsigned int			g_iWinSizeY_Imgui = 600;

	const unsigned int			g_iWinSizeX_Imgui_JJH = 1600;
	const unsigned int			g_iWinSizeY_Imgui_JJH = 900;

	const unsigned int			g_iWinSizeX_Imgui_JOH = 2200;
	const unsigned int			g_iWinSizeY_Imgui_JOH = 1200;

	enum class LEVEL { STATIC, LOADING, EDITOR, MAP, ANIMATION, EFFECT, UI, SHADER, END };
	enum class COLLISION_LAYER { MAP, PLAYER, MONSTER, ITEM, EFFECT, SKILL, END };
}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;
using namespace Editor;
