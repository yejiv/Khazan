#include "DeferredShader.h"

using namespace Engine;

extern thread_local uint32_t t_worker_idx; // ThreadPool에서 Worker_Thread(i) 시작 시 세팅

static HRESULT CompileEffect(ID3D11Device* dev, const _tchar* path, ID3DX11Effect** outEff)
{
    UINT flags = 0;
#ifdef _DEBUG
    flags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
    flags = D3DCOMPILE_OPTIMIZATION_LEVEL1;
#endif
    return D3DX11CompileEffectFromFile(path, nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE,
        flags, 0, dev, outEff, nullptr);
}


CDeferredShader::CDeferredShader(ID3D11Device* pDevice)
    : CComponent(pDevice, nullptr)
{
}

CDeferredShader::CDeferredShader(const CDeferredShader& Prototype)
    : CComponent{ Prototype }
    , m_vEffects{ Prototype.m_vEffects }
    , m_iNumPasses{ Prototype.m_iNumPasses }
    , m_InputLayouts{ Prototype.m_InputLayouts }
{

    for (auto& Effect : m_vEffects)
        Safe_AddRef(Effect);

    for (auto& pInputLayout : m_InputLayouts)
        Safe_AddRef(pInputLayout);
}

HRESULT CDeferredShader::Initialize_Prototype(const _tchar* strShaderPath,
    const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements,
    _uint iNumThreads)
{
    if (!strShaderPath || !pElements || iNumElements == 0 || iNumThreads == 0) 
        return E_FAIL;

    m_vEffects.resize(iNumThreads, nullptr);
    for (_uint i = 0; i < iNumThreads; ++i)
    {
        if (FAILED(CompileEffect(m_pDevice, strShaderPath, &m_vEffects[i])))
            return E_FAIL;
    }

    if (FAILED(BuildInputLayoutsFromEffect(m_vEffects[0], pElements, iNumElements)))
        return E_FAIL;

    return S_OK;
}

HRESULT CDeferredShader::Initialize_Clone(void* pArg)
{
    return S_OK;
}

HRESULT CDeferredShader::BuildInputLayoutsFromEffect(ID3DX11Effect* pEffect,
    const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements)
{
    if (!pEffect) return E_FAIL;

    ID3DX11EffectTechnique* tech = pEffect->GetTechniqueByIndex(0);
    if (!tech) return E_FAIL;

    D3DX11_TECHNIQUE_DESC tdesc{};
    tech->GetDesc(&tdesc);
    m_iNumPasses = tdesc.Passes;

    m_InputLayouts.resize(m_iNumPasses, nullptr);

    for (UINT p = 0; p < tdesc.Passes; ++p)
    {
        ID3DX11EffectPass* pass = tech->GetPassByIndex(p);
        if (!pass) return E_FAIL;

        D3DX11_PASS_DESC pdesc{};
        pass->GetDesc(&pdesc);

        ID3D11InputLayout* layout = nullptr;
        if (FAILED(m_pDevice->CreateInputLayout(
            pElements, iNumElements, pdesc.pIAInputSignature, pdesc.IAInputSignatureSize, &layout)))
            return E_FAIL;

        m_InputLayouts[p] = layout;
    }

    return S_OK;
}

ID3DX11Effect* CDeferredShader::GetEffectForCurrentThread() const
{
    uint32_t idx = t_worker_idx;
    if (idx >= m_vEffects.size()) idx = 0; // 안전장치: 메인스레드 등
    return m_vEffects[idx];
}

HRESULT CDeferredShader::Begin(_uint iPassIdx, ID3D11DeviceContext* pContext)
{
    if (iPassIdx >= m_iNumPasses || !pContext) return E_FAIL;

    ID3DX11Effect* eff = GetEffectForCurrentThread();
    if (!eff) return E_FAIL;

    ID3DX11EffectTechnique* tech = eff->GetTechniqueByIndex(0);
    if (!tech) return E_FAIL;

    ID3DX11EffectPass* pass = tech->GetPassByIndex(iPassIdx);
    if (!pass) return E_FAIL;

    if (FAILED(pass->Apply(0, pContext))) return E_FAIL;
    pContext->IASetInputLayout(m_InputLayouts[iPassIdx]);

    return S_OK;
}


HRESULT CDeferredShader::Bind_RawValue(const _char* szName, const void* data, _uint byteSize)
{
    ID3DX11Effect* eff = GetEffectForCurrentThread(); if (!eff) return E_FAIL;
    ID3DX11EffectVariable* v = eff->GetVariableByName(szName); if (!v) return E_FAIL;
    return v->SetRawValue(data, 0, byteSize);
}

