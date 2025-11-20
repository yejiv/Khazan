#include "VIBuffer_Mesh_Instance.h"
#include "GameInstance.h"
#include "ComputeShader.h"

CVIBuffer_Mesh_Instance::CVIBuffer_Mesh_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CVIBuffer_Instance { pDevice, pDeviceContext }
{
}

CVIBuffer_Mesh_Instance::CVIBuffer_Mesh_Instance(const CVIBuffer_Mesh_Instance& Prototype)
	: CVIBuffer_Instance { Prototype }
	//, m_pSRVNoise{ Prototype.m_pSRVNoise } //나중에 필요하면 상수버퍼로 넘기기
	, m_pParticleParams{ Prototype.m_pParticleParams }
	, m_sData {Prototype.m_sData}
{
    for (_uint i = 0; i < CS_PASS::END; ++i)
        m_ComputeShaders[i] = Prototype.m_ComputeShaders[i];
    m_pLinearWrapSampler = Prototype.m_pLinearWrapSampler;

	//Safe_AddRef(m_pSRVNoise);	//이거 해줘야되는지 확인좀
}

void CVIBuffer_Mesh_Instance::Reset()
{
    COMPUTE_PASS_DESC PassDesc{};
    PassDesc.SRVs.push_back(m_pSRV);
    PassDesc.SRVs.push_back(m_pSRVNoise);
    PassDesc.UAVs.push_back(m_pUAV);
    PassDesc.UAVs.push_back(m_pUAVSpeed);
    PassDesc.ConstantBuffers.push_back(m_pCB);
    m_pContext->CSSetSamplers(0, 1, &m_pLinearWrapSampler);
    _uint iNumThreadPerGroup = 256;
    _uint iNumGroups = (m_iNumInstance + iNumThreadPerGroup - 1) / iNumThreadPerGroup;
    PassDesc.x = iNumGroups;
    PassDesc.y = 1;
    PassDesc.z = 1;

    CComputeShader_Manager::COMPUTE_JOB_DESC JobDesc{};
    JobDesc.pShader = m_ComputeShaders[ENUM_CLASS(CS_PASS::RESET)];
    JobDesc.PassDesc = PassDesc;

    m_bLoop = m_sData.bIsLoop;
    m_pGameInstance->Add_Job(COMPUTEJOB::UPDATE, JobDesc, true);

    m_pContext->CopyResource(m_pVBInstance, m_pStructuredBuffer);
}

