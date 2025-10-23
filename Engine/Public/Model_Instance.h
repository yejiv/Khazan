#pragma once

#include "Component.h"

#include "ModelMesh_Instance.h"

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
	virtual HRESULT Initialize_Prototype(const _char* pModelFilePath, const CModelMesh_Instance::INSTANCE_DESC* pDesc);
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual HRESULT Render(_uint iMeshIndex);

public:
	_uint Get_NumMeshes() const { return m_iNumMeshes; }

	_float4x4* Get_BoneMatrix(const _char* pBoneName);

public:
	const _uint Get_NumInstances() const;

public:
	HRESULT Bind_Materials(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, aiTextureType eTextureType, _uint iIndex);
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	_bool Play_Animation(_float fTimeDelta);
	void Set_Animation(_uint iIndex, _bool isLoop = false);

private:
	/* 파일로부터 읽은 모든 정보를 다 저장해주는 구조체. */
	_wstring							m_strModelName{};
	_wstring							m_strModelFilePath{};
	MODELTYPE							m_eModelType = {};
	_float4x4							m_PreTransformMatrix = {};

	// m_pAIScene = m_Importer.ReadFile(경로);

private:
	_uint								m_iNumMeshes = {};
	vector<CModelMesh_Instance*>		m_Meshes;

private:
	/* Diffuse, Ambient, Specular */
	_uint								m_iNumMaterials = {};
	vector<class CMeshMaterial*>		m_Materials;

private:
	vector<class CBone*>				m_Bones;

private:
	_uint								m_iNumAnimations = { 0 };			/* 애니메이션 수 */
	_uint								m_iCurrentAnimIndex = { 0 };		/* 현재 애니메이션 인덱스 */
	_float								m_fCurrentTrackPosition = { 0.f };  /* 현재 애니메이션 재생 위치 */
	_uint								m_iPrevAnimIndex = { 0 };			/* 이전 애니메이션 인덱스 */
	vector<class CAnimation*>			m_Animations;						/* 애니메이션 클래스 저장  */
	_bool								m_isLoop = { false };				/* 애니메이션 루프 여부 */
	_bool								m_isFinished = { false };			/* 루프가 아닌 애니메이션이 끝났는지 여부  */
	//_bool								m_isRepeatAnimation = { false };	/* 루프가 아닌 애니메이션이 반복되는 경우 애니메이션 끝났는지*/
	//_bool								m_isChangedAnimation = { false };	/* 애니메이션이 변경된 경우*/
	//_bool								m_isFixedAnimation = { false };		/* 애니메이션이 끝나기 전까지 다른 애니메이션 금지	*/
	//_bool								m_isForceAnimation = { false };		/* 강제로 애니메이션을 바꾸는 경우*/
	//_bool								m_isFinishAnimation = { false };	/* 애니메이션이 다 끝나고 다음 애니메이션으로 넘어가는 경우*/

private:
	HRESULT Ready_Meshes(MODEL_DATA& Data, const CModelMesh_Instance::INSTANCE_DESC* pDesc);
	HRESULT Ready_Materials(MODEL_DATA& Data);
	HRESULT Ready_Bones(MODEL_DATA& Data);
	HRESULT Ready_Animations(MODEL_DATA& Data);

public:
	static CModel_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath, const CModelMesh_Instance::INSTANCE_DESC* pDesc);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END