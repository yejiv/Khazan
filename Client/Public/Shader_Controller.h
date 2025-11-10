#pragma once
#include "Client_Defines.h"
#include "Base.h"

NS_BEGIN(Client)

class CShader_Controller final : public CBase
{
private:
	CShader_Controller();
	virtual ~CShader_Controller() = default;

public:
	HRESULT Initialize();
	void Update(_float fTimeDelta);

private:
	void Ready_Level();
	void Ready_Shader();
	

private:
	class CGameInstance* m_pGameInstance = { nullptr };
	class CClientInstance* m_pClientInstance = { nullptr };

	// Cascade
	CASCADE_CONFIG			m_CascadeConfig = {};

	_float					m_fCameraNear = { 0.1f };
	_float					m_fCameraFar = { 6000.f };
	_uint					m_iNumCascades = {};

	_float					m_fShadowTransDuration = {};
	_float					m_fTargetShadowIntensity = {};

	// Render Flag
	_bool					m_isRenderShadow = { true };
	_bool					m_isRenderSSAO = { true };
	_bool					m_isEnableFog = { true };
	_bool					m_isEnableToonShade = {};
	_bool					m_isEnableOutline = {};

	// SSAO
	SSAO_CONFIG				m_SSAOConfig = {};

	// Fog
	FOG_CONFIG				m_FogConfig = {};
	_bool					m_isWorldSpaceFog = {};
	_float					m_fFogTransDuration = {};	
	FOG_TRANSITION_DESC		m_TargetFogDesc = {};
    FOG_CONFIG              m_InitFogConfig = {};

	// Cartoon Rendering
	_float					m_fToonShadeLevel = { 3.f };
	OUTLINE_CONFIG			m_OutlineConfig = {};

	LEVEL m_eCurrentLevel = { LEVEL::TITLE };
	_bool m_isSelectLevel[ENUM_CLASS(LEVEL::END)];

public:
	static CShader_Controller* Create();
	virtual void Free() override;
};

NS_END