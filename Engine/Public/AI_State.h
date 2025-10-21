#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CAI_State abstract : public CBase
{
protected:
	CAI_State();
	virtual ~CAI_State() = default;

public:
	virtual void Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) = 0;
	virtual void Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) = 0;
	virtual void Exit(class CStateMachine* pFSM, class CGameObject* pOwner) = 0;


protected:
	class CGameInstance*		m_pGameInstance = { nullptr };

public:
	virtual void Free() override;


};

NS_END