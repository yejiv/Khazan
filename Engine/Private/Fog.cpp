#include "Fog.h"
#include "GameInstance.h"

CFog::CFog(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CFog::Initialize()
{
    m_Config.eType = FOG_CONFIG::EXP;
    m_Config.fNear = 0.1f;
    m_Config.fFar = 100.f;
    m_Config.fDensity = 0.05f;
    m_Config.vColor = { 0.f, 0.106f, 0.137f, 1.f };
    m_Config.Noise.vSpeed = { 0.05f, 0.f };
    m_Config.Noise.vScale = { 0.05f, 0.05f };
    m_Config.Noise.fStrength = 0.5f;
    m_Config.Noise.fContrast = 1.f;
    m_Config.Noise.isEnable = false;
    m_Config.fBaseHeight = 63.5f;
    m_Config.fHeightDensity = 0.001f;

    if (FAILED(Ready_NoiseTexture()))
        return E_FAIL;

	return S_OK;
}

void CFog::Update(_float fTimeDelta)
{
    m_fNoiseTimeAcc += fTimeDelta;

    if (false == m_isTransition)
        return;

    // Density, Color 보간
    m_fTransTimeAcc += fTimeDelta;

	_float fRatio = m_fTransTimeAcc / m_fDuration;
	if (fRatio >= 1.f)
	{
		fRatio = 1.f;
		m_isTransition = false;
	}

    m_Config.fDensity = Lerp(m_StartFog.fDensity, m_TargetFog.fDensity, fRatio);
    XMStoreFloat4(&m_Config.vColor, XMVectorLerp(XMLoadFloat4(&m_StartFog.vColor), XMLoadFloat4(&m_TargetFog.vColor), fRatio));
}

HRESULT CFog::Bind_Fog_ShaderResources(CShader* pShader)
{
    _uint iFogMode = static_cast<_uint>(m_Config.eType);
    if (FAILED(pShader->Bind_RawValue("g_iFogMode", &iFogMode, sizeof(_uint))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_fFogDensity", &m_Config.fDensity, sizeof(_float))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_fFogNear", &m_Config.fNear, sizeof(_float))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_fFogFar", &m_Config.fFar, sizeof(_float))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_vFogColor", &m_Config.vColor, sizeof(_float4))))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(pShader, "g_NoiseTexture", m_iTextureIndex)))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_fTimeDelta", &m_fNoiseTimeAcc, sizeof(_float))))
        return E_FAIL;

    if (FAILED(pShader->Bind_Bool("g_isEnableNoise", &m_Config.Noise.isEnable)))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_vNoiseSpeed", &m_Config.Noise.vSpeed, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_vNoiseScale", &m_Config.Noise.vScale, sizeof(_float2))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_fNoiseStrength", &m_Config.Noise.fStrength, sizeof(_float))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_fNoiseContrast", &m_Config.Noise.fContrast, sizeof(_float))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_fFogBaseHeight", &m_Config.fBaseHeight, sizeof(_float))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_fFogHeightDensity", &m_Config.fHeightDensity, sizeof(_float))))
        return E_FAIL;

    if (FAILED(pShader->Bind_Bool("g_isWorldFog", &m_isWorldFog)))
        return E_FAIL;

	return S_OK;
}

_uint CFog::Get_NumFogNoiseTextures()
{
    return m_pTextureCom->Get_NumTextures();
}

ID3D11ShaderResourceView* CFog::Get_FogNoiseTexture(_uint iTextureIndex)
{
    return m_pTextureCom->Get_Texture(iTextureIndex);
}

void CFog::Start_FogTransition(_float fDuration, const FOG_TRANSITION_DESC& Desc)
{
	m_isTransition = true;
    m_fTransTimeAcc = 0.f;
	m_fDuration = fDuration;
	m_TargetFog = Desc;
	m_StartFog.fDensity = m_Config.fDensity;
	m_StartFog.vColor = m_Config.vColor;
}

HRESULT CFog::Ready_NoiseTexture()
{
    vector<const _tchar*> TextureTags;
    TextureTags =
    {
        TEXT("FT_Cloud.png"),
        TEXT("FT_Cloud_001.png"),
        TEXT("FT_Cloud_01.png"),
        TEXT("FT_Cloud_002.png"),
        TEXT("FT_Ele_Noise.png"),
        TEXT("FT_Mask_Smoke_001.png"),
        TEXT("FT_Mask_Smoke_02.png"),
        TEXT("FT_Mask_Smoke_2.png"),
        TEXT("FT_Noise.png"),
        TEXT("FT_Noise_001.png"),
        TEXT("FT_Noise_001_LSY.png"),
        TEXT("FT_Noise_002.png"),
        TEXT("FT_Noise_011.png"),
        TEXT("FT_Noise_016.png"),
        TEXT("FT_Noise_017.png"),
        TEXT("FT_Snow_Noise_002.png"),
        TEXT("FT_Soft_Noise.png"),
        TEXT("FT_Tile_Smoke_01.png"),
        TEXT("FT_World_Common_Noise_01.png"),
        TEXT("T_TX_Cloud_01.png"),
        TEXT("T_TX_Cloud_02.png"),
    };

    m_pTextureCom = CTexture::Create(m_pDevice, m_pContext, TEXT("../../Client/Bin/Resources/Shader/Noise/"), TextureTags);
    if (nullptr == m_pTextureCom)
        return E_FAIL;

    return S_OK;
}

CFog* CFog::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CFog* pInstance = new CFog(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Create : CFog"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CFog::Free()
{
    __super::Free();

    Safe_Release(m_pTextureCom);
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);
}
