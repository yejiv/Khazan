#pragma once

#include "VIBuffer_Instance.h"

// 메쉬 인스턴싱용 클래스
// JSON 배우고 나중에 파일입출력으로 바꾸겠습니다.

NS_BEGIN(Engine)

class CMesh_Instance final : public CVIBuffer_Instance
{
public:
	typedef struct tagMeshInstance : public CVIBuffer_Instance::INSTANCE_DESC
	{

	}MESH_INSTANCE_DESC;

private:
	CMesh_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh_Instance(const CMesh_Instance& Prototype);
	virtual ~CMesh_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(MODELTYPE eType, const aiMesh* pAIMesh, const vector<class CBone*>& Bones, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, const vector<class CBone*>& Bones);
	const _uint Get_MaterialIndex() const { return m_iMaterialIndex; }

public:
	const _uint Get_NumInstances() const { return m_iNumInstance; }

	void Add_Instance(MESH_INSTANCE_DATA  InstanceData);
	void Fix_Instance(MESH_INSTANCE_DATA  InstanceData, _uint iInstanceIndex);

private:
	_char m_szName[MAX_PATH] = {};
	_uint m_iMaterialIndex = {};
	_uint m_iNumBones = {};

	/* 전체 뼈 기준으로 이 메시에 영향을 주는 뼈들의 인덱스를 저장 ( 전체 뼈 기준 영향을 주는 뼈의 인덱스 ) */
	vector<_int> m_BoneIndices;
	_float4x4	m_BoneMatrices[g_iMaxNumBones];

	/* 뼈의 개수만큼 OffsetMatrix 를 저장 */
	vector<_float4x4> m_OffsetMatrices;

private:
	HRESULT Ready_Vertices_For_NonAnim(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT Ready_Vertices_For_Anim(const aiMesh* pAIMesh, const vector<class CBone*>& Bones);

	HRESULT Ready_Indices_For_2Byte(const aiMesh* pAIMesh);
	HRESULT Ready_Indices_For_4Byte(const aiMesh* pAIMesh);

public:
	static CMesh_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eType, const aiMesh* pAIMesh, const vector<class CBone*>& Bones, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END