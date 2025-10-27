#include "SSAO.h"
#include "GameInstance.h"

CSSAO::CSSAO(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
    , m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
    Safe_AddRef(m_pGameInstance);
}

HRESULT CSSAO::Initialize()
{
    m_Config.iNumKernels = 16;
    m_Config.fRadius = 1.f;
    m_Config.fIntensity = 1.f;
    m_Config.fConstrast = 1.f;

	if (FAILED(Ready_Kernel()))
		return E_FAIL;

	if (FAILED(Ready_NoiseTexture()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSSAO::Bind_SSAO_ShaderResources(CShader* pShader)
{
    if (FAILED(pShader->Bind_SRV("g_NoiseTexture", m_pNoiseSRV)))
        return E_FAIL;

    if (FAILED(pShader->Bind_SRV("g_Kernels", m_pKernelSRV)))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_iNumKernels", &m_Config.iNumKernels, sizeof(_uint))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_fRadius", &m_Config.fRadius, sizeof(_float))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_fIntensity", &m_Config.fIntensity, sizeof(_float))))
        return E_FAIL;

    if (FAILED(pShader->Bind_RawValue("g_fContrast", &m_Config.fConstrast, sizeof(_float))))
        return E_FAIL;

    return S_OK;
}

void CSSAO::Set_SSAOConfig(SSAO_CONFIG Config)
{
    if (m_Config.iNumKernels != Config.iNumKernels)
    {
        m_Config = Config;

        if (m_KernelBuffer)
            Safe_Release(m_KernelBuffer);
        if (m_pKernelSRV)
            Safe_Release(m_pKernelSRV);

        Ready_Kernel();
    }
    else
        m_Config = Config;
}

HRESULT CSSAO::Ready_NoiseTexture()
{
    // 회전은 4x4 작은 사이즈의 노이즈 텍스처를 만들고
    // Shader에서 노이즈 텍스처를 샘플러 point, wrap 모드 설정하고 샘플링
    // 뷰포트 사이즈를 받아서 노이즈 텍스처를 생성했던 4픽셀만큼 나누고
    // 이렇게 구한 노이즈 스케일을 텍스쿠드에 곱해서 화면 사이즈만큼 노이즈 텍스처가 반복되도록 함
    // 여기서 xyz를 가져와 랜덤 벡터로 만듦
    // 노이즈 평면 회전 정보 생성

    vector<_float3> Noise;
    Noise.reserve(m_Config.iNumKernels);

    for (_uint i = 0; i < m_Config.iNumKernels; ++i)
    {
        _float fX = m_pGameInstance->Rand(-1.f, 1.f);
        _float fY = m_pGameInstance->Rand(-1.f, 1.f);
        _float fZ = 0.f;

        _float3 vNoise = _float3(fX, fY, fZ);

        XMStoreFloat3(&vNoise, XMVector3Normalize(XMLoadFloat3(&vNoise)));

        Noise.push_back(vNoise);
    }

    // Noise Texture 생성
    ID3D11Texture2D* pTexture = { nullptr };
    D3D11_TEXTURE2D_DESC TextureDesc{};

    TextureDesc.Width = 4;
    TextureDesc.Height = 4;
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = 1;
    TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.SampleDesc.Count = 1;
    TextureDesc.Usage = D3D11_USAGE_IMMUTABLE;  // 초기화 후 변경 X
    TextureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    TextureDesc.CPUAccessFlags = 0;
    TextureDesc.MiscFlags = 0;

    D3D11_SUBRESOURCE_DATA InitialData{};
    InitialData.pSysMem = Noise.data();
    InitialData.SysMemPitch = sizeof(_float3) * TextureDesc.Width;  // 개행 바이트 수
    InitialData.SysMemSlicePitch = InitialData.SysMemPitch * TextureDesc.Height; // 총 바이트

    if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, &InitialData, &pTexture)))
        return E_FAIL;

    // Noise SRV 생성
    D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
    SRVDesc.Format = TextureDesc.Format;
    SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    SRVDesc.Texture2D.MostDetailedMip = 0;
    SRVDesc.Texture2D.MipLevels = 1;

    if (FAILED(m_pDevice->CreateShaderResourceView(pTexture, &SRVDesc, &m_pNoiseSRV)))
        return E_FAIL;

    return S_OK;
}

HRESULT CSSAO::Ready_Kernel()
{
    vector<_float3> Kernels;
    for (_uint i = 0; i < m_Config.iNumKernels; ++i)
    {
        // 랜덤 방향 벡터 생성
        _float3 vSample = _float3
        (
            m_pGameInstance->Rand(-1.f, 1.f),
            m_pGameInstance->Rand(-1.f, 1.f),
            m_pGameInstance->Rand(0.f, 1.f) // (로컬)표면 법선 앞쪽
        );

        // 정규화
        XMStoreFloat3(&vSample, XMVector3Normalize(XMLoadFloat3(&vSample)));

        // 스케일 i / KernelSize
        _float fScale = static_cast<_float>(i) / static_cast<_float>(m_Config.iNumKernels);

        // 0.1에서 1까지 Scale 제곱만큼의 비율로 보간하여 Scale 저장
        fScale = Lerp(0.1f, 1.f, fScale * fScale);

        // 위 랜덤 벡터와 스케일을 곱해서 벡터에 저장
        vSample.x *= fScale;
        vSample.y *= fScale;
        vSample.z *= fScale;

        Kernels.push_back(vSample);
    }

    D3D11_BUFFER_DESC Desc{};
    Desc.ByteWidth = sizeof(_float3) * m_Config.iNumKernels;
    Desc.StructureByteStride = sizeof(_float3);
    Desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    Desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

    D3D11_SUBRESOURCE_DATA InitialData{};
    InitialData.pSysMem = Kernels.data();

    if (FAILED(m_pDevice->CreateBuffer(&Desc, &InitialData, &m_KernelBuffer)))
        return E_FAIL;

    D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
    SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
    SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
    SRVDesc.Buffer.NumElements = m_Config.iNumKernels;

    if (FAILED(m_pDevice->CreateShaderResourceView(m_KernelBuffer, &SRVDesc, &m_pKernelSRV)))
        return E_FAIL;

    // Tool에서 샘플 개수 조정 시 샘플 개수 갱신 후 삭제 후 재생성

	return S_OK;
}

CSSAO* CSSAO::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSSAO* pInstance = new CSSAO(pDevice, pContext);

	if (FAILED(pInstance->Initialize()))
	{
		MSG_BOX(TEXT("Failed to Create : CSSAO"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSSAO::Free()
{
	__super::Free();

	Safe_Release(m_pKernelSRV);
	Safe_Release(m_KernelBuffer);
	Safe_Release(m_pNoiseSRV);

	Safe_Release(m_pGameInstance);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}
