#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CGaussianBlur final : public CBase
{
private:
	CGaussianBlur(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CGaussianBlur() = default;

public:
	HRESULT						Initialize();

public:
	HRESULT						Bind_GaussianBlur_ShaderResources(class CShader* pShader);
	GAUSSIAN_BLUR_CONFIG		Get_GaussianBlurConfig() { return m_Config; }
	void						Set_GaussianBlurConfig(GAUSSIAN_BLUR_CONFIG Config);

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };
	class CGameInstance*		m_pGameInstance = { nullptr };

	GAUSSIAN_BLUR_CONFIG		m_Config = {};
	_uint						m_iNumWeights = {};

	ID3D11Buffer*				m_pWeightBuffer = { nullptr };
	ID3D11ShaderResourceView*	m_pWeightSRV = { nullptr };

private:
	HRESULT						Ready_Weight();

public:
	static CGaussianBlur*		Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void				Free() override;
};

NS_END