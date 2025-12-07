#pragma once

#include "Client_Defines.h"
#include "Prop_Interactive.h"

NS_BEGIN(Client)

class CStatue final : public CProp_Interactive
{
private:
    enum class ANIM_STATE { IDLE_0, ROTATE_180, IDLE_180, ROTATE_270, IDLE_270, ROTATE_360, ROTATE_90, IDLE_90 };

    enum class STATUE_TRIGGER { FRONT, RIGHT, BACK, LEFT, END };
    // Position_Ch_F, _R, _B, _L

    // 0번 이벤트 ID ( 첫번째 퍼즐 ) | 1번 이벤트 ID ( 두번째 퍼즐 )

public:
    typedef struct tagStatueRotation
    {
        // 0 : 0도 | 1 : 90도 | 2 : 180도 | 3 : 270도
        _int iUnLockRotation = {};

    }STATUE_ROTATION;

    typedef struct tagStatueDesc : public CProp_Interactive::PROP_INTERACTIVE_DESC
    {
        STATUE_ROTATION StatueRotation{};

        _uint iStatueIndex{};

    }STATUE_DESC;

private:
    CStatue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CStatue(const CStatue& Prototype);
    virtual ~CStatue() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize_Clone(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

public:
    _int Get_StatueRotation() { return m_iRotation; }
    _int Get_StatueUnLockRotation() { return m_iUnLockRotation; }
    void Set_StatueUnLockRotation(_int iUnLockRotation) { m_iUnLockRotation = iUnLockRotation; }

private:
    CBody* m_pStaticCom = { nullptr };
    CBody* m_pTriggerCom[ENUM_CLASS(STATUE_TRIGGER::END)] = {nullptr};

    class CInteraction_Guide* m_pGuide = { nullptr };

private:
    ANIM_STATE m_eAnimState = { ANIM_STATE::IDLE_0 };

    EventObject m_Event = {};

    EventGimmick m_EventGimmick = {};

    _float3 m_vCharacterPosition = {};

    _int m_iUnLockRotation = {};
    _int m_iRotation = {};

    _uint m_iStatueIndex = {};

    _float m_fColTimeAcc = { 0.f };

    _uint m_iObjectInteractEventID = { 0 };

    _uint m_iEventID = { 0 };

    _bool m_isLateInit = { false };

private:
    class CEffect_Prefab* m_pEffect = { nullptr };
    _bool   m_bPrevState;

private:
    COLLISION_DESC m_TriggerCollisionDesc;

private:
    virtual HRESULT Ready_Components(void* pArg) override;
    HRESULT Ready_PartObjects(void* pArg);
    HRESULT Ready_Collision(void* pArg);
    HRESULT Ready_Interaction_Guide(void* pArg);
    HRESULT Ready_Effect();

    void Check_Solved();

    void Input_Interact_Event(_float fTimeDelta);
    void Animation_Update(_float fTimeDelta);
    void Animation_Change(_float fTimeDelta);

public:
    virtual void Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc = nullptr) override;

    _matrix Calculate_BonePosition_MinLength(_float4 vPlayerPosition);
    
    _bool isIdleState() { return ANIM_STATE::IDLE_0 == m_eAnimState   || ANIM_STATE::IDLE_90 == m_eAnimState ||
                                 ANIM_STATE::IDLE_180 == m_eAnimState || ANIM_STATE::IDLE_270 == m_eAnimState; }

    _bool isRotateState() { return ANIM_STATE::ROTATE_90 == m_eAnimState  || ANIM_STATE::ROTATE_180 == m_eAnimState ||
                                   ANIM_STATE::ROTATE_270 == m_eAnimState || ANIM_STATE::ROTATE_360 == m_eAnimState;
    }

public:
    static CStatue* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END