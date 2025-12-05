#pragma once
#include "Editor_Defines.h"
#include "VIBuffer.h"

NS_BEGIN(Engine)
class CTransform;
class CShader;
NS_END

NS_BEGIN(Editor)

class CEditor_Mesh final:  public CVIBuffer
{
private:
	CEditor_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEditor_Mesh(const CEditor_Mesh& Prototype);
	virtual ~CEditor_Mesh() = default;

public:
	virtual HRESULT Initialize_Prototype(MODELTYPE eType, const aiMesh* pAIMesh, const vector<class CEditor_Bone*>& Bones, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize_Clone(void* pArg) override;

public:
	const _uint Get_MaterialIndex() const { return m_iMaterialIndex; }
	HRESULT Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, const vector<class CEditor_Bone*>& Bones);

public:
	void	Get_Data(MESH_DATA& data) { data = m_Mesh_Data; }

	//_bool	Is_Picked(CTransform* pTransform, _float3* pOut);
	//_bool	Is_Picked(CTransform* pTransform, _float3* pOut, _float* pMinDistance);


public:
    // Softbody 정점 추출
    HRESULT Extract_SoftbodyData( const vector<CEditor_Bone*>& Bones, const CLOTH_BONE_CONFIG& Config, SOFTBODY_EXTRACT_DATA& outData);

    // 기본 천 설정으로 추출
    HRESULT Extract_ClothSoftbodyData(const vector<CEditor_Bone*>& Bones, SOFTBODY_EXTRACT_DATA& outData);

private:
    // 뼈 이름이 패턴과 일치하는지 확인
    _bool MatchBonePattern(const char* pBoneName, const vector<string>& patterns);

private:
	_char					m_szName[MAX_PATH] = {};
	_uint					m_iMaterialIndex = {};
	_uint					m_iNumBones = {};
	vector<_int>			m_BoneIndices;
	_float4x4				m_BoneMatrices[g_iMaxNumBones] = {};
	vector<_float4x4>		m_OffsetMatrices;

	MESH_DATA				m_Mesh_Data = {};

private:
	HRESULT  Ready_Vertices_For_NonAnim(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);
	HRESULT  Ready_Vertices_For_Anim(const aiMesh* pAIMesh, const vector<CEditor_Bone*>& Bones); 


public:
	static CEditor_Mesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eType, const aiMesh* pAIMesh, const vector<class CEditor_Bone*>& Bones, _fmatrix PreTransformMatrix);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END