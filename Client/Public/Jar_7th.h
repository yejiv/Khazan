#pragma once

#include "Client_Defines.h"
#include "DanjinJar.h"

NS_BEGIN(Client)

class CJar_7th final : public CDanjinJar
{
private:
    enum STEP_STATE {
        STEP1,
        STEP2,
        STEP3,
        STEP4,
        STEP5,
        STEP6,
        STEP7,
        STEP8,
        STEP9,
        STEP10,
        STEP11,
        STEP12,
        STEP13,
        STEP14,
        STEP15,
        STEP16
    };

private:
    CJar_7th(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CJar_7th(const CJar_7th& Prototype);
    virtual ~CJar_7th() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize_Clone(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    _uint m_iStepState = {};

private:
    virtual HRESULT Ready_Components(void* pArg) override;
    HRESULT Ready_TalkUI(void* pArg);
    virtual HRESULT Bind_Materials(_uint iMeshIndex) override;

private:
    void Animation_Update(_float fTimeDelta);
    void Animation_Change(_float fTimeDelta);

    void Update_Step(_float fTimeDelta);
    void Check_Step();
    _float4 Get_NextStepPos();

public:
    virtual void Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc = nullptr) override;

public:
    static CJar_7th* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END