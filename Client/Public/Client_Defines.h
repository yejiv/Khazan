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

	enum class LEVEL { STATIC, LOADING, TITLE, HEINMACH, CREVICE, EMBARS, VIPER, END };
	enum class COLLISION_LAYER { MAP_STATIC, MAP_DYNAMIC, MAP_INTERACT, PLAYER, MONSTER, MONSTERATTACK ,ITEM, EFFECT, SKILL, CAMERA, HAIR, END };

	enum class KHAZAN_MAP { HEINMACH, CREVICE, EMBARS, VIPER, END };

	enum class UI_RENDER_TYPE { DEFAULT, ATLAS, WORLD, END };
	enum class UITYPE { PANEL, TAP, SLOT, BUTTON, SCROLLBAR, PROGRESSBAR, TEXTURE, TEXT, RENDER_GROUP, END };
	
	enum class CAMERATYPE { FREE, SPRING, END };

	enum class TEAM { GOBLIN, YETI, END };

	enum class DATATYPE { ITEM, EQUIPEFFECT, OTHEREFFECT, KHAZAN_SPEAR_ANIM, STATE, END };

	enum class PLAYTER_STATE { MAXHP, MAXSTAMINA, ATK, DEF, WEIGHT, AGILE, STAMINAATK, STAMINAREGEN, EVASION_STAMINADOWN, DAMAGE_STAMINADOWN, GUARD_STAMINADOWN,
		FIRE, WATER, LIGHTNING, EARTH, CHAOS,  DISEASE, POISON, END };

	enum class HITREACTION { NONE, GROGGY, KNOCKBACK_WEAK, KNOCKBACK_NORMAL, KNOCKBACK_STRONG, PARRY, GRAB, END };

}

extern HWND g_hWnd;
extern HINSTANCE g_hInst;
using namespace Client;
