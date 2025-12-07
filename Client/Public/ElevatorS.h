#pragma once

#include "Client_Defines.h"
#include "Prop_Interactive.h"

NS_BEGIN(Engine)
class CBody;
NS_END

NS_BEGIN(Client)

class CElevatorS final : public CProp_Interactive
{
private:
    enum class ELEVATOR_STATE { UP, DOWN, END };

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

private:
    CBody* m_pBodyCom = { nullptr };
    CBody* m_pTriggerCom = { nullptr };

    ELEVATOR_STATE m_eState = { ELEVATOR_STATE::UP };
    _bool m_isActiveElevator = { false };               // 엘리베이터 가동중인지 아닌지
    _bool m_isAvailableSwitch = { false };              // 스위치의 애니메이션이 끝나서 상호 작용 가능해야 할 때 ( 스위치에서 다룰 것 )
    _bool m_isSwitchPressed = { false };                // 스위치가 눌렸는지 안눌렸는지

    _float4 m_vUpPos = {};
    _float4 m_vDownPos = {};

    _float m_fTimeAcc = { 0.f };

private:
    COLLISION_DESC m_TriggerCollisionDesc;

private:
    void Lerp_ElevatorMove(_float fTimeDelta, _float4 vStartPos, _float4 vTargetPos, _float fDuration);

private:
    virtual HRESULT Ready_Components(void* pArg) override;
    HRESULT Ready_PartObjects(void* pArg);
    HRESULT Ready_Collision(void* pArg);

public:
    virtual void Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc = nullptr) override;

public:
    static CElevatorS* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END