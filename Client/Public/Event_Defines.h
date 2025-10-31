#pragma once

namespace Client {
	// ENUMs
	enum class EVENT_TYPE { TEST, UI_QUICK_SLOT,
		OBJECT_INTERACT,							// 상호 작용할 오브젝트에 접촉 ( 맵 오브젝트가 구독 )
		INTERACT_TYPE,								// 상호 작용할 오브젝트에 접촉 ( 플레이어가 구독 )
		END };

	// Structs
	struct EventTest {
		int data;
	};

	// 상호작용 오브젝트 활성화 이벤트 구조체 ( 1. 오브젝트 활성화, 2. 오브젝트 비활성화 ) ( 플레이어 -> 오브젝트 )
	struct EventObject {
		bool isObjectOn{ false };		// 상호작용 오브젝트 활성화
		bool isObjectOff{ false };		// 상호작용 오브젝트 비활성화
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

	// 상호작용 오브젝트 어떤 종류인지 받아오는 이벤트 구조체 ( 오브젝트 -> 플레이어 )
	struct EventInteractType {
		enum EVENT_STATE { BEGIN, END, NONE };						// 이벤트 시작, 끝 여부
		EVENT_STATE eState = { EVENT_STATE::NONE };					// 이벤트 상태 여부
		_bool isEvent{ false };										// 이벤트 발생 시 TRUE
		INTERACTIVE_TYPE eInteractType{ INTERACTIVE_TYPE::END };	// 상호작용 오브젝트의 타입 ( 체크포인트, 상자, 사다리 등등 )
		EventBladeNexus BNEvent{};									// 귀검 구조체
		EventChest ChestEvent{};									// 상자 구조체
	};

	// 상자 관련 이벤트
	struct EventBigChest {
		XMFLOAT3 vPosition{};
	};
	struct EVENT_HUD_QUICKSLOT {
		_bool isEquip;
		_int  iIndex;
		_int  iItemIndex;
		_int* iItemCount;
	};
}