HRESULT CVIBuffer_Mesh_Instance::Initialize_Prototype(INSTANCE_DESC* pArg)
{
	const POINT_MESH_DESC* pMeshDesc = static_cast<const POINT_MESH_DESC*>(pArg);

	ifstream is{ pMeshDesc->pFilePath , std::ios::binary };
	if (!is)
		return E_FAIL;

	MODEL_DATA tModelInfo {};
	tModelInfo.LoadBinary(is);
	MESH_DATA tMeshInfo = tModelInfo.vecMeshes[0];

	m_iNumInstance = pMeshDesc->iNumInstance;

	m_iNumIndexPerInstance = tMeshInfo.iNumFace *3;
	m_iInstanceVertexStride = sizeof(IB_MESHINSTANCE_EFFECT);
	m_iNumVertices = tMeshInfo.iNumVertices;
	m_iVertexStride = sizeof(VB_MESHINSTANCE_EFFECT);
	m_iNumIndices = tMeshInfo.iNumFace * 3;
	m_iIndexStride = 4;
	m_iNumVertexBuffers = 2;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_sData.IsCircle = pMeshDesc->IsCircle;
	m_sData.fOffset = pMeshDesc->fOffset;
	m_sData.bIsLoop = pMeshDesc->bIsLoop;
    m_bLoop = m_sData.bIsLoop;
	m_sData.vRange = pMeshDesc->vRange;
	m_sData.fTurbulenceSpeed = pMeshDesc->fTurbulenceSpeed;
	m_sData.fTurbulenceSampleSize = pMeshDesc->fTurbulenceSampleSize;
	m_sData.fRotation = pMeshDesc->fRotation;
	memcpy(m_sData.pNoiseFilePath, pMeshDesc->pNoiseFilePath, sizeof(pMeshDesc->pNoiseFilePath));


	D3D11_BUFFER_DESC		VBDesc{};
	VBDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	VB_MESHINSTANCE_EFFECT* pVertices = new VB_MESHINSTANCE_EFFECT[m_iNumVertices];
	m_pVertexPositions = new _float3[m_iNumVertices];
	ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iNumVertices);
	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &tMeshInfo.vecVertices[i].position, sizeof(_float3));
		memcpy(&pVertices[i].vNormal, &tMeshInfo.vecVertices[i].normal, sizeof(_float3)); 
		memcpy(&pVertices[i].vTangent, &tMeshInfo.vecVertices[i].tangent, sizeof(_float3)); 
		memcpy(&pVertices[i].vBinormal, &tMeshInfo.vecVertices[i].binormal, sizeof(_float3)); 
		memcpy(&pVertices[i].vTexcoord, &tMeshInfo.vecVertices[i].texcoord, sizeof(_float2));

		m_pVertexPositions[i] = pVertices[i].vPosition;
	}

	D3D11_SUBRESOURCE_DATA	VBInitialData{};
	VBInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &VBInitialData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	D3D11_BUFFER_DESC		IBDesc{};
	IBDesc.ByteWidth = m_iNumIndices * m_iIndexStride;
	IBDesc.Usage = D3D11_USAGE_DEFAULT;
	IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBDesc.CPUAccessFlags = 0;
	IBDesc.MiscFlags = 0;
	IBDesc.StructureByteStride = m_iIndexStride;

	_uint* pIndices = new _uint[m_iNumIndices];
	_uint iIndex = {};

	for (_uint i = 0; i < tMeshInfo.iNumFace; i++)
	{
		memcpy(&pIndices[iIndex++], &tMeshInfo.vecIndices[i].x, sizeof(_uint));
		memcpy(&pIndices[iIndex++], &tMeshInfo.vecIndices[i].y, sizeof(_uint));
		memcpy(&pIndices[iIndex++], &tMeshInfo.vecIndices[i].z, sizeof(_uint));
	}

	D3D11_SUBRESOURCE_DATA	IBInitialData{};
	IBInitialData.pSysMem = pIndices;

	if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &IBInitialData, &m_pIB)))
		return E_FAIL;

	Safe_Delete_Array(pIndices);

	m_VBInstanceDesc.ByteWidth = m_iNumInstance * m_iInstanceVertexStride;
	m_VBInstanceDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_VBInstanceDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBInstanceDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_VBInstanceDesc.MiscFlags = 0;
	m_VBInstanceDesc.StructureByteStride = m_iInstanceVertexStride;

	m_pInstanceVertices = new IB_MESHINSTANCE_EFFECT[m_iNumInstance];
	m_pParticleParams = new MESH_INSTANCE_PARAMS[m_iNumInstance];

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		IB_MESHINSTANCE_EFFECT* pInstanceVertices = static_cast<IB_MESHINSTANCE_EFFECT*>(m_pInstanceVertices);

		_float		fScale = m_pGameInstance->Rand(pMeshDesc->vSize.x, pMeshDesc->vSize.y);
		_float		fLifeTime = m_pGameInstance->Rand(pMeshDesc->vLifeTime.x, pMeshDesc->vLifeTime.y);

		XMVECTOR rotation;
		if (m_sData.fRotation.x == 0.f && m_sData.fRotation.y == 0.f && m_sData.fRotation.z == 0.f && m_sData.iNumInstance > 1)
			rotation = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(m_pGameInstance->Rand(0, 360)),
				XMConvertToRadians(m_pGameInstance->Rand(0, 360)),
				XMConvertToRadians(m_pGameInstance->Rand(0, 360)));
		else
			rotation = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(m_sData.fRotation.x), XMConvertToRadians(m_sData.fRotation.y), XMConvertToRadians(m_sData.fRotation.z));

		XMStoreFloat4(&pInstanceVertices[i].vRight, XMVector3Rotate(XMVectorSet(fScale, 0.f, 0.f, 0.f), rotation));
		XMStoreFloat4(&pInstanceVertices[i].vUp, XMVector3Rotate(XMVectorSet(0.f, fScale, 0.f, 0.f), rotation));
		XMStoreFloat4(&pInstanceVertices[i].vLook, XMVector3Rotate(XMVectorSet(0.f, 0.f, fScale * pMeshDesc->fSizeRatio, 0.f), rotation));

		//_matrix		RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_pGameInstance->Rand(0, 360)));

		//XMStoreFloat4(&pInstanceVertices[i].vRight, XMVector4Transform(XMVectorSet(fScale, 0.f, 0.f, 0.f), RotationMatrix));
		//XMStoreFloat4(&pInstanceVertices[i].vUp, XMVector4Transform(XMVectorSet(0.f, fScale, 0.f, 0.f), RotationMatrix));
		//XMStoreFloat4(&pInstanceVertices[i].vLook, XMVector4Transform(XMVectorSet(0.f, 0.f, fScale * pMeshDesc->fSizeRatio, 0.f), RotationMatrix));

		//XMStoreFloat4(&pInstanceVertices[i].vRight, XMVectorSet(1.f, 0.f, 0.f, 0.f) * fScale);
		//XMStoreFloat4(&pInstanceVertices[i].vUp, XMVectorSet(0.f, 1.f, 0.f, 0.f) * fScale);
		//XMStoreFloat4(&pInstanceVertices[i].vLook, XMVectorSet(0.f, 0.f, 1.f, 0.f) * fScale);

		if (m_sData.IsCircle)
		{
			_vector Dir = XMVectorSet(m_pGameInstance->Rand(-1.f, 1.f), 0.f, m_pGameInstance->Rand(-1.f, 1.f), 0.f);
			XMStoreFloat4(&pInstanceVertices[i].vTranslation, XMVectorSetW(XMVector4Normalize(Dir) * m_sData.fOffset, 1.f));
		}
		else
		{
			pInstanceVertices[i].vTranslation = _float4(
				m_pGameInstance->Rand(pMeshDesc->vCenter.x - pMeshDesc->vRange.x * 0.5f, pMeshDesc->vCenter.x + pMeshDesc->vRange.x * 0.5f),
				m_pGameInstance->Rand(pMeshDesc->vCenter.y - pMeshDesc->vRange.y * 0.5f, pMeshDesc->vCenter.y + pMeshDesc->vRange.y * 0.5f),
				m_pGameInstance->Rand(pMeshDesc->vCenter.z - pMeshDesc->vRange.z * 0.5f, pMeshDesc->vCenter.z + pMeshDesc->vRange.z * 0.5f),
				1.f
			);
		}

		pInstanceVertices[i].vLifeTime = _float2(0.f, fLifeTime);
		m_pParticleParams[i].vInitTranslation = pInstanceVertices[i].vTranslation;
		m_pParticleParams[i].vRight = pInstanceVertices[i].vRight;
		m_pParticleParams[i].vUp = pInstanceVertices[i].vUp;
		m_pParticleParams[i].vLook= pInstanceVertices[i].vLook;
		m_pParticleParams[i].fSize = fScale;
	} 

    if (FAILED(Ready_ComputeShader()))
        return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Mesh_Instance::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	if (FAILED(Ready_SRV(m_pParticleParams)))
		return E_FAIL;

	if (FAILED(Ready_UAV()))
		return E_FAIL;

	if (FAILED(Ready_CB()))
		return E_FAIL;

	return S_OK;
}

