#pragma once

#include "Client_Defines.h"
#include "Trigger.h"

NS_BEGIN(Engine)
class CBody;
class CTransform;
NS_END

NS_BEGIN(Client)

class CEmbars_Trigger final : public CTrigger
{
public:
    typedef struct tagEmbarsDesc : public CTrigger::TRIGGER_DESC
    {

    }TRIGGER_EMBARS_DESC;

private:
    CEmbars_Trigger(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CEmbars_Trigger(const CEmbars_Trigger& Prototype);
    virtual ~CEmbars_Trigger() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize_Clone(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

    string Get_TriggerKey() { return m_strTriggerKey; }
    void Set_TriggerKey(const string strTriggerKey) { m_strTriggerKey = strTriggerKey; }

private:
    virtual HRESULT Ready_Components(void* pArg) override;
    HRESULT Ready_Collision(void* pArg);
    HRESULT Ready_TriggerType(void* pArg);

private:
    virtual void Collision_Enter(COLLISION_DESC* pDesc, _uint	iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;
    virtual void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;
    virtual void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer) override;

private:
    //class CSequence_HeinMach_Field* m_pHeinMach_Field = { nullptr };
    //class CSequence_HeinMach_Yetuga* m_pHeinMach_Yetuga = { nullptr };
    //class CSequence_HeinMach_Start_Chat* m_pHeinMach_Start_Chat = { nullptr };
    class CClientInstance* m_pClientInstance = { nullptr };

    // FOG 구조체
    FOG_CONFIG m_FogConfig = {};
    EventVerticalGate m_EventVTGate = {};

private:
    void Set_FogConfig(FOG_CONFIG FogConfig);

public:
    static CEmbars_Trigger* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END