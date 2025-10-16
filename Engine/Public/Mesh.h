#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& Prototype);
	virtual ~CMesh() = default;


public:
	virtual HRESULT Initialize_Prototype(MODELTYPE eType, _fmatrix PreTransformMatrix, MESH_DATA& data);
	virtual HRESULT Initialize_Clone(void* pArg) override;

public:
	_uint Get_MaterialIndex() const { return m_iMaterialIndex; }
	vector<_float3> Get_VerticesPos() { return m_vVerticesPos; }
	vector<_uint> Get_Indices() { return m_vIndices; }

public:
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, const vector<class CBone*>& Bones);

private:
	_wstring				m_strName;
	_uint					m_iMaterialIndex = {};
	_uint					m_iNumBones = {};

	vector<_int>			m_BoneIndices;	
	_float4x4				m_BoneMatrices[g_iMaxNumBones] = {};
	vector<_float4x4>		m_OffsetMatrices;
	vector<_float3>			m_vVerticesPos;
	vector<_uint>			m_vIndices;

private:
	HRESULT  Ready_Vertices_For_NonAnim(MESH_DATA& data);
	HRESULT Ready_Vertices_For_Anim(MESH_DATA& data);

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eType, _fmatrix PreTransformMatrix, MESH_DATA& data);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
