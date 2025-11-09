#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CDistortion final : public CBase
{
private:
	CDistortion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CDistortion() = default;

public:
	HRESULT						Initialize();
	void						Update(_float fTimeDelta);

public:
	HRESULT						Bind_Distortion_ShaderResources(class CShader* pShader);
	void						Start_Distortion(const DISTORTION_DESC& Desc);

	DISTORTION_DESC				Get_DistortionDesc() { return m_Desc; }
	_uint						Get_NumDistortionNoiseTextures();
	ID3D11ShaderResourceView*	Get_DistortionNoiseTexture(_uint iTextureIndex);

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

	DISTORTION_DESC				m_Desc = {};

	_float						m_fTimeAcc = {};
	_bool						m_isEnable = {};
	_float						m_fTargetPower = {};

	// Noise Textures
	class CTexture*				m_pTextureCom = { nullptr };

private:
	HRESULT						Ready_NoiseTexture();

public:
	static CDistortion*			Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void				Free() override;
};

NS_END