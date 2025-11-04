#pragma once
#include "Client_Defines.h"
#include "Creature.h"

NS_BEGIN(Engine)
class CNavigation;
class CCharacterVirtual;
class CRigidBody; 
class CCollider;
NS_END

NS_BEGIN(Client)

using DIR = DIRECTION_INFO;

class CKhazan_Spear final:  public CCreature
{
private:

	enum PLAYER_STATUS : _uint
	{
		BAREHAND = 1 << 0,
		SPEAR = 1 << 1,

		RESERVED = 1 << 2,

		CHARGING_SPRINT = 1 << 3,
		BACK_DODGE = 1 << 4,

		ROTATION = 1 << 5,
		KEY_ROTATION = 1 << 6,
		SAMEDIRECTION = 1 << 7,  // 키입력 방향과 플레이어의 룩방향 일치성



	};
	enum PLAYER_CAMERA_DIR {
		PC_FRONT, PC_FRONT_RIGHT, PC_RIGHT, PC_BACK_RIGHT,
		PC_BACK, PC_BACK_LEFT, PC_LEFT, PC_FRONT_LEFT
	};

private:
	CKhazan_Spear(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKhazan_Spear(const CKhazan_Spear& Prototype);
	virtual ~CKhazan_Spear() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual void Priority_Update(_float fTimeDelta);
	virtual void Update(_float fTimeDelta);
	virtual void Late_Update(_float fTimeDelta);
	virtual HRESULT Render();

public:
	void Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;
	void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;

	const _float4x4* Get_BoneSpearFXMatrixPtr() { return &m_pSpearFX_WorldMatrix; }
	_matrix Get_BoneSpearFXMatrix() { return XMLoadFloat4x4(&m_pSpearFX_WorldMatrix); }


private:
	class CBody_Khazan_Spear*		m_pBody = { nullptr };
	class CSpear_Khazan_Spear*		m_pSpear = { nullptr };
	class CKhazan_Spear_Anim_Move*	m_pAnimMove = { nullptr };
	class CKhazan_Spear_Anim_Attack* m_pAnimAttack = { nullptr };
	//kHAZAN_ANIM_INFO			m_eCurAnimInfo = {}; //후보지에서 선택된 애님인포 
	//vector<kHAZAN_ANIM_INFO>	m_AnimCandidates; // 매 프레임 후보 리스트 적립

	/* state */
	_uint						m_iStatus = {};
	_uint						m_iCurMainState = {};
	_uint						m_iPrevMainState = {};
	_uint						m_iCurSubState = {};
	_uint						m_iPrevSubState = {};
	_uint						m_iCycle;
	_uint						m_iPrevCycle;
	DIR							m_eDir = {};		//플레이어 dir
	_uint						m_ePrevDir = {};

	_uint						m_iCurAnimIndex = {};
	_uint						m_iReserveAnimIndex = {};

	/* info */
	_float4x4*					m_pWeaponR_Matrix = { nullptr };
	_float4x4*					m_pSpearFX_Matrix = { nullptr };
	_float4x4					m_pSpearFX_WorldMatrix = {};
	_matrix						m_SpearOffset_Matrix = {};
	_bool						m_isEnableControl = { true };

	vector<_float2>				m_vCoolTime;

	/* Move*/
	DIR							m_eWorldDir = {}; // 키 입력 방향
	_float						m_fRotateTime[2] = { 0.f,0.1f };
	_vector						m_vRotateStart;
	_float						m_fSprintTime = { 0.f };

	/* const */
	const	_float				m_fMinSprintTime = { 0.15f };

	/* Move Speed */
	const _float				m_fWalkSpeed = { 1.6f };
	const _float				m_fRunSpeed = { 4.f };
	const _float				m_fSprintSpeed = { 7.f };

private:
	void			Update_State(_float fTimeDelta);
	void			Move_Input(_float fTimeDelta);
	void			Attack_Input(_float fTimeDelta);
	void			ChangeAnimation();
	void			ExecuteAnimationExit();
	void			Apply_PlayerMovement(_float fTimeDelta);
	void			Check_KeyInput_Direction(_float fTimeDelta);

private:
	HRESULT			Ready_Components();
	HRESULT			Ready_PartObjects();
	HRESULT			Ready_Collision();
	HRESULT			Ready_AnimationStateMachine();

private:
	inline void		Add_Status(_uint i) { m_iStatus |= i; }
	inline void		Remove_Status(_uint i) { m_iStatus &= ~i; }
	inline _bool	Has_Status(_uint i) { return (m_iStatus & i) != 0; }
	inline void		Clear_Status( ) { m_iStatus = 0; }

	inline void		Add_State(_uint i) { m_iCurMainState |= i; }
	inline void		Toggle_State(_uint i) { m_iCurMainState ^= i; }
	inline void		Remove_State(_uint i) { m_iCurMainState &= ~i; }
	inline _bool	Has_State(_uint i) { return (m_iCurMainState & i) != 0; }
	inline _bool	Has_AllStates(_uint i) { return (m_iCurMainState & i) == i; }
	inline _bool	Has_States();	//하나의 상태라도 있는지 없는지
	inline void		Clear_State() { m_iCurMainState = 0; }

	inline void		Add_SubState(_uint i) { m_iCurSubState |= i; }
	inline void		Toggle_SubState(_uint i) { m_iCurSubState ^= i; }
	inline void		Remove_SubState(_uint i) { m_iCurSubState &= ~i; }
	inline _bool	Has_SubState(_uint i) { return (m_iCurSubState & i) != 0; }
	inline _bool	Has_AllSubStates(_uint i) { return (m_iCurSubState & i) == i; }
	inline _bool	Has_SubStates();	//하나의 상태라도 있는지 없는지
	inline void		Clear_SubState() { m_iCurSubState = 0; }

	inline void		Add_CycleState(_uint i) { m_iCycle |= i; }
	inline void		Remove_CycleState(_uint i) { m_iCycle &= ~i; }
	inline _bool	Has_CycleState(_uint i) { return (m_iCycle & i) != 0; }
	inline void		Clear_CycleState() { m_iCycle = 0; }
	inline void		AllClear_CycleState() { m_iCycle = 0;	}

	_uint			ConvertCameraToPlayerDir( PLAYER_CAMERA_DIR playerCamDir);

#ifdef _DEBUG
	void			Debug_Widget();
	void			Debug_Widget_States();      // 상태 디버깅
	void			Debug_Widget_Animation();   // 애니메이션 디버깅
	void			Debug_Widget_Movement();    // 이동 디버깅

	const char*		GetStateName(_uint state);
	const char*		GetSubStateName(_uint subState);
	const char*		GetCycleName(_uint cycle);
	std::string		GetDirectionString();
#endif // _DEBUG


public:
	static CKhazan_Spear* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;


};

NS_END
