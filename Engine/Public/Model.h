#pragma once

#include "Component.h"

NS_BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
private:
	enum MODEL_STATE
	{
		ANIM_LOOP = 1 << 0,
		USED_ANIM_LOOP = 1 << 1,

		CHANGE_ANIMATION = 1 << 2,

		ANIMSET_PLAYING = 1 << 3,
		ANIMSET_NEXT = 1 << 4,
		//ANIMSET_FINISHED = 1 << 5,

		ROOTMOTION = 1 << 6,
		ROOTMOTION_POSITION = 1 << 7,
		ROOTMOTION_ROTATION = 1 << 8,

	};

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
	_uint				Get_NumMeshes() const { return m_iNumMeshes; }
	_float4x4*			Get_BoneMatrix(const _char* pBoneName);
	_float4x4*			Get_ContainNameBoneMatrix(const _char* pBoneName);
	vector<_float3>		Get_VerticesPos(_uint iIndex);		//졸트
	vector<_uint>		Get_Indices(_uint iIndex);			//졸트

	/* 애니메이션 기능  */
	_bool			Play_Animation(_float fTimeDelta);
	void			Set_Animation(_uint iIndex);
	void			Set_AnimationSet(const string& strKey);
	void			Set_AnimationLoop(_bool isLoop);


private:
	/* State*/
	_uint								m_iState = { 0 };


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
	_bool								m_isFinished = { false };			/* 루프가 아닌 애니메이션이 끝났는지 여부  */
	_uint								m_iNumAnimations = { 0 };			/* 애니메이션 수 */
	_int								m_iCurrentAnimIndex = { -1 };		/* 현재 애니메이션 인덱스 */
	_float								m_fCurrentTrackPosition = { 0.f };  /* 현재 애니메이션 재생 위치 */
	_int								m_iPrevAnimIndex = { -1 };			/* 이전 애니메이션 인덱스 */
	vector< class CAnimation* >			m_Animations;						/* 애니메이션 클래스 저장  */
	vector<ANIMATION_SETUP_DATA>		m_AnimationsSetup;					/* 애니메이션들 정보 */

	/* 애니메이션 세트 */
	vector< ANIMATION_SET_DATA >		m_AnimationSets;					/* 애니메이션 세트 */
	_uint								m_iCurrentAnimSetsIndex = { 0 };	/* 선택된 애니메이션 세트 */
	_uint								m_iCurrentAnimSetsMaxIndex = { 0 };	/* 선택된 애니메이션 세트에서 애니메이션 수*/
	_uint								m_iCurrentAnimSetIndex = { 0 };		/* 애니메이션 세트에서 어떤 애니메이션의 인덱스 */

	/* 루트 모션 */
	_uint							m_iRootBoneIndex = {};				/* 루트 모션을 적용할 뼈의 인덱스(루트 본) */
	_vector							m_vRootMotionScale = {};				/* 어떤 축에 적용할건지 */ 
	_float							m_fCurrentRootMotionBlendTime = {0.f};	/* 현재 루트모션 블랜딩 시간*/
	_float							m_fRootMotionBlendTime = { 0.15f };		/* 루트모션 블랜딩 총 시간 */
	const _float					m_fBaseRootMotionBlendTime = {0.15f};   /* 만약 블랜딩 시간이 안써져있으면 사용할 기본 블랜딩 시간 */
	_matrix							m_PreRootMatrix = {}; 					/* 이전 루트 모션 행렬 */



private:
	void			Check_RootMotion();
	void			Update_RootMotion(_float fTimeDelta);

private:
	HRESULT Ready_Meshes(MODEL_DATA& data);
	HRESULT Ready_Materials(MODEL_DATA& data);
	HRESULT Ready_Bones(MODEL_DATA& data);
	HRESULT Ready_Animations(MODEL_DATA& data);

private:
	inline void		Add_State(MODEL_STATE s) { m_iState |= s; }
	inline void		Add_State(_uint i) { m_iState |= i; }
	inline void		Remove_State(MODEL_STATE s) { m_iState &= ~s; }
	inline void		Remove_State(_uint i) { m_iState &= ~i; }
	inline _bool	Has_State(MODEL_STATE s) { return (m_iState & s) != 0; }
	inline _bool	Has_State(_uint i) { return (m_iState & i) != 0; }
	inline void		Clear_State() { m_iState = 0; }

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pModelFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;

};

NS_END
