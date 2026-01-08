#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CAI_State abstract : public CBase
{
protected:
	CAI_State();
	virtual ~CAI_State() = default;

public:
    void        Set_StateIndex(_uint iIndex) { m_iStateIndex = iIndex; }
    _uint       Get_StateIndex() { return m_iStateIndex; }


public:
	virtual void Enter(class CStateMachine* pFSM, class CGameObject* pOwner = nullptr) = 0;
	virtual void Update(class CStateMachine* pFSM, class CGameObject* pOwner, _float fTimeDelta) = 0;
	virtual void Exit(class CStateMachine* pFSM, class CGameObject* pOwner) = 0;
	
public:
	virtual void OnCollision(COLLISION_DESC* pDesc, _uint iCollisionLayer ,class CGameObject* pOwner = nullptr) {};

protected:
	class CGameInstance*		m_pGameInstance = { nullptr };
    _uint                       m_iStateIndex = {};


public:
	virtual void Free() override;


};

NS_END