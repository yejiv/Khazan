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

    // ===== Lighting Calculation =====
    // Lit / Unlit Mode
    // Specular
    // RimLight
        // ===== Cartoon Rendering =====
        // ToonShade
        // Edge, Shade
    
    // ===== Light Transition =====
    // Light Transition

    // ===== Ambient Occlusion =====
    // SSAO
    
    // ===== Cascade Shadow =====
    // Shadow
    
    // ===== Post Processing =====
    // Fog
    // Bloom + GaussianBlur Setting
    // MotionBlur
    // LUT

    // ===== VFX =====
    // Motion Trail
    // Decal
    // RadialBlur
    // Distortion
    // Vignette

    // 기타
    // 하늘 / 구름 세팅
    // 메쉬 트레일 세팅

    // 추가해야 될 것
    // 블룸 가우시안 블러 세팅
    // 셰도우 PCF ON / OFF

private:
    void Lighting_Calculation_Settings();
    void Cartoon_Rendering_Settings();
    void SSAO_Settings();
    void Shadow_Settings();

    void Fog_Settings();
    void MotionBlur_Settings();

    void MotionTrail_Settings();
    void Decal_Settings();
    void RadialBlur_Settings();
    void Distortion_Settings();
    void Vignette_Settings();
    
    void Light_Settings();
    void Light_Transition_Settings();

    void MeshTrail_Settings();
    void Environment_Settings();

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
    _bool					m_isEnableVignette = {};
    _bool                   m_isUnlitMode = {};
    _bool                   m_isLitMode = {};
    _bool                   m_isEnableSpecular = {};
    _bool                   m_isEnableBloom = { true };
    _bool                   m_isEnableDistortionSettings = {};
    _bool                   m_isEnableShadowPCF = { true };

	// SSAO
	SSAO_CONFIG				m_SSAOConfig = {};

	// Fog
	FOG_CONFIG				m_FogConfig = {};
	_bool					m_isWorldSpaceFog = {};
	_float					m_fFogTransDuration = {};	
	FOG_TRANSITION_DESC		m_TargetFogDesc = {};
    FOG_CONFIG              m_InitFogConfig = {};

    // Vignette
    VIGNETTE_CONFIG			m_VignetteConfig = {};
    _float                  m_fVignetteIntensity = {};
    _bool                   m_isVignetteReturnOff = {};

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

    // Effect Trail
    _int                    m_iTrailOwnerIndex = { -1 };
    TRAIL_CONFIG            m_TrailConfig = {};

    // Sky, Cloud
    SKY_DESC                m_SkyDesc = {};
    CLOUD_DESC              m_CloudDesc = {};
    _float                  m_fSkyCloudDuration = {};

	LEVEL                   m_eCurrentLevel = { LEVEL::TITLE };
	_bool                   m_isSelectLevel[ENUM_CLASS(LEVEL::END)];

    // Decal
    DECAL_DESC				m_DecalDesc = {};

    // Distortion
    DISTORTION_DESC			m_DistortionDesc = {};

    // Blur
    GAUSSIAN_BLUR_CONFIG	m_GaussianBlurConfig = {};

public:
	static CShader_Controller* Create();
	virtual void Free() override;
};

NS_END