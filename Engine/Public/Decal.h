#pragma once
#include "GameObject.h"

NS_BEGIN(Engine)

class ENGINE_DLL CDecal final : public CGameObject
{
private:
	CDecal(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDecal(const CDecal& Prototype);
	virtual ~CDecal() = default;

public:
	virtual HRESULT			Initialize_Prototype();
	virtual HRESULT			Initialize_Clone(void* pArg);
	virtual void			Priority_Update(_float fTimeDelta);
	virtual void			Update(_float fTimeDelta);
	virtual void			Late_Update(_float fTimeDelta);
	virtual HRESULT			Render();
	virtual void			Reset() override;

public:
	_float					Get_Opacity() { return m_fOpacity; }
	_float					Get_LifeRatio() { return m_fTimeAcc / m_fLifeTime; }

private:
	_float					m_fLifeTime = { 5.f };
	_float					m_fTimeAcc = {};
	_float					m_fOpacity = {};
	_float					m_fFadeStartTime = { 4.f };

public:
	static CDecal*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject*	Clone(void* pArg) override;
	virtual void			Free() override;
};

NS_END