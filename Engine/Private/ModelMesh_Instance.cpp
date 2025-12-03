#include "ModelMesh_Instance.h"

#include "GameInstance.h"

#include "Bone.h"
#include "Shader.h"

CModelMesh_Instance::CModelMesh_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instance { pDevice, pContext }
{
}

CModelMesh_Instance::CModelMesh_Instance(const CModelMesh_Instance& Prototype)
	: CVIBuffer_Instance { Prototype }
{
}

HRESULT CModelMesh_Instance::Initialize_Prototype(MODELTYPE eType, _fmatrix PreTransformMatrix, MESH_DATA& Data, const INSTANCE_DESC* pDesc)
{
	const MODELMESH_INSTANCE_DESC* pModelMeshDesc = static_cast<const MODELMESH_INSTANCE_DESC*>(pDesc);

	m_iNumVertexBuffers = 2;

	m_strName = AnsiToWString(Data.strName);
	m_iMaterialIndex = Data.iMaterialIndex;

	m_iNumVertices = Data.iNumVertices;
	m_iNumIndices = Data.iNumIndices;
	m_iIndexStride = Data.iIndexStride;

	m_iIndexStride = 4;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;

	m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_iNumInstance = pModelMeshDesc->iNumInstance;
	m_iNumIndexPerInstance = m_iNumIndices;
	m_iInstanceVertexStride = sizeof(VTXINSTANCE_MESH);

#pragma region VERTEX_SETTING

	HRESULT hr = MODELTYPE::NONANIM == eType ? Ready_Vertices_For_NonAnim(Data) : Ready_Vertices_For_Anim(Data);

	CHECK_FAILED_MSG(hr, TEXT("Mesh Instance - Ready Vertices 실패"), E_FAIL);

#pragma endregion

#pragma region INDEX_SETTING

	CHECK_FAILED_MSG(Ready_Indices(Data), TEXT("Mesh Instance - Ready Indices 실패"), E_FAIL);

#pragma endregion

#pragma region INSTANCING_SETTING

	m_VBInstanceDesc.ByteWidth = m_iNumInstance * m_iInstanceVertexStride;
	m_VBInstanceDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_VBInstanceDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBInstanceDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_VBInstanceDesc.MiscFlags = 0;
	m_VBInstanceDesc.StructureByteStride = m_iInstanceVertexStride;

	m_pInstanceVertices = new VTXINSTANCE_MESH[m_iNumInstance];
	ZeroMemory(m_pInstanceVertices, sizeof(VTXINSTANCE_MESH) * m_iNumInstance);

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		VTXINSTANCE_MESH* pInstanceVertices = static_cast<VTXINSTANCE_MESH*>(m_pInstanceVertices);
		CHECK_NULLPTR_MSG(pInstanceVertices, TEXT("Mesh Instance - nullptr == pInstanceVertices"), E_FAIL);

		XMStoreFloat4(&pInstanceVertices[i].vRight, XMLoadFloat4(&pModelMeshDesc->InstanceData[i].vRight));
		XMStoreFloat4(&pInstanceVertices[i].vUp, XMLoadFloat4(&pModelMeshDesc->InstanceData[i].vUp));
		XMStoreFloat4(&pInstanceVertices[i].vLook, XMLoadFloat4(&pModelMeshDesc->InstanceData[i].vLook));
		XMStoreFloat4(&pInstanceVertices[i].vTranslation, XMLoadFloat4(&pModelMeshDesc->InstanceData[i].vTranslation));

		pInstanceVertices[i].iID = pModelMeshDesc->InstanceData[i].iID;
	}

#pragma endregion

	return S_OK;
}

HRESULT CModelMesh_Instance::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}

HRESULT CModelMesh_Instance::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, const vector<class CBone*>& Bones)
{
	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		XMStoreFloat4x4(&m_BoneMatrices[i], XMLoadFloat4x4(&m_OffsetMatrices[i]) * Bones[m_BoneIndices[i]]->Get_CombinedTransformationMatrix());
	}

	return pShader->Bind_Matrices(pConstantName, m_BoneMatrices, m_iNumBones);
}

