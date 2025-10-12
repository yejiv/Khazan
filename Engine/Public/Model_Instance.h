#pragma once

#include "Component.h"

// 모델 인스턴싱용 클래스
// JSON 배우고 나중에 파일입출력으로 바꾸겠습니다.

NS_BEGIN(Engine)

class ENGINE_DLL CModel_Instance final : public CComponent
{
private:
	CModel_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel_Instance(const CModel_Instance& Prototype);
	virtual ~CModel_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(MODELTYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
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
	const aiScene*			m_pAIScene = { nullptr };
	Assimp::Importer		m_Importer = {};
	MODELTYPE				m_eModelType = {};
	_float4x4				m_PreTransformMatrix = {};

	// m_pAIScene = m_Importer.ReadFile(경로);

private:
	_uint							m_iNumMeshes = {};
	vector<class CMesh_Instance*>	m_Meshes;

private:
	/* Diffuse, Ambient, Specular */
	_uint							m_iNumMaterials = {};
	vector<class CMeshMaterial*>	m_Materials;

private:
	vector<class CBone*>			m_Bones;

private:
	_uint							m_iCurrentAnimIndex = { 0 };
	_uint							m_iNumAnimations = { 0 };
	vector<class CAnimation*>		m_Animations;
	_bool							m_isLoop = {};
	_bool							m_isFinished = {};

private:
	HRESULT Ready_Meshes();
	HRESULT Ready_Materials(const _char* pModelFilePath);
	HRESULT Ready_Bones(const aiNode* pAINode, _int iParentIndex);
	HRESULT Ready_Animations();

public:
	static CModel_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END