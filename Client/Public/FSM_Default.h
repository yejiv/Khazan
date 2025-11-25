#pragma once
#include "StateMachine.h"
#include "Client_Defines.h"

NS_BEGIN(Client)
class CFSM_Default final : public CStateMachine
{
private:
    CFSM_Default();
    virtual ~CFSM_Default() = default;

public:
    HRESULT				        Add_State(_uint iStateIndex, class CAI_State* pState);
    HRESULT                     Set_CulState(_uint iStateIndex, CGameObject* pOwner);
public:
    virtual HRESULT				Initialize(class CGameObject* pOwner);
    virtual void				Update(class CGameObject* pOwner, _float fTimeDelta) override;

public:
    static CFSM_Default*        Create(CGameObject* pOwner);
    virtual void				Free() override;
};

NS_END