
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
	m_PreTransformMatrix = PreTransformMatrix;

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

void CMesh::Build_BoneNameList(const vector<class CBone*>& Bones)
{
	m_BoneNames.clear();
	m_BoneNames.reserve(m_iNumBones);

	for (_int iBoneIndex : m_BoneIndices)
	{
		if (iBoneIndex >= 0 && iBoneIndex < Bones.size()) 
		{
			m_BoneNames.emplace_back(Bones[iBoneIndex]->Get_Name());
#ifdef _DEBUG
			// 본 이름 확인용
			OutputDebugStringW((L"[Mesh:" + m_strName + L"] Bone[" +
				to_wstring(m_BoneNames.size() - 1) + L"]: " + Bones[iBoneIndex]->Get_Name() + L"\n").c_str());
#endif

		}
		else 
		{
			m_BoneNames.emplace_back(L"");

#ifdef _DEBUG
			OutputDebugStringA(("[CMesh::Build_BoneNameList] Invalid bone index: "
				+ to_string(iBoneIndex) + "\n").c_str());
#endif

		}
	}

#ifdef _DEBUG
	OutputDebugStringA(("@@@@@ [CMesh::Build_BoneNameList] Mesh: "+ WStringToAnsi(m_strName) + ", Bones: "+ to_string(m_BoneNames.size()) + "@@@@@@@@@\n").c_str());
#endif
}

void CMesh::Build_MasterBoneCache(const vector<class CBone*>& MasterBones)
{
	if (m_BoneNames.empty())
	{
#ifdef _DEBUG
		OutputDebugStringA("[CMesh::Build_MasterBoneCache] BoneNames가 비어있음!\n");
#endif
		return;
	}

	m_MasterBoneCache.clear();
	m_MasterBoneCache.reserve(m_iNumBones);

	_int		iMappedCount = {};
	_int		iFailedCount = {};

	// 각 파츠 본 이름에 해당하는 마스터 본 인덱스 찾기
	for (size_t i = 0; i < m_BoneNames.size(); ++i)
	{
		const _wstring& strBoneName = m_BoneNames[i];
		_int iMasterBoneIndex = -1;

		if (!strBoneName.empty())
		{
			// 마스터에서 같은 이름의 본 찾기
			for (_int j = 0; j < MasterBones.size(); ++j)
			{
				if (MasterBones[j]->Compare_Name(strBoneName))
				{
					iMasterBoneIndex = j;
					iMappedCount++;
#ifdef _DEBUG
					OutputDebugStringW((L"[Mesh:" + m_strName + L"] Mapped[" + to_wstring(i) + L"]: " +
						strBoneName + L" -> Master[" + to_wstring(j) + L"]\n").c_str());
#endif		
					break;
				}
			}

			if (iMasterBoneIndex == -1)
			{
				iFailedCount++;
#ifdef _DEBUG
				OutputDebugStringW((L"[Mesh:" + m_strName + L"] 매핑 실패["
					+ to_wstring(i) + L"]: " + strBoneName + L"\n").c_str());
#endif
			}
		}

		m_MasterBoneCache.emplace_back(iMasterBoneIndex);
	}

	m_isBoneIndicesCached = true;

#ifdef _DEBUG
	OutputDebugStringA(("@@@@@@@@@[CMesh::Build_MasterBoneCache] 캐시 완료.@@@@@@@@@\n"));
	OutputDebugStringA(("  전체 본: " + to_string(m_BoneNames.size()) + "\n").c_str());
	OutputDebugStringA(("  매핑 성공: " + to_string(iMappedCount) + "\n").c_str());
	OutputDebugStringA(("  매핑 실패: " + to_string(iFailedCount) + "\n").c_str());

	if (iFailedCount > 0)
	{
		OutputDebugStringA((" 대체 본 매핑 필요\n"));
	}
#endif

}
void CMesh::Build_FallbackBoneCache(const vector<class CBone*>& PartBones, const vector<class CBone*>& MasterBones)
{
	m_FallbackBoneCache.clear();
	//m_IsStaticBone.clear(); 

	m_FallbackBoneCache.reserve(m_iNumBones);
	//m_IsStaticBone.reserve(m_iNumBones);




	OutputDebugStringW((L" [" + m_strName + L"] 대체 본 매핑\n").c_str());

	for (size_t i = 0; i < m_MasterBoneCache.size(); ++i)
	{
		_int iMasterBoneIndex = m_MasterBoneCache[i];

		_bool isStatic = false;

		//매핑 성공
		if (iMasterBoneIndex >= 0) {
			m_FallbackBoneCache.emplace_back(iMasterBoneIndex);
			//m_IsStaticBone.emplace_back(false);
		}
		// 매핑 실패
		else
		{
//			/* 고정 시킬 본 먼저 탐색 */
//			const _wstring& boneName = m_BoneNames[i];
//
//			for (const auto& pattern : staticBonePatterns)
//			{
//				if (boneName.find(pattern) != wstring::npos)
//				{
//					isStatic = true;
//#ifdef _DEBUG
//					OutputDebugStringW((L"[" + to_wstring(i) + L"] " +boneName + L" -> 초기 위치 고정\n").c_str());
//#endif
//					break;
//				}
//			}
//
//			if (!isStatic)
//			{
				// 고정 대상 아님 - 부모 본 찾기
				_int iFallbackBoneIndex = -1;

				if (m_BoneIndices[i] >= 0 && m_BoneIndices[i] < PartBones.size())
				{
					CBone* pPartBone = PartBones[m_BoneIndices[i]];
					_int iParentIndex = pPartBone->Get_ParentBoneIndex();

					while (iParentIndex >= 0 && iParentIndex < PartBones.size())
					{
						_wstring strParentName = PartBones[iParentIndex]->Get_Name();

						for (_int j = 0; j < MasterBones.size(); ++j)
						{
							if (MasterBones[j]->Compare_Name(strParentName))
							{
								iFallbackBoneIndex = j;
#ifdef _DEBUG
								OutputDebugStringW((L"[" + to_wstring(i) + L"] " +	m_BoneNames[i] + L"\n").c_str());
								OutputDebugStringW((L" -> 대체: " + strParentName +L" [Master:" + to_wstring(j) + L"]\n").c_str());
#endif
								break;
							}
						}

						if (iFallbackBoneIndex >= 0)
							break;

						iParentIndex = PartBones[iParentIndex]->Get_ParentBoneIndex();
					}
				}

				// 부모도 못 찾으면 Root
				if (iFallbackBoneIndex < 0)
				{
					for (_int j = 0; j < MasterBones.size(); ++j)
					{
						if (MasterBones[j]->Compare_Name(L"Root") || MasterBones[j]->Compare_Name(L"Bip001"))
						{
							iFallbackBoneIndex = j;
							break;
						}
					}
				}

				m_FallbackBoneCache.push_back(iFallbackBoneIndex);
			//}
			//else
			//{
			//	// 고정 대상
			//	m_FallbackBoneCache.push_back(-1);
			//}

			//m_IsStaticBone.push_back(isStatic);
		}
	}

#ifdef _DEBUG
	OutputDebugStringA(("!@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@2\n\n"));
#endif
}





