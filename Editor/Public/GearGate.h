#pragma once

#include "Editor_Defines.h"
#include "Prop_Interactive.h"

NS_BEGIN(Editor)

class CGearGate final : public CProp_Interactive
{
private:
    enum class ANIM_STATE { ACTIVATION, IDLE1, IDLE2, END };

    enum PART { L, R, PART_END };

public:
    typedef struct tagDoorGearEventID
    {
        _int iLeftEventID{};
        _int iRightEventID{};

    }DOOR_GEAR_EVENTID;

    typedef struct tagGearGateDesc : public CProp_Interactive::PROP_INTERACTIVE_DESC
    {
        DOOR_GEAR_EVENTID GearEventID{};

    }GEARGATE_DESC;

    // 플레이어 포지션 Position_Ch or IA_BeginLoc_Open

private:
    CGearGate(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CGearGate(const CGearGate& Prototype);
    virtual ~CGearGate() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize_Clone(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

    DOOR_GEAR_EVENTID Get_DoorGear_EventID();
    void Set_DoorGear_EventID(_int iLeftEventID, _int iRightEventID);

private:
    ANIM_STATE m_eAnimState = { ANIM_STATE::IDLE1 };
    _bool m_isUnLock[PART_END] = { false };

    class CDoor_Gear* m_pLeftDoor_Gear = { nullptr };
    class CDoor_Gear* m_pRightDoor_Gear = { nullptr };

    _float m_fTimeAcc = { 0.f };

private:
    virtual HRESULT Ready_Components(void* pArg) override;
    HRESULT Ready_PartObjects(void* pArg);

public:
    static CGearGate* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END