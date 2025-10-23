#pragma once

#include "../Default/framework.h"
#include <process.h>
namespace Editor
{
	const unsigned int			g_iWinSizeX = 1920;
	const unsigned int			g_iWinSizeY = 1080;

	const unsigned int			g_iWinSizeX_Imgui = 1600;
	const unsigned int			g_iWinSizeY_Imgui = 900;

	const unsigned int			g_iWinSizeX_Imgui_JJH = 1920;
	const unsigned int			g_iWinSizeY_Imgui_JJH = 1080;

	const unsigned int			g_iWinSizeX_Imgui_JOH = 2200;
	const unsigned int			g_iWinSizeY_Imgui_JOH = 1200;

	const unsigned int			g_iWinSizeX_Imgui_TYJ = 1280;
	const unsigned int			g_iWinSizeY_Imgui_TYJ = 720;

	const unsigned int			g_iWinSizeX_Imgui_JYP = 1600;
	const unsigned int			g_iWinSizeY_Imgui_JYP = 900;

		const unsigned int		g_iWinSizeX_Imgui_KBS = 1600;
	const unsigned int			g_iWinSizeY_Imgui_KBS = 900;


	enum class LEVEL { STATIC, LOADING, EDITOR, MAP, ANIMATION, EFFECT, UI, SHADER, CAMERA, AI, END };

	enum class COLLISION_LAYER { MAP, PLAYER, MONSTER, ITEM, EFFECT, SKILL, END };

	enum class KHAZAN_MAP { HEINMACH, YETUGA, THECREVICE, EMBARS, VIPER, END };

	enum class CAMERATYPE { FREE, SPRING, END };
	enum class CAMERA_EVENTTYPE { A, B, C, END};
}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;
using namespace Editor;
