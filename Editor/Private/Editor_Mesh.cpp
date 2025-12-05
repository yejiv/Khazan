#include "Editor_Mesh.h"
#include "GameInstance.h"
#include "Editor_Bone.h"

#include <set>

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

    cout << m_szName << endl;

    //SOFTBODY_EXTRACT_DATA data;
    //Extract_ClothSoftbodyData(Bones, data);

	return S_OK;

}

HRESULT CEditor_Mesh::Initialize_Clone(void* pArg)
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

HRESULT CEditor_Mesh::Extract_SoftbodyData(const vector<CEditor_Bone*>& Bones, const CLOTH_BONE_CONFIG& Config, SOFTBODY_EXTRACT_DATA& outData)
{
    outData = SOFTBODY_EXTRACT_DATA();
    outData.strMeshName = m_szName;


    //  1.천 뼈 인덱스 수집 (모델 뼈 인덱스 기준)
    set<_uint> clothModelBoneIndices;   // 모든 천 뼈
    set<_uint> fixedModelBoneIndices;   // 고정점 뼈

    for (_uint i = 0; i < static_cast<_uint>(Bones.size()); i++)
    {
        const char* boneName = Bones[i]->Get_Name();

        // 천 뼈인지 확인
        if (MatchBonePattern(boneName, Config.vecClothBonePatterns))
        {
            clothModelBoneIndices.insert(i);

            // 고정점 뼈인지 추가 확인
            if (MatchBonePattern(boneName, Config.vecFixedBonePatterns))
            {
                fixedModelBoneIndices.insert(i);
            }
        }
    }

    if (clothModelBoneIndices.empty())
    {
        OutputDebugStringA("[Extract_SoftbodyData] No cloth bones found.\n");
        return S_FALSE;
    }



    //  2.메시 뼈 인덱스 → 모델 뼈 인덱스 매핑 (이미 m_BoneIndices에 있음)
    // m_BoneIndices[meshBoneIdx] = modelBoneIdx

    // 메시 내에서 천 뼈/고정 뼈 인덱스 집합
    set<_uint> clothMeshBoneIndices;
    set<_uint> fixedMeshBoneIndices;

    for (_uint meshBoneIdx = 0; meshBoneIdx < static_cast<_uint>(m_BoneIndices.size()); meshBoneIdx++)
    {
        _uint modelBoneIdx = m_BoneIndices[meshBoneIdx];

        if (clothModelBoneIndices.count(modelBoneIdx))
        {
            clothMeshBoneIndices.insert(meshBoneIdx);

            if (fixedModelBoneIndices.count(modelBoneIdx))
            {
                fixedMeshBoneIndices.insert(meshBoneIdx);
            }
        }
    }

    // 3. Softbody 정점 추출
    map<_uint, _uint> originalToSoftbody;  // 원본 정점 인덱스 → Softbody 정점 인덱스

    for (_uint i = 0; i < static_cast<_uint>(m_Mesh_Data.vecVertices.size()); i++)
    {
        const MESH_VERTEX_DATA& vertex = m_Mesh_Data.vecVertices[i];

        // 정점의 뼈 인덱스와 가중치
        _uint meshBoneIndices[4] = {
            vertex.blendIndex.x,
            vertex.blendIndex.y,
            vertex.blendIndex.z,
            vertex.blendIndex.w
        };
        _float weights[4] = {
            vertex.blendWeight.x,
            vertex.blendWeight.y,
            vertex.blendWeight.z,
            vertex.blendWeight.w
        };

        // 천 뼈 가중치 합계 / 고정 뼈 가중치 합계 계산
        _float clothWeight = 0.f;
        _float fixedWeight = 0.f;
        _uint dominantFixedBone = UINT_MAX;
        _float maxFixedWeight = 0.f;

        for (int j = 0; j < 4; j++)
        {
            _uint meshBoneIdx = meshBoneIndices[j];

            if (clothMeshBoneIndices.count(meshBoneIdx))
            {
                clothWeight += weights[j];

                if (fixedMeshBoneIndices.count(meshBoneIdx))
                {
                    fixedWeight += weights[j];

                    if (weights[j] > maxFixedWeight)
                    {
                        maxFixedWeight = weights[j];
                        dominantFixedBone = m_BoneIndices[meshBoneIdx];  // 모델 뼈 인덱스 저장
                    }
                }
            }
        }

        // Softbody 정점 조건: 천 뼈 가중치 합 >= 임계값
        if (clothWeight >= Config.fClothWeightThreshold)
        {
            SOFTBODY_VERTEX_DATA sbVertex;
            sbVertex.iOriginalIndex = i;
            sbVertex.position = vertex.position;
            sbVertex.normal = vertex.normal;
            sbVertex.texcoord = vertex.texcoord;

            // 고정점 판별:
            // - 고정 뼈(시작점) 가중치가 높고
            // - 다른 천 뼈(동적) 가중치가 낮음
            _float dynamicClothWeight = clothWeight - fixedWeight;
            sbVertex.isFixed = (fixedWeight >= Config.fFixedWeightThreshold) &&
                (dynamicClothWeight < Config.fDynamicWeightThreshold);
            sbVertex.fFixedWeight = fixedWeight;
            sbVertex.iClosestFixedBone = dominantFixedBone;

            // 매핑 저장
            _uint softbodyIdx = static_cast<_uint>(outData.vecVertices.size());
            originalToSoftbody[i] = softbodyIdx;
            outData.vecSoftbodyToOriginal.push_back(i);
            outData.vecVertices.push_back(sbVertex);

            if (sbVertex.isFixed)
                outData.iNumFixedVertices++;
            else
                outData.iNumDynamicVertices++;
        }
    }

    // 4. 삼각형 재구성 (모든 정점이 Softbody에 포함된 삼각형만)
    for (const auto& tri : m_Mesh_Data.vecIndices)
    {
        auto itA = originalToSoftbody.find(tri.x);
        auto itB = originalToSoftbody.find(tri.y);
        auto itC = originalToSoftbody.find(tri.z);

        // 세 정점 모두 Softbody에 포함되어야 함
        if (itA != originalToSoftbody.end() &&
            itB != originalToSoftbody.end() &&
            itC != originalToSoftbody.end())
        {
            UINT3_DATA newTri;
            newTri.x = itA->second;
            newTri.y = itB->second;
            newTri.z = itC->second;
            outData.vecTriangles.push_back(newTri);
        }
    }

    // 5. 원본 → Softbody 매핑 테이블 (렌더링 시 정점 위치 업데이트용)
    outData.vecOriginalToSoftbody.resize(m_Mesh_Data.vecVertices.size(), UINT_MAX);
    for (const auto& pair : originalToSoftbody)
    {
        outData.vecOriginalToSoftbody[pair.first] = pair.second;
    }




    // 디버그 출력
    char debugMsg[512];
    sprintf_s(debugMsg,
        "[Extract_SoftbodyData] Mesh: %s\n"
        "  - Total vertices: %u\n"
        "  - Softbody vertices: %u (Fixed: %u, Dynamic: %u)\n"
        "  - Softbody triangles: %u\n"
        "  - Cloth bones in mesh: %zu\n"
        "  - Fixed bones in mesh: %zu\n",
        m_szName,
        static_cast<_uint>(m_Mesh_Data.vecVertices.size()),
        static_cast<_uint>(outData.vecVertices.size()),
        outData.iNumFixedVertices,
        outData.iNumDynamicVertices,
        static_cast<_uint>(outData.vecTriangles.size()),
        clothMeshBoneIndices.size(),
        fixedMeshBoneIndices.size()
    );
    OutputDebugStringA(debugMsg);

    return S_OK;
}

