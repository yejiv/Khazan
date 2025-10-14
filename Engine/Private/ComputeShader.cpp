#include "ComputeShader.h"

CComputeShader::CComputeShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice{ pDevice }
	, m_pContext{ pContext }
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}


HRESULT CComputeShader::Initialize(const _tchar* pShaderFilePath, const _char* pEntryPoint)
{
    // Binary large object : 단순 메모리 덩어리
    // ID3DBlob는 셰이더 코드나 컴파일 에러 메세지등을 담음
    // D3DCompileFromFile()로 hlsl을 컴파일하면 결과 바이트 코드가  ID3DBlob에 들어감 -> 이후 CreateComputeShader()를 통해 GPU용 셰이더객체를 생성
    // 즉, ID3DBlob는 컴파일된 Compute Shader 코드를 담는 임시 저장소

    ID3DBlob* pCSBlob = { nullptr };

#ifdef _DEBUG
    ID3DBlob* pErrorBlob = { nullptr };

    if (FAILED(Check_ShaderFileExists(pShaderFilePath, pEntryPoint)))
        return E_FAIL;

    HRESULT hr = D3DCompileFromFile(
        pShaderFilePath,                                      // HLSL 파일 경로
        nullptr,                                            // #define 매크로 설정 (NULL 가능)
        D3D_COMPILE_STANDARD_FILE_INCLUDE,                  // include 처리 (보통 D3D_COMPILE_STANDARD_FILE_INCLUDE)
        pEntryPoint,                                                // HLSL 진입점 함수명 (예: "CSMain")
        "cs_5_0",                                           // 셰이더 모델 (예: "cs_5_0")
        D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_DEBUG,    // 컴파일 옵션 (디버그, 최적화 등)
        0,                                                  // 효과용?? (거의 0)
        &pCSBlob,                                            // 컴파일 성공 시 바이트코드 출력
        &pErrorBlob                                          // 실패 시 에러 메시지 출력
    );

    if (FAILED(hr))
    {
        if (pErrorBlob)
        {
            OutputDebugStringA("Shader Compile Error:\n");
            OutputDebugStringA(reinterpret_cast<_char*>(pErrorBlob->GetBufferPointer()));
            OutputDebugStringA("\n");
            Safe_Release(pErrorBlob);
        }
        else
        {
            OutputDebugStringA("!! Unknown error during shader compilation. !! \n");
        }
        return hr;
    }

    Safe_Release(pErrorBlob);
    OutputDebugStringA("[CComputeShader::Initialize] Compile SUCCESS!\n");

#endif // _DEBUG

#ifndef _DEBUG
    if (FAILED(D3DCompileFromFile(
        pShaderFilePath,
        nullptr, nullptr,
        pEntryPoint, "cs_5_0",
        D3DCOMPILE_ENABLE_STRICTNESS, 0,
        &pCSBlob, nullptr)))
    {
        MSG_BOX(TEXT("Compute Shader Compile Error"));
        return E_FAIL;
    }
#endif // _DEBUG

    if (FAILED(m_pDevice->CreateComputeShader(
        pCSBlob->GetBufferPointer(),    // 컴파일된 셰이더 바이트코드
        pCSBlob->GetBufferSize(),        // 코드 크기
        nullptr,                         // 동적 클래스 링크 (거의 NULL)
        &m_pShader)))                   // 출력: 실제 ComputeShader 객체
    {
        MSG_BOX(TEXT("Failed to Create Compute Shader Object"));
        Safe_Release(pCSBlob);
        return E_FAIL;
    }

    Safe_Release(pCSBlob);

	return S_OK;
}

void CComputeShader::Execute(const COMPUTE_PASS_DESC& Desc)
{
    // 셰이더 바인딩
    m_pContext->CSSetShader(m_pShader, nullptr, 0);

    // Constant Buffer 바인딩
    if (!Desc.ConstantBuffers.empty())
        m_pContext->CSSetConstantBuffers(0, static_cast<_uint>(Desc.ConstantBuffers.size()), Desc.ConstantBuffers.data());

    // SRV 바인딩
    if (!Desc.SRVs.empty())
        m_pContext->CSSetShaderResources(0, static_cast<_uint>(Desc.SRVs.size()), Desc.SRVs.data());

    // UAV 바인딩
    if (!Desc.UAVs.empty())
        m_pContext->CSSetUnorderedAccessViews(0, static_cast<_uint>(Desc.UAVs.size()), Desc.UAVs.data(), 
            Desc.UAVInitialCounts.empty() ? nullptr : Desc.UAVInitialCounts.data());

    // 실행
    m_pContext->Dispatch(Desc.x, Desc.y, Desc.z);

    // UAV 언바인딩
    if (!Desc.UAVs.empty())
    {
        vector<ID3D11UnorderedAccessView*> nullUAVs(Desc.UAVs.size(), nullptr);
        m_pContext->CSSetUnorderedAccessViews(0, static_cast<_uint>(nullUAVs.size()), nullUAVs.data(), 
            Desc.UAVInitialCounts.empty() ? nullptr : Desc.UAVInitialCounts.data());
    }

    // SRV 언바인딩
    if (!Desc.SRVs.empty())
    {
        vector<ID3D11ShaderResourceView*> nullSRVs(Desc.SRVs.size(), nullptr);
        m_pContext->CSSetShaderResources(0, static_cast<_uint>(nullSRVs.size()), nullSRVs.data());
    }

    // Constant Buffer 언바인딩
    if (!Desc.ConstantBuffers.empty())
    {
        vector<ID3D11Buffer*> nullCBs(Desc.ConstantBuffers.size(), nullptr);
        m_pContext->CSSetConstantBuffers(0, static_cast<_uint>(nullCBs.size()), nullCBs.data());
    }

    // 컴퓨트 셰이더 언바인딩
    m_pContext->CSSetShader(nullptr, nullptr, 0);
}

HRESULT CComputeShader::Check_ShaderFileExists(const _tchar* pShaderFilePath, const _char* pEntryPoint)
{
    // 파일 경로 확인
    _char szPathBuffer[512];
    WideCharToMultiByte(CP_ACP, 0, pShaderFilePath, -1, szPathBuffer, 512, NULL, NULL);
    OutputDebugStringA("=== [CComputeShader::Initialize] Shader Compile Info ===\n");
    OutputDebugStringA("Path: ");
    OutputDebugStringA(szPathBuffer);
    OutputDebugStringA("\nEntry Point: ");
    OutputDebugStringA(pEntryPoint);
    OutputDebugStringA("\n");

    // 파일 존재 여부 확인
    FILE* pFile = { nullptr };
    _wfopen_s(&pFile, pShaderFilePath, L"r");

    if (pFile == nullptr)
    {
        OutputDebugStringA("ERROR: File does not exist!\n");
        return E_FAIL;
    }

    fclose(pFile);
    OutputDebugStringA("File exists: OK\n");

    return S_OK;
}

CComputeShader* CComputeShader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, const _char* pEntryPoint)
{
	CComputeShader* pInstance = new CComputeShader(pDevice, pContext);

	if (FAILED(pInstance->Initialize(pShaderFilePath, pEntryPoint)))
	{
		MSG_BOX(TEXT("Failed to Create : CComputeShader"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CComputeShader::Free()
{
	__super::Free();

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
	Safe_Release(m_pShader);
}
