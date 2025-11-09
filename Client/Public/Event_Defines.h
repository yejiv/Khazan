#pragma once

namespace Client {
	// ENUMs
	enum class EVENT_TYPE { LEVEL_CHANGE, UI_QUICK_SLOT,
		OBJECT_INTERACT,
		INTERACT_TYPE,	
		ANNOUNCE_MAPNAME,
        ANNOUNCE_TALK,	
		LOCKON_VISIBLE,	
		SKILL_EVENT,		
        PreSKILL_On,
		END };

	// Structs
	struct EVENT_LEVEL_CHANGE {
		_int iLevel;
	};

	struct EventObject {
		enum class EVENT_STATE { NONE, ON, OFF };	
		EVENT_STATE eState{ EVENT_STATE::NONE };

		void On() { eState = EVENT_STATE::ON; }
		void Off() { eState = EVENT_STATE::OFF; }
		void None() { eState = EVENT_STATE::NONE; }

		_bool isOn() const { return eState == EVENT_STATE::ON; }
		_bool isOff() const { return eState == EVENT_STATE::OFF; }


		static EventObject OnEvent()
		{
			EventObject e = {};
			e.On();
			return e;
		}

		static EventObject OffEvent()
		{
			EventObject e = {};
			e.Off();
			return e;
		}

		static EventObject NoneEvent()
		{
			EventObject e = {};
			e.None();
			return e;
		}
	};


	struct EventBladeNexus {
		XMFLOAT3 vPosition{};
		bool isUnLock{ false };
		bool isBNOpened{ false };
	};

	
	struct EventChest {
		XMFLOAT3 vPosition{};
		XMFLOAT3 vPlayerPosition{};
		BOX_ITEMS Items{};
		bool isChestOpened{ false };
	};

	
	struct EventTombStone {
		XMFLOAT3 vPosition{};
		XMFLOAT3 vPlayerPosition{};
		bool isTSOpened{ false };
	};

	
	struct EventInteractType {
		enum EVENT_STATE { BEGIN, END, NONE };						
		EVENT_STATE eState = { EVENT_STATE::NONE };					
		_bool isEvent{ false };										
		INTERACTIVE_TYPE eInteractType{ INTERACTIVE_TYPE::END };	
		EventBladeNexus BNEvent{};									
		EventChest ChestEvent{};									
		EventTombStone TSEvent{};									

		void End_Event() { isEvent = false; }
	};


	struct EventBigChest {
		XMFLOAT3 vPosition{};
	};


	struct EVENT_HUD_QUICKSLOT {
		_bool isEquip;
		_int  iIndex;
		_int  iItemIndex;
		_int* iItemCount;
	};


	struct EVENT_ANNOUNCE_MAPNAME {
		_wstring	wstrName;				
		_int		iMapType;				
		_float		fTime;					
		_int		fFadeOutTime;			
		_bool		isDissovle;
	};

    //락온
	struct EVENT_LOCKON_VISIBLE {
		const _float4* pTargetPos;
		_bool isVisible;
	};
    
    //스킬 트리 오픈
	struct EVENT_SKILL_OPEN {};


    struct EVENT_SKILL_ON {
        _bool isUp;
        _int  SkillIndex;
    };



    struct EVENT_ANNOUNCE_TALK {
        _int  iTalkIndex;
    };
}