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
    m_Config.fIntensity = 1.f;

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
    if (false == m_isEnable)
        return;

    // 시간 누적
    m_fTimeAcc += fTimeDelta;

    // 누적 시간이 Duration을 넘어갔을 경우 비활성화, Intensity 0 초기화
    if (m_fDuration <= m_fTimeAcc)
    {
        m_isEnable = false;
        m_fTimeAcc = 0.f;
        m_Config.fIntensity = 0.f;
    }
    else
    {
        _float fRatio = m_fTimeAcc / m_fDuration;
        _float fProgress = 0.f;

        switch (m_Config.eMode)
        {
        case VIGNETTE_CONFIG::ANIMMODE::SMOOTH_SMOOTH:
            fProgress = sin(fRatio * PI);
            break;
        case VIGNETTE_CONFIG::ANIMMODE::SMOOTH_INTANT:
            fProgress = fRatio;
            break;
        case VIGNETTE_CONFIG::ANIMMODE::INTANT_SMOOTH:
            fProgress = 1.f - fRatio;
            break;
        case VIGNETTE_CONFIG::ANIMMODE::NONE:
            fProgress = m_Config.fMaxIntensity;
            break;
        }
        
        m_Config.fIntensity = m_Config.fMaxIntensity * fProgress;
    }

    // 아닐 경우 Ratio = 누적 시간 / 지속 시간
    // SS -> 0 -> 1 -> 0 -> sin(t * PI)
    // SI -> 0 -> 1 -> t
    // IS -> 1 -> 0 -> 1 - t
    // Intensity = Max Intensity * Progress
}

HRESULT CVignette::Bind_Vignette_ShaderResources(CShader* pShader)
{
    if (FAILED(pShader->Bind_RawValue("g_fVignettePower", &m_Config.fPower, sizeof(_float))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_fVignetteIntensity", &m_Config.fIntensity, sizeof(_float))))
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

void CVignette::Start_VignetteAnimation(_float fDuration, const VIGNETTE_CONFIG& Config)
{
    m_isEnable = true;
    m_fDuration = fDuration;
    m_Config = Config;
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
