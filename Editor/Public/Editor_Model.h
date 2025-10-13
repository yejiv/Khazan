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

	/* 파일로부터 읽은 모든 정보를 다 저장해주는 구조체. */
	MODEL_DATA				m_Model_Data = {};

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
	_uint							m_iCurrentAnimIndex = { 0 };
	vector< class CEditor_Animation* >		m_Animations;

	_bool							m_isLoop = {};
	_bool							m_isFinished = {};

private:
	HRESULT			Ready_Meshes();
	HRESULT			Ready_Materials();
	HRESULT			Ready_Bones(const aiNode* pAINode, _int iParentIndex);
	HRESULT			Ready_Animation();

	_bool			Export_AnimationJson(const string& strFilePath, const string& strFilePath2);
	_bool			Export_MaterialJson(const string& strFilePath);
	void			Export_Binary(const string& strFilePath);

	string			PostProcessJSON(const string& jsonStr);
	string			CompressArray(const string& arrayStr);

public:
	static  CEditor_Model* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eModelType, const _char* pModelFilePath, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void			Free() override;

};
NS_END