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
	class CGameInstance*    m_pGameInstance = { nullptr };
	class CClientInstance*  m_pClientInstance = { nullptr };

	// Cascade
	CASCADE_CONFIG			m_CascadeConfig = {};

	_float					m_fCameraNear = { 0.1f };
	_float					m_fCameraFar = { 6000.f };
	_uint					m_iNumCascades = {};

	_float					m_fShadowTransDuration = {};
	_float					m_fTargetShadowIntensity = {};

    // Shadow
    SHADOW_DESC             m_ShadowDesc = {};

	// Render Flag
	_bool					m_isRenderShadow = { true };
	_bool					m_isRenderSSAO = { true };
	_bool					m_isEnableFog = { true };
	_bool					m_isEnableToonShade = { true };
	_bool					m_isEnableOutline = {};
    _bool                   m_isEnableLUT = { true };
    _bool                   m_isEnableRadialBlur = {};
    _bool                   m_isEnableMotionBlur = { true };
    _bool                   m_isEnableEdge = { true };
    _bool                   m_isEnableRimLight = { true };
    _bool                   m_isEnableMotionTrail = {};

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

    // Specular
    _float2                 m_vSpecularPower = {};

    // LUT
    _float                  m_fLUTIntensity = { 0.2f };

    // Radial Blur
    RADIAL_BLUR_DESC        m_RadialBlurDesc = {};

    // Motion Blur
    MOTION_BLUR_DESC        m_MotionBlurDesc = {};

    // Rim Light
    RIM_LIGHT_DESC          m_RimLightDesc = {};

    // Motion Trail
    MOTIONTRAIL_CONFIG      m_MotionTrailConfig = {};
    _int                    m_iCurrentGameObjectIndex = { -1 };
    _float                  m_fMotionTrailDuration = {};

    // Light
    _wstring                m_strSelectedLightTag = {};

    vector<_wstring>        m_wstrLightTags;
    vector<string>          m_strLightTags;
    vector<const _char*>    m_szLightTags;
    _bool                   m_isEnableLight = {};

    LIGHT_TRANSITION_DESC   m_TargetLightDesc = {};
    _bool                   m_isRestoreLight = {};

	LEVEL                   m_eCurrentLevel = { LEVEL::TITLE };
	_bool                   m_isSelectLevel[ENUM_CLASS(LEVEL::END)];

public:
	static CShader_Controller* Create();
	virtual void Free() override;
};

NS_END