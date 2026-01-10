#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CFog final : public CBase
{
private:
	CFog(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CFog() = default;

public:
	HRESULT						Initialize();
	void						Update(_float fTimeDelta);

public:
	HRESULT						Bind_Fog_ShaderResources(class CShader* pShader);
    void                        Set_EnableFog(_bool isEnable) { m_isEnable = isEnable; }
    FOG_CONFIG					Get_FogConfig() { return m_Config; }
	void						Set_FogConfig(FOG_CONFIG Config) { m_Config = Config; }
	_uint						Get_NumFogNoiseTextures();
	ID3D11ShaderResourceView*	Get_FogNoiseTexture(_uint iTextureIndex);
	void						Set_FogNoiseTextureIndex(_uint iTextureIndex) { m_iTextureIndex = iTextureIndex; }

	void						Start_FogTransition(_float fDuration, const FOG_TRANSITION_DESC& Desc);

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

	FOG_CONFIG					m_Config = {};
    _bool                       m_isEnable = { true };

	// Noise
	class CTexture*				m_pTextureCom = { nullptr };
	_uint						m_iTextureIndex = {};
	_float						m_fNoiseTimeAcc = {};

	// Transition
	_bool						m_isTransition = {};
	_float						m_fDuration = {};
	_float						m_fTransTimeAcc = {};
	FOG_TRANSITION_DESC			m_StartFog = {};
	FOG_TRANSITION_DESC			m_TargetFog = {};

    // Callback
    function<void()>            m_Callback = { nullptr };

private:
	HRESULT						Ready_NoiseTexture();

public:
	static CFog*				Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void				Free() override;
};

NS_END