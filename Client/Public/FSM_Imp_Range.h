#pragma once
#include "StateMachine.h"
#include "Client_Defines.h"

enum class  IMPRANGE_STATE
{
    IDLE,
    MOVE,
    MAGIC,
    BOOMARANG,
    HIT,
    DEAD,
    SLEEP,
    LOCKON,
    END,
};
NS_BEGIN(Engine)
class CGameObject;
NS_END

NS_BEGIN(Client)


class CFSM_Imp_Range final : public CStateMachine
{
private:
    CFSM_Imp_Range();
    virtual ~CFSM_Imp_Range() = default;

public:
    virtual HRESULT				Initialize(CGameObject* pOwner);
    virtual void				Update(class CGameObject* pOwner, _float fTimeDelta) override;

public:
    static CFSM_Imp_Range*      Create(CGameObject* pOwner);
    virtual void				Free() override;
};

NS_END