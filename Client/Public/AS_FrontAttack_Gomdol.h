#pragma once
#include "AI_State.h"
#include "Client_Defines.h"

NS_BEGIN(Client)

class CAS_FrontAttack_Gomdol final : public CAI_State
{
private:
	CAS_FrontAttack_Gomdol();
	virtual ~CAS_FrontAttack_Gomdol() = default;

public:
	virtual void					Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
	virtual void					Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
	virtual void					Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

public:
	virtual void					OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, class CGameObject* pOwner = nullptr) override;

private:


public:
	static CAS_FrontAttack_Gomdol*	Create();
	virtual void					Free() override;
};

NS_END