#pragma once

#include "Editor_Defines.h"
#include "VIBuffer_Instance.h"

// 메쉬 인스턴싱용 클래스
// JSON 배우고 나중에 파일입출력으로 바꾸겠습니다.

NS_BEGIN(Engine)
class CShader;
NS_END

NS_BEGIN(Editor)

class CEditor_ModelMesh_Instance final : public CVIBuffer_Instance
{
public:
	typedef struct tagEditorModelMeshInstance : public CVIBuffer_Instance::INSTANCE_DESC
	{
		vector<MESH_INSTANCE_DATA> InstanceData;

	}EDITOR_MODELMESH_INSTANCE_DESC;

private:
	CEditor_ModelMesh_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEditor_ModelMesh_Instance(const CEditor_ModelMesh_Instance& Prototype);
	virtual ~CEditor_ModelMesh_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(MODELTYPE eType, const aiMesh* pAIMesh, const vector<class CEditor_Bone*>& Bones, _fmatrix PreTransformMatrix, const INSTANCE_DESC* pDesc);
	virtual HRESULT Initialize_Clone(void* pArg) override;

public:
	HRESULT Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, const vector<class CEditor_Bone*>& Bones);
	const _uint Get_MaterialIndex() const { return m_iMaterialIndex; }

public:
	const _uint Get_NumInstances() const { return m_iNumInstance; }

	void	Get_Data(MESH_DATA& Data) { Data = m_Mesh_Data; }

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

	MESH_DATA				m_Mesh_Data = {};

private:
	HRESULT Ready_Vertices_For_NonAnim(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT Ready_Vertices_For_Anim(const aiMesh* pAIMesh, const vector<class CEditor_Bone*>& Bones);

	HRESULT Ready_Indices(const aiMesh* pAIMesh);

public:
	static CEditor_ModelMesh_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eType, const aiMesh* pAIMesh, const vector<class CEditor_Bone*>& Bones, _fmatrix PreTransformMatrix, const INSTANCE_DESC* pDesc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END