#pragma once
#include "AI_State.h"
#include "Client_Defines.h"


enum class GOMDOL_SLEEP_STATE {SLEEP, WAKEUP, LOAR ,END, };

NS_BEGIN(Client)

class CAS_Sleep_Gomdol final : public CAI_State
{
private:
	CAS_Sleep_Gomdol();
	virtual ~CAS_Sleep_Gomdol() = default;

public:
	virtual void					Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
	virtual void					Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
	virtual void					Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

public:
	virtual void					OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer, class CGameObject* pOwner = nullptr) {};

private:
	GOMDOL_SLEEP_STATE				m_eState = { GOMDOL_SLEEP_STATE::END };
	_bool							m_isChanged = { false };

public:
	static CAS_Sleep_Gomdol*		Create();
	virtual void					Free() override;
};

NS_END