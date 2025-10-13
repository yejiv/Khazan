#pragma once
#include "Editor_Defines.h"
#include "Base.h"

NS_BEGIN(Engine)
class CShader;
NS_END

NS_BEGIN(Editor)

class CEditor_MeshMaterial final : public CBase
{
private:
	CEditor_MeshMaterial(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CEditor_MeshMaterial() = default;

public:
	HRESULT Initialize(const _char* pModelFilePath, const aiMaterial* pAIMaterial);
	
public:
	HRESULT Bind_Resources(CShader* pShader, const _char* pConstantName, _uint iTextureType, _uint iTextureIndex);
	void Get_Data(MATERIAL_DATA& data) { data = m_Material_Data; }

private:
	ID3D11Device* m_pDevice;
	ID3D11DeviceContext* m_pContext;
	vector<ID3D11ShaderResourceView*> m_SRVs[AI_TEXTURE_TYPE_MAX];

private:
	MATERIAL_DATA	m_Material_Data = {};

public:
	static CEditor_MeshMaterial* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, const aiMaterial* pAIMaterial);
	virtual void Free() override;
};

NS_END