_bool CVIBuffer_Mesh_Instance::Update(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE SubResource;
	if (SUCCEEDED(m_pContext->Map(m_pCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource)))
	{
		POINT_INSTANCE_CB* pPointInstanceCB = reinterpret_cast<POINT_INSTANCE_CB*>(SubResource.pData);
		pPointInstanceCB->vPivot = m_vPivot;
		pPointInstanceCB->fTimeDelta = fTimeDelta;
		pPointInstanceCB->iNumInstances = m_iNumInstance;
		pPointInstanceCB->bIsLoop = m_bLoop;
		pPointInstanceCB->vSpawnRange = m_sData.vRange;
		m_pContext->Unmap(m_pCB, 0);
	}

	COMPUTE_PASS_DESC PassDesc{};
	PassDesc.SRVs.push_back(m_pSRV);
    PassDesc.SRVs.push_back(m_pSRVNoise);
	PassDesc.UAVs.push_back(m_pUAV);
	PassDesc.UAVs.push_back(m_pUAVSpeed);
	PassDesc.ConstantBuffers.push_back(m_pCB);
    m_pContext->CSSetSamplers(0, 1, &m_pLinearWrapSampler);
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

	return m_bLoop ? false : IsFinish();
}

void CVIBuffer_Mesh_Instance::UpdateGravity(_float fTimeDelta)
{
	COMPUTE_PASS_DESC PassDesc{};
    PassDesc.SRVs.push_back(m_pSRV);
    PassDesc.SRVs.push_back(m_pSRVNoise);
	PassDesc.UAVs.push_back(m_pUAV);
	PassDesc.UAVs.push_back(m_pUAVSpeed);
	PassDesc.ConstantBuffers.push_back(m_pCB);
    m_pContext->CSSetSamplers(0, 1, &m_pLinearWrapSampler);
	_uint iNumThreadPerGroup = 256;
	_uint iNumGroups = (m_iNumInstance + iNumThreadPerGroup - 1) / iNumThreadPerGroup;
	PassDesc.x = iNumGroups;
	PassDesc.y = 1;
	PassDesc.z = 1;

	CComputeShader_Manager::COMPUTE_JOB_DESC JobDesc{};
	JobDesc.pShader = m_ComputeShaders[ENUM_CLASS(CS_PASS::GRAVITY)];
	JobDesc.PassDesc = PassDesc;

	m_pGameInstance->Add_Job(COMPUTEJOB::UPDATE, JobDesc, true);

	m_pContext->CopyResource(m_pVBInstance, m_pStructuredBuffer);
}