HRESULT CMesh::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName, const vector<class CBone*>& Bones)
{
	// 본 이름 리스트가 비어있으면 기존 방식 사용
	if (m_BoneNames.empty())
	{
		for (size_t i = 0; i < m_iNumBones; i++)
		{
			XMStoreFloat4x4(&m_BoneMatrices[i],
				XMLoadFloat4x4(&m_OffsetMatrices[i]) * Bones[m_BoneIndices[i]]->Get_CombinedTransformationMatrix());
		}
	}
	else
	{
		// 파츠: 캐시된 마스터 본 인덱스 사용
		if (!m_isBoneIndicesCached)
			return E_FAIL;
		
		const vector<_int>& boneCache = m_FallbackBoneCache.empty() ? m_MasterBoneCache : m_FallbackBoneCache;

		for (size_t i = 0; i < m_iNumBones; i++)
		{
			//// 고정 본인지 확인
			//if (!m_IsStaticBone.empty() && m_IsStaticBone[i])
			//{
			//	//고정 본이묜 OffsetMatrix
			//	XMStoreFloat4x4(&m_BoneMatrices[i], XMMatrixIdentity());
			//}
			//else
			//{
				// 그냥 본이면 애니메이션 적용
				_int iMasterBoneIndex = boneCache[i];

				if (iMasterBoneIndex >= 0 && iMasterBoneIndex < Bones.size())
				{
					XMStoreFloat4x4(&m_BoneMatrices[i],
						XMLoadFloat4x4(&m_OffsetMatrices[i]) * Bones[iMasterBoneIndex]->Get_CombinedTransformationMatrix());
				}
				else
				{
					XMStoreFloat4x4(&m_BoneMatrices[i], XMMatrixIdentity());
				}
			//}
		}
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
