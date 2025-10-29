#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CFog final : public CBase
{
private:
	CFog();
	virtual ~CFog() = default;

public:
	HRESULT			Initialize();

public:
	HRESULT			Bind_Fog_ShaderResources(class CShader* pShader);
	FOG_CONFIG		Get_FogConfig() { return m_Config; }
	void			Set_FogConfig(FOG_CONFIG Config) { m_Config = Config; }

private:
	FOG_CONFIG		m_Config = {};

public:
	static CFog*	Create();
	virtual void	Free() override;
};

NS_END