#pragma once
#include "StateMachine.h"
#include "Client_Defines.h"

enum class GOMDOL_STATE
{
	SLEEP,
	IDLE,
	MOVE,
	TURN,
	ATTACK,
};

NS_BEGIN(Engine)
class CGameObject;
NS_END


NS_BEGIN(Client)

class CFSM_Gomdol final : public CStateMachine
{
private:
	CFSM_Gomdol();
	virtual ~CFSM_Gomdol() = default;

public:
	virtual HRESULT				Initialize(class CGameObject* pOwner);
	virtual void				Update(class CGameObject* pOwner, _float fTimeDelta) override;

public:
	static CFSM_Gomdol*			Create(class CGameObject* pOwner);
	virtual void				Free() override;

};

NS_END