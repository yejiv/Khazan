
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
//#ifdef _DEBUG
//			// 본 이름 확인용
//			OutputDebugStringW((L"[Mesh:" + m_strName + L"] Bone[" +
//				to_wstring(m_BoneNames.size() - 1) + L"]: " + Bones[iBoneIndex]->Get_Name() + L"\n").c_str());
//#endif

		}
		else 
		{
			m_BoneNames.emplace_back(L"");

#ifdef _DEBUG
			OutputDebugStringA(("[CMesh::Build_BoneNameList] Invalid bone index: "	+ to_string(iBoneIndex) + "\n").c_str());
#endif

		}
	}

#ifdef _DEBUG
	OutputDebugStringA(("[CMesh::Build_BoneNameList] Mesh: "+ WStringToAnsi(m_strName) + ", Bones: "+ to_string(m_BoneNames.size()) + "\n").c_str());
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
//#ifdef _DEBUG
//					OutputDebugStringW((L"[Mesh:" + m_strName + L"] Mapped[" + to_wstring(i) + L"]: " +
//						strBoneName + L" -> Master[" + to_wstring(j) + L"]\n").c_str());
//#endif		
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
	m_IsStaticBone.clear();
	m_StaticBoneChainParent.clear();

	m_FallbackBoneCache.reserve(m_iNumBones);
	m_IsStaticBone.reserve(m_iNumBones);
	m_StaticBoneChainParent.reserve(m_iNumBones);

#ifdef _DEBUG
	OutputDebugStringW((L"[" + m_strName + L"] 대체 본 매핑 시작\n").c_str());
#endif

	for (size_t i = 0; i < m_MasterBoneCache.size(); ++i)
	{
		_int iMasterBoneIndex = m_MasterBoneCache[i];
		_bool isStatic = false;
		_int iStaticParent = -1;

		// 매핑 성공한 본
		if (iMasterBoneIndex >= 0)
		{
			m_FallbackBoneCache.emplace_back(iMasterBoneIndex);
			m_IsStaticBone.emplace_back(false);
			m_StaticBoneChainParent.emplace_back(-1);
		}
		// 매핑 실패한 본 (파츠 전용 본)
		else
		{
			const _wstring& boneName = m_BoneNames[i];

			// 1단계: 고정시킬 본인지 확인 (물리 시뮬레이션용)
			for (const auto& pattern : s_StaticBonePatterns)
			{
				if (boneName.find(pattern) != wstring::npos)
				{
					isStatic = true;
					break;
				}
			}

			if (isStatic)
			{
				// 2단계: 고정 본의 부모 찾기 (체인 구조 유지)
				if (m_BoneIndices[i] >= 0 && m_BoneIndices[i] < PartBones.size())
				{
					CBone* pPartBone = PartBones[m_BoneIndices[i]];
					_int iParentIndex = pPartBone->Get_ParentBoneIndex();

					// 부모 본이 파츠 내부에 있는지 확인
					if (iParentIndex >= 0 && iParentIndex < PartBones.size())
					{
						// 부모 본의 이름 찾기
						_wstring strParentName = PartBones[iParentIndex]->Get_Name();

						// 부모 본이 m_BoneNames에서 몇 번째인지 찾기
						for (size_t j = 0; j < m_BoneNames.size(); ++j)
						{
							if (m_BoneNames[j] == strParentName)
							{
								iStaticParent = static_cast<_int>(j);
								break;
							}
						}
					}
				}

				m_FallbackBoneCache.emplace_back(-1); // 고정 본은 마스터 인덱스 없음
				m_IsStaticBone.emplace_back(true);
				m_StaticBoneChainParent.emplace_back(iStaticParent);

//#ifdef _DEBUG
//				OutputDebugStringW((L"  [고정본] " + boneName +
//					L" -> 부모: " + (iStaticParent >= 0 ? m_BoneNames[iStaticParent] : L"없음") +
//					L"\n").c_str());
//#endif
			}
			else
			{
				// 3단계: 일반 대체 본 처리 (마스터에 있는 부모 본 찾기)
				_int iFallbackBoneIndex = -1;

				if (m_BoneIndices[i] >= 0 && m_BoneIndices[i] < PartBones.size())
				{
					CBone* pPartBone = PartBones[m_BoneIndices[i]];
					_int iParentIndex = pPartBone->Get_ParentBoneIndex();

					// 부모 체인을 따라 올라가며 마스터에 있는 본 찾기
					while (iParentIndex >= 0 && iParentIndex < PartBones.size())
					{
						_wstring strParentName = PartBones[iParentIndex]->Get_Name();

						for (_int j = 0; j < MasterBones.size(); ++j)
						{
							if (MasterBones[j]->Compare_Name(strParentName))
							{
								iFallbackBoneIndex = j;
								break;
							}
						}

						if (iFallbackBoneIndex >= 0)
							break;

						iParentIndex = PartBones[iParentIndex]->Get_ParentBoneIndex();
					}
				}

				// 부모도 못 찾으면 Root 본으로 대체
				if (iFallbackBoneIndex < 0)
				{
					for (_int j = 0; j < MasterBones.size(); ++j)
					{
						if (MasterBones[j]->Compare_Name(L"Root") ||
							MasterBones[j]->Compare_Name(L"Bip001"))
						{
							iFallbackBoneIndex = j;
							break;
						}
					}
				}

				m_FallbackBoneCache.emplace_back(iFallbackBoneIndex);
				m_IsStaticBone.emplace_back(false);
				m_StaticBoneChainParent.emplace_back(-1);

//#ifdef _DEBUG
//				if (iFallbackBoneIndex >= 0)
//				{
//					OutputDebugStringW((L"  [대체본] " + boneName +
//						L" -> " + MasterBones[iFallbackBoneIndex]->Get_Name() +
//						L"\n").c_str());
//				}
//#endif
			}
		}
	}

