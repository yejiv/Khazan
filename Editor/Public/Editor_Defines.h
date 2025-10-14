#pragma once

#include "../Default/framework.h"
#include <process.h>
namespace Editor
{
	const unsigned int			g_iWinSizeX = 1920;
	const unsigned int			g_iWinSizeY = 1080;

	const unsigned int			g_iWinSizeX_Imgui = 1600;
	const unsigned int			g_iWinSizeY_Imgui = 900;

	enum class LEVEL { STATIC, LOADING, EDITOR, MAP, ANIMATION, EFFECT, UI, END };
	enum class COLLISION_LAYER { MAP, PLAYER, MONSTER, ITEM, EFFECT, SKILL, END };
}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;
using namespace Editor;