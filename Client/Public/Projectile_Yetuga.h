#pragma once
#include "Projectile.h"

NS_BEGIN(Client)

class CProjectile_Yetuga final : public CProjectile
{
	CProjectile_Yetuga(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CProjectile_Yetuga(const CProjectile_Yetuga& Protptype);
	virtual ~CProjectile_Yetuga() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize_Clone(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;
	virtual HRESULT				Render_Shadow() { return S_OK; }


public:
	virtual void				Reset() override;

private:
	HRESULT						Ready_Components();
	HRESULT						Bind_ShaderResources();


public:
	static CProjectile_Yetuga*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

NS_END