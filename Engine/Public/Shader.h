#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CShader final : public CComponent
{
private:
	CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShader(const CShader& Prototype);
	virtual ~CShader() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);
	virtual HRESULT Initialize_Clone(void* pArg);

public:
	HRESULT Begin(_uint iPassIndex);
	HRESULT Deferred_Begin(_uint iPassIndex, ID3D11DeviceContext* pDeferredContext);

	HRESULT Bind_RawValue(const _char* pConstantName, const void* pData, _uint iLength);
	HRESULT Bind_FloatArray(const _char* pConstantName, const _float* pData, _uint iCount);
	HRESULT Bind_IntArray(const _char* pConstantName, const _int* pData, _uint iCount);
	HRESULT Bind_BoolArray(const _char* pConstantName, const _bool* pData, _uint iCount);
	HRESULT Bind_VectorArray(const _char* pConstantName, const _float4* pData, _uint iCount);
	HRESULT Bind_Bool(const _char* pConstantName, const _bool* pData);

	HRESULT Bind_Matrix(const _char* pConstantName, const _float4x4* pMatrix);
	HRESULT Bind_Matrices(const _char* pConstantName, const _float4x4* pMatrix, _uint iNumMatrices);
	HRESULT Bind_SRV(const _char* pConstantName, ID3D11ShaderResourceView* pSRV);
	HRESULT Bind_SRVs(const _char* pConstantName, ID3D11ShaderResourceView** ppSRVs, _uint iNumSRVs);

private:
	ID3DX11Effect*				m_pEffect = { nullptr };

	vector<ID3D11InputLayout*>	m_InputLayouts;

	_uint						m_iNumPasses = { };

	mutex						m_Mutex;


public:
	static CShader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, _uint iNumElements);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END