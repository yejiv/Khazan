#pragma once

#include "Editor_Defines.h"
#include "Component.h"

#include "Editor_ModelMesh_Instance.h"

NS_BEGIN(Editor)

class CEditor_Model_Instance final : public CComponent
{
private:
	CEditor_Model_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEditor_Model_Instance(const CEditor_Model_Instance& Prototype);
	virtual ~CEditor_Model_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(MODELTYPE eModelType, const _char* pModelFilePath, const CEditor_ModelMesh_Instance::INSTANCE_DESC* pDesc, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual HRESULT Render(_uint iMeshIndex);

public:
	_uint Get_NumMeshes() const { return m_iNumMeshes; }

	_float4x4* Get_BoneMatrix(const _char* pBoneName);

public:
	const _uint Get_NumInstances() const;

	void Add_Instance(MESH_INSTANCE_DATA InstanceData);
	void Fix_Instance(MESH_INSTANCE_DATA InstanceData, _uint iInstanceIndex);

public:
	HRESULT Bind_Materials(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType, _uint iIndex);
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	_bool Play_Animation(_float fTimeDelta);
	void Set_Animation(_uint iIndex, _bool isLoop = false);

private:
	/* 파일로부터 읽은 모든 정보를 다 저장해주는 구조체. */
	const aiScene* m_pAIScene = { nullptr };
	Assimp::Importer		m_Importer = {};
	MODELTYPE				m_eModelType = {};
	_float4x4				m_PreTransformMatrix = {};

	// m_pAIScene = m_Importer.ReadFile(경로);

private:
	_uint								m_iNumMeshes = {};
	vector<CEditor_ModelMesh_Instance*>	m_Meshes;

private:
	/* Diffuse, Ambient, Specular */
	_uint								m_iNumMaterials = {};
	vector<class CEditor_MeshMaterial*>		m_Materials;

private:
	vector<class CEditor_Bone*>				m_Bones;

private:
	_uint								m_iCurrentAnimIndex = { 0 };
	_uint								m_iNumAnimations = { 0 };
	vector<class CEditor_Animation*>			m_Animations;
	_bool								m_isLoop = {};
	_bool								m_isFinished = {};

private:
	HRESULT Ready_Meshes(const CEditor_ModelMesh_Instance::INSTANCE_DESC* pDesc);
	HRESULT Ready_Materials(const _char* pModelFilePath);
	HRESULT Ready_Bones(const aiNode* pAINode, _int iParentIndex);
	HRESULT Ready_Animations();

public:
	static CEditor_Model_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eModelType, const _char* pModelFilePath, const CEditor_ModelMesh_Instance::INSTANCE_DESC* pDesc, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END