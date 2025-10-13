#include "Mesh_Instance.h"

#include "GameInstance.h"

#include "Shader.h"

CMesh_Instance::CMesh_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instance { pDevice, pContext }
{
}

CMesh_Instance::CMesh_Instance(const CMesh_Instance& Prototype)
	: CVIBuffer_Instance { Prototype }
{
}

HRESULT CMesh_Instance::Initialize_Prototype(const aiMesh* pAIMesh, const INSTANCE_DESC* pDesc, _fmatrix PreTransformMatrix)
{
	const MESH_INSTANCE_DESC* pMeshDesc = static_cast<const MESH_INSTANCE_DESC*>(pDesc);

	m_iNumVertexBuffers = 2;

	strcpy_s(m_szName, pAIMesh->mName.data);

	m_iMaterialIndex = pAIMesh->mMaterialIndex;

	m_iNumVertices = pAIMesh->mNumVertices;

	m_iNumIndices = pAIMesh->mNumFaces * 3;

	// Vertex 개수에 따라 인덱스 사이즈 설정 ( 적으면 용량 덜 먹게 할려고 한거 )
	if (m_iNumVertices <= USHRT_MAX)
	{
		m_iIndexStride = 2;
		m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	}
	else
	{
		m_iIndexStride = 4;
		m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	}

	m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// 인스턴스용 데이터 세팅
	m_iNumInstance = pMeshDesc->iNumInstance;
	m_iNumIndexPerInstance = m_iNumIndices;
	m_iInstanceVertexStride = sizeof(VTXINSTANCE_MESH);

#pragma region VERTEX_SETTING

	CHECK_FAILED_MSG(Ready_Vertices(pAIMesh, PreTransformMatrix), TEXT("Mesh Instance - Ready Vertices 실패"), E_FAIL);

#pragma endregion

#pragma region INDEX_SETTING

	HRESULT hr = m_iIndexStride == sizeof(_ushort) ? Ready_Indices_For_2Byte(pAIMesh) : Ready_Indices_For_4Byte(pAIMesh);

	CHECK_FAILED_MSG(hr, TEXT("Mesh Instance - Ready Indices 실패"), E_FAIL);

#pragma endregion

#pragma region INSTANCING_SETTING

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

		// VIBuffer_Instance_Rect 꺼 배껴s놓고 주석
		/*
		_float		fScale = m_pGameInstance->Rand(pRectDesc->vSize.x, pRectDesc->vSize.y);
		_float		fLifeTime = m_pGameInstance->Rand(pRectDesc->vLifeTime.x, pRectDesc->vLifeTime.y);
		m_pSpeeds[i] = m_pGameInstance->Rand(pRectDesc->vSpeed.x, pRectDesc->vSpeed.y);

		pInstanceVertices[i].vRight = _float4(fScale, 0.f, 0.f, 0.f);
		pInstanceVertices[i].vUp = _float4(0.f, fScale, 0.f, 0.f);
		pInstanceVertices[i].vLook = _float4(0.f, 0.f, fScale, 0.f);
		pInstanceVertices[i].vTranslation = _float4(
			m_pGameInstance->Rand(pRectDesc->vCenter.x - pRectDesc->vRange.x * 0.5f, pRectDesc->vCenter.x + pRectDesc->vRange.x * 0.5f),
			m_pGameInstance->Rand(pRectDesc->vCenter.y - pRectDesc->vRange.y * 0.5f, pRectDesc->vCenter.y + pRectDesc->vRange.y * 0.5f),
			m_pGameInstance->Rand(pRectDesc->vCenter.z - pRectDesc->vRange.z * 0.5f, pRectDesc->vCenter.z + pRectDesc->vRange.z * 0.5f),
			1.f
		);

		pInstanceVertices[i].vLifeTime = _float2(0.f, fLifeTime);

		*/
	}

#pragma endregion

	return S_OK;
}

HRESULT CMesh_Instance::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}

void CMesh_Instance::Add_Instance(MESH_INSTANCE_DATA InstanceData)
{
	VTXINSTANCE_MESH* pNewInstance = new VTXINSTANCE_MESH[m_iNumInstance + 1];
	ZeroMemory(pNewInstance, sizeof(VTXINSTANCE_MESH) * (m_iNumInstance + 1));

	VTXINSTANCE_MESH* pVertices = reinterpret_cast<VTXINSTANCE_MESH*>(m_pInstanceVertices);
	//CHECK_NULLPTR_MSG(pVertices, TEXT("Mesh_Instance > Add_Instance 함수 > nullptr == pVertices"));

	memcpy(pNewInstance, pVertices, sizeof(VTXINSTANCE_MESH) * m_iNumInstance);

	Safe_Delete_Array(m_pInstanceVertices);

	_matrix MatrixData = InstanceData.InstanceMatrix;

	XMStoreFloat4(&pNewInstance[m_iNumInstance].vRight, XMVectorSetW(MatrixData.r[0], 0.f));
	XMStoreFloat4(&pNewInstance[m_iNumInstance].vUp, XMVectorSetW(MatrixData.r[1], 0.f));
	XMStoreFloat4(&pNewInstance[m_iNumInstance].vLook, XMVectorSetW(MatrixData.r[2], 0.f));
	XMStoreFloat4(&pNewInstance[m_iNumInstance].vTranslation, XMVectorSetW(MatrixData.r[3], 1.f));

	pNewInstance->iID = InstanceData.InstanceID;

	++m_iNumInstance;

	m_pInstanceVertices = pNewInstance;

	Safe_Release(m_pVBInstance);

	D3D11_SUBRESOURCE_DATA	InitialDesc{};
	InitialDesc.pSysMem = m_pInstanceVertices;

	m_VBInstanceDesc.ByteWidth = m_iNumInstance * m_iInstanceVertexStride;

	if (FAILED(m_pDevice->CreateBuffer(&m_VBInstanceDesc, &InitialDesc, &m_pVBInstance)))
		return;
}