#ifdef _DEBUG
	_int iStaticCount = static_cast<_int>(count(m_IsStaticBone.begin(), m_IsStaticBone.end(), true));
	OutputDebugStringA(("대체 본 매핑 완료: 고정본 " + to_string(iStaticCount) + "개\n").c_str());
#endif
}

HRESULT CMesh::Bind_BoneMatrices(CShader* pShader, const _char* pConstantName,
	const vector<class CBone*>& Bones, const vector<_float4x4>* PartLocalBoneMatrices)
{
	if (m_BoneNames.empty())
	{
		// 마스터 메시: 기존 로직
		for (size_t i = 0; i < m_iNumBones; i++)
		{
			XMStoreFloat4x4(&m_BoneMatrices[i],
				XMLoadFloat4x4(&m_OffsetMatrices[i]) *
				Bones[m_BoneIndices[i]]->Get_CombinedTransformationMatrix());
		}
	}
	else
	{
		// 파츠 메시: 파츠 로컬 본 사용
		if (!m_isBoneIndicesCached || !PartLocalBoneMatrices)
			return E_FAIL;

		for (size_t i = 0; i < m_iNumBones; i++)
		{
			// m_BoneIndices[i]는 파츠 본 인덱스
			_int partBoneIdx = m_BoneIndices[i];

			if (partBoneIdx >= 0 && partBoneIdx < PartLocalBoneMatrices->size())
			{
				// 파츠 로컬 본 행렬 사용 (초기 위치 포함됨)
				XMStoreFloat4x4(&m_BoneMatrices[i],
					XMLoadFloat4x4(&m_OffsetMatrices[i]) *
					XMLoadFloat4x4(&(*PartLocalBoneMatrices)[partBoneIdx]));
			}
			else
			{
				XMStoreFloat4x4(&m_BoneMatrices[i], XMMatrixIdentity());
			}
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
