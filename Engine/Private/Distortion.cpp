#include "Distortion.h"
#include "GameInstance.h"

CDistortion::CDistortion(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CDistortion::Initialize()
{
	if (FAILED(Ready_NoiseTexture()))
		return E_FAIL;

    m_Desc.vCenter = { 0.f, 0.f, 0.f };
    m_Desc.fRange = 1.f;
    m_Desc.fPower = 0.05f;
    m_Desc.fDuration = 1.f;
	m_Desc.vFadeTime = { 0.2f, 0.2f };
    m_Desc.fSpeed = 1.f;

	return S_OK;
}

void CDistortion::Update(_float fTimeDelta)
{
	if (false == m_isEnable)
		return;

    m_fTimeAcc += fTimeDelta;

    if (m_Desc.fDuration <= m_fTimeAcc)
    {
        m_isEnable = false;
        m_fTimeAcc = 0.f;
		m_fTargetPower = 0.f;
    }

	_float fPowerRatio = 0.f;

    // 페이드 아웃 계산
    if (m_fTimeAcc > m_Desc.vFadeTime.y)
    {
        _float fFadeDuration = m_Desc.fDuration - m_Desc.vFadeTime.y;	// 페이드 아웃 총 시간
        _float fFadeTimeAcc = m_fTimeAcc - m_Desc.vFadeTime.y;			// 페이드 아웃 시작 후 누적 시간
        _float fRatio = (fFadeTimeAcc / fFadeDuration);					// 페이드 아웃 비율
        fPowerRatio = 1.f - fRatio;
        fPowerRatio = max(0.f, fPowerRatio);							// 비율 0 -> 불투명, 비율 1 -> 투명
    }

    // Fade In
    if (m_fTimeAcc < m_Desc.vFadeTime.x)
    {
        fPowerRatio = m_fTimeAcc / m_Desc.vFadeTime.x;		// 페이드 인 총 시간
        fPowerRatio = min(1.f, fPowerRatio);
    }

	m_Desc.fPower = m_fTargetPower * fPowerRatio;
}

HRESULT CDistortion::Bind_Distortion_ShaderResources(class CShader* pShader)
{
    if (FAILED(m_pTextureCom->Bind_Shader_Resource(pShader, "g_NoiseTexture", m_Desc.iNoiseIndex)))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_fDistortionPower", &m_Desc.fPower, sizeof(_float))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_fDistortionSpeed", &m_Desc.fSpeed, sizeof(_float))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_fTimeDelta", &m_fTimeAcc, sizeof(_float))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_fDistortionRange", &m_Desc.fRange, sizeof(_float))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_vWorldCenterPos", &m_Desc.vCenter, sizeof(_float3))))
        return E_FAIL;

    if (FAILED(pShader->Bind_Bool("g_isEnableDistortion", &m_isEnable)))
        return E_FAIL;

	return S_OK;
}

void CDistortion::Start_Distortion(const DISTORTION_DESC& Desc)
{
	m_isEnable = true;

	m_Desc = Desc;
	m_fTargetPower = Desc.fPower;
}

_uint CDistortion::Get_NumDistortionNoiseTextures()
{
    return m_pTextureCom->Get_NumTextures();
}

ID3D11ShaderResourceView* CDistortion::Get_DistortionNoiseTexture(_uint iTextureIndex)
{
    return m_pTextureCom->Get_Texture(iTextureIndex);
}

HRESULT CDistortion::Ready_NoiseTexture()
{
    // Distortion Test
    vector<const _tchar*> TextureTags;
    TextureTags =
    {
        TEXT("FT_Ring_01_n.png"),
        TEXT("FT_2Ch_Noise_001.png"),
        TEXT("FT_2ch_Tile_01.png"),
        TEXT("FT_Colormap_002.png"),
        TEXT("FT_ColormapMarbling_001.png"),
        TEXT("FT_Ele_Noise.png"),
        TEXT("FT_Fire_Noise.png"),
        TEXT("FT_Flow_07.png"),
        TEXT("FT_FlowRGB_001.png"),
        TEXT("FT_Noise_3ch_001.png"),
        TEXT("FT_Noise_007.png"),
        TEXT("FT_Noise_Beam_002.png"),
        TEXT("FT_Noise_Beam_003.png"),
        TEXT("FT_Noise_RG_Fire_001.png"),
        TEXT("FT_Noise_Soft_001.png"),
        TEXT("FT_NoiseStreakyColorfulBlurred.png"),
        TEXT("FT_PerlinNoise_RG.png"),
        TEXT("FT_PrismTex_01.png"),
        TEXT("FT_Sparkle_Noise_002.png"),
    };

    m_pTextureCom = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Shader/Noise/"), TextureTags);
    if (nullptr == m_pTextureCom)
        return E_FAIL;

	return S_OK;
}

CDistortion* CDistortion::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CDistortion* pInstance = new CDistortion(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Create : CDistortion"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDistortion::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);
}
