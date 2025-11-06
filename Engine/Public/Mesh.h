#pragma once

#include "VIBuffer.h"

NS_BEGIN(Engine)

class ENGINE_DLL CMesh final : public CVIBuffer
{
private:
	CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh(const CMesh& Prototype);
	virtual ~CMesh() = default;


public:
	virtual HRESULT Initialize_Prototype(MODELTYPE eType, _fmatrix PreTransformMatrix, MESH_DATA& data);
	virtual HRESULT Initialize_Clone(void* pArg) override;

public:
	_uint			Get_MaterialIndex() const { return m_iMaterialIndex; }
	vector<_float3> Get_VerticesPos() { return m_vVerticesPos; }
	vector<_uint>	Get_Indices() { return m_vIndices; }

	//파츠용 
	void			Build_BoneNameList(const vector<class CBone*>& Bones);
	void			Build_MasterBoneCache(const vector<class CBone*>& MasterBones);
	void			Build_FallbackBoneCache(const vector<class CBone*>& PartBones, const vector<class CBone*>& MasterBones);

public:
	HRESULT Bind_BoneMatrices(class CShader* pShader, const _char* pConstantName, const vector<class CBone*>& Bones);

private:
	_wstring				m_strName;
	_uint					m_iMaterialIndex = {};
	_uint					m_iNumBones = {};

	vector<_int>			m_BoneIndices;	
	_float4x4				m_BoneMatrices[g_iMaxNumBones] = {};
	vector<_float4x4>		m_OffsetMatrices;
	vector<_float3>			m_vVerticesPos;
	vector<_uint>			m_vIndices;
	_matrix					m_PreTransformMatrix = {};

	/* 파츠용 */
	vector<_wstring>		m_BoneNames;
	vector<_int>            m_MasterBoneCache;  // 캐시- 마스터 본 인덱스
	vector<_int>			m_FallbackBoneCache; // 파츠 전용으로 존재하는 인덱스들
	//vector<_bool>           m_IsStaticBone;  // 고정시킬 본 인덱스 (일단 물리 줘야하는것들 이것으로 대체)
	_bool                   m_isBoneIndicesCached = { false };


	///* 고정 시킬 본 이름*/
	//inline static const vector<_wstring> staticBonePatterns = {
	//	L"Cloth_",
	//	L"Hair_BoneRoot",
	//	L"_HBR",
	//};
private:
	HRESULT  Ready_Vertices_For_NonAnim(MESH_DATA& data);
	HRESULT Ready_Vertices_For_Anim(MESH_DATA& data);

public:
	static CMesh* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eType, _fmatrix PreTransformMatrix, MESH_DATA& data);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END