void CVIBuffer_Mesh_Instance::UpdateTurbulence(_float fTimeDelta, _float fAccTime)
{

	D3D11_MAPPED_SUBRESOURCE SubResource;
	if (SUCCEEDED(m_pContext->Map(m_pCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource)))
	{
		POINT_INSTANCE_CB* pPointInstanceCB = reinterpret_cast<POINT_INSTANCE_CB*>(SubResource.pData);
		pPointInstanceCB->fTotalTime = fAccTime;
		pPointInstanceCB->fTimeDelta = fTimeDelta;
		pPointInstanceCB->iNumInstances = m_iNumInstance;
		pPointInstanceCB->fTurbulenceSpeed = m_sData.fTurbulenceSpeed;
		pPointInstanceCB->fTurbulenceSampleSize = m_sData.fTurbulenceSampleSize;
		m_pContext->Unmap(m_pCB, 0);
	}

	COMPUTE_PASS_DESC PassDesc{};
	PassDesc.SRVs.push_back(m_pSRV);
	PassDesc.SRVs.push_back(m_pSRVNoise);
	PassDesc.UAVs.push_back(m_pUAV);
	PassDesc.ConstantBuffers.push_back(m_pCB);
    m_pContext->CSSetSamplers(0, 1, &m_pLinearWrapSampler);
	_uint iNumThreadPerGroup = 256;
	_uint iNumGroups = (m_iNumInstance + iNumThreadPerGroup - 1) / iNumThreadPerGroup;
	PassDesc.x = iNumGroups;
	PassDesc.y = 1;
	PassDesc.z = 1;

	CComputeShader_Manager::COMPUTE_JOB_DESC JobDesc{};
	JobDesc.pShader = m_ComputeShaders[ENUM_CLASS(CS_PASS::TURBULENCE)];
	JobDesc.PassDesc = PassDesc;

	m_pGameInstance->Add_Job(COMPUTEJOB::UPDATE, JobDesc, true);

	m_pContext->CopyResource(m_pVBInstance, m_pStructuredBuffer);
}

