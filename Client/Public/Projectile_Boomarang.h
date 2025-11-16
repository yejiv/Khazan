#pragma once
#include "Projectile.h"
#include "Effect_Prefab.h"

NS_BEGIN(Engine)
class CBody;
NS_END

NS_BEGIN(Client)

class CProjectile_Boomarang final : public CProjectile
{
public:
    typedef struct tagBoomarangDesc : public CProjectile::PROJECTILE_DESC
    {
        class CGameObject* pTarget = { nullptr };

    }BOOMARANG_DESC;



private:
    CProjectile_Boomarang(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CProjectile_Boomarang(const CProjectile_Boomarang& Protptype);
    virtual ~CProjectile_Boomarang() = default;

public:
    virtual HRESULT				        Initialize_Prototype() override;
    virtual HRESULT				        Initialize_Clone(void* pArg) override;
    virtual void				        Priority_Update(_float fTimeDelta) override;
    virtual void				        Update(_float fTimeDelta) override;
    virtual void				        Late_Update(_float fTimeDelta) override;
    virtual HRESULT				        Render() override;
    virtual HRESULT				        Render_Shadow() { return S_OK; }


public:
    virtual void				        Reset() override;


private:
    HRESULT						        Ready_Components();
    HRESULT						        Ready_Colliders();
    HRESULT						        Bind_ShaderResources();



    virtual void				        Collision_Enter(COLLISION_DESC* pDesc, _uint	iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;
    virtual void				        Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal) override;
    virtual void				        Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer) override;


private:
    CBody*                              m_pBody = { nullptr };
    _float                              m_fReturnTime = { 2.f };  
    _float                              m_fPauseTime = { 2.f };

    CGameObject*                        m_pTarget = { nullptr };
    class CEffect_Prefab*               m_fEffect = { nullptr };

    _bool                               m_isDamageForward = { false };
    _bool                               m_isDamageReturn = { false };


public:
    static CProjectile_Boomarang*       Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject*                Clone(void* pArg) override;
    virtual void				        Free() override;
};

NS_END