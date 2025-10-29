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
		bool isObjectOn{ false };
		bool isObjectOff{ false };
	};

	// 귀검 관련 이벤트 ( 1. 귀검 월드 포지션 ( 플레이어 -> 귀검 방향 용 ), 2. 귀검 활성화 )
	struct EventBladeNexus {
		XMFLOAT3 vPosition{};
		bool isBNOpened{ false };
	};

	// 상자 관련 이벤트 ( 1. 상자 월드 포지션 ( 플레이어 -> 귀검 방향 용 ), 2. 상자 앞 플레이어 위치 ( 로컬 본 행렬 * 해당 오브젝트의 월드 행렬 ).r[3], 3. 상자 활성화 )
	struct EventChest {
		XMFLOAT3 vPosition{};
		XMFLOAT3 vPlayerPosition{};
		bool isChestOpened{ false };
	};

	// 상호작용 오브젝트 어떤 종류인지 받아오는 이벤트 구조체 ( 오브젝트 -> 플레이어 )
	struct EventInteractType {
		bool isEvent{ false };
		INTERACTIVE_TYPE eInteractType{ INTERACTIVE_TYPE::END };
		EventBladeNexus BNEvent{};
		EventChest ChestEvent{};
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