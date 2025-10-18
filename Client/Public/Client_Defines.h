#pragma once

#include "../Default/framework.h"
#include <process.h>

/* 클라이언트에서 사용할 수 있는 공통적인 정의를 모아놓은 파일 */
namespace Client
{
	const unsigned int			g_iWinSizeX = 1600;
	const unsigned int			g_iWinSizeY = 900;

	const unsigned int			g_iWinSizeX_Imgui = 800;
	const unsigned int			g_iWinSizeY_Imgui = 600;

	enum class LEVEL { STATIC, LOADING, TITLE, STAGE1, END };
	enum class COLLISION_LAYER { MAP, PLAYER, MONSTER, ITEM, EFFECT, SKILL, CAMERA, END };

	enum class UI_RENDER_TYPE { DEFAULT, ATLAS, END };
	enum class UITYPE { PANEL, TAP, BUTTON, SLOT, SCROLLBAR, PROGRESSBAR, END };
	
	enum class CAMERATYPE { FREE, SPRING, END };
}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;
using namespace Client;
