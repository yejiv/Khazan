#pragma once
#include "AI_State.h"

NS_BEGIN(Client)

class CAS_RightHand_5Hit_Yetuga : public CAI_State
{
private:
	CAS_RightHand_5Hit_Yetuga();
	virtual ~CAS_RightHand_5Hit_Yetuga() = default;

public:
	virtual void Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr);
	virtual void Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta);
	virtual void Exit(class CStateMachine* pFSM, class CGameObject* pOwner);

public:
	virtual void OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, class CGameObject* pOwner = nullptr) override;


private:
	_uint	m_iComboCount = {};
	_bool	m_bisHit = {};

public:
	static CAS_RightHand_5Hit_Yetuga*	Create();
	virtual void						Free() override;

};

NS_END