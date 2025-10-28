#pragma once

#include "../Default/framework.h"
#include <process.h>
#include "Client_Struct.h"
#include "Event_Defines.h"

/* 클라이언트에서 사용할 수 있는 공통적인 정의를 모아놓은 파일 */
namespace Client
{
	const unsigned int			g_iWinSizeX = 1920;
	const unsigned int			g_iWinSizeY = 1080;

	const unsigned int			g_iWinSizeX_Imgui = 800;
	const unsigned int			g_iWinSizeY_Imgui = 600;

	enum class LEVEL { STATIC, LOADING, TITLE, STAGE1, END };
	enum class COLLISION_LAYER { MAP_STATIC, MAP_DYNAMIC, MAP_INTERACT, PLAYER, MONSTER, ITEM, EFFECT, SKILL, CAMERA, END };

	enum class KHAZAN_MAP { HEINMACH, YETUGA, THECREVICE, EMBARS, VIPER, END };

	enum class UI_RENDER_TYPE { DEFAULT, ATLAS, END };
	enum class UITYPE { PANEL, TAP, SLOT, BUTTON, SCROLLBAR, PROGRESSBAR, TEXTURE, TEXT, RENDER_GROUP, END };
	
	enum class CAMERATYPE { FREE, SPRING, END };

	enum class TEAM { GOBLIN, YETI, END };

	enum class DATATYPE { ITEM, EQUIPEFFECT, OTHEREFFECT, END };
}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;
using namespace Client;