HRESULT CEditor_Mesh::Extract_ClothSoftbodyData(const vector<CEditor_Bone*>& Bones, SOFTBODY_EXTRACT_DATA& outData)
{
    /* 테스트용임. 툴에서 CLOTH_BONE_CONFIG값 넘겨줄거임!! */
    // 기본 바지 천 설정
    CLOTH_BONE_CONFIG config;

    // 모든 천 뼈 패턴 (B_Cloth_ 로 시작하는 모든 뼈)
    config.vecClothBonePatterns = {
        "B_Cloth_"
    };

    // 고정점 뼈 패턴 (01_01 = 시작점)
    config.vecFixedBonePatterns = {
        "B_Cloth_01_01_F_Pelvis",
        "B_Cloth_01_01_BL_Pelvis",
        "B_Cloth_01_01_BR_Pelvis",
        "B_Cloth_01_01_R_Pelvis",
        "B_Cloth_01_01_L_Pelvis",
        "B_Cloth_01_02_F_Pelvis",
        "B_Cloth_01_02_BL_Pelvis",
        "B_Cloth_01_02_BR_Pelvis",
        "B_Cloth_01_02_R_Pelvis",
        "B_Cloth_01_02_L_Pelvis",
    };

    config.fClothWeightThreshold = 0.0001f;
    config.fFixedWeightThreshold = 0.5f;
    config.fDynamicWeightThreshold = 0.3f;

    return Extract_SoftbodyData(Bones, config, outData);
}

_bool CEditor_Mesh::MatchBonePattern(const char* pBoneName, const vector<string>& patterns)
{
    for (const auto& pattern : patterns)
    {
        if (strstr(pBoneName, pattern.c_str()) != nullptr)
            return true;
    }
    return false;
}

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
		XMStoreFloat3(&pVertices[i].vTangent, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vTangent), PreTransformMatrix));

		memcpy(&pVertices[i].vBinormal, &pAIMesh->mBitangents[i], sizeof(_float3));
		XMStoreFloat3(&pVertices[i].vBinormal, XMVector3TransformNormal(XMLoadFloat3(&pVertices[i].vBinormal), PreTransformMatrix));

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

	if (FAILED(pInstance->Initialize_Clone(pArg)))
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
