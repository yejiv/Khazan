#pragma once

#include "Editor_Defines.h"
#include "Level.h"
#include "MotionTrail.h"

NS_BEGIN(Editor)
class CMotionTrail;
NS_END

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

private:
	// Cascade
	CASCADE_CONFIG			m_CascadeConfig = {};

	_float					m_fCameraNear = { 0.1f };
	_float					m_fCameraFar = { 1000.f };
	_uint					m_iNumCascades = {};

	// Render Flag
	_bool					m_isRenderShadow = { false };
	_bool					m_isRenderSSAO = { true };
	_bool					m_isEnableEmissive = {};
	_bool					m_isEnableBloom = {};
	_bool					m_isEnableFog = { false };
	_bool					m_isEnableToonShade = {};
	_bool					m_isEnableOutline = {};
	_bool					m_isEnableVignette = {};
    _bool                   m_isEnableLUT = {};
    _bool                   m_isEnableRadialBlur = {};
    _bool                   m_isEnableMotionBlur = {};
    _bool                   m_isEnableEdge = { true };
    _bool                   m_isEnableRimLight = { true };
    _bool                   m_isEnableMotionTrail = {};

	// SSAO
	SSAO_CONFIG				m_SSAOConfig = {};

	// PostProcessing
	_float					m_fEmissiveIntensity = {};

	// Blur
	GAUSSIAN_BLUR_CONFIG	m_GaussianBlurConfig = {};

	// Fog
	FOG_CONFIG				m_FogConfig = {};
	_bool					m_isWorldSpaceFog = {};

	// Cartoon Rendering
	_float					m_fToonShadeLevel = { 3.f };
	OUTLINE_CONFIG			m_OutlineConfig = {};

	// Vignette
	VIGNETTE_CONFIG			m_VignetteConfig = {};
	_float					m_fVignetteAnimDuration = {};

	// Decal
	DECAL_DESC				m_DecalDesc = {};

	// Distortion
	DISTORTION_DESC			m_DistortionDesc = {};

    // LUT
    _float                  m_fLUTIntensity = { 1.f };

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

private:
	// 파라미터 ( 1. 등록할 레이어 태그 | 2. 데이터 파일 이름 | 3. 현재 로드할 레벨 | 4. 맵 타입 ( 안넣으면 폴더 내부 X ) )
	HRESULT Ready_Layer_MapObject(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap = KHAZAN_MAP::END);
	// 파라미터 ( 1. 등록할 레이어 태그 | 2. 데이터 파일 이름 | 3. 현재 로드할 레벨 | 4. 맵 타입 ( 안넣으면 폴더 내부 X ) )
	HRESULT Ready_Layer_MapObject_Inst(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap = KHAZAN_MAP::END);
	// 파라미터 ( 1. 데이터 파일 이름 | 2. 현재 로드할 레벨 | 3. 맵 타입 ( 안넣으면 폴더 내부 X ) )
	HRESULT Ready_Lights(const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap = KHAZAN_MAP::END);
    // 상호 작용 오브젝트
    HRESULT Ready_Layer_MapObject_Interactive(const _wstring& strLayerTag, const _tchar* pDataFileName, LEVEL eCurrentLevel, KHAZAN_MAP eMap = KHAZAN_MAP::END);

public:
	static CLevel_Shader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;
};

NS_END