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
        ANNOUNCE_WARNING,
        SKILL_QUICKSLOT,
        GATE_GEAR0,
        GATE_GEAR1,
        SKILL_RESET,
        STATUE_PUZZLE0,
        STATUE_PUZZLE1,
        HALL_ELEVATOR_UNLOCK,
		END };

	// Structs
	struct EVENT_LEVEL_CHANGE {
		_int iLevel;
	};

	struct EventObject {
		enum class EVENT_STATE { NONE, ON, OFF };	
		EVENT_STATE eState{ EVENT_STATE::NONE };

        XMFLOAT4 vPosition{};

		void On() { eState = EVENT_STATE::ON; }
		void Off() { eState = EVENT_STATE::OFF; }
		void None() { eState = EVENT_STATE::NONE; }
        void SetPosition(XMFLOAT4 vPos) { vPosition = vPos; }

		_bool isOn() const { return eState == EVENT_STATE::ON; }
		_bool isOff() const { return eState == EVENT_STATE::OFF; }


        static EventObject OnEvent()
        {
            EventObject e = {};
            e.On();
            return e;
        }

        static EventObject OnEvent_Player(XMFLOAT4 vPosition)
        {
            EventObject e = {};
            e.SetPosition(vPosition);
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
        bool isInCave{ false };
    };

    struct EventLever
    {
        enum LEVER_STATE { ACTIVE, DEACTIVE, NONE };
        XMFLOAT4 vPosition{};
        XMFLOAT4 vPlayerPosition{};
        LEVER_STATE eState{ LEVER_STATE::NONE };
    };

    struct EventGateGear
    {
        bool isActiveLever{ false };
        bool isActiveGear1{ false };
        bool isActiveGear2{ false };

        bool isFirstStep() { return true == isActiveLever; }
        bool isSecondStep() { return (true == isActiveLever && true == isActiveGear1); }
        bool isFinal() { return true == isActiveGear2; }
    };

    struct EventStatue {
        XMFLOAT4 vPosition{};
        XMFLOAT4 vPlayerPosition{};
    };

    // 상호작용 오브젝트끼리의 이벤트 ( 조각상 -> 차단봉 )
    struct EventVerticalGate
    {
        bool isActiveStatue[4] = { false, false, false, false };

        void SetActiveStatue(unsigned int iStatueIndex)
        {
            isActiveStatue[iStatueIndex] = true;
        }

        void SetDeActiveStatue(unsigned int iStatueIndex)
        {
            isActiveStatue[iStatueIndex] = false;
        }

        bool isSection0()       // 4개 조각상
        {
            for (_uint i = 0; i < 4; ++i)
            {
                if (false == isActiveStatue[i]) return false;
            }

            return true;
        }
        bool isSection1()       // 3개 조각상
        {
            for (_uint i = 0; i < 3; ++i)
            {
                if (false == isActiveStatue[i]) return false;
            }

            return true;
        }

        bool isUnLockGate(unsigned int iGateEventID)
        {
            if (0 == iGateEventID)
                return isSection0();
            if (1 == iGateEventID)
                return isSection1();
            return false;
        }
    };

    // 상호작용 오브젝트끼리의 이벤트 ( 해제 기어 -> 홀 엘리베이터 )
    struct EventHallElevator
    {
        enum UNLOCK_STATE { NONE, STEP_1, STEP_2, STEP_3, END};

        UNLOCK_STATE eStep{};

        void Set_UnLockState(bool isUnLock)
        {
            if (false == isUnLock)
                return;

            if (UNLOCK_STATE::NONE == eStep)
                eStep = STEP_1;
            else if (UNLOCK_STATE::STEP_1 == eStep)
                eStep = STEP_2;
            else if (UNLOCK_STATE::STEP_2 == eStep)
                eStep = STEP_3;
        }

        bool IsThirdStep() {
            if (UNLOCK_STATE::STEP_3 == eStep)
                return true;
            return false;
        }
    };

    struct EventIronGate
    {
        XMFLOAT4 vPosition{};
        XMFLOAT4 vPlayerPosition{};
    };

    struct EventUnLockGear
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
        EventStatue StatueEvent{};
        EventIronGate IronGateEvent{};
        EventUnLockGear UnLockGearEvent{};

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

    struct EVENT_ANNOUNCE_WARNING {
        _wstring strText;
    };

    struct EVENT_SKILL_SLOT    {
        _int    iIndex;
        _int    iSkillIndex;
        _bool   isEquip;
    };
    struct EVENT_SKILL_RESET { };
}