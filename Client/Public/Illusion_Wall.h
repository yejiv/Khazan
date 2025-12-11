#pragma once

#include "Client_Defines.h"
#include "Prop_Interactive.h"

NS_BEGIN(Engine)
class CBody;
class CTexture;
NS_END

NS_BEGIN(Client)

class CIllusion_Wall final : public CProp_Interactive
{
public:
    typedef struct tagIllusionWallDesc : public CProp_Interactive::PROP_INTERACTIVE_DESC
    {


    }ILLUSION_WALL_DESC;

private:
    CIllusion_Wall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CIllusion_Wall(const CIllusion_Wall& Prototype);
    virtual ~CIllusion_Wall() = default;

public:
    virtual HRESULT Initialize_Prototype() override;
    virtual HRESULT Initialize_Clone(void* pArg) override;
    virtual void Priority_Update(_float fTimeDelta) override;
    virtual void Update(_float fTimeDelta) override;
    virtual void Late_Update(_float fTimeDelta) override;
    virtual HRESULT Render() override;

private:
    CBody* m_pStaticCom = { nullptr };
    CBody* m_pTriggerCom = { nullptr };
    CTexture* m_pDissolveTextureCom = { nullptr };

    COLLISION_DESC m_TriggerCollisionDesc;

private:
    _float m_fTimeAcc = { 0.f };
    _float m_fDecreaseAlpha = { 0.f };

    _bool m_isRemoveWall = { false };

private:
    virtual HRESULT Ready_Components(void* pArg) override;
    HRESULT Ready_Collision(void* pArg);
    virtual HRESULT Bind_Materials(_uint iMeshIndex) override;
    HRESULT Bind_DissolveValues();

public:
    virtual void Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc = nullptr) override;

public:
    static CIllusion_Wall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg) override;
    virtual void Free() override;
};

NS_END