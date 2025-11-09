#pragma once

namespace Client {
	// ENUMs
	enum class EVENT_TYPE { LEVEL_CHANGE, UI_QUICK_SLOT,
		OBJECT_INTERACT,							// ��ȣ �ۿ��� ������Ʈ�� ���� ( �� ������Ʈ�� ���� )
		INTERACT_TYPE,								// ��ȣ �ۿ��� ������Ʈ�� ���� ( �÷��̾ ���� )
		ANNOUNCE_MAPNAME,							// ���� �̸�, ���� �̸� ���
        ANNOUNCE_TALK,			    				// ���� �̸�, ���� �̸� ���
		LOCKON_VISIBLE,								// �Ͽ� ���� ��/����
		SKILL_EVENT,								// ��ų ���� ���� �̺�Ʈ��
        PreSKILL_On,
		END };

	// Structs
	struct EVENT_LEVEL_CHANGE {
		_int iLevel;
	};


	struct EventObject {
		enum class EVENT_STATE { NONE, ON, OFF };			// NONE : ???�션 | ON : ?�성??| OFF : 비활?�화

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

	// �Ͱ� �̺�Ʈ ( �Ͱ� ��ġ, ù �ر� ����, �Ͱ� Ȱ��ȭ ���� )
	struct EventBladeNexus {
		XMFLOAT3 vPosition{};
		bool isUnLock{ false };
		bool isBNOpened{ false };
	};

	// ���� �̺�Ʈ ( ���� ��ġ, �÷��̾��� ���� ��ȣ�ۿ� ��ġ, �����۵�, ���� Ȱ��ȭ ���� )
	struct EventChest {
		XMFLOAT3 vPosition{};
		XMFLOAT3 vPlayerPosition{};
		BOX_ITEMS Items{};
		bool isChestOpened{ false };
	};

	// ������ �̺�Ʈ ( ������ ��ġ, �÷��̾��� ������ ��ȣ �ۿ� ��ġ, ������ Ȱ��ȭ ���� )
	struct EventTombStone {
		XMFLOAT3 vPosition{};
		XMFLOAT3 vPlayerPosition{};
		bool isTSOpened{ false };
	};

	// ��ȣ �ۿ� �̺�Ʈ ( ������Ʈ -> �÷��̾� )
	struct EventInteractType {
		enum EVENT_STATE { BEGIN, END, NONE };						// ?�벤???�작, ???��?
		EVENT_STATE eState = { EVENT_STATE::NONE };					// ?�벤???�태 ?��?
		_bool isEvent{ false };										// ?�벤??발생 ??TRUE
		INTERACTIVE_TYPE eInteractType{ INTERACTIVE_TYPE::END };	// ?�호?�용 ?�브?�트???�??( 체크?�인?? ?�자, ?�다�??�등 )
		EventBladeNexus BNEvent{};									// 귀검 구조�?
		EventChest ChestEvent{};									// ?�자 구조�?
		EventTombStone TSEvent{};									// 경계?????�스??구조�?

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

	//지???�름 ?�시?�는 ?�벤??
	struct EVENT_ANNOUNCE_MAPNAME {
		_wstring	wstrName;				//지??���?
		_int		iMapType;				//?�면???�시?�는 지???�름
		_float		fTime;					//?�면??출력?�간
		_int		fFadeOutTime;			//?�면?�서 ?�라지???�간
		_bool		isDissovle;
	};

    //락온
	struct EVENT_LOCKON_VISIBLE {
		const _float4* pTargetPos;
		_bool isVisible;
	};
    
    //스킬 트리 오픈
	struct EVENT_SKILL_OPEN {};

    //��ų Ʈ�� ����
    struct EVENT_SKILL_ON {
        _bool isUp;
        _int  SkillIndex;
    };


    //�� ��ȭ
    struct EVENT_ANNOUNCE_TALK {
        _int  iTalkIndex;
    };
}