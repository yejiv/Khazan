#pragma once
#include "Engine_Defines.h"
namespace Client
{
	typedef struct tag_UI_Player_State_Data
	{
		_int	iLevel;
		_int	iUPPoint;
		_int	iUpLachryma;
		_int	iLachryma;
		
		_int	iMaxHp;
		_int	iMaxStamina;
		_int	iAtk;
		_int	iDef;
		_float	fWeight;
		_float	fMaxWeight;
		_float	fAgile;

		_float	fStaminaAttack;
		_float	fStaminaRegen;
		_float	fEvasion_StaminaDown;
		_float	fDamage_StaminaDown;
		_float	fGuard_StaminaDown;

		_int	iFire;
		_int	iWater;
		_int	iLightning;
		_int	iEarth;
		_int	iChaos;
		_int	iDisease;
		_int	iPoison;
	}UI_PLAYER_STATE_DATA;

	typedef struct tag_Item_DB
	{
		_uint			iID;
		_wstring		strName;
		_uint			iType;
		_uint			iEffect_ID;
		_uint			iLevel;
		_uint			iGrade;
		_bool			bisDrop;
		_bool			bisBox;
		_int			iGold;
		_int			iLachryma;
		_int			iMaxValue;
		_uint			iTexPass;
		_wstring		strIconName;
		_wstring		strText;
	}ITEM_DATA;

	typedef struct tag_EquipItem_DB
	{
		_uint			iID;
		_wstring		strName;
		_uint			iType;
		_uint			iValue_Type_1;
		_uint			iValue_1;
		_uint			iValue_Type_2;
		_uint			iValue_2;
		_uint			iValue_Type_3;
		_uint			iValue_3;
		_uint			iClother_Type;
		_float			fWeight;
	}EQUIPITEM_DATA;

	typedef struct tag_OtherItem_DB
	{
		_uint				iID;
		_wstring			strName;
		_uint				iState_Type_1;
		_uint				iValue_Type_1;
		_uint				iValue_1;
		_uint				iState_Type_2;
		_uint				iValue_Type_2;
		_uint				iValue_2;
		_uint				iState_Type_3;
		_uint				iValue_Type_3;
		_uint				iValue_3;

		_wstring			strText;
	}OTHERITEM_DATA;

	typedef struct tagBoxItems
	{
		_int iItem_0{};
		_int iItem_1{};
		_int iItem_2{};

	}BOX_ITEMS;
}
