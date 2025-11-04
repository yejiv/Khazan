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

class CKhazan_Sample final:  public CCreature
{
public:
	enum PLAYER_STATE {
		IDLE = 1 << 0,
		WALK = 1 << 1,
		RUN = 1 << 2,
		MOVING = WALK | RUN,
		ATTACK_FAST = 1 << 3, //약공
		ATTACK_STRONG = 1 << 4, //강공
		ATTACK_FULLMOON = 1 << 5, //보름달
		ATTACK_SHADOW1 = 1 << 6,//그림자 베기  = 강공
		ATTACK_SPIRAL = 1 << 7,//기본찌르기
		ATTACK_TWISTE = 1 << 8,//찌르기, 소용돌이
		ATTACK_STRIKE = 1 << 9, //강습

		ATTACK_SOON = 1 << 10, //찰나베기
		ATTACK_VITALPOINT = 1 << 11, // 급소타격
		ATTACK_SHADOW2 = 1 << 12,//그림자 참격
		ATTACK_BRUTAL = 1 << 13, // 브루탈
		ATTACK_COUNT = 1 << 14, //  카운터   (애니메이션 안넣음)

		ATTACK_ALL =
		ATTACK_FAST |
		ATTACK_STRONG |
		ATTACK_FULLMOON |
		ATTACK_SHADOW1 |
		ATTACK_SPIRAL |
		ATTACK_TWISTE |
		ATTACK_STRIKE |
		ATTACK_SOON |
		ATTACK_VITALPOINT |
		ATTACK_SHADOW2 |
		ATTACK_BRUTAL ,
		//ATTACK_COUNT,

		ATTACK_FAST_ING = 1 << 15,
		ATTACK_STRING_ING = 1 << 16,


		END = 1<<17,
	};

private:
	enum  MOVE_DIR {
		UP = 1 << 0,
		DOWN = 1 << 1,
		RIGHT = 1 << 2,
		LEFT = 1 << 3,
	};

private:
	CKhazan_Sample(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKhazan_Sample(const CKhazan_Sample& Prototype);
	virtual ~CKhazan_Sample() = default;

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

	HRESULT RayCast(_float fTimeDelta);

	const _float4x4* Get_BoneSpearFX_WorldMatrixPtr() { return &m_SpearFX_WorldMatrix; }
	_matrix Get_BoneSpearFX_WorldMatrix() { return XMLoadFloat4x4(&m_SpearFX_WorldMatrix); }

	const _float4x4* Get_BoneSpearFX_CombinedMatrixPtr() { return m_pSpearFX_Matrix; }
	_matrix Get_BoneSpearFX_CombinedMatrix() { return XMLoadFloat4x4(m_pSpearFX_Matrix); }

	_matrix Get_BoneSpearFX_Offset() { return m_SpearOffset_Matrix; }

private:
	class CBody_Khazan_Sample*	m_pBody = { nullptr };
	class CSpear_Khazan_Sample* m_pSpear = { nullptr };

	_uint						m_iState = { };
	_uint						m_iDirState = { MOVE_DIR::RIGHT };

	//class CRigidBody*			m_pRigidBodyCom = { nullptr };
	//CCharacterVirtual*			m_pCharVirCom = { nullptr };
	_float4x4*					m_pWeaponR_Matrix = { nullptr };
	_float4x4*					m_pSpearFX_Matrix = { nullptr };
	_float4x4					m_SpearFX_WorldMatrix = {};
	_matrix						m_SpearOffset_Matrix = {};

	_bool						m_isEnableControl = { true };
	_int						m_isMove = {0};


	//락온 테스트
	_float4						m_vPos = {};


	/*  공격 */
	_uint						m_iFastAttackIndex = { 0 };
	_uint						m_iStrongAttackIndex = { 0 };

#pragma region 상호 작용 맵 오브젝트 이벤트 임시 테스트용
private:
	EventInteractType			m_EventInteract = {};				// 트리거 접촉 여부, 이벤트 발생 여부, 상호 작용 타입, 상호 작용 타입들의 구조체
	_float						m_fEventTimeAcc = { 0.f };
	_ushort						m_sNextItem = { 0 };

private:
	void						Event_Interact_Object(_float fTimeDelta);
	void						BladeNexus_Event(_float fTimeDelta);
	void						Chest_Event(_float fTimeDelta);
#pragma endregion

private:
	void				Update_State(_float fTimeDelta);
	void				Key_Input(_float fTimeDelta);

private:
	HRESULT				Ready_Components();
	HRESULT				Ready_PartObjects();
	HRESULT				Ready_Collision();

private:
	_int			m_iMaxHp = {};
	_int			m_iHp = {};
	_int			m_iMaxStamina = {};
	_int			m_iStamina = {};

private:
	inline void		Add_State(_uint i) { m_iState |= i; }
	inline void		Toggle_State(_uint i) { m_iState ^= i; }
	inline void		Remove_State(_uint i) { m_iState &= ~i; }
	inline _bool	Has_State(_uint i) { return (m_iState & i) != 0; }
	inline void		Clear_State() { m_iState = 0; }
	inline _bool	Has_States();

	inline void		Add_DirState(_uint i) { m_iDirState |= i; }
	inline void		Toggle_DirState(_uint i) { m_iDirState ^= i; }
	inline void		Remove_DirState(_uint i) { m_iDirState &= ~i; }
	inline _bool	Has_DirState(_uint i) { return (m_iDirState & i) != 0; }
	inline void		Clear_DirState() { m_iDirState = 0; }

#ifdef _DEBUG
	void			Debug_Widget();
#endif // _DEBUG


public:
	static CKhazan_Sample* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;


};

NS_END
