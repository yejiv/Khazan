#pragma once

#include "../Default/framework.h"
#include <process.h>
namespace Editor
{
	const unsigned int			g_iWinSizeX = 1920;
	const unsigned int			g_iWinSizeY = 1080;

	const unsigned int			g_iWinSizeX_Imgui = 800;
	const unsigned int			g_iWinSizeY_Imgui = 600;

	enum class LEVEL { STATIC, EDITOR, END };
}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;
using namespace Editor;