#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& Prototype);
	virtual ~CModel() = default;

public:
	virtual HRESULT Initialize_Prototype( const _char* pModelFilePath);
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual HRESULT Render(_uint iMeshIndex);

public:
	HRESULT Bind_Materials(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, _uint iTextureType, _uint iIndex);
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);


public:
	/* 정보 */
	_uint Get_NumMeshes() const { return m_iNumMeshes; }
	_float4x4* Get_BoneMatrix(const _char* pBoneName);
	_float4x4* Get_ContainNameBoneMatrix(const _char* pBoneName);
	vector<_float3> Get_VerticesPos(_uint iIndex);
	vector<_uint> Get_Indices(_uint iIndex);

	/* 애니메이션 기능  */
	_bool Play_Animation(_float fTimeDelta);
	void Set_Animation(_uint iIndex, _bool isLoop = false);


private:
	/* 모델 */
	_wstring							m_strModelName{};
	_wstring							m_strModelFilePath{};
	MODELTYPE							m_eModelType = {};
	_float4x4							m_PreTransformMatrix = {};

	/* 매쉬 */
	vector<class CMesh*>				m_Meshes;
	_uint								m_iNumMeshes = {};

	/* 머터리얼 */
	vector<class CMeshMaterial*>		m_Materials;
	_uint								m_iNumMaterials = {};

	/* 뼈 */
	vector<class CBone*>				m_Bones;

	/* 애니메이션 */
	_uint								m_iNumAnimations = { 0 };			/* 애니메이션 수 */
	_uint								m_iCurrentAnimIndex = { 0 };		/* 현재 애니메이션 인덱스 */
	_float								m_fCurrentTrackPosition = { 0.f };  /* 현재 애니메이션 재생 위치 */
	_uint								m_iPrevAnimIndex = { 0 };			/* 이전 애니메이션 인덱스 */
	vector< class CAnimation* >			m_Animations;						/* 애니메이션 클래스 저장  */
	_bool								m_isLoop = { false };				/* 애니메이션 루프 여부 */
	_bool								m_isFinished = { false };			/* 루프가 아닌 애니메이션이 끝났는지 여부  */
	//_bool								m_isRepeatAnimation = { false };	/* 루프가 아닌 애니메이션이 반복되는 경우 애니메이션 끝났는지*/
	//_bool								m_isChangedAnimation = { false };	/* 애니메이션이 변경된 경우*/
	//_bool								m_isFixedAnimation = { false };		/* 애니메이션이 끝나기 전까지 다른 애니메이션 금지	*/
	//_bool								m_isForceAnimation = { false };		/* 강제로 애니메이션을 바꾸는 경우*/
	//_bool								m_isFinishAnimation = { false };	/* 애니메이션이 다 끝나고 다음 애니메이션으로 넘어가는 경우*/

private:
	HRESULT Ready_Meshes(MODEL_DATA& data);
	HRESULT Ready_Materials(MODEL_DATA& data);
	HRESULT Ready_Bones(MODEL_DATA& data);
	HRESULT Ready_Animations(MODEL_DATA& data);

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END
