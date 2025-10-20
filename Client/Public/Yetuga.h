#pragma once
#include "Monster.h"

NS_BEGIN(Client)

class CYetuga final : public CMonster
{
private:
	CYetuga(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CYetuga(const CYetuga& Prototype);
	virtual ~CYetuga() = default;

public:
	virtual HRESULT				Initialize_Prototype() override;
	virtual HRESULT				Initialize_Clone(void* pArg) override;
	virtual void				Priority_Update(_float fTimeDelta) override;
	virtual void				Update(_float fTimeDelta) override;
	virtual void				Late_Update(_float fTimeDelta) override;
	virtual HRESULT				Render() override;
	virtual HRESULT				Render_Shadow() { return S_OK; }

private:
	HRESULT						Ready_Components();

public:
	static CYetuga*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*		Clone(void* pArg) override;
	virtual void				Free() override;
};

NS_END
