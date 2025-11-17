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

	typedef struct tag_State_DATA
	{
		_uint				iID;
		_uint				iType_1;
		_uint				iValue_1;
		_uint				iType_2;
		_uint				iValue_2;
		_uint				iType_3;
		_float				fValue_3;
		_uint				iType_4;
		_float				fValue_4;
	}STATE_DATA;

	typedef struct tagBoxItems
	{
		_int iItem_0{};
		_int iItem_1{};
		_int iItem_2{};

	}BOX_ITEMS;

	typedef struct tag_Announce_Talk
	{
		_float fTime{};
		wstring wstrName{};
		wstring wstrText_1{};
		wstring wstrText_2{};
	}ANNOUNCE_TALK_DB;

	typedef struct tag_Skill_DB
	{
		_int iType{};
		_int iSubID{};
		_int iSkillType{};
		_wstring wstrName{};
		_int iLevel{};
		_int iMaxPoint{};
		_int iIndex{};
		_int iSlotX{};
		_int iTexPass{};
		_wstring wstrIcon{};
		_int iPreSkill{};
        _wstring wstrInfo{};
        _int iGauge;
	}SKILL_DB;

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
	}kHAZAN_ANIM_INFO;

	typedef struct tag_Player_Data
	{
        _float  fMaxHp = {};                    //최대 체력
        _float  fCulHp = {};                    //현재 체력
        _float  fMaxStamina = {};               //최대 스태미나
        _float  fCulStamina = {};               //현재 스태미나
        _float  fStaminaRegen = {};             //스태미나 회복 속도
        _int    iMaxDoggednessCount = {};       //최대 투지 개수
        _float  fCulDoggedness = {};            //현재 투지

        _float  fDamage = {};                   //공격력
        _float  fGuard = {};                    //방어력

		_uint	iLevel = {};                    //레벨

		_uint	iLachryma = {};                 //라크리마
		_uint	iGold = {};                     //골드
        _uint	iStone = {};                    //스톤

		_uint	iSkillLevel = {};               //스킬 레벨
		_float	fSkillLevel_EXP = {};           //스킬 레벨 경험치
		_uint	iSkilPoint = {};                //스킬 포인트

        _uint   iVitality = {};                 //스탯1
        _uint   iEndurance = {};                //스탯2
        _uint   iPower = {};                    //스탯3
        _uint   iCompetency = {};               //스탯4
        _uint   iWill = {};                     //스탯5

        _float  fWeight = {};                   //무게
        
	}PLAYER_DATA;

}
