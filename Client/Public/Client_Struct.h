#pragma once
#include "Engine_Defines.h"
namespace Client
{
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


	typedef struct tagKhazanAnimationFindInfo
	{
		_uint iWeapon = { 0 };
		_uint iCategory = { 0 };
		_uint iSubType = { 0 };
		_uint iDirection = { 0 };
		_uint iCycle = { 0 };
		_uint iSet = { 0 };

		tagKhazanAnimationFindInfo() {}
		tagKhazanAnimationFindInfo(_uint weapon, _uint cat, _uint sub, _uint dir = 0, _uint cycle = 0, _uint set = 0)
			: iWeapon{ weapon }, iCycle{ cycle }, iSet{ set }, iCategory{ cat }, iSubType{ sub }, iDirection{ dir }
		{
		}
	}kHAZAN_ANIM_FIND;
}
