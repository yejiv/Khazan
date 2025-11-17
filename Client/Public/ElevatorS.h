#pragma once

#include "Client_Defines.h"
#include "Prop_Interactive.h"

NS_BEGIN(Client)

class CElevatorS final : public CProp_Interactive
{
private:
    enum class ELEVATOR_STATE { IDLE_UP, IDLE_DOWN, MOVE_UP, MOVE_DOWN, END };

public:
    typedef struct tagElevatorPos
    {
        _float4 vUp{};
        _float4 vDown{};

    }ELEVATOR_POS;

    typedef struct tagSmallElevatorDesc : public CProp_Interactive::PROP_INTERACTIVE_DESC
    {
        ELEVATOR_POS ElevatorPos{};

    }SMALL_ELEVATOR_DESC;

private:
    CElevatorS(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CElevatorS(const CElevatorS& Prototype);
    virtual ~CElevatorS() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize_Clone(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

    _float4 Get_Elevator_UpPos() { return m_vUpPos; }
    _float4 Get_Elevator_DownPos() { return m_vDownPos; }

    void Set_Elevator_UpPos(_float4 vUpPos) { m_vUpPos = vUpPos; }
    void Set_Elevator_DownPos(_float4 vDownPos) { m_vDownPos = vDownPos; }

    ELEVATOR_STATE m_eState = { ELEVATOR_STATE::IDLE_UP };
    _bool m_isActive = { false };

    _float4 m_vUpPos = {};
    _float4 m_vDownPos = {};

    _float m_fTimeAcc = { 0.f };

private:
    void Lerp_ElevatorMove(_float fTimeDelta, _float4 vStartPos, _float4 vTargetPos, _float fDuration);

private:
    virtual HRESULT Ready_Components(void* pArg) override;
    HRESULT Ready_PartObjects(void* pArg);

public:
    static CElevatorS* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END