void CVIBuffer_Mesh_Instance::Setting_Speed(SPEED_VALUE type, _float2 range)
{
	D3D11_MAPPED_SUBRESOURCE SubResource;
	if (SUCCEEDED(m_pContext->Map(m_pCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource)))
	{
		POINT_INSTANCE_CB* pInstanceSpeedCB = reinterpret_cast<POINT_INSTANCE_CB*>(SubResource.pData);
		pInstanceSpeedCB->iSpeedType = static_cast<_uint>(type);
		pInstanceSpeedCB->fSpeedRange = range;
		m_pContext->Unmap(m_pCB, 0);
	}

	COMPUTE_PASS_DESC PassDesc{};
    PassDesc.SRVs.push_back(m_pSRV);
    PassDesc.SRVs.push_back(m_pSRVNoise);
	PassDesc.UAVs.push_back(m_pUAV);
	PassDesc.UAVs.push_back(m_pUAVSpeed);
	PassDesc.ConstantBuffers.push_back(m_pCB);
    m_pContext->CSSetSamplers(0, 1, &m_pLinearWrapSampler);
	_uint iNumThreadPerGroup = 256;
	_uint iNumGroups = (m_iNumInstance + iNumThreadPerGroup - 1) / iNumThreadPerGroup;
	PassDesc.x = iNumGroups;
	PassDesc.y = 1;
	PassDesc.z = 1;

	CComputeShader_Manager::COMPUTE_JOB_DESC JobDesc{};
	JobDesc.pShader = m_ComputeShaders[ENUM_CLASS(CS_PASS::UPDATE_SPEED)];
	JobDesc.PassDesc = PassDesc;

	m_pGameInstance->Add_Job(COMPUTEJOB::UPDATE, JobDesc, true);
}

void CVIBuffer_Mesh_Instance::Remove_Speed(SPEED_VALUE type)
{
	D3D11_MAPPED_SUBRESOURCE SubResource;
	if (SUCCEEDED(m_pContext->Map(m_pCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource)))
	{
		POINT_INSTANCE_CB* pInstanceSpeedCB = reinterpret_cast<POINT_INSTANCE_CB*>(SubResource.pData);
		pInstanceSpeedCB->iSpeedType = static_cast<_uint>(type);
		pInstanceSpeedCB->iNumInstances = m_iNumInstance;
		m_pContext->Unmap(m_pCB, 0);
	}

	COMPUTE_PASS_DESC PassDesc{};
	PassDesc.SRVs.push_back(m_pSRV);
    PassDesc.SRVs.push_back(m_pSRVNoise);
	PassDesc.UAVs.push_back(m_pUAV);
	PassDesc.UAVs.push_back(m_pUAVSpeed);
	PassDesc.ConstantBuffers.push_back(m_pCB);
    m_pContext->CSSetSamplers(0, 1, &m_pLinearWrapSampler);
	_uint iNumThreadPerGroup = 256;
	_uint iNumGroups = (m_iNumInstance + iNumThreadPerGroup - 1) / iNumThreadPerGroup;
	PassDesc.x = iNumGroups;
	PassDesc.y = 1;
	PassDesc.z = 1;

	CComputeShader_Manager::COMPUTE_JOB_DESC JobDesc{};
	JobDesc.pShader = m_ComputeShaders[ENUM_CLASS(CS_PASS::RESET_SPEED)];
	JobDesc.PassDesc = PassDesc;

	m_pGameInstance->Add_Job(COMPUTEJOB::UPDATE, JobDesc, true);
    m_pContext->CopyResource(m_pVBInstance, m_pStructuredBuffer);
}

