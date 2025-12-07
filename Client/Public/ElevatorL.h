#pragma once

#include "Client_Defines.h"
#include "Prop_Interactive.h"

// 회전중일때 사용하는 조명 태그 : ElevatorLight

NS_BEGIN(Client)

class CElevatorL final : public CProp_Interactive
{
private:
    enum class ANIM_STATE
    {
        ALL,                    // 1, 2, 3 다돌고
        MID_STOP,               // 2 멈추고 1, 3 돌고
        INNER_STOPPING,         // 1 멈추는 애니메이션, 3은 돌고
        NO_USE,                 // 1, 2, 3 같은 방향으로 회전하는건데 안쓰고
        OUTER_STOPPING,         // 3 슬슬 멈추는 거고 ( 루프 일단 돌리면 될 거 같고 )
        IDLE,                   // 3번도 멈추면 쓰면 되고
        END
    };

    enum class MOVE_STATE
    {
        MID,            // 회전 위치
        MIDTODOWN,      // 회전 위치 -> 아래로 내려감
        DOWN,           // 아래 위치
        DOWNTOUP,       // 아래 -> 위 올라감
        UP,             // 위 위치
        END
    };

public:
    typedef struct tagLargeElevatorPos
    {
        _float4 vUp{};
        _float4 vMid{};
        _float4 vDown{};

    }LARGE_ELEVATOR_POS;

    typedef struct tagLargeElevatorDesc : public CProp_Interactive::PROP_INTERACTIVE_DESC
    {
        LARGE_ELEVATOR_POS ElevatorPos{};

    }LARGE_ELEVATOR_DESC;

private:
    CElevatorL(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CElevatorL(const CElevatorL& Prototype);
    virtual ~CElevatorL() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize_Clone(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

    _float4 Get_Elevator_UpPos() { return m_vUpPos; }
    _float4 Get_Elevator_MidPos() { return m_vMidPos; }
    _float4 Get_Elevator_DownPos() { return m_vDownPos; }

    void Set_Elevator_UpPos(_float4 vUpPos) { m_vUpPos = vUpPos; }
    void Set_Elevator_MidPos(_float4 vMidPos) { m_vMidPos = vMidPos; }
    void Set_Elevator_DownPos(_float4 vDownPos) { m_vDownPos = vDownPos; }

private:
    class CBody* m_pBodyCom = { nullptr };
    class CBody* m_pTriggerCom = { nullptr };

private:
    EventHallElevator m_Event = {};
    EventElevatorSkip m_SkipEvent = {};

    EVENT_TYPE m_eGimmickType = { EVENT_TYPE::END };

    ANIM_STATE m_eAnimState = { ANIM_STATE::ALL };
    MOVE_STATE m_eMoveState = { MOVE_STATE::MID };

    _bool m_isActiveElevator = { false };               // 엘리베이터 가동중인지 아닌지
    _bool m_isAvailableSwitch = { false };              // 스위치의 애니메이션이 끝나서 상호 작용 가능해야 할 때 ( 스위치에서 다룰 것 )
    _bool m_isSwitchPressed = { false };                // 스위치가 눌렸는지 안눌렸는지

    _bool m_isAnimChange = { false };

    _float4 m_vUpPos = {};
    _float4 m_vMidPos = {};
    _float4 m_vDownPos = {};

    _float m_fTimeAcc = { 0.f };
    _float m_fLimitTimeAcc = { 0.f };

    _bool m_isVerticalActive = { false };
    _float m_fVerticalTimeAcc = { 0.f };

    _uint m_iEventID = { 0 };
    _uint m_iSkipEventID = { 0 };

private:
    class CEffect_Prefab* m_pEffect = { nullptr };

private:
    void Lerp_ElevatorMove(_float fTimeDelta, _float4 vStartPos, _float4 vTargetPos, _float fDuration);

private:
    virtual HRESULT Ready_Components(void* pArg) override;
    virtual HRESULT Bind_Materials(_uint iMeshIndex) override;
    HRESULT Ready_PartObjects(void* pArg);
    HRESULT Ready_Collision(void* pArg);
    HRESULT Ready_Effect();

    void Animation_Update(_float fTimeDelta);
    void Animation_Change(_float fTimeDelta);

public:
    virtual void Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc = nullptr) override;

private:
    void VerticalOnTime_Update(_float fTimeDelta);
    void Gimmick_Event_Skip(_float fTimeDelta);

public:
    static CElevatorL* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END