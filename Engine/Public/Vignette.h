#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CVignette final : public CBase
{
private:
	CVignette();
	virtual ~CVignette() = default;

public:
	HRESULT						Initialize();
	void						Update(_float fTimeDelta);

public:
	HRESULT						Bind_Vignette_ShaderResources(class CShader* pShader);
	VIGNETTE_CONFIG				Get_VignetteConfig() { return m_Config; }
	void						Set_VignetteConfig(VIGNETTE_CONFIG Config) { m_Config = Config; }
	void						Set_EnableVignette(_bool isEnable) { m_isEnable = isEnable; }
	void						Start_VignetteAnimation(_float fDuration, VIGNETTE_CONFIG::ANIMMODE eMode);

private:
	VIGNETTE_CONFIG				m_Config = {};

	_bool						m_isEnable = {};

	// Animation
	_float						m_fTimeAcc = {};
	_float						m_fDuration = {};

public:
	static CVignette*			Create();
	virtual void				Free() override;
};

NS_END