void CVIBuffer_Mesh_Instance::Remove_Speed()
{
    D3D11_MAPPED_SUBRESOURCE SubResource;
    if (SUCCEEDED(m_pContext->Map(m_pCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource)))
    {
        POINT_INSTANCE_CB* pInstanceSpeedCB = reinterpret_cast<POINT_INSTANCE_CB*>(SubResource.pData);
        pInstanceSpeedCB->iSpeedType = 4;
        pInstanceSpeedCB->iNumInstances = m_iNumInstance;
        m_pContext->Unmap(m_pCB, 0);
    }

    COMPUTE_PASS_DESC PassDesc{};
    PassDesc.SRVs.push_back(m_pSRV);
    PassDesc.SRVs.push_back(m_pSRVNoise);
    PassDesc.UAVs.push_back(m_pUAV);
    PassDesc.UAVs.push_back(m_pUAVSpeed);
    PassDesc.ConstantBuffers.push_back(m_pCB);
    m_pContext->CSSetSamplers(0, 1, &m_pLinearWrapSampler);
    _uint iNumThreadPerGroup = 256;
    _uint iNumGroups = (m_iNumInstance + iNumThreadPerGroup - 1) / iNumThreadPerGroup;
    PassDesc.x = iNumGroups;
    PassDesc.y = 1;
    PassDesc.z = 1;

    CComputeShader_Manager::COMPUTE_JOB_DESC JobDesc{};
    JobDesc.pShader = m_ComputeShaders[ENUM_CLASS(CS_PASS::RESET_SPEED)];
    JobDesc.PassDesc = PassDesc;

    m_pGameInstance->Add_Job(COMPUTEJOB::UPDATE, JobDesc, true);
    m_pContext->CopyResource(m_pVBInstance, m_pStructuredBuffer);
}

void CVIBuffer_Mesh_Instance::Setting_Pivot(_float3 pivot)
{
	m_vPivot = pivot;
}
HRESULT CVIBuffer_Mesh_Instance::Bind_Resources()
{
	__super::Bind_Resources();

	m_pContext->CopyResource(m_pDebugInstanceBuffer, m_pVBInstance);
	
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = m_pContext->Map(m_pDebugInstanceBuffer, 0, D3D11_MAP_READ, 0, &mappedResource);
	
	if (SUCCEEDED(hr))
	{
		IB_MESHINSTANCE_EFFECT* pParticles = (IB_MESHINSTANCE_EFFECT*)mappedResource.pData;
	
		m_pContext->Unmap(m_pDebugInstanceBuffer, 0);
	}

	return S_OK;;
}
HRESULT CVIBuffer_Mesh_Instance::Ready_SRV(void* pSysmem)
{
	ID3D11Buffer* pBuffer = { nullptr };

	D3D11_BUFFER_DESC ParticleParamsBufferDesc{};
	ParticleParamsBufferDesc.ByteWidth = sizeof(MESH_INSTANCE_PARAMS) * m_iNumInstance;
	ParticleParamsBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	ParticleParamsBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	ParticleParamsBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	ParticleParamsBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	ParticleParamsBufferDesc.StructureByteStride = sizeof(MESH_INSTANCE_PARAMS);

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

	_char fullpath[MAX_PATH];
	HRESULT     hr = {};
	_tchar		tpath[MAX_PATH] = {};
	MultiByteToWideChar(CP_UTF8, 0, m_sData.pNoiseFilePath, -1, tpath, 100);
	filesystem::path path(tpath);
	string FileExt = path.extension().string();

	if (FileExt == ".dds")
		hr = CreateDDSTextureFromFile(m_pDevice, tpath, nullptr, &m_pSRVNoise);
	else //png
		hr = CreateWICTextureFromFile(m_pDevice, tpath, nullptr, &m_pSRVNoise);

	if (FAILED(hr))
	{
		MSG_BOX(TEXT("Noise Texture :: Create Error!"));
		return E_FAIL;
	}

	return S_OK;
}
HRESULT CVIBuffer_Mesh_Instance::Ready_UAV()
{
	D3D11_BUFFER_DESC StructuredBufferDesc{};
	StructuredBufferDesc.ByteWidth = m_iNumInstance * m_iInstanceVertexStride;
	StructuredBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	StructuredBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	StructuredBufferDesc.CPUAccessFlags = 0;
	StructuredBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	StructuredBufferDesc.StructureByteStride = m_iInstanceVertexStride;

	D3D11_SUBRESOURCE_DATA InitData{};
	InitData.pSysMem = m_pInstanceVertices;
	if (FAILED(m_pDevice->CreateBuffer(&StructuredBufferDesc, &InitData, &m_pStructuredBuffer)))
		return E_FAIL;

	D3D11_UNORDERED_ACCESS_VIEW_DESC UAVDesc{};
	UAVDesc.Format = DXGI_FORMAT_UNKNOWN;
	UAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	UAVDesc.Buffer.FirstElement = 0;
	UAVDesc.Buffer.NumElements = m_iNumInstance;

	if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pStructuredBuffer, &UAVDesc, &m_pUAV)))
		return E_FAIL;

	StructuredBufferDesc.ByteWidth = m_iNumInstance * sizeof(POINT_INSTANCE_SPEED_PARAMS);
	StructuredBufferDesc.StructureByteStride = sizeof(POINT_INSTANCE_SPEED_PARAMS);
	if (FAILED(m_pDevice->CreateBuffer(&StructuredBufferDesc, nullptr, &m_pSpeedBuffer)))
		return E_FAIL;
	if (FAILED(m_pDevice->CreateUnorderedAccessView(m_pSpeedBuffer, &UAVDesc, &m_pUAVSpeed)))
		return E_FAIL;

	StructuredBufferDesc.Usage = D3D11_USAGE_STAGING;
	StructuredBufferDesc.BindFlags = 0;
	StructuredBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	if (FAILED(m_pDevice->CreateBuffer(&StructuredBufferDesc, nullptr, &m_pStagingBuffer)))
		return E_FAIL;

	// [Debug]
	D3D11_BUFFER_DESC DebugBufferDesc{};
	m_pVBInstance->GetDesc(&DebugBufferDesc);
	
	DebugBufferDesc.Usage = D3D11_USAGE_STAGING;     // 용도를 스테이징으로 변경
	DebugBufferDesc.BindFlags = 0;                   // GPU가 바인딩하지 않음
	DebugBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ; // CPU가 읽을 수 있게 설정
	DebugBufferDesc.MiscFlags = 0;
	
	D3D11_SUBRESOURCE_DATA DebugInitData{};
	DebugInitData.pSysMem = m_pInstanceVertices;
	if (FAILED(m_pDevice->CreateBuffer(&DebugBufferDesc, &DebugInitData, &m_pDebugInstanceBuffer)))
		return E_FAIL;
	
	return S_OK;
}
HRESULT CVIBuffer_Mesh_Instance::Ready_CB()
{
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
	m_ComputeShaders[ENUM_CLASS(CS_PASS::MOVE)] = CComputeShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Engine_Shader_Model_Instance_Compute.hlsl"), "CS_MOVE");
	if (nullptr == m_ComputeShaders[ENUM_CLASS(CS_PASS::MOVE)])
		return E_FAIL; 

	m_ComputeShaders[ENUM_CLASS(CS_PASS::GRAVITY)] = CComputeShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Engine_Shader_Point_Instance_Compute.hlsl"), "CS_UPDATE_GRAVITY");
	if (nullptr == m_ComputeShaders[ENUM_CLASS(CS_PASS::GRAVITY)])
		return E_FAIL;

	m_ComputeShaders[ENUM_CLASS(CS_PASS::UPDATE_SPEED)] = CComputeShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Engine_Shader_Model_Instance_Compute.hlsl"), "CS_UPDATE_SPEED");
	if (nullptr == m_ComputeShaders[ENUM_CLASS(CS_PASS::UPDATE_SPEED)])
		return E_FAIL;

	m_ComputeShaders[ENUM_CLASS(CS_PASS::RESET)] = CComputeShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Engine_Shader_Model_Instance_Compute.hlsl"), "CS_RESET");
	if (nullptr == m_ComputeShaders[ENUM_CLASS(CS_PASS::RESET)])
		return E_FAIL;

	
	m_ComputeShaders[ENUM_CLASS(CS_PASS::RESET_SPEED)] = CComputeShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Engine_Shader_Model_Instance_Compute.hlsl"), "CS_RESET_SPEED");
	if (nullptr == m_ComputeShaders[ENUM_CLASS(CS_PASS::RESET_SPEED)])
		return E_FAIL; 

	m_ComputeShaders[ENUM_CLASS(CS_PASS::TURBULENCE)] = CComputeShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Engine_Shader_Model_Instance_Compute.hlsl"), "CS_TURBULENCE");
	if (nullptr == m_ComputeShaders[ENUM_CLASS(CS_PASS::TURBULENCE)])
		return E_FAIL;

    m_ComputeShaders[ENUM_CLASS(CS_PASS::RESET_DEAD_FLAG)] = CComputeShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Engine_Shader_Model_Instance_Compute.hlsl"), "CS_RESET_DEAD_FLAG");
    if (nullptr == m_ComputeShaders[ENUM_CLASS(CS_PASS::RESET_DEAD_FLAG)])
        return E_FAIL;

    D3D11_SAMPLER_DESC samplerDesc = {};
    samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // Filter
    samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;    // AddressU
    samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;    // AddressV
    samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    samplerDesc.MinLOD = 0;
    samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    m_pDevice->CreateSamplerState(&samplerDesc, &m_pLinearWrapSampler);

	return S_OK;
}
_bool CVIBuffer_Mesh_Instance::IsFinish()
{ 
    _bool flag = false;

    m_pContext->CopyResource(m_pStagingBuffer, m_pSpeedBuffer);

    D3D11_MAPPED_SUBRESOURCE mappedResource;
    if (SUCCEEDED(m_pContext->Map(m_pStagingBuffer, 0, D3D11_MAP_READ, 0, &mappedResource)))
    {
        POINT_INSTANCE_SPEED_PARAMS aliveCount = *reinterpret_cast<POINT_INSTANCE_SPEED_PARAMS*>(mappedResource.pData);
        m_pContext->Unmap(m_pStagingBuffer, 0);

        if (aliveCount.bDead)
            flag = true;
    }

    COMPUTE_PASS_DESC PassDesc{};
    PassDesc.SRVs.push_back(m_pSRV);
    PassDesc.SRVs.push_back(m_pSRVNoise);
    PassDesc.UAVs.push_back(m_pUAV);
    PassDesc.UAVs.push_back(m_pUAVSpeed);
    m_pContext->CSSetSamplers(0, 1, &m_pLinearWrapSampler);
    _uint iNumThreadPerGroup = 256;
    _uint iNumGroups = (m_iNumInstance + iNumThreadPerGroup - 1) / iNumThreadPerGroup;
    PassDesc.x = iNumGroups;
    PassDesc.y = 1;
    PassDesc.z = 1;

    CComputeShader_Manager::COMPUTE_JOB_DESC JobDesc{};
    JobDesc.pShader = m_ComputeShaders[ENUM_CLASS(CS_PASS::RESET_DEAD_FLAG)];
    JobDesc.PassDesc = PassDesc;

    m_pGameInstance->Add_Job(COMPUTEJOB::UPDATE, JobDesc, true);

    return flag;
}
CVIBuffer_Mesh_Instance* CVIBuffer_Mesh_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, INSTANCE_DESC* pArg)
{
	CVIBuffer_Mesh_Instance* pInstance = new CVIBuffer_Mesh_Instance(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pArg)))
	{
		MSG_BOX(TEXT("Failed Created : CVIBuffer_Mesh_Instance"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Mesh_Instance::Clone(void* pArg)
{
	CVIBuffer_Mesh_Instance* pInstance = new CVIBuffer_Mesh_Instance(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CVIBuffer_Mesh_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Mesh_Instance::Free()
{
	__super::Free();

	Safe_Release(m_pCB);
	Safe_Release(m_pStructuredBuffer);
	Safe_Release(m_pSpeedBuffer);
	Safe_Release(m_pStagingBuffer);
	Safe_Release(m_pSRV);
	Safe_Release(m_pSRVNoise);
	Safe_Release(m_pUAV);
	Safe_Release(m_pUAVSpeed);

	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pParticleParams);
		//Safe_Release(m_pSRVNoise);
        for (_uint i = 0; i < CS_PASS::END; ++i)
            Safe_Release(m_ComputeShaders[i]);
        Safe_Release(m_pLinearWrapSampler);

	}
}

