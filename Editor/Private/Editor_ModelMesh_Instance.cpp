#include "Editor_ModelMesh_Instance.h"

#include "GameInstance.h"

#include "Editor_Bone.h"

CEditor_ModelMesh_Instance::CEditor_ModelMesh_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instance { pDevice, pContext }
{
}

CEditor_ModelMesh_Instance::CEditor_ModelMesh_Instance(const CEditor_ModelMesh_Instance& Prototype)
	: CVIBuffer_Instance { Prototype }
	, m_Mesh_Data { Prototype.m_Mesh_Data }
{
}

HRESULT CEditor_ModelMesh_Instance::Initialize_Prototype(MODELTYPE eType, const aiMesh* pAIMesh, const vector<CEditor_Bone*>& Bones, _fmatrix PreTransformMatrix, const INSTANCE_DESC* pDesc)
{
	const EDITOR_MODELMESH_INSTANCE_DESC* pModelMeshDesc = static_cast<const EDITOR_MODELMESH_INSTANCE_DESC*>(pDesc);

	m_iNumVertexBuffers = 2;

	strcpy_s(m_szName, pAIMesh->mName.data);

	m_iMaterialIndex = pAIMesh->mMaterialIndex;

	m_iNumVertices = pAIMesh->mNumVertices;

	m_iNumIndices = pAIMesh->mNumFaces * 3;

	m_iIndexStride = 4;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;

	m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_iNumInstance = pModelMeshDesc->iNumInstance;
	m_iNumIndexPerInstance = m_iNumIndices;
	m_iInstanceVertexStride = sizeof(VTXINSTANCE_MESH);

#pragma region VERTEX_SETTING

	HRESULT hr = MODELTYPE::NONANIM == eType ?
		Ready_Vertices_For_NonAnim(pAIMesh, PreTransformMatrix) : Ready_Vertices_For_Anim(pAIMesh, Bones);

	CHECK_FAILED_MSG(hr, TEXT("Mesh Instance - Ready Vertices 실패"), E_FAIL);

#pragma endregion

#pragma region INDEX_SETTING

	CHECK_FAILED_MSG(Ready_Indices(pAIMesh), TEXT("Mesh Instance - Ready Indices 실패"), E_FAIL);

#pragma endregion

#pragma region 파일 입출력용

	m_Mesh_Data.iMaterialIndex		= m_iMaterialIndex;
	m_Mesh_Data.iNumVertices		= m_iNumVertices;
	m_Mesh_Data.iVertexStride		= m_iVertexStride;
	m_Mesh_Data.iNumIndices			= m_iNumIndices;
	m_Mesh_Data.iIndexStride		= m_iIndexStride;
	m_Mesh_Data.iNumVertexBuffers	= m_iNumVertexBuffers;
	m_Mesh_Data.iIndexFormat		= m_eIndexFormat;
	m_Mesh_Data.iPrimitiveType		= m_ePrimitiveType;
	m_Mesh_Data.iNumFace			= pAIMesh->mNumFaces;
	m_Mesh_Data.strName				= string(m_szName);

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

HRESULT CEditor_ModelMesh_Instance::Initialize_Clone(void* pArg)
{
	CHECK_EQUAL(0, m_iNumInstance, S_OK);

	CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

	return S_OK;
}

HRESULT CEditor_ModelMesh_Instance::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, const vector<CEditor_Bone*>& Bones)
{
	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		XMStoreFloat4x4(&m_BoneMatrices[i], XMLoadFloat4x4(&m_OffsetMatrices[i]) * Bones[m_BoneIndices[i]]->Get_CombinedTransformationMatrix());
	}

	return pShader->Bind_Matrices(pConstantName, m_BoneMatrices, m_iNumBones);
}

HRESULT CEditor_ModelMesh_Instance::Ready_Vertices_For_NonAnim(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
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

HRESULT CEditor_ModelMesh_Instance::Ready_Vertices_For_Anim(const aiMesh* pAIMesh, const vector<CEditor_Bone*>& Bones)
{
	m_iVertexStride = sizeof(VTXANIMMESH);

	VTXANIMMESH* pVertices = new VTXANIMMESH[m_iNumVertices];
	ZeroMemory(pVertices, sizeof(VTXANIMMESH) * m_iNumVertices);

	m_pVertexPositions = new _float3[m_iNumVertices];
	ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));

		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));

		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));

		memcpy(&pVertices[i].vBinormal, &pAIMesh->mBitangents[i], sizeof(_float3));

		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));

		m_pVertexPositions[i] = pVertices[i].vPosition;
	}

	// 이 매쉬에 영향을 주는 뼈들의 갯수
	m_iNumBones = pAIMesh->mNumBones;

	/* 이 메시에 영향을 주는 뼈들을 하나씩 순회할것 */
	for (_uint i = 0; i < m_iNumBones; ++i)
	{
		/* i번째 뼈가 영향을 주는 정점의 갯수 */
		aiBone* pAIBone = pAIMesh->mBones[i];

		_float4x4	OffsetMatrix;

		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));

		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));

		m_OffsetMatrices.push_back(OffsetMatrix);

		_uint	iBoneIndex = { 0 };

		auto	iter = find_if(Bones.begin(), Bones.end(), [&](CEditor_Bone* pBone)->_bool
			{
				if (true == pBone->Compare_Name(pAIBone->mName.data))
					return true;

				iBoneIndex++;

				return false;
			});

		m_BoneIndices.push_back(iBoneIndex);

		for (_uint j = 0; j < pAIBone->mNumWeights; ++j)
		{
			aiVertexWeight	AIVertexWeight = pAIBone->mWeights[j];

			/* i번째 뼈가 영향을 주는 j번째 정점의 정점버퍼상의 인덱스 */
			if (0.f == pVertices[AIVertexWeight.mVertexId].vBlendWeight.x)
			{
				pVertices[AIVertexWeight.mVertexId].vBlendIndex.x = i;
				pVertices[AIVertexWeight.mVertexId].vBlendWeight.x = AIVertexWeight.mWeight;
			}
			else if (0.f == pVertices[AIVertexWeight.mVertexId].vBlendWeight.y)
			{
				pVertices[AIVertexWeight.mVertexId].vBlendIndex.y = i;
				pVertices[AIVertexWeight.mVertexId].vBlendWeight.y = AIVertexWeight.mWeight;
			}
			else if (0.f == pVertices[AIVertexWeight.mVertexId].vBlendWeight.z)
			{
				pVertices[AIVertexWeight.mVertexId].vBlendIndex.z = i;
				pVertices[AIVertexWeight.mVertexId].vBlendWeight.z = AIVertexWeight.mWeight;
			}
			else
			{
				pVertices[AIVertexWeight.mVertexId].vBlendIndex.w = i;
				pVertices[AIVertexWeight.mVertexId].vBlendWeight.w = AIVertexWeight.mWeight;
			}
		}
	}

	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		_uint	iBoneIndex = { 0 };

		auto	iter = find_if(Bones.begin(), Bones.end(), [&](CEditor_Bone* pBone)->_bool
			{
				if (true == pBone->Compare_Name(m_szName))
					return true;

				iBoneIndex++;

				return false;
			});

		m_BoneIndices.push_back(iBoneIndex);

		_float4x4		OffsetMatrix;
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

		m_OffsetMatrices.push_back(OffsetMatrix);
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

HRESULT CEditor_ModelMesh_Instance::Ready_Indices(const aiMesh* pAIMesh)
{
	_uint* pIndices = new _uint[m_iNumIndices];
	ZeroMemory(pIndices, sizeof(_uint) * m_iNumIndices);

	_uint	iNumIndices = {};

	for (_uint i = 0; i < pAIMesh->mNumFaces; ++i)
	{
		aiFace AIFace = pAIMesh->mFaces[i];
		UINT3_DATA Temp = {};

		Temp.x = pIndices[iNumIndices++] = AIFace.mIndices[0];
		Temp.y = pIndices[iNumIndices++] = AIFace.mIndices[1];
		Temp.z = pIndices[iNumIndices++] = AIFace.mIndices[2];

		m_Mesh_Data.vecIndices.push_back(Temp);
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

CEditor_ModelMesh_Instance* CEditor_ModelMesh_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eType, const aiMesh* pAIMesh, const vector<CEditor_Bone*>& Bones, _fmatrix PreTransformMatrix, const INSTANCE_DESC* pDesc)
{
	CEditor_ModelMesh_Instance* pInstance = new CEditor_ModelMesh_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pAIMesh, Bones, PreTransformMatrix, pDesc)))
	{
		MSG_BOX(TEXT("Failed to Created : CEditor_ModelMesh_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CEditor_ModelMesh_Instance::Clone(void* pArg)
{
	CEditor_ModelMesh_Instance* pInstance = new CEditor_ModelMesh_Instance(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEditor_ModelMesh_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEditor_ModelMesh_Instance::Free()
{
	__super::Free();
}
