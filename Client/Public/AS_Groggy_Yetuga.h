#pragma once
#include "AI_State.h"
#include "Client_Defines.h"

enum class GROGGY { START, LOOP, END };

NS_BEGIN(Client)

class CAS_Groggy_Yetuga final : public CAI_State
{
private:
	CAS_Groggy_Yetuga();
	virtual ~CAS_Groggy_Yetuga() = default;

public:
	virtual void Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) override;
	virtual void Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) override;
	virtual void Exit(class CStateMachine* pFSM, class CGameObject* pOwner) override;

private:
private:
	_float						m_fCurrentTime = {};
	_float						m_fGroggyTime = {};
	
	_float4*					m_vBonePosition = {};

	_bool						m_isBrutalAttackSuccess = { false };

	GROGGY						m_eState = { GROGGY::START };
	class CTarget_BrutalAttack* m_pBrutalAttack = { nullptr };



public:
	static CAS_Groggy_Yetuga*	Create();
	virtual void				Free() override;

};

NS_END