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
        EMBARS_GIMMICK0,
        EMBARS_GIMMICK1,
        EMBARS_GIMMICK2,
        HALL_ELEVATOR_UNLOCK,
        ELEVATOR_SKIP,
        BLADENEXUS_POP,
        OBELISK_DESTRUCT,
        PET,
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

    struct EventPopBN {
        bool isPop{ false };
    };

	struct EventBladeNexus {
		XMFLOAT4 vPosition{};
		bool isUnLock{ false };
		bool isBNOpened{ false };
	};

    struct EventDestinyStone {
        XMFLOAT4 vPosition{};
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
        XMFLOAT4 vPlayerTPPos{};
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

    // 상호작용 오브젝트끼리의 이벤트 ( 조각상 -> 차단봉 | 엘리베이터 -> 차단봉 )
    struct EventGimmick
    {
    private:
        bool isStatueSolved[4] = { false, false, false, false };    // 석상

        bool isUnLockGearAvailableFlag = { false };     // 잠금 해제 기어 상호작용 가능 여부

        bool isUnLockGearActiveFlag = { false };        // 잠금 장치 활성화 여부

        bool isVerticalGateActiveFlag = { false };      // 수직 차단봉 활성화 여부

        bool isElevatorDone = { false };                // 엘리베이터완료 시 

    public:
        void Set_SolveStatue(unsigned int iStatueIndex)
        {
            isStatueSolved[iStatueIndex] = true;
        }

        static EventGimmick Set_InitSolve(unsigned int iStatueIndex)
        {
            EventGimmick e = {};
            e.Set_SolveStatue(iStatueIndex);
            return e;
        }

        void Reset_SolveStatue(unsigned int iStatueIndex)
        {
            isStatueSolved[iStatueIndex] = false;
        }

        void Set_AvailableUnLockGear()              // 잠금 장치 상호작용 활성화
        {
            isUnLockGearAvailableFlag = true;
        }

        void Set_ActiveUnLockGear()                 // 잠금 장치 활성화
        {
            isUnLockGearActiveFlag = true;
        }

        void Set_ActiveGate()                       // 차단봉 활성화
        {
            isVerticalGateActiveFlag = true;
        }

        bool isStatueSection0()       // 4개 조각상
        {
            for (_uint i = 0; i < 4; ++i)
            {
                if (false == isStatueSolved[i]) return false;
            }

            return true;
        }

        bool isStatueSection1()       // 3개 조각상
        {
            for (_uint i = 0; i < 3; ++i)
            {
                if (false == isStatueSolved[i]) return false;
            }

            return true;
        }

        bool isUnLockGearAvailable(unsigned int iEventID)
        {
            if (0 == iEventID)
                return isStatueSection0();
            if (1 == iEventID)
                return isStatueSection1();

            return false;
        }

        bool isActiveGate() { return isVerticalGateActiveFlag; }

        void Set_GimmickClear()
        {
            for (_uint i = 0; i < 4; ++i)
            {
                isStatueSolved[i] = true;
            }

            isUnLockGearAvailableFlag = false;
            isUnLockGearActiveFlag = false;
            isVerticalGateActiveFlag = false;
        }
    };

    // 상호작용 오브젝트끼리의 이벤트 ( 해제 기어 -> 홀 엘리베이터 )
    struct EventHallElevator
    {
        enum UNLOCK_STATE { NONE, STEP_1, STEP_2, STEP_3, END};

        UNLOCK_STATE eStep{};
        bool isEventOn = {};

        bool isEvent() { return isEventOn; }

        void EventOn() { isEventOn = true; }
        void EventOff() { isEventOn = false; }

        void Set_UnLockState(bool isUnLock)
        {
            if (false == isUnLock)
                return;

            isEventOn = isUnLock;

            if (UNLOCK_STATE::NONE == eStep)
                eStep = STEP_1;
            else if (UNLOCK_STATE::STEP_1 == eStep)
                eStep = STEP_2;
            else if (UNLOCK_STATE::STEP_2 == eStep)
                eStep = STEP_3;
        }

        bool IsFirstStep() {
            if (UNLOCK_STATE::STEP_1 == eStep)
                return true;
            return false;
        }

        bool IsSecondStep() {
            if (UNLOCK_STATE::STEP_2 == eStep)
                return true;
            return false;
        }

        bool IsThirdStep() {
            if (UNLOCK_STATE::STEP_3 == eStep)
                return true;
            return false;
        }
    };

    struct EventElevatorSkip {
        bool isSkip{};

        void Skip() { isSkip = true; }

        static EventElevatorSkip SkipEvent()
        {
            EventElevatorSkip e = {};
            e.Skip();
            return e;
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

    struct EventGiantGate
    {
        XMFLOAT4 vPosition{};
        XMFLOAT4 vPlayerPosition{};
    };

    struct EventNPC
    {
        XMFLOAT4 vPosition{};
        XMFLOAT4 vPlayerPosition{};
    };

    struct EventLadder
    {
        enum class LADDER_ACTION {
            UPTODOWN,       // 위에서 아래로가는 액션
            DOWNTOUP,       // 아래에서 위로가는 액션
            UPEND,          // 위쪽 도착했을때 액션
            DOWNEND,        // 아래쪽 도착했을때 액션
            NONE };

        LADDER_ACTION eLadderState{ LADDER_ACTION::NONE };
        XMFLOAT4 vPosition{};
        XMFLOAT4 vPlayerPosition{};
        XMFLOAT2 vEndActionPos{};               // x : 위쪽 값 | y : 아래쪽 값

        bool isStartAction() {
            return LADDER_ACTION::UPTODOWN == eLadderState || LADDER_ACTION::DOWNTOUP == eLadderState;
        }

        bool isEndAction() {
            return LADDER_ACTION::DOWNEND == eLadderState || LADDER_ACTION::UPEND == eLadderState;
        }
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
        EventGiantGate GiantGateEvent{};
        EventNPC NPCEvent{};
        EventLadder LadderEvent{};
        EventDestinyStone DSEvent{};

		void End_Event() { isEvent = false; }

        bool isInCave() { return CaveEvent.isInCave; }
        void CaveIn() { CaveEvent.isInCave = true; }
        void CaveOut() { CaveEvent.isInCave = false; }

        bool isNPC() {
            return (INTERACTIVE_TYPE::DANJIN == eInteractType ||
                INTERACTIVE_TYPE::DAPHRONA == eInteractType ||
                INTERACTIVE_TYPE::DUIMUK == eInteractType ||
                INTERACTIVE_TYPE::GACHANPC == eInteractType);
        }
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

    struct EVENT_OBELISK_DESTRUCT {
        _bool   isDestruct;
    };

    struct EVENT_PET_STATE {
        _bool isStart;
    };
}