#include "VIBuffer_Mesh_Instance.h"
#include "GameInstance.h"
#include "ComputeShader.h"

CVIBuffer_Mesh_Instance::CVIBuffer_Mesh_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instance { pDevice, pContext }
{
}

CVIBuffer_Mesh_Instance::CVIBuffer_Mesh_Instance(const CVIBuffer_Mesh_Instance& Prototype)
	: CVIBuffer_Instance(Prototype)
	, m_iMaterialIndex { Prototype.m_iMaterialIndex }
	, m_vPivot{ Prototype.m_vPivot }
	, m_pSpeeds{ Prototype.m_pSpeeds }
	, m_isLoop{ Prototype.m_isLoop }
	, m_pSRV{ Prototype.m_pSRV }
	, m_pUAV{ Prototype.m_pUAV }
	, m_pCB{ Prototype.m_pCB }
	, m_pStructuredBuffer{ Prototype.m_pStructuredBuffer }
{
	for (_uint i = 0; i < ENUM_CLASS(CS_PASS::END); ++i)
	{
		m_ComputeShaders[i] = Prototype.m_ComputeShaders[i];
		Safe_AddRef(m_ComputeShaders[i]);
	}

	Safe_AddRef(m_pSRV);
	Safe_AddRef(m_pUAV);
	Safe_AddRef(m_pCB);
	Safe_AddRef(m_pStructuredBuffer);
}

HRESULT CVIBuffer_Mesh_Instance::Initialize_Prototype(const _char* pMeshFilePath, const INSTANCE_DESC* pDesc, _fmatrix PreTransformMatrix)
{
	const MESH_INSTANCE_DESC* pMeshDesc = static_cast<const MESH_INSTANCE_DESC*>(pDesc);

	_uint iFlag = { aiProcess_ConvertToLeftHanded | aiProcessPreset_TargetRealtime_Fast | aiProcess_PreTransformVertices };

	m_pAIScene = m_Importer.ReadFile(pMeshFilePath, iFlag);
	if (nullptr == m_pAIScene)
		return E_FAIL;

	strcpy_s(m_szName, m_pAIScene->mMeshes[0]->mName.data);
	m_iMaterialIndex = m_pAIScene->mMeshes[0]->mMaterialIndex;

	m_vPivot = pMeshDesc->vPivot;
	m_isLoop = pMeshDesc->isLoop;

	m_iInstanceVertexStride = sizeof(VTXINSTANCE_PARTICLE);
	m_iNumIndices = m_pAIScene->mMeshes[0]->mNumFaces * 3;
	m_iNumVertices = m_pAIScene->mMeshes[0]->mNumVertices;
	m_iNumVertexBuffers = 2;
	m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	m_iIndexStride = 4;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;

	// 인스턴스용 데이터 세팅
	m_iNumInstance = pMeshDesc->iNumInstance;
	m_iNumIndexPerInstance = m_iNumIndices;

#pragma region VERTEX_SETTING

	CHECK_FAILED_MSG(Ready_Vertices(m_pAIScene->mMeshes[0], PreTransformMatrix), TEXT("Mesh Instance - Ready Vertices 실패"), E_FAIL);

#pragma endregion

#pragma region INDEX_SETTING

	CHECK_FAILED_MSG(Ready_Indices_For_4Byte(m_pAIScene->mMeshes[0]), TEXT("Mesh Instance - Ready Indices 실패"), E_FAIL);

#pragma endregion

#pragma region INSTANCING_SETTING

	m_VBInstanceDesc.ByteWidth = m_iNumInstance * m_iInstanceVertexStride;
	m_VBInstanceDesc.Usage = D3D11_USAGE_DEFAULT;
	m_VBInstanceDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBInstanceDesc.CPUAccessFlags = 0;
	m_VBInstanceDesc.MiscFlags = 0;
	m_VBInstanceDesc.StructureByteStride = m_iInstanceVertexStride;

	m_pInstanceVertices = new VTXINSTANCE_PARTICLE[m_iNumInstance];
	ZeroMemory(m_pInstanceVertices, sizeof(VTXINSTANCE_PARTICLE) * m_iNumInstance);

	m_pSpeeds = new _float[m_iNumInstance];
	ZeroMemory(m_pSpeeds, sizeof(_float) * m_iNumInstance);

	POINT_INSTANCE_PARAMS* pParticleParams = new POINT_INSTANCE_PARAMS[m_iNumInstance];

	for (_uint i = 0; i < m_iNumInstance; ++i)
	{
		VTXINSTANCE_PARTICLE* pInstanceVertices = static_cast<VTXINSTANCE_PARTICLE*>(m_pInstanceVertices);
		CHECK_NULLPTR_MSG(pInstanceVertices, TEXT("Mesh Instance - nullptr == pInstanceVertices"), E_FAIL);

		_float		fSize = m_pGameInstance->Rand(pMeshDesc->vSize.x, pMeshDesc->vSize.y);
		_float		fLifeTime = m_pGameInstance->Rand(pMeshDesc->vLifeTime.x, pMeshDesc->vLifeTime.y);
		m_pSpeeds[i] = m_pGameInstance->Rand(pMeshDesc->vSpeed.x, pMeshDesc->vSpeed.y);

		pInstanceVertices[i].vRight = _float4(fSize, 0.f, 0.f, 0.f);
		pInstanceVertices[i].vUp = _float4(0.f, fSize, 0.f, 0.f);
		pInstanceVertices[i].vLook = _float4(0.f, 0.f, fSize, 0.f);
		pInstanceVertices[i].vTranslation = _float4(
			m_pGameInstance->Rand(pMeshDesc->vCenter.x - pMeshDesc->vRange.x * 0.5f, pMeshDesc->vCenter.x + pMeshDesc->vRange.x * 0.5f),
			m_pGameInstance->Rand(pMeshDesc->vCenter.y - pMeshDesc->vRange.y * 0.5f, pMeshDesc->vCenter.y + pMeshDesc->vRange.y * 0.5f),
			m_pGameInstance->Rand(pMeshDesc->vCenter.z - pMeshDesc->vRange.z * 0.5f, pMeshDesc->vCenter.z + pMeshDesc->vRange.z * 0.5f),
			1.f
		);

		_float4 vBaseDirection = _float4(pMeshDesc->vDirection.x, pMeshDesc->vDirection.y, pMeshDesc->vDirection.z, 0.f);
		_float4 vFinalDirection = {};

		if (true == pMeshDesc->isRandomVector)
		{
			_float fRadianX = XMConvertToRadians(m_pGameInstance->Rand(pMeshDesc->vMinAngle.x, pMeshDesc->vMaxAngle.x));
			_float fRadianY = XMConvertToRadians(m_pGameInstance->Rand(pMeshDesc->vMinAngle.y, pMeshDesc->vMaxAngle.y));
			_float fRadianZ = XMConvertToRadians(m_pGameInstance->Rand(pMeshDesc->vMinAngle.z, pMeshDesc->vMaxAngle.z));

			_vector vQuaternion = XMQuaternionRotationRollPitchYaw(fRadianX, fRadianY, fRadianZ);
			_matrix RotationMatrix = XMMatrixRotationQuaternion(vQuaternion);
			XMStoreFloat4(&vFinalDirection,
				XMVectorSetW(XMVector3Normalize(XMVector3TransformNormal(XMLoadFloat4(&vBaseDirection), RotationMatrix)), 0.f));
		}
		else
			vFinalDirection = vBaseDirection;

		vFinalDirection.x *= pMeshDesc->vVectorScale.x;
		vFinalDirection.y *= pMeshDesc->vVectorScale.y;
		vFinalDirection.z *= pMeshDesc->vVectorScale.z;

		pInstanceVertices[i].vLifeTime = _float2(0.f, fLifeTime);

		// Compute Shader SRV Structured Buffer
		pParticleParams[i].fSpeed = m_pSpeeds[i];
		pParticleParams[i].vInitTranslation = pInstanceVertices[i].vTranslation;
		pParticleParams[i].vDirection = vFinalDirection;
	}

	if (FAILED(Ready_ShaderResourceView(pParticleParams)))
		return E_FAIL;

	if (FAILED(Ready_UnorderedAccessView()))
		return E_FAIL;

	if (FAILED(Ready_ConstantBuffer()))
		return E_FAIL;

	if (FAILED(Ready_ComputeShader()))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Mesh_Instance::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}

