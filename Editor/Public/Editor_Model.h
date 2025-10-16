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

public:
	const char*		Get_ModelName() const { return m_Model_Data.strModelName.c_str(); }
	const _uint		Get_NumMeshes() const { return m_iNumMeshes; }
	void			Get_ModelData(MODEL_DATA* data) { data = &m_Model_Data; }

public:
	void			ExportModel();
	void			LoadModel(_wstring strModelName);
	void			Update_DAT_From_JSON();

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
	_int							m_iCurrentAnimIndex = { -1 };
	_int							m_iPrevAnimIndex = { -1 };
	_float							m_fCurrentTrackPosition = { 0.f }; /* 현재 애니메이션 재생 위치 */
	vector< class CEditor_Animation* >		m_Animations;

	_bool							m_isLoop = {};
	_bool							m_isFinished = {};
	_bool							m_isChangedAnimation = {};

	/* 루트모션 */
	_uint							m_iRootBoneIndex = { 0 }; // 루트 모션을 적용할 뼈의 인덱스	
	_bool							m_isRootMotion = { false }; // 루트 모션 사용 여부
	_float							m_fCurRootMotionBlendTime = {};
	_float							m_fRootMotionBlendTime = { 0.15f }; // 루트 모션 보간에 사용할 시간
	_matrix							m_PreRootMatrix = {}; // 이전 루트 모션 행렬	

private:
	HRESULT			Ready_Meshes();
	HRESULT			Ready_Materials();
	HRESULT			Ready_Bones(const aiNode* pAINode, _int iParentIndex);
	HRESULT			Ready_Animation();

private:
	/* 루트 모션 */
	void			OnRootMotion();
	void			Update_RootMotion(_float fTimeDelta);

	/* Export */
	_bool			Export_AnimationJson(const string& strFilePath, const string& strFilePath2);
	_bool			Export_MaterialJson(const string& strFilePath);
	void			Export_Binary(const string& strFilePath);



	/* Json에 이쁘게 쓰기  */
	string			PostProcessJSON(const string& jsonStr);
	string			CompressArray(const string& arrayStr);

public:
	static  CEditor_Model* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void			Free() override;

};
NS_END