HRESULT CDeferredShader::Bind_Matrix(const _char* szName, const _float4x4* m)
{
    ID3DX11Effect* eff = GetEffectForCurrentThread(); if (!eff) return E_FAIL;
    ID3DX11EffectMatrixVariable* mv = eff->GetVariableByName(szName)->AsMatrix(); if (!mv) return E_FAIL;
    return mv->SetMatrix(reinterpret_cast<const _float*>(m));
}

HRESULT CDeferredShader::Bind_Matrices(const _char* szName, const _float4x4* m, _uint count)
{
    ID3DX11Effect* eff = GetEffectForCurrentThread(); if (!eff) return E_FAIL;
    ID3DX11EffectMatrixVariable* mv = eff->GetVariableByName(szName)->AsMatrix(); if (!mv) return E_FAIL;
    return mv->SetMatrixArray(reinterpret_cast<const _float*>(m), 0, count);
}

HRESULT CDeferredShader::Bind_SRV(const _char* szName, ID3D11ShaderResourceView* pSRV)
{
    ID3DX11Effect* eff = GetEffectForCurrentThread(); if (!eff) return E_FAIL;
    ID3DX11EffectShaderResourceVariable* sv = eff->GetVariableByName(szName)->AsShaderResource(); if (!sv) return E_FAIL;
    return sv->SetResource(pSRV);
}

HRESULT CDeferredShader::Bind_SRVs(const _char* szName, ID3D11ShaderResourceView** ppSRVs, _uint count)
{
    ID3DX11Effect* eff = GetEffectForCurrentThread(); if (!eff) return E_FAIL;
    ID3DX11EffectShaderResourceVariable* sv = eff->GetVariableByName(szName)->AsShaderResource(); if (!sv) return E_FAIL;
    return sv->SetResourceArray(ppSRVs, 0, count);
}

HRESULT CDeferredShader::Bind_Bool(const _char* szName, const _bool* pVal)
{
    ID3DX11Effect* eff = GetEffectForCurrentThread(); if (!eff) return E_FAIL;
    return eff->GetVariableByName(szName)->AsScalar()->SetBool(*pVal);
}

HRESULT CDeferredShader::Bind_VectorArray(const _char* szName, const _float4* pData, _uint count)
{
    ID3DX11Effect* eff = GetEffectForCurrentThread(); if (!eff) return E_FAIL;
    return eff->GetVariableByName(szName)->AsVector()->SetFloatVectorArray(reinterpret_cast<const float*>(pData), 0, count);
}

HRESULT CDeferredShader::Bind_FloatArray(const _char* szName, const _float* pData, _uint count)
{
    ID3DX11Effect* eff = GetEffectForCurrentThread(); if (!eff) return E_FAIL;
    return eff->GetVariableByName(szName)->AsScalar()->SetFloatArray(pData, 0, count);
}

HRESULT CDeferredShader::Bind_IntArray(const _char* szName, const _int* pData, _uint count)
{
    ID3DX11Effect* eff = GetEffectForCurrentThread(); if (!eff) return E_FAIL;
    return eff->GetVariableByName(szName)->AsScalar()->SetIntArray(pData, 0, count);
}

HRESULT CDeferredShader::Bind_BoolArray(const _char* szName, const _bool* pData, _uint count)
{
    ID3DX11Effect* eff = GetEffectForCurrentThread(); if (!eff) return E_FAIL;
    return eff->GetVariableByName(szName)->AsScalar()->SetBoolArray(pData, 0, count);
}

CDeferredShader* CDeferredShader::Create(ID3D11Device* pDevice,
    const _tchar* strShaderPath,
    const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements,
    _uint iNumThreads)
{
    CDeferredShader* pInstance = new CDeferredShader(pDevice);

    if (FAILED(pInstance->Initialize_Prototype(strShaderPath, pElements, iNumElements, iNumThreads)))
    {
        MSG_BOX(TEXT("Failed to Created : CShader"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CDeferredShader::Clone(void* pArg)
{
    CDeferredShader* pInstance = new CDeferredShader(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Cloned : CShader"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CDeferredShader::Free()
{
    for (auto*& e : m_vEffects) Safe_Release(e);
    m_vEffects.clear();

    for (auto*& il : m_InputLayouts) Safe_Release(il);
    m_InputLayouts.clear();

    __super::Free();
}
