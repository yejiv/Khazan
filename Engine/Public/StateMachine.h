#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class ENGINE_DLL CStateMachine abstract : public CBase
{
protected:
	CStateMachine();
	virtual ~CStateMachine() = default;

public:
	virtual HRESULT		Initialize(class CAI_State* pCurrentState);
	virtual void		Update(class CGameObject* pOwner, _float fTimeDelta);

public:
	void				Add_Flag(_uint iFlag) { m_iStateFlag |= iFlag; }
	void				Delete_Flag(_uint iFlag) { m_iStateFlag &= ~iFlag; }
	void				Clear_Flag() { m_iStateFlag = 0; }
	_bool				Check_Flag(_uint iFlag) { return (m_iStateFlag & iFlag) != 0; }


public:
	HRESULT				Change_State(_uint iStateIndex, class CGameObject* pOwner);
	void				OnCollision(COLLISION_DESC* pDesc);


protected:
	HRESULT				Add_State(_uint iStateIndex, class CAI_State* pState);
	class CAI_State*	Find_State(_uint iStateIndex);
	HRESULT				Set_State(class CAI_State* pNextState, class CGameObject* pOwner);

protected:
	_uint					m_iStateFlag = {};

	class CAI_State*		m_pCurrentState = { nullptr };


	unordered_map<_uint, class CAI_State*> m_States;


public:
	virtual void		Free() override;


};

NS_END