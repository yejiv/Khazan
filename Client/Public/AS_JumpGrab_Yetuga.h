#pragma once
#include "AI_State.h"
#include "Client_Defines.h"

NS_BEGIN(Client)

class CAS_JumpGrab_Yetuga final : public CAI_State
{
private:
	CAS_JumpGrab_Yetuga();
	virtual ~CAS_JumpGrab_Yetuga() = default;

public:
	virtual void Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
	virtual void Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
	virtual void Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

public:
	virtual void OnCollision(COLLISION_DESC* pDesc);



private:
	_bool						m_isGrabbed = { false };
	_float3						m_vGrabPoint = {};

public:
	static CAS_JumpGrab_Yetuga* Create();
	virtual void				Free() override;

};

NS_END