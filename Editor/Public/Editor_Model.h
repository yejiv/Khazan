#pragma once
#include "Editor_Defines.h"
#include "Component.h"

NS_BEGIN(Engine)
class CTransform;
NS_END

NS_BEGIN(Editor)

class CEditor_Model final : public CComponent
{

private:
	CEditor_Model(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEditor_Model(const CEditor_Model& Prototype);
	virtual ~CEditor_Model() = default;

public:
	virtual HRESULT Initialize_Prototype(MODELTYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize_Clone(void* pArg);
	virtual HRESULT Render(_uint iMeshIndex);

public:
	HRESULT			Bind_Materials(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex, _uint iTextureType, _uint iTextureIndex);
	HRESULT			Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, _uint iMeshIndex);
	_bool			Play_Animation(_float fTimeDelta);
	void			Set_Animation(_uint iIndex, _bool isLoop);
	void			Set_SetAnimation(const string& strKey);
	void			Set_OwnerTransform(class CTransform** pTransform) {
		m_pOwnerTransform = *pTransform;
		Safe_AddRef(m_pOwnerTransform);
	}

public:
	const char*					Get_ModelName() const { return m_Model_Data.strModelName.c_str(); }
	const _uint					Get_NumMeshes() const { return m_iNumMeshes; }

	MODEL_DATA*					Get_ModelData() { return &m_Model_Data; }
	ANIMATION_SETUP_DATA*		Get_CurAnimSet() { return &m_Model_Data.vecAnimation[m_iCurrentAnimIndex].animSetup; }
	class CEditor_Animation*	Get_CurAnimtion();
	_int						Get_CurAnimIndex() { return m_iCurrentAnimIndex; }
	_float*						Get_CurTrackPosition() { return &m_fCurrentTrackPosition; }
	//const _matrix&				Get_RootMotionDelta()const { return m_vRootMotionDelta; }
	//_bool						isRootMotion() { return m_isRootMotion; }



public:
	void			ExportModel(string& strPath);
	void			ExportModel_NoMsg(string& strPath);	// 확인 창 말고 디버그 창에 띄우는 추출 함수
	void			LoadModel(string& strPath);
	void			Update_DAT_From_JSON(string& strPath);
	_bool			Test()
	{
		if (m_isTest)
		{
			m_isTest = false;
			return true;
		}
		return m_isTest;
	}

private:
	const aiScene*			m_pAIScene = { nullptr };
	Assimp::Importer		m_Importer = {};
	MODELTYPE				m_eModelType = {};
	_float4x4				m_PreTransformMatrix = {};

	const _char*			m_pModelFilePath = {};

	MODEL_DATA				m_Model_Data = {};	/* 파일로부터 읽은 모든 정보를 다 저장해주는 구조체. */

	/* 매쉬 */
	_uint					m_iNumMeshes = {};
	vector< class CEditor_Mesh* >	m_Meshes;

	/* 머터리얼 */
	_uint					 m_iNumMaterials = {};
	vector<class CEditor_MeshMaterial* >   m_Materials;

	/* 뼈 */
	vector < class CEditor_Bone* > m_Bones;

	/* 애니메이션 */
	_uint							m_iNumAnimations = { 0 };
	_int							m_iCurrentAnimIndex = { 0 };
	_int							m_iPrevAnimIndex = { 0 };
	_float							m_fCurrentTrackPosition = { 0.f }; /* 현재 애니메이션 재생 위치 */
	vector< class CEditor_Animation* >		m_Animations;

	/* 루트모션 */
	_uint							m_iRootBoneIndex = { 0 };      // Root (2) - 위치용
	//_uint							m_iRotationBoneIndex = { 0 };  // Bip001 (3) - 회전용
	_bool							m_isRootMotion = { false }; // 루트 모션 사용 여부
	_bool							m_isRootMotion_Pos = { false };
	_bool							m_isIgonreRootRot = { false };
	_bool							m_isIgnoreRootPos = { false};             // 매 프레임 루트본 위치값 0
	_bool							m_isIgnoreRootPosFirstFrame = { false };   //첫 번째 프레임만 루트본 위치값 0
	_bool							m_isAbsoluteRootPosition = { true }; //루트본이 절대 위치로 시작하는 애나메이션에 사용(오프셋 필요) (ex- 앞으로 가는 콤보공격 2번째같은경우)
	_vector							m_vRootMotionScale = {};	//루트모션 포지션 값 어디 축에만 적용할 것인지.
	_vector							m_vFirstFrameRootOffset = {}; //첫 프레임 오프셋 저장. 콤보애니같은경우 위치값이 크므로.
	_vector							m_vRootDeltaQuat = {};
	_vector							m_vPreTransformQuat = {}; /////
	//_matrix							m_FirstFrameRootMatrix = {};
	_matrix							m_PreRootMatrix = {}; // 이전 루트 모션 행렬	
	//_matrix							m_PreRotRootMatrix = {}; // 이전 루트 모션 행렬	(rotataion 용)
	_matrix							m_vRootMotionDelta = {};// 루트모션 변화량
	_bool							m_isFirstRootMotionFrame = { false };
	//_vector							m_vPreRootPos = XMVectorSet(0.f, 0.f, 0.f, 1.f);  // 위치만 저장
	//_matrix							m_BaseMatrix = {};	//초기 스케일값만 적용된 기본 매트릭스 



	_bool							m_isLoop = {};
	_bool							m_isAnimationLooped = { false };  // 이번 프레임에 애니메이션이 루프되었는지
	_bool							m_isFinished = {};
	_bool							m_isChangedAnimation = {};

	//set Anim
	_bool							m_isSetAnimPlaying = { false };
	_bool							m_isSetAnimNextPlay = {false};
	_bool							m_isSetAnimFinished = { false };
	_uint							m_iCurSetAnimIndex = { 0 };// 애님세트가 진행중인데 몇번째인지? 
	_uint							m_iCurSetAnimMaxIndex = { 0 }; //애님 세트에서 총 몇개의 애님이 있는지
	_uint							m_iCurSelectSetAnimIndex = { 0 }; // 키값에 해당하는 애니메이션 세트
	//string							m_strSetAnimKey = "";

	class CTransform* m_pOwnerTransform = nullptr;

	//test
	_bool	m_isTest = { false };
	_int	m_iTest = { 0 };
private:
	HRESULT			Ready_Meshes();
	HRESULT			Ready_Materials();
	HRESULT			Ready_Bones(const aiNode* pAINode, _int iParentIndex);
	HRESULT			Ready_Animation();

private:
	/* 루트 모션 */
	void			OnRootMotion();
	void			Update_RootMotion(_float fTimeDelta, _bool isBlending);
	void			Apply_RootMotion_To_Transform();

	/* Export */
	_bool			Export_AnimationJson(const string& strFilePath, const string& strFilePath2);
	_bool			Export_MaterialJson(const string& strFilePath);
	// png 말고 바로 dds로 뽑는 함수
	_bool			Export_MaterialJson_ForDDS(const string& strFilePath);
	void			Export_Binary(const string& strFilePath);
	// 확인 창 말고 디버그 창에 띄우는 추출 함수
	void			Export_Binary_NoMsg(const string& strFilePath);

	/* Json에 이쁘게 쓰기  */
	string			PostProcessJSON(const string& jsonStr);
	string			CompressArray(const string& arrayStr);

public:
	static  CEditor_Model* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void			Free() override;

	void DebugDumpRootBonePerFrame();

};
NS_END
