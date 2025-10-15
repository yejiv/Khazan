
#include "Mesh.h"
#include "Bone.h"
#include "Shader.h"

CMesh::CMesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer { pDevice, pContext }
{
}

CMesh::CMesh(const CMesh& Prototype)
	: CVIBuffer{ Prototype }
{
}

HRESULT CMesh::Initialize_Prototype(MODELTYPE eType, _fmatrix PreTransformMatrix, MESH_DATA& data)
{
	m_strName = AnsiToWString(data.strName);
	m_iMaterialIndex = data.iMaterialIndex;

	m_iNumVertices = data.iNumVertices;
	m_iNumIndices = data.iNumIndices;
	m_iIndexStride = data.iIndexStride;
	m_iNumVertexBuffers = data.iNumVertexBuffers;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	HRESULT hr = eType == MODELTYPE::NONANIM ?
		Ready_Vertices_For_NonAnim(data) :
		Ready_Vertices_For_Anim(data);

	if (FAILED(hr))
		return E_FAIL;


	D3D11_BUFFER_DESC		IBDesc{};
	IBDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
	IBDesc.Usage = D3D11_USAGE_DEFAULT;
	IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBDesc.CPUAccessFlags = 0;
	IBDesc.MiscFlags = 0;
	IBDesc.StructureByteStride = m_iIndexStride;

	_uint* pIndices = new _uint[m_iNumIndices];

	_uint	iNumIndices = {};

	m_vIndices.reserve(data.iNumFace * 3);

	for (size_t i = 0; i < data.iNumFace; i++)
	{
		pIndices[iNumIndices++] = data.vecIndices[i].x;
		m_vIndices.push_back(data.vecIndices[i].x);
		pIndices[iNumIndices++] = data.vecIndices[i].y;
		m_vIndices.push_back(data.vecIndices[i].y);
		pIndices[iNumIndices++] = data.vecIndices[i].z;
		m_vIndices.push_back(data.vecIndices[i].z);
	}

	D3D11_SUBRESOURCE_DATA	IBInitialData{};
	IBInitialData.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &IBInitialData, &m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CMesh::Initialize_Clone(void* pArg)
{
	return S_OK;
}

HRESULT CMesh::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, const vector<class CBone*>& Bones)
{
	for (size_t i = 0; i < m_iNumBones; i++)
	{
		XMStoreFloat4x4(&m_BoneMatrices[i],
			XMLoadFloat4x4(&m_OffsetMatrices[i]) * Bones[m_BoneIndices[i]]->Get_CombinedTransformationMatrix());
	}

	return pShader->Bind_Matrices(pConstantName, m_BoneMatrices, m_iNumBones);
}


HRESULT CMesh::Ready_Vertices_For_NonAnim(MESH_DATA& data)
{
	m_iVertexStride = sizeof(VTXMESH);

	D3D11_BUFFER_DESC		VBDesc{};
	VBDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	m_vVerticesPos.reserve(m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &data.vecVertices[i].position, sizeof(_float3));
		m_vVerticesPos.push_back(pVertices[i].vPosition);
		memcpy(&pVertices[i].vNormal, &data.vecVertices[i].normal, sizeof(_float3));
		memcpy(&pVertices[i].vTangent, &data.vecVertices[i].tangent, sizeof(_float3));
		memcpy(&pVertices[i].vBinormal, &data.vecVertices[i].binormal, sizeof(_float3));
		memcpy(&pVertices[i].vTexcoord, &data.vecVertices[i].texcoord, sizeof(_float2));
	}

	D3D11_SUBRESOURCE_DATA	VBInitialData{};
	VBInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &VBInitialData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CMesh::Ready_Vertices_For_Anim(MESH_DATA& data)
{
	m_iVertexStride = sizeof(VTXANIMMESH);

	D3D11_BUFFER_DESC		VBDesc{};
	VBDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &data.vecVertices[i].position, sizeof(_float3));
		m_vVerticesPos.push_back(pVertices[i].vPosition);
		memcpy(&pVertices[i].vNormal, &data.vecVertices[i].normal, sizeof(_float3));
		memcpy(&pVertices[i].vTangent, &data.vecVertices[i].tangent, sizeof(_float3));
		memcpy(&pVertices[i].vBinormal, &data.vecVertices[i].binormal, sizeof(_float3));
		memcpy(&pVertices[i].vBlendIndex, &data.vecVertices[i].blendIndex, sizeof(_uint) * 4);
		memcpy(&pVertices[i].vBlendWeight, &data.vecVertices[i].blendWeight, sizeof(_float4));
		memcpy(&pVertices[i].vTexcoord, &data.vecVertices[i].texcoord, sizeof(_float2));
	}

	m_iNumBones = data.iNumBones;

	m_BoneIndices = data.vecBoneIndices;

	m_OffsetMatrices.resize(m_iNumBones);
	memcpy(m_OffsetMatrices.data(), data.vecOffsetMatrices.data(),
		sizeof(_float4x4) * m_iNumBones);

	D3D11_SUBRESOURCE_DATA	VBInitialData{};
	VBInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &VBInitialData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}


CMesh* CMesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eType, _fmatrix PreTransformMatrix, MESH_DATA& data)
{
	CMesh* pInstance = new CMesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, PreTransformMatrix, data)))
	{
		MSG_BOX(TEXT("Failed to Created : CMesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CMesh::Clone(void* pArg)
{
	CMesh* pInstance = new CMesh(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMesh::Free()
{
	__super::Free();


}
