#pragma once

#include "VIBuffer_Instance.h"

// 메쉬 인스턴싱용 클래스
// 기존에 쓰던거에서 안쓰는 Bone 파라미터 삭제, PreTransformMatrix 는 Identity, 필요한거는 추가해주셈 ㅎ;

NS_BEGIN(Engine)

class ENGINE_DLL CMesh_Instance final : public CVIBuffer_Instance
{
public:
	typedef struct tagMeshInstance : public CVIBuffer_Instance::INSTANCE_DESC
	{

	}MESH_INSTANCE_DESC;

private:
	CMesh_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMesh_Instance(const CMesh_Instance& Prototype);
	virtual ~CMesh_Instance() = default;

public:
	virtual HRESULT Initialize_Prototype(const aiMesh* pAIMesh, const MESH_INSTANCE_DESC* pDesc, _fmatrix PreTransformMatrix);
	virtual HRESULT Initialize_Clone(void* pArg) override;

public:
	const _uint Get_MaterialIndex() const { return m_iMaterialIndex; }

public:
	const _uint Get_NumInstances() const { return m_iNumInstance; }

	void Add_Instance(MESH_INSTANCE_DATA InstanceData);									// 억지로 되게 한거라 나중에 수정해주시면 됨 ( 버퍼 삭제 > 재할당 )
	void Fix_Instance(MESH_INSTANCE_DATA InstanceData, _uint iInstanceIndex);			// 이것도 개별로 수정해주면 될거같음 지우거나

private:
	_char m_szName[MAX_PATH] = {};
	_uint m_iMaterialIndex = {};

private:
	HRESULT Ready_Vertices(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix);			// Vertices 세팅

	HRESULT Ready_Indices_For_2Byte(const aiMesh* pAIMesh);								// 2 Bytes Indices 세팅
	HRESULT Ready_Indices_For_4Byte(const aiMesh* pAIMesh);								// 4 Bytes Indices 세팅

public:
	static CMesh_Instance* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiMesh* pAIMesh, const MESH_INSTANCE_DESC* pDesc, _fmatrix PreTransformMatrix = XMMatrixIdentity());
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

NS_END