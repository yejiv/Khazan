#pragma once

#include "Editor_Defines.h"
#include "Level.h"

NS_BEGIN(Editor)

class CLevel_Shader final : public CLevel
{
private:
	CLevel_Shader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Shader() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Update(_float fTimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT Ready_Lights();
	HRESULT Ready_Layer_Camera();
	HRESULT Ready_Layer_BackGround();
	HRESULT Ready_Layer_Player();

#ifdef _DEBUG
private:
	// Frame Check
	_char					m_szFPS[MAX_PATH] = {};
	_uint					m_iRenderCount = {};
	_float					m_fTimeAcc = {};
#endif

	// Cascade
	_float4					m_vLightDir = { 1.f, -1.f, 1.f, 0.f };

	_float					m_fShadowBias = {};
	_float					m_fShadowLamda = {};

	_bool					m_isInitShadow = {};
	_float					m_fCameraNear = { 0.1f };
	_float					m_fCameraFar = { 1000.f };
	vector<_float>			m_CascadeSplits = {};
	_uint					m_iNumCascades = {};

	// Render Flag
	_bool					m_isRenderShadow = { true };

public:
	static CLevel_Shader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END