void CMesh_Instance::Fix_Instance(MESH_INSTANCE_DATA InstanceData, _uint iInstanceIndex)
{
	D3D11_MAPPED_SUBRESOURCE SubResource = {};

	VTXINSTANCE_MESH* pInstanceVertices = static_cast<VTXINSTANCE_MESH*>(m_pInstanceVertices);
	CHECK_NULLPTR_MSG(pInstanceVertices, TEXT("Mesh_Instance > Fix_Instance 함수 > nullptr == pInstanceVertices"), );

	CHECK_FAILED_MSG(m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource), TEXT("Mesh Instance - Map 실패"), );

	VTXINSTANCE_MESH* pVertices = static_cast<VTXINSTANCE_MESH*>(SubResource.pData);
	CHECK_NULLPTR_MSG(pVertices, TEXT("Mesh_Instance > Fix_Instance 함수 > nullptr == pVertices"), );

	_matrix FixMatrixData = InstanceData.InstanceMatrix;

	XMStoreFloat4(&pVertices[iInstanceIndex - 1].vRight, XMVectorSetW(FixMatrixData.r[0], 0.f));
	XMStoreFloat4(&pVertices[iInstanceIndex - 1].vUp, XMVectorSetW(FixMatrixData.r[1], 0.f));
	XMStoreFloat4(&pVertices[iInstanceIndex - 1].vLook, XMVectorSetW(FixMatrixData.r[2], 0.f));
	XMStoreFloat4(&pVertices[iInstanceIndex - 1].vTranslation, XMVectorSetW(FixMatrixData.r[3], 1.f));

	pInstanceVertices[iInstanceIndex - 1] = pVertices[iInstanceIndex - 1];

	m_pContext->Unmap(m_pVBInstance, 0);
}

HRESULT CMesh_Instance::Ready_Vertices(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
{
	m_iVertexStride = sizeof(VTXMESH);

	VTXMESH* pVertices = new VTXMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXMESH) * m_iNumVertices);

	m_pVertexPositions = new _float3[m_iNumVertices];
	ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));

		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PreTransformMatrix));

		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vTangent, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vTangent), PreTransformMatrix));

		memcpy(&pVertices[i].vBinormal, &pAIMesh->mBitangents[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vBinormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vBinormal), PreTransformMatrix));

		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));

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

HRESULT CMesh_Instance::Ready_Indices_For_2Byte(const aiMesh* pAIMesh)
{
	_ushort* pIndices = new _ushort[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_ushort) * m_iNumIndices);

	_uint	iNumIndices = {};

	for (_uint i = 0; i < pAIMesh->mNumFaces; ++i)
	{
		aiFace AIFace = pAIMesh->mFaces[i];

		pIndices[iNumIndices++] = AIFace.mIndices[0];
		pIndices[iNumIndices++] = AIFace.mIndices[1];
		pIndices[iNumIndices++] = AIFace.mIndices[2];
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

	return S_OK;
}

HRESULT CMesh_Instance::Ready_Indices_For_4Byte(const aiMesh* pAIMesh)
{
	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint	iNumIndices = {};

	for (_uint i = 0; i < pAIMesh->mNumFaces; ++i)
	{
		aiFace AIFace = pAIMesh->mFaces[i];

		pIndices[iNumIndices++] = AIFace.mIndices[0];
		pIndices[iNumIndices++] = AIFace.mIndices[1];
		pIndices[iNumIndices++] = AIFace.mIndices[2];
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

	return S_OK;
}

CMesh_Instance* CMesh_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const aiMesh* pAIMesh, const INSTANCE_DESC* pDesc, _fmatrix PreTransformMatrix)
{
	CMesh_Instance* pInstance = new CMesh_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pAIMesh, pDesc, PreTransformMatrix)))
	{
		MSG_BOX(TEXT("Failed to Created : CMesh_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CMesh_Instance::Clone(void* pArg)
{
	CMesh_Instance* pInstance = new CMesh_Instance(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CMesh_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CMesh_Instance::Free()
{
	__super::Free();
}