HRESULT CModelMesh_Instance::Ready_Vertices_For_NonAnim(MESH_DATA& Data)
{
	m_iVertexStride = sizeof(VTXMESH);

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	m_pVertexPositions = new _float3[m_iNumVertices];
	ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &Data.vecVertices[i].position, sizeof(_float3));

		memcpy(&pVertices[i].vNormal, &Data.vecVertices[i].normal, sizeof(_float3));

		memcpy(&pVertices[i].vTangent, &Data.vecVertices[i].tangent, sizeof(_float3));

		memcpy(&pVertices[i].vBinormal, &Data.vecVertices[i].binormal, sizeof(_float3));

		memcpy(&pVertices[i].vTexcoord, &Data.vecVertices[i].texcoord, sizeof(_float2));

		m_pVertexPositions[i] = pVertices[i].vPosition;
	}

	D3D11_BUFFER_DESC		VBDesc = {};

	VBDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	D3D11_SUBRESOURCE_DATA	VBInitialData = {};
	VBInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &VBInitialData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CModelMesh_Instance::Ready_Vertices_For_Anim(MESH_DATA& Data)
{
	m_iVertexStride = sizeof(VTXANIMMESH);

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	m_pVertexPositions = new _float3[m_iNumVertices];
	ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &Data.vecVertices[i].position, sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &Data.vecVertices[i].normal, sizeof(_float3));
		memcpy(&pVertices[i].vTangent, &Data.vecVertices[i].tangent, sizeof(_float3));
		memcpy(&pVertices[i].vBinormal, &Data.vecVertices[i].binormal, sizeof(_float3));
		memcpy(&pVertices[i].vBlendIndex, &Data.vecVertices[i].blendIndex, sizeof(_uint) * 4);
		memcpy(&pVertices[i].vBlendWeight, &Data.vecVertices[i].blendWeight, sizeof(_float4));
		memcpy(&pVertices[i].vTexcoord, &Data.vecVertices[i].texcoord, sizeof(_float2));

		m_pVertexPositions[i] = pVertices[i].vPosition;
	}

	m_iNumBones = Data.iNumBones;

	m_BoneIndices = Data.vecBoneIndices;

	m_OffsetMatrices.resize(m_iNumBones);
	memcpy(m_OffsetMatrices.data(), Data.vecOffsetMatrices.data(), sizeof(_float4x4) * m_iNumBones);

	D3D11_BUFFER_DESC		VBDesc = {};
	VBDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	D3D11_SUBRESOURCE_DATA	VBInitialData = {};
	VBInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &VBInitialData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CModelMesh_Instance::Ready_Indices(MESH_DATA& Data)
{
	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint	iNumIndices = {};

	for (_uint i = 0; i < Data.iNumFace; ++i)
	{
		pIndices[iNumIndices++] = Data.vecIndices[i].x;
		pIndices[iNumIndices++] = Data.vecIndices[i].y;
		pIndices[iNumIndices++] = Data.vecIndices[i].z;
	}

	D3D11_BUFFER_DESC		IBDesc = {};
	IBDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
	IBDesc.Usage = D3D11_USAGE_DEFAULT;
	IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBDesc.CPUAccessFlags = 0;
	IBDesc.MiscFlags = 0;
	IBDesc.StructureByteStride = m_iIndexStride;

	D3D11_SUBRESOURCE_DATA	IBInitialData = {};
	IBInitialData.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &IBInitialData, &m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

	return S_OK;
}

CModelMesh_Instance* CModelMesh_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eType, _fmatrix PreTransformMatrix, MESH_DATA& Data, const INSTANCE_DESC* pDesc)
{
	CModelMesh_Instance* pInstance = new CModelMesh_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, PreTransformMatrix, Data, pDesc)))
	{
		MSG_BOX(TEXT("Failed to Created : CModelMesh_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CModelMesh_Instance::Clone(void* pArg)
{
	CModelMesh_Instance* pInstance = new CModelMesh_Instance(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CModelMesh_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModelMesh_Instance::Free()
{
	__super::Free();
}
