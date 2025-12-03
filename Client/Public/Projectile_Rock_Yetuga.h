#pragma once
#include "Projectile.h"

NS_BEGIN(Engine)
class CBody;
NS_END


NS_BEGIN(Client)

class CProjectile_Rock_Yetuga final : public CProjectile
{
	CProjectile_Rock_Yetuga(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProjectile_Rock_Yetuga(const CProjectile_Rock_Yetuga& Protptype);
	virtual ~CProjectile_Rock_Yetuga() = default;

public:
	virtual HRESULT						Initialize_Prototype() override;
	virtual HRESULT						Initialize_Clone(void* pArg) override;
	virtual void						Priority_Update(_float fTimeDelta) override;
	virtual void						Update(_float fTimeDelta) override;
	virtual void						Late_Update(_float fTimeDelta) override;
	virtual HRESULT						Render() override;
	virtual HRESULT						Render_Shadow() { return S_OK; }

public:
	virtual void						Reset() override;

public:
    virtual void Collision_Enter(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Stay(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, _float3 vContactPoint, _float3 ContactNormal, COLLISION_DESC* pMyDesc = nullptr) override;
    virtual void Collision_Exit(COLLISION_DESC* pDesc, _uint iOtherObjectLayer, COLLISION_DESC* pMyDesc = nullptr) override;


private:
	HRESULT								Ready_Components();
    HRESULT						        Ready_Colliders();
	HRESULT								Bind_ShaderResources();

private:
    CBody*                              m_pBody = { nullptr };

    _bool						        m_isPicked = { false };


public:
	static	CProjectile_Rock_Yetuga*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*				Clone(void* pArg) override;
	virtual void						Free() override;
};

NS_END
