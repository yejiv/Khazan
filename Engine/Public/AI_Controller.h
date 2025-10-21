#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CAI_Controller abstract : public CBase
{

protected:
	CAI_Controller();
	virtual ~CAI_Controller() = default;

public:
	class CStateMachine* Get_State_Machine() const { return m_pFSM; }

public:
	void				AI_MoveTo(class CGameObject* pOwner, class CGameObject* pTarget, _float fLimit, _float fTimeDelta);
	void				AI_ApplyDamage(class CGameObject* pAttacker, _float fDamage);

public:
	virtual HRESULT		Initialize();
	virtual void		Update(class CGameObject* pOwner, _float fTimeDelta);

protected:
	class CStateMachine*	m_pFSM = { nullptr };
	class CBehaviorTree*	m_pBT = { nullptr };
	class CBlackBoard*		m_pBB = { nullptr };
	class CPerception*		m_pPerception = { nullptr };
	class CGameInstance*	m_pGameInstance = { nullptr };

public:
	virtual void			Free() override;


};

NS_END