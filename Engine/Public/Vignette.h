#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CVignette final : public CBase
{
private:
	CVignette(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CVignette() = default;

public:
	HRESULT						Initialize();
	void						Update(_float fTimeDelta);

public:
	HRESULT						Bind_Vignette_ShaderResources(class CShader* pShader);
	VIGNETTE_CONFIG				Get_VignetteConfig() { return m_Config; }
	void						Set_VignetteConfig(const VIGNETTE_CONFIG& Config) { m_Config = Config; }
	void						Set_EnableVignette(_bool isEnable, _float fIntensity) 
    { 
        m_isEnable = isEnable;
        m_fCurrentIntensity = fIntensity;
    }
	void						Start_VignetteAnimation(const VIGNETTE_CONFIG& Config, _bool isReturnOff);

private:
    HRESULT                     Ready_NoiseTexture();

private:
	VIGNETTE_CONFIG				m_Config = {};
	_bool						m_isEnable = {};
    _bool                       m_isReturnOff = {};
    _bool                       m_isTransition = {};
    _float                      m_fCurrentIntensity = {};

	// Animation
	_float						m_fTimeAcc = {};

    // Noise
    ID3D11Device*               m_pDevice = { nullptr };
    ID3D11DeviceContext*        m_pContext = { nullptr };
    class CTexture*             m_pNoiseTextureCom = { nullptr };

public:
	static CVignette*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void				Free() override;
};

NS_END