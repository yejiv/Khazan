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
        ANNOUNCE_RESULT,
        ANNOUNCE_OVER,
        SKILL_QUICKSLOT,
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
		XMFLOAT4 vPosition{};
		bool isUnLock{ false };
		bool isBNOpened{ false };
	};

	
	struct EventChest {
		XMFLOAT4 vPosition{};
		XMFLOAT4 vPlayerPosition{};
		BOX_ITEMS Items{};
		bool isChestOpened{ false };
	};

	
	struct EventTombStone {
		XMFLOAT4 vPosition{};
		XMFLOAT4 vPlayerPosition{};
		bool isTSOpened{ false };
	};

    struct EventCave
    {
        bool isInCave = { false };
    };

    struct EventLever
    {
        XMFLOAT4 vPosition{};
        XMFLOAT4 vPlayerPosition{};
    };

    //상호작용 오브젝트 어떤 종류인지 받아오는 이벤트 구조체(오브젝트->플레이어)
	struct EventInteractType {
		enum EVENT_STATE { BEGIN, END, NONE };

        EVENT_STATE eState = { EVENT_STATE::NONE };
        _bool isEvent{ false };
		INTERACTIVE_TYPE eInteractType{ INTERACTIVE_TYPE::END };
		EventBladeNexus BNEvent{};	
		EventChest ChestEvent{};
		EventTombStone TSEvent{};
        EventCave CaveEvent{};
        EventLever LeverEvent{};

		void End_Event() { isEvent = false; }

        bool isInCave() { return CaveEvent.isInCave; }
        void CaveIn() { CaveEvent.isInCave = true; }
        void CaveOut() { CaveEvent.isInCave = false; }
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

	struct EVENT_LOCKON_VISIBLE {
		const _float4* pTargetPos;
		_bool isVisible;
	};

	struct EVENT_SKILL_OPEN {};


    struct EVENT_SKILL_ON {
        _bool isUp;
        _int  SkillIndex;
    };



    struct EVENT_ANNOUNCE_TALK {
        _int  iTalkIndex;
    };

    struct EVENT_ANNOUNCE_RESULT { };

    struct EVENT_SKILL_SLOT    {
        _int    iIndex;
        _int    iSkillIndex;
        _bool   isEquip;
    };
}