void CVIBuffer_Mesh_Instance::Move(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE SubResource;
	if (SUCCEEDED(m_pContext->Map(m_pCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource)))
	{
		POINT_INSTANCE_CB* pPointInstanceCB = reinterpret_cast<POINT_INSTANCE_CB*>(SubResource.pData);
		pPointInstanceCB->vPivot = m_vPivot;
		pPointInstanceCB->fTimeDelta = fTimeDelta;
		pPointInstanceCB->iNumInstances = m_iNumInstance;
		m_pContext->Unmap(m_pCB, 0);
	}

	// 컴퓨트 실행 단위 추가
	COMPUTE_PASS_DESC PassDesc{};
	PassDesc.SRVs.push_back(m_pSRV);
	PassDesc.UAVs.push_back(m_pUAV);
	//	PassDesc.UAVInitialCounts.push_back(0);		// 나중에 Append / Consume 사용할 때 추가하기
	PassDesc.ConstantBuffers.push_back(m_pCB);
	_uint iNumThreadPerGroup = 256;
	_uint iNumGroups = (m_iNumInstance + iNumThreadPerGroup - 1) / iNumThreadPerGroup;
	PassDesc.x = iNumGroups;
	PassDesc.y = 1;
	PassDesc.z = 1;

	CComputeShader_Manager::COMPUTE_JOB_DESC JobDesc{};
	JobDesc.pShader = m_ComputeShaders[ENUM_CLASS(CS_PASS::MOVE)];
	JobDesc.PassDesc = PassDesc;

	m_pGameInstance->Add_Job(COMPUTEJOB::UPDATE, JobDesc, true);

	m_pContext->CopyResource(m_pVBInstance, m_pStructuredBuffer);
}

HRESULT CVIBuffer_Mesh_Instance::Ready_Vertices(const aiMesh* pAIMesh, _fmatrix PreTransformMatrix)
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

HRESULT CVIBuffer_Mesh_Instance::Ready_Indices_For_4Byte(const aiMesh* pAIMesh)
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

HRESULT CVIBuffer_Mesh_Instance::Ready_ShaderResourceView(void* pSysmem)
{
	// 1. SRV로 쓸 파티클 파라미터 버퍼 만들기
	ID3D11Buffer* pBuffer = { nullptr };

	D3D11_BUFFER_DESC ParticleParamsBufferDesc{};
	ParticleParamsBufferDesc.ByteWidth = sizeof(POINT_INSTANCE_PARAMS) * m_iNumInstance;
	ParticleParamsBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	ParticleParamsBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	ParticleParamsBufferDesc.CPUAccessFlags = 0;
	ParticleParamsBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	ParticleParamsBufferDesc.StructureByteStride = sizeof(POINT_INSTANCE_PARAMS);

	D3D11_SUBRESOURCE_DATA ParticleParamsInitialData{};
	ParticleParamsInitialData.pSysMem = pSysmem;

	if (FAILED(m_pDevice->CreateBuffer(&ParticleParamsBufferDesc, &ParticleParamsInitialData, &pBuffer)))
		return E_FAIL;

	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc{};
	SRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	SRVDesc.Buffer.FirstElement = 0;
	SRVDesc.Buffer.NumElements = m_iNumInstance;

	if (FAILED(m_pDevice->CreateShaderResourceView(pBuffer, &SRVDesc, &m_pSRV)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Mesh_Instance::Ready_UnorderedAccessView()
{
	// 2. UAV 생성 (CS에서 구조체 버퍼로 쓸 VBInstance와 같은 크기와 정보의 버퍼 만들기)
	D3D11_BUFFER_DESC StructuredBufferDesc{};
	StructuredBufferDesc.ByteWidth = m_iNumInstance * m_iInstanceVertexStride; // stride * count
	StructuredBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	StructuredBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	StructuredBufferDesc.CPUAccessFlags = 0;
	StructuredBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	StructuredBufferDesc.StructureByteStride = m_iInstanceVertexStride;

	D3D11_SUBRESOURCE_DATA InitData{};
	InitData.pSysMem = m_pInstanceVertices; // 초기 데이터가 있으면
	if (FAILED(m_pDevice->CreateBuffer(&StructuredBufferDesc, &InitData, &m_pStructuredBuffer)))
		return E_FAIL;

	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc{};
	UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	UAVDesc.Buffer.FirstElement = 0;
	UAVDesc.Buffer.NumElements = m_iNumInstance;

	if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pStructuredBuffer, &UAVDesc, &m_pUAV)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Mesh_Instance::Ready_ConstantBuffer()
{
	// 3. 상수 버퍼 생성
	D3D11_BUFFER_DESC CBDesc{};
	CBDesc.ByteWidth = sizeof(POINT_INSTANCE_CB);
	CBDesc.Usage = D3D11_USAGE_DYNAMIC;
	CBDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	CBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	CBDesc.MiscFlags = 0;
	CBDesc.StructureByteStride = 0;

	if (FAILED(m_pDevice->CreateBuffer(&CBDesc, nullptr, &m_pCB)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Mesh_Instance::Ready_ComputeShader()
{
	// 4. 컴퓨트 셰이더 생성
	m_ComputeShaders[ENUM_CLASS(CS_PASS::MOVE)] = CComputeShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Engine_Shader_Compute.hlsl"), "CS_MOVE");
	if (nullptr == m_ComputeShaders[ENUM_CLASS(CS_PASS::MOVE)])
		return E_FAIL;

	return S_OK;
}

CVIBuffer_Mesh_Instance* CVIBuffer_Mesh_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _char* pMeshFilePath, const INSTANCE_DESC* pDesc, _fmatrix PreTransformMatrix)
{
	CVIBuffer_Mesh_Instance* pInstance = new CVIBuffer_Mesh_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pMeshFilePath, pDesc, PreTransformMatrix)))
	{
		MSG_BOX(TEXT("Failed to Create : CVIBuffer_Mesh_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Mesh_Instance::Clone(void* pArg)
{
	CVIBuffer_Mesh_Instance* pInstance = new CVIBuffer_Mesh_Instance(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CVIBuffer_Mesh_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Mesh_Instance::Free()
{
	__super::Free();

	if (false == m_isCloned)
		Safe_Delete_Array(m_pSpeeds);

	for (auto& pComputeShader : m_ComputeShaders)
		Safe_Release(pComputeShader);
	Safe_Release(m_pStructuredBuffer);
	Safe_Release(m_pCB);
	Safe_Release(m_pUAV);
	Safe_Release(m_pSRV);
}
