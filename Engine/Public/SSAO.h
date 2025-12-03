#pragma once
#include "Base.h"

/* Screen Space Ambient Occlusion */

NS_BEGIN(Engine)

class CSSAO final : public CBase
{
private:
	CSSAO(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CSSAO() = default;

public:
	HRESULT						Initialize();

public:
	HRESULT						Bind_SSAO_ShaderResources(class CShader* pShader);

public:
	SSAO_CONFIG					Get_SSAOConfig() { return m_Config; }
	void						Set_SSAOConfig(SSAO_CONFIG Config);

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };

	ID3D11ShaderResourceView*	m_pNoiseSRV = { nullptr };
	ID3D11Buffer*				m_KernelBuffer = { nullptr };
	ID3D11ShaderResourceView*	m_pKernelSRV = { nullptr };

private:
	SSAO_CONFIG					m_Config = {};

private:
	HRESULT						Ready_NoiseTexture();
	HRESULT						Ready_Kernel();

public:
	static CSSAO*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void				Free() override;
};

NS_END