#pragma once

namespace Client {
	// ENUMs
	enum class EVENT_TYPE { TEST, UI_QUICK_SLOT,
		INTERACT_TYPE,								// 상호 작용할 오브젝트의 타입
		BLADENEXUS, BLADENEXUS_ACTIVE,				// 귀검 시작, 끝 ( 맵 오브젝트에서 구독할 타입 ) | 플레이어 & UI 활성화 ( 플레이어, UI에서 구독할 타입 )
		BIGCHEST, BIGCHEST_ACTIVE,					// 상자 시작, 끝 ( 맵 오브젝트에서 구독할 타입 ) | 플레이어 & UI 활성화 ( 플레이어, UI에서 구독할 타입 )
		END };

	// Structs
	struct EventTest {
		int data;
	};

	// 상호작용 오브젝트 활성화 이벤트 구조체 ( 1. 오브젝트 활성화, 2. 오브젝트 비활성화 )
	struct EventObject {
		bool isObjectOn{ false };
		bool isObjectOff{ false };
	};

	// 귀검 관련 이벤트 ( 1. 귀검 월드 포지션 ( 플레이어 -> 귀검 방향 용 ), 2. 귀검 활성화 )
	struct EventBNActive {
		XMFLOAT3 vPosition{};
		bool isSignal{ false };
		bool isBNActive{ false };
	};

	// 상자 관련 이벤트 ( 1. 상자 월드 포지션 ( 플레이어 -> 귀검 방향 용 ), 2. 상자 앞 플레이어 위치 ( 로컬 본 행렬 * 해당 오브젝트의 월드 행렬 ).r[3], 3. 상자 활성화 )
	struct EventChestActive {
		XMFLOAT3 vPosition{};
		XMFLOAT3 vPlayerPosition{};
		bool isSignal{ false };
		bool isChestActive{ false };
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