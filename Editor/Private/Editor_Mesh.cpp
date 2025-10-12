#include "EditorPch.h"
#include "Editor_Mesh.h"
#include "GameInstance.h"
#include "Editor_Bone.h"


CEditor_Mesh::CEditor_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer{ pDevice, pContext }
{
}

CEditor_Mesh::CEditor_Mesh(const CEditor_Mesh& Prototype)
	: CVIBuffer{ Prototype }
	, m_Mesh_Data {Prototype.m_Mesh_Data }
{

}

HRESULT CEditor_Mesh::Initialize_Prototype(MODELTYPE eType, const aiMesh* pAIMesh, const vector<class CEditor_Bone*>& Bones, _fmatrix PreTransformMatrix)
{
	strcpy_s(m_szName, pAIMesh->mName.data);
	m_iMaterialIndex = pAIMesh->mMaterialIndex;
	m_iNumVertices = pAIMesh->mNumVertices;
	m_iNumIndices = pAIMesh->mNumFaces * 3;
	m_iIndexStride = 4;
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	HRESULT hr = eType == MODELTYPE::NONANIM ?
		Ready_Vertices_For_NonAnim(pAIMesh, PreTransformMatrix) :
		Ready_Vertices_For_Anim(pAIMesh, Bones);

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

	_uint		iNumIndices = {};

	for (size_t i = 0; i < pAIMesh->mNumFaces; i++)
	{
		aiFace	AIFace = pAIMesh->mFaces[i];

		pIndices[iNumIndices++] = AIFace.mIndices[0];
		pIndices[iNumIndices++] = AIFace.mIndices[1];
		pIndices[iNumIndices++] = AIFace.mIndices[2];

		UINT3_DATA temp;
		temp.x = AIFace.mIndices[0];  //pIndices[iNumIndices - 3];
		temp.y = AIFace.mIndices[1];  //pIndices[iNumIndices - 2];
		temp.z = AIFace.mIndices[2];  //pIndices[iNumIndices - 1];
		m_Mesh_Data.vecIndices.push_back(temp);
	}

	D3D11_SUBRESOURCE_DATA	IBInitialData{};
	IBInitialData.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &IBInitialData, &m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);


	m_Mesh_Data.iMaterialIndex = m_iMaterialIndex;
	m_Mesh_Data.iNumVertices = m_iNumVertices;
	m_Mesh_Data.iVertexStride = m_iVertexStride;
	m_Mesh_Data.iNumIndices = m_iNumIndices;
	m_Mesh_Data.iIndexStride = m_iIndexStride;
	m_Mesh_Data.iNumVertexBuffers = m_iNumVertexBuffers;
	m_Mesh_Data.iIndexFormat = m_eIndexFormat;
	m_Mesh_Data.iPrimitiveType = m_ePrimitiveType;
	m_Mesh_Data.iNumFace = pAIMesh->mNumFaces;
	m_Mesh_Data.strName = string(m_szName);

	return S_OK;
}

HRESULT CEditor_Mesh::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CEditor_Mesh::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, const vector<class CEditor_Bone*>& Bones)
{
	for (size_t i = 0; i < m_iNumBones; i++)
	{
		XMStoreFloat4x4(&m_BoneMatrices[i],
			XMLoadFloat4x4(&m_OffsetMatrices[i]) * Bones[m_BoneIndices[i]]->Get_CombinedTransformationMatrix());
	}
	return pShader->Bind_Matrices(pConstantName, m_BoneMatrices, m_iNumBones);
}

//_bool CEditor_Mesh::Is_Picked(CTransform* pTransform, _float3* pOut)
//{
//
//	//_float3*			vLocalPickPos = { pOut };
//	_bool			isPicked = { false };
//
//	/* 레이 로컬까지 보내기  */
//	m_pGameInstance->Transform_ToLocalSpace(pTransform);
//
//	for (size_t i = 0; i < m_Mesh_Data.iNumFace; i++)
//	{
//		//_uint	iIndices[3] = {};
//
//		//_uint	iIndex = i * 3 * m_iIndexStride;
//
//		//memcpy(&iIndices[0], static_cast<_ubyte*>(m_pIndices) + 0 * m_iIndexStride + iIndex, m_iIndexStride);
//		//memcpy(&iIndices[1], static_cast<_ubyte*>(m_pIndices) + 1 * m_iIndexStride + iIndex, m_iIndexStride);
//		//memcpy(&iIndices[2], static_cast<_ubyte*>(m_pIndices) + 2 * m_iIndexStride + iIndex, m_iIndexStride);
//
//		_vector tempA = XMVectorSet(
//			m_Mesh_Data.vecVertices[m_Mesh_Data.vecIndices[i].x].position.x,
//			m_Mesh_Data.vecVertices[m_Mesh_Data.vecIndices[i].x].position.y,
//			m_Mesh_Data.vecVertices[m_Mesh_Data.vecIndices[i].x].position.z, 
//			1.f
//		);
//		_vector tempB = XMVectorSet(
//			m_Mesh_Data.vecVertices[m_Mesh_Data.vecIndices[i].y].position.x,
//			m_Mesh_Data.vecVertices[m_Mesh_Data.vecIndices[i].y].position.y,
//			m_Mesh_Data.vecVertices[m_Mesh_Data.vecIndices[i].y].position.z, 
//			1.f
//		);
//		_vector tempC = XMVectorSet(
//			m_Mesh_Data.vecVertices[m_Mesh_Data.vecIndices[i].z].position.x,
//			m_Mesh_Data.vecVertices[m_Mesh_Data.vecIndices[i].z].position.y,
//			m_Mesh_Data.vecVertices[m_Mesh_Data.vecIndices[i].z].position.z, 
//			1.f
//		);
//
//		//_vector tempA = XMLoadFloat3(&m_pVertexPositions[iIndices[0]]);
//		//_vector tempB = XMLoadFloat3(&m_pVertexPositions[iIndices[1]]);
//		//_vector tempC = XMLoadFloat3(&m_pVertexPositions[iIndices[2]]);
//		isPicked = m_pGameInstance->isPicked_InLocalSpace(tempA, tempB, tempC, pOut);
//		if (isPicked)
//		{
//			_vector worldPos = XMVector3TransformCoord(XMLoadFloat3(pOut), pTransform->Get_WorldMatrix());
//			XMStoreFloat3(pOut, worldPos);
//			break;
//		}
//	}
//	return isPicked;
//}

//_bool CEditor_Mesh::Is_Picked(CTransform* pTransform, _float3* pOut, _float* pMinDistance)
//{
//	_bool isPicked = false;
//	_float minDistance = FLT_MAX;  // 가장 가까운 거리 저장
//	_float3 closestPoint = {};     // 가장 가까운 교차점
//
//	// 레이를 로컬 공간으로 변환
//	m_pGameInstance->Transform_ToLocalSpace(pTransform);
//
//	for (size_t i = 0; i < m_Mesh_Data.iNumFace; i++)
//	{
//		// 삼각형의 세 꼭짓점
//		_vector tempA = XMVectorSet(
//			m_Mesh_Data.vecVertices[m_Mesh_Data.vecIndices[i].x].position.x,
//			m_Mesh_Data.vecVertices[m_Mesh_Data.vecIndices[i].x].position.y,
//			m_Mesh_Data.vecVertices[m_Mesh_Data.vecIndices[i].x].position.z,
//			1.f
//		);
//		_vector tempB = XMVectorSet(
//			m_Mesh_Data.vecVertices[m_Mesh_Data.vecIndices[i].y].position.x,
//			m_Mesh_Data.vecVertices[m_Mesh_Data.vecIndices[i].y].position.y,
//			m_Mesh_Data.vecVertices[m_Mesh_Data.vecIndices[i].y].position.z,
//			1.f
//		);
//		_vector tempC = XMVectorSet(
//			m_Mesh_Data.vecVertices[m_Mesh_Data.vecIndices[i].z].position.x,
//			m_Mesh_Data.vecVertices[m_Mesh_Data.vecIndices[i].z].position.y,
//			m_Mesh_Data.vecVertices[m_Mesh_Data.vecIndices[i].z].position.z,
//			1.f
//		);
//
//		_float3 tempOut = {};
//		_float currentDistance = 0.0f;
//
//		// 현재 삼각형과의 교차점 검사
//		if (m_pGameInstance->isPicked_InLocalSpace(tempA, tempB, tempC, &tempOut, &currentDistance))
//		{
//			// 더 가까운 교차점인지 확인
//			if (currentDistance < minDistance)
//			{
//				minDistance = currentDistance;
//				closestPoint = tempOut;
//				isPicked = true;
//			}
//		}
//	}
//
//	if (isPicked)
//	{
//		// 가장 가까운 점을 월드 좌표로 변환
//		_vector worldPos = XMVector3TransformCoord(XMLoadFloat3(&closestPoint), pTransform->Get_WorldMatrix());
//		XMStoreFloat3(pOut, worldPos);
//
//		if (pMinDistance)
//			*pMinDistance = minDistance;
//	}
//
//	return isPicked;
//}


HRESULT CEditor_Mesh::Ready_Vertices_For_NonAnim(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
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

	for (size_t i = 0; i < m_iNumVertices; i++)
	{
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vPosition, XMVector3TransformCoord(XMLoadFloat3(&pVertices[i].vPosition), PreTransformMatrix));

		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vNormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vNormal), PreTransformMatrix));
		
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
		memcpy(&pVertices[i].vBinormal, &pAIMesh->mBitangents[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));


		MESH_VERTEX_DATA temp;
		temp.position.x = pVertices[i].vPosition.x;
		temp.position.y = pVertices[i].vPosition.y;
		temp.position.z = pVertices[i].vPosition.z;
		temp.normal.x = pVertices[i].vNormal.x;
		temp.normal.y = pVertices[i].vNormal.y;
		temp.normal.z = pVertices[i].vNormal.z;
		temp.tangent.x = pVertices[i].vTangent.x;
		temp.tangent.y = pVertices[i].vTangent.y;
		temp.tangent.z = pVertices[i].vTangent.z;
		temp.binormal.x = pVertices[i].vBinormal.x;
		temp.binormal.y = pVertices[i].vBinormal.y;
		temp.binormal.z = pVertices[i].vBinormal.z;
		temp.texcoord.x = pVertices[i].vTexcoord.x;
		temp.texcoord.y = pVertices[i].vTexcoord.y;

		m_Mesh_Data.vecVertices.push_back(temp);
	}

	D3D11_SUBRESOURCE_DATA	VBInitialData{};
	VBInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &VBInitialData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CEditor_Mesh::Ready_Vertices_For_Anim(const aiMesh* pAIMesh, const vector<CEditor_Bone*>& Bones)
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
		memcpy(&pVertices[i].vPosition, &pAIMesh->mVertices[i], sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &pAIMesh->mNormals[i], sizeof(_float3));
		memcpy(&pVertices[i].vTangent, &pAIMesh->mTangents[i], sizeof(_float3));
		memcpy(&pVertices[i].vBinormal, &pAIMesh->mBitangents[i], sizeof(_float3));
		memcpy(&pVertices[i].vTexcoord, &pAIMesh->mTextureCoords[0][i], sizeof(_float2));

		MESH_VERTEX_DATA temp;
		temp.position.x = pVertices[i].vPosition.x;
		temp.position.y = pVertices[i].vPosition.y;
		temp.position.z = pVertices[i].vPosition.z;
		temp.normal.x = pVertices[i].vNormal.x;
		temp.normal.y = pVertices[i].vNormal.y;
		temp.normal.z = pVertices[i].vNormal.z;
		temp.tangent.x = pVertices[i].vTangent.x;
		temp.tangent.y = pVertices[i].vTangent.y;
		temp.tangent.z = pVertices[i].vTangent.z;
		temp.binormal.x = pVertices[i].vBinormal.x;
		temp.binormal.y = pVertices[i].vBinormal.y;
		temp.binormal.z = pVertices[i].vBinormal.z;
		temp.texcoord.x = pVertices[i].vTexcoord.x;
		temp.texcoord.y = pVertices[i].vTexcoord.y;

		m_Mesh_Data.vecVertices.push_back(temp);


	}
	m_iNumBones = pAIMesh->mNumBones;

	m_Mesh_Data.iNumBones = m_iNumBones;

	for (_uint i = 0; i < m_iNumBones; i++)
	{
		aiBone* pAIBone = pAIMesh->mBones[i];

		_float4x4	OffsetMatrix;
		memcpy(&OffsetMatrix, &pAIBone->mOffsetMatrix, sizeof(_float4x4));
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixTranspose(XMLoadFloat4x4(&OffsetMatrix)));
		m_OffsetMatrices.push_back(OffsetMatrix);

		FLOAT4X4_DATA OffsetData;
		memcpy(&OffsetData, &OffsetMatrix, sizeof(_float4x4));
		m_Mesh_Data.vecOffsetMatrices.push_back(OffsetData);

		/* CModel의 m_Bones의 인덱스와 동기화되는 해당 뼈의 인덱스를 저장 */
		_uint	iBoneIndex = { 0 };
		auto	iter = find_if(Bones.begin(), Bones.end(), [&](CEditor_Bone* pBone)->_bool
			{
				if (true == pBone->Compare_Name(pAIBone->mName.data))
					return true;

				iBoneIndex++;

				return false;
			});
		m_BoneIndices.push_back(iBoneIndex);

		m_Mesh_Data.vecBoneIndices.push_back(iBoneIndex);

		for (_uint j = 0; j < pAIBone->mNumWeights; j++)
		{
	
			aiVertexWeight AIVertexWeight = pAIBone->mWeights[j];

			if (0.f == pVertices[AIVertexWeight.mVertexId].vBlendWeight.x)
			{
				pVertices[AIVertexWeight.mVertexId].vBlendIndex.x = i;
				pVertices[AIVertexWeight.mVertexId].vBlendWeight.x = AIVertexWeight.mWeight;

				m_Mesh_Data.vecVertices[AIVertexWeight.mVertexId].blendIndex.x = i; 
				m_Mesh_Data.vecVertices[AIVertexWeight.mVertexId].blendWeight.x = AIVertexWeight.mWeight;

			}

			else if (0.f == pVertices[AIVertexWeight.mVertexId].vBlendWeight.y)
			{
				pVertices[AIVertexWeight.mVertexId].vBlendIndex.y = i;
				pVertices[AIVertexWeight.mVertexId].vBlendWeight.y = AIVertexWeight.mWeight;

				m_Mesh_Data.vecVertices[AIVertexWeight.mVertexId].blendIndex.y = i;
				m_Mesh_Data.vecVertices[AIVertexWeight.mVertexId].blendWeight.y = AIVertexWeight.mWeight;

			}
			else if (0.f == pVertices[AIVertexWeight.mVertexId].vBlendWeight.z)
			{
				pVertices[AIVertexWeight.mVertexId].vBlendIndex.z = i;
				pVertices[AIVertexWeight.mVertexId].vBlendWeight.z = AIVertexWeight.mWeight;

				m_Mesh_Data.vecVertices[AIVertexWeight.mVertexId].blendIndex.z = i;
				m_Mesh_Data.vecVertices[AIVertexWeight.mVertexId].blendWeight.z = AIVertexWeight.mWeight;

			}
			else
			{
				pVertices[AIVertexWeight.mVertexId].vBlendIndex.w = i;
				pVertices[AIVertexWeight.mVertexId].vBlendWeight.w = AIVertexWeight.mWeight;

				m_Mesh_Data.vecVertices[AIVertexWeight.mVertexId].blendIndex.w = i;
				m_Mesh_Data.vecVertices[AIVertexWeight.mVertexId].blendWeight.w = AIVertexWeight.mWeight;
			}

		}
	}

	if (0 == m_iNumBones)
	{
		m_iNumBones = 1;

		m_Mesh_Data.iNumBones = m_iNumBones;

		_uint	iBoneIndex = { 0 };

		auto	iter = find_if(Bones.begin(), Bones.end(), [&](CEditor_Bone* pBone)->_bool
			{
				if (true == pBone->Compare_Name(m_szName))
					return true;

				iBoneIndex++;

				return false;
			});
		m_BoneIndices.push_back(iBoneIndex);

		m_Mesh_Data.vecBoneIndices.push_back(iBoneIndex);

		_float4x4		OffsetMatrix;
		XMStoreFloat4x4(&OffsetMatrix, XMMatrixIdentity());

		m_OffsetMatrices.push_back(OffsetMatrix);

		FLOAT4X4_DATA OffsetData;
		memcpy(&OffsetData, &OffsetMatrix, sizeof(_float4x4));
		m_Mesh_Data.vecOffsetMatrices.push_back(OffsetData);

	}


	D3D11_SUBRESOURCE_DATA	VBInitialData{};
	VBInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &VBInitialData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}
CEditor_Mesh* CEditor_Mesh::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MODELTYPE eType, const aiMesh* pAIMesh, const vector<class CEditor_Bone*>& Bones, _fmatrix PreTransformMatrix)
{
	CEditor_Mesh* pInstance = new CEditor_Mesh(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType, pAIMesh, Bones, PreTransformMatrix)))
	{
		MSG_BOX(TEXT("Failed to Created : CEditor_Mesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CEditor_Mesh::Clone(void* pArg)
{
	CEditor_Mesh* pInstance = new CEditor_Mesh(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CEditor_Mesh"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEditor_Mesh::Free()
{
	__super::Free();

	//Safe_Delete_Array(m_pIndices);
	//Safe_Delete_Array(m_pVertexPositions);

}
