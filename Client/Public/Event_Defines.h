#pragma once

namespace Client {
	// ENUMs
	enum class EVENT_TYPE { LEVEL_CHANGE, UI_QUICK_SLOT,
		OBJECT_INTERACT,							// 상호 작용할 오브젝트에 접촉 ( 맵 오브젝트가 구독 )
		INTERACT_TYPE,								// 상호 작용할 오브젝트에 접촉 ( 플레이어가 구독 )
		ANNOUNCE_MAPNAME,							// 지역 이름, 연출 이름 출력
		LOCKON_VISIBLE,								// 록온 랜더 온/오프
		SKILL_EVENT,								// 스킬 슬롯 관련 이벤트들
		END };

	// Structs
	struct EVENT_LEVEL_CHANGE {
		_int iLevel;
	};

	// 상호작용 오브젝트 활성화 이벤트 구조체 ( 1. 오브젝트 활성화, 2. 오브젝트 비활성화 ) ( 플레이어 -> 오브젝트 )
	struct EventObject {
		enum class EVENT_STATE { NONE, ON, OFF };			// NONE : 노 액션 | ON : 활성화 | OFF : 비활성화
		EVENT_STATE eState{ EVENT_STATE::NONE };

		void On() { eState = EVENT_STATE::ON; }
		void Off() { eState = EVENT_STATE::OFF; }
		void None() { eState = EVENT_STATE::NONE; }

		_bool isOn() const { return eState == EVENT_STATE::ON; }
		_bool isOff() const { return eState == EVENT_STATE::OFF; }

		// 플레이어 -> 오브젝트 ( 이벤트 활성화 )
		static EventObject OnEvent()
		{
			EventObject e = {};
			e.On();
			return e;
		}

		// 플레이어 -> 오브젝트 ( 이벤트 끝났다 )
		static EventObject OffEvent()
		{
			EventObject e = {};
			e.Off();
			return e;
		}

		// 플레이어 -> 오브젝트 ( 이벤트 없다 )
		static EventObject NoneEvent()
		{
			EventObject e = {};
			e.None();
			return e;
		}
	};

	// 귀검 관련 이벤트 ( 1. 귀검 월드 포지션 ( LOOK 플레이어 -> 귀검 방향 용 ), 2. 귀검 활성화 )
	struct EventBladeNexus {
		XMFLOAT3 vPosition{};			// 귀검의 월드 위치
		bool isUnLock{ false };			// 귀검 첫 해금인지 아닌지 여부 ( TRUE 면 첫 해금, FALSE 면 해금 한 귀검 )
		bool isBNOpened{ false };		// 귀검 가동 애니메이션 종료되면 TRUE
	};

	// 상자 관련 이벤트 ( 1. 상자 월드 포지션 ( LOOK 플레이어 -> 귀검 방향 용 ), 2. 상자 앞 플레이어 위치 ( 로컬 본 행렬 * 해당 오브젝트의 월드 행렬 ).r[3], 3. 상자가 가진 아이템들, 4. 상자 활성화 )
	struct EventChest {
		XMFLOAT3 vPosition{};			// 상자 월드 위치
		XMFLOAT3 vPlayerPosition{};		// 상자의 플레이어 보정 위치
		BOX_ITEMS Items{};				// 상자안에 있는 아이템 0, 1, 2
		bool isChestOpened{ false };	// 상자 열림 애니메이션 종료되면 TRUE
	};

	// 경계의 틈 툼스톤 관련 이벤트 ( 1. 상자 월드 포지션 ( LOOK 플레이어 -> 귀검 방향 용 ), 2. 상자 앞 플레이어 위치 ( 로컬 본 행렬 * 해당 오브젝트의 월드 행렬 ).r[3], 3. 상자가 가진 아이템들, 4. 상자 활성화 )
	struct EventTombStone {
		XMFLOAT3 vPosition{};			// 툼스톤 월드 위치
		XMFLOAT3 vPlayerPosition{};		// 툼스톤의 플레이어 보정 위치			( 부서진 거, 안 부서진거 위치가 다름 )
		bool isTSOpened{ false };	    // 툼스톤 가동 애니메이션 종료되면 TRUE
	};

	// 상호작용 오브젝트 어떤 종류인지 받아오는 이벤트 구조체 ( 오브젝트 -> 플레이어 )
	struct EventInteractType {
		enum EVENT_STATE { BEGIN, END, NONE };						// 이벤트 시작, 끝 여부
		EVENT_STATE eState = { EVENT_STATE::NONE };					// 이벤트 상태 여부
		_bool isEvent{ false };										// 이벤트 발생 시 TRUE
		INTERACTIVE_TYPE eInteractType{ INTERACTIVE_TYPE::END };	// 상호작용 오브젝트의 타입 ( 체크포인트, 상자, 사다리 등등 )
		EventBladeNexus BNEvent{};									// 귀검 구조체
		EventChest ChestEvent{};									// 상자 구조체
		EventTombStone TSEvent{};									// 경계의 틈 툼스톤 구조체

		void End_Event() { isEvent = false; }
	};

	// 상자 관련 이벤트
	struct EventBigChest {
		XMFLOAT3 vPosition{};
	};

	//UI 퀵슬롯 관련 이벤트
	struct EVENT_HUD_QUICKSLOT {
		_bool isEquip;
		_int  iIndex;
		_int  iItemIndex;
		_int* iItemCount;
	};

	//지역 이름 표시하는 이벤트
	struct EVENT_ANNOUNCE_MAPNAME {
		_wstring	wstrName;				//지역이름
		_int		iMapType;				//화면에 표시하는 지역 이름
		_float		fTime;					//화면에 출력시간
		_int		fFadeOutTime;			//화면에서 사라지는 시간
		_bool		isDissovle;
	};

    //락온
	struct EVENT_LOCKON_VISIBLE {
		const _float4* pTargetPos;
		_bool isVisible;
	};
    
    //스킬 트리 오픈
	struct EVENT_SKILL_OPEN {};

}