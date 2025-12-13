#include "Vignette.h"
#include "Shader.h"
#include "Texture.h"

CVignette::CVignette(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CVignette::Initialize()
{
    m_Config.vColor = _float3(0.f, 0.f, 0.f);
    m_Config.fPower = 4.f;
    m_Config.fMinIntensity = 0.f;
    m_Config.fMaxIntensity = 5.f;

    m_Config.isUseNoise = false;
    m_Config.iTextureIndex = 0;
    m_Config.fContrast = 1.f;

    if (FAILED(Ready_NoiseTexture()))
        return E_FAIL;

    return S_OK;
}

void CVignette::Update(_float fTimeDelta)
{
    if (false == m_isTransition)
        return;

    m_fTimeAcc += fTimeDelta;

    if (m_Config.fDuration <= m_fTimeAcc)
    {
        m_fCurrentIntensity = m_Config.fMaxIntensity;
        m_isTransition = false;
        if (true == m_isReturnOff)
        {
            m_isEnable = false;
            m_isReturnOff = false;
        }
        m_fTimeAcc = 0.f;
    }

    _float fIntensityRatio = 0.f;

    // Fade In
    if (m_fTimeAcc < m_Config.vFadeTime.x)
    {
        fIntensityRatio = m_fTimeAcc / m_Config.vFadeTime.x;		// 페이드 인 총 시간
        fIntensityRatio = min(1.f, fIntensityRatio);
    }
    else if (m_fTimeAcc >= m_Config.vFadeTime.x && m_fTimeAcc <= m_Config.vFadeTime.y)
    {
        fIntensityRatio = 1.f;
    }
    // Fade Out
    else
    {
        _float fFadeDuration = m_Config.fDuration - m_Config.vFadeTime.y;	// 페이드 아웃 총 시간
        _float fFadeTimeAcc = m_fTimeAcc - m_Config.vFadeTime.y;			// 페이드 아웃 시작 후 누적 시간
        _float fRatio = (fFadeTimeAcc / fFadeDuration);					    // 페이드 아웃 비율
        fIntensityRatio = 1.f - fRatio;
        fIntensityRatio = max(0.f, fIntensityRatio);						// 비율 0 -> 불투명, 비율 1 -> 투명
    }

    m_fCurrentIntensity = Lerp(m_Config.fMinIntensity, m_Config.fMaxIntensity, fIntensityRatio);
}

HRESULT CVignette::Bind_Vignette_ShaderResources(CShader* pShader)
{
    if (FAILED(pShader->Bind_RawValue("g_fVignettePower", &m_Config.fPower, sizeof(_float))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_fVignetteIntensity", &m_fCurrentIntensity, sizeof(_float))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_vVignetteColor", &m_Config.vColor, sizeof(_float3))))
        return E_FAIL;

    if (FAILED(pShader->Bind_Bool("g_isEnableVignette", &m_isEnable)))
        return E_FAIL;

    if (FAILED(pShader->Bind_Bool("g_isUseVignetteNoise", &m_Config.isUseNoise)))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_fVignetteContrast", &m_Config.fContrast, sizeof(_float))))
        return E_FAIL;

    if (FAILED(m_pNoiseTextureCom->Bind_Shader_Resource(pShader, "g_NoiseTexture", m_Config.iTextureIndex)))
        return E_FAIL;

    return S_OK;
}

void CVignette::Start_VignetteAnimation(const VIGNETTE_CONFIG& Config, _bool isReturnOff)
{
    m_isReturnOff = isReturnOff;
    m_isTransition = true;
    m_isEnable = true;
    m_Config = Config;
    m_Config.vFadeTime.y = m_Config.fDuration - m_Config.vFadeTime.y;
}

HRESULT CVignette::Ready_NoiseTexture()
{
    vector<const _tchar*> TextureTags;
    TextureTags =
    {
        TEXT("VFT_S_Tile_03A.png"),
        TEXT("VFT_S_Tile_03B.png"),
        TEXT("VFT_S_Tile_06B.png"),
        TEXT("VFT_Noise_27.png"),
    };

    m_pNoiseTextureCom = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Shader/Noise/"), TextureTags);
    if (nullptr == m_pNoiseTextureCom)
        return E_FAIL;

    return S_OK;
}

CVignette* CVignette::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CVignette* pInstance = new CVignette(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Create : CVignette"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CVignette::Free()
{
    __super::Free();

    Safe_Release(m_pNoiseTextureCom);
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);
}
