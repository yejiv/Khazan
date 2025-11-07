#pragma once
#include "Component.h"
#include <vector>
#include <mutex>

NS_BEGIN(Engine)

class ENGINE_DLL CDeferredShader final : public CComponent
{
private:
    CDeferredShader(ID3D11Device* pDevice);
    CDeferredShader(const CDeferredShader& Prototype);
    virtual ~CDeferredShader() = default;

public:
    virtual HRESULT Initialize_Prototype(const _tchar* strShaderPath,
        const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements,
        _uint iNumThreads);
    virtual HRESULT Initialize_Clone(void* pArg);

    HRESULT Begin(_uint iPassIdx, ID3D11DeviceContext* pContext);

    // === Bind helpers (현재 워커의 Effect에 바인드) ===
    HRESULT Bind_RawValue(const _char* szName, const void* data, _uint byteSize);
    HRESULT Bind_Matrix(const _char* szName, const _float4x4* m);
    HRESULT Bind_Matrices(const _char* szName, const _float4x4* m, _uint count);
    HRESULT Bind_SRV(const _char* szName, ID3D11ShaderResourceView* pSRV);
    HRESULT Bind_SRVs(const _char* szName, ID3D11ShaderResourceView** ppSRVs, _uint count);
    HRESULT Bind_Bool(const _char* szName, const _bool* pVal);
    HRESULT Bind_VectorArray(const _char* szName, const _float4* pData, _uint count);
    HRESULT Bind_FloatArray(const _char* szName, const _float* pData, _uint count);
    HRESULT Bind_IntArray(const _char* szName, const _int* pData, _uint count);
    HRESULT Bind_BoolArray(const _char* szName, const _bool* pData, _uint count);

    // 패스 개수 조회(디버깅용)
    _uint GetNumPasses() const { return m_iNumPasses; }

public:
    static CDeferredShader* Create(ID3D11Device* pDevice,
        const _tchar* strShaderPath,
        const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements,
        _uint iNumThreads);
    virtual CComponent* Clone(void* pArg) override;
    virtual void Free() override;

private:
    // 내부 유틸
    ID3DX11Effect* GetEffectForCurrentThread() const;
    HRESULT         BuildInputLayoutsFromEffect(ID3DX11Effect* pEffect,
        const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);

private:
    vector<ID3DX11Effect*>      m_vEffects;        // thread별 독립 Effect
    vector<ID3D11InputLayout*>  m_InputLayouts;    // pass별 공유 InputLayout

    _uint                       m_iNumPasses = 0;
};

NS_END
