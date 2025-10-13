#pragma once
#include "Base.h"

NS_BEGIN(Engine)

class CComputeShader final : public CBase
{
private:
	CComputeShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CComputeShader() = default;

public:
	HRESULT		Initialize(const _tchar* pShaderFilePath, const _char* pEntryPoint);
	void		Execute(const COMPUTE_PASS_DESC& Desc);

private:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };
	ID3D11ComputeShader*	m_pShader = { nullptr };

private:
	HRESULT					Check_ShaderFileExists(const _tchar* pShaderFilePath, const _char* pEntryPoint);

public:
	static CComputeShader*	Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pShaderFilePath, const _char* pEntryPoint);
	virtual void			Free() override;
};

NS_END