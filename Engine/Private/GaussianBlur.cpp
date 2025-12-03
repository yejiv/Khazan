#include "GaussianBlur.h"
#include "GameInstance.h"

CGaussianBlur::CGaussianBlur(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
    , m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CGaussianBlur::Initialize()
{
    // Default
    //  m_Config.fSigma = 2.5f;
    //  m_Config.iRadius = 6;
    //  m_Config.fNormalization = 7.5f;

    // Before DownSampling
    //  m_Config.fSigma = 10.f;
    //  m_Config.iRadius = 12;
    //  m_Config.fNormalization = 15.f;

    // 가중치 밀집도
    m_Config.fSigma = 2.5f;

    // 중앙 픽셀 기준 범위
    m_Config.iRadius = 4;

    // 정규화 해주기 위한 수치
    m_Config.fNormalization = 4.5f;

    if (FAILED(Ready_Weight()))
        return E_FAIL;

    return S_OK;
}

HRESULT CGaussianBlur::Bind_GaussianBlur_ShaderResources(class CShader* pShader)
{
    if (FAILED(pShader->Bind_SRV("g_Weights", m_pWeightSRV)))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_fBlurNormalization", &m_Config.fNormalization, sizeof(_float))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_iWeightRadius", &m_Config.iRadius, sizeof(_int))))
        return E_FAIL;

    return S_OK;
}

void CGaussianBlur::Set_GaussianBlurConfig(GAUSSIAN_BLUR_CONFIG Config)
{
    m_Config = Config;

    if (m_pWeightBuffer)
        Safe_Release(m_pWeightBuffer);
    if (m_pWeightSRV)
        Safe_Release(m_pWeightSRV);

    Ready_Weight();
}

HRESULT CGaussianBlur::Ready_Weight()
{
    m_iNumWeights = 2 * m_Config.iRadius + 1;
    vector<_float> Weights;
    Weights.resize(m_iNumWeights);

    _float fTwoSigmaSq = 2.f * m_Config.fSigma * m_Config.fSigma;

    for (_uint i = 0; i < m_iNumWeights; ++i)
    {
        _float fDistance = abs(static_cast<_float>(i) - static_cast<_float>(m_Config.iRadius));
        _float fWeight = exp(-(static_cast<_float>(fDistance * fDistance) / fTwoSigmaSq));

        Weights[i] = fWeight;
    }

    D3D11_BUFFER_DESC Desc{};
    Desc.ByteWidth = sizeof(_float) * m_iNumWeights;
    Desc.StructureByteStride = sizeof(_float);
    Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

    D3D11_SUBRESOURCE_DATA InitialData{};
    InitialData.pSysMem = Weights.data();

    if (FAILED(m_pDevice->CreateBuffer(&Desc, &InitialData, &m_pWeightBuffer)))
        return E_FAIL;

    D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
    SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
    SRVDesc.Buffer.NumElements = m_iNumWeights;

    if (FAILED(m_pDevice->CreateShaderResourceView(m_pWeightBuffer, &SRVDesc, &m_pWeightSRV)))
        return E_FAIL;

    return S_OK;
}

CGaussianBlur* CGaussianBlur::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CGaussianBlur* pInstance = new CGaussianBlur(pDevice, pContext);

    if (FAILED(pInstance->Initialize()))
    {
        MSG_BOX(TEXT("Failed to Create : CGaussianBlur"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CGaussianBlur::Free()
{
    __super::Free();

    Safe_Release(m_pWeightSRV);
    Safe_Release(m_pWeightBuffer);

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pContext);
    Safe_Release(m_pDevice);
}
