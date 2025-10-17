#pragma once

#include "../Default/framework.h"
#include <process.h>
namespace Editor
{
	const unsigned int			g_iWinSizeX = 1920;
	const unsigned int			g_iWinSizeY = 1080;

	const unsigned int			g_iWinSizeX_Imgui = 1920;
	const unsigned int			g_iWinSizeY_Imgui = 1080;

	const unsigned int			g_iWinSizeX_Imgui_JJH = 1920;
	const unsigned int			g_iWinSizeY_Imgui_JJH = 1080;

	const unsigned int			g_iWinSizeX_Imgui_JOH = 1000;
	const unsigned int			g_iWinSizeY_Imgui_JOH = 1600;

	const unsigned int			g_iWinSizeX_Imgui_TYJ = 1280;
	const unsigned int			g_iWinSizeY_Imgui_TYJ = 720;

	enum class LEVEL { STATIC, LOADING, EDITOR, MAP, ANIMATION, EFFECT, UI, SHADER, END };
	enum class COLLISION_LAYER { MAP, PLAYER, MONSTER, ITEM, EFFECT, SKILL, END };
}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;
using namespace Editor;
