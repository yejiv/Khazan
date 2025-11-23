#include "VIBuffer_Point_Instance.h"
#include "GameInstance.h"
#include "ComputeShader.h"

CVIBuffer_Point_Instance::CVIBuffer_Point_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CVIBuffer_Instance { pDevice, pDeviceContext }
{
}

CVIBuffer_Point_Instance::CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance& Prototype)
	: CVIBuffer_Instance { Prototype }
	, m_pParticleParams{ Prototype.m_pParticleParams}
	, m_sData {Prototype.m_sData}

{
    for (_uint i = 0; i < CS_PASS::END; ++i) 
        m_ComputeShaders[i] = Prototype.m_ComputeShaders[i]; 
    m_pLinearWrapSampler = Prototype.m_pLinearWrapSampler;
	//Safe_AddRef(m_pSRVNoise); //이거 해줘야되는지 확인좀
}

void CVIBuffer_Point_Instance::Reset()
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

HRESULT CVIBuffer_Point_Instance::Initialize_Prototype(const INSTANCE_DESC* pDesc)
{
	const POINT_INSTANCE_DESC* pPointDesc = static_cast<const POINT_INSTANCE_DESC*>(pDesc);

	m_iInstanceVertexStride = sizeof(IB_POINTINSTANCE_EFFECT);
	m_iNumInstance = pPointDesc->iNumInstance;
	m_iNumVertices = 1;
	m_iVertexStride = sizeof(VTXPOS);
	m_iNumVertexBuffers = 2;
	m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

	m_sData.IsCircle = pPointDesc->IsCircle;
	m_sData.fOffset = pPointDesc->fOffset;
	m_sData.vRange = pPointDesc->vRange;
	m_sData.fTurbulenceSpeed = pPointDesc->fTurbulenceSpeed;
	m_sData.fTurbulenceSampleSize = pPointDesc->fTurbulenceSampleSize;
    m_sData.bIsLoop = pPointDesc->bIsLoop;
    m_bLoop = m_sData.bIsLoop;
    memcpy(m_sData.pNoiseFilePath, pPointDesc->pNoiseFilePath, sizeof(pPointDesc->pNoiseFilePath));

	D3D11_BUFFER_DESC		VBDesc{};
	VBDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	VTXPOS* pVertices = new VTXPOS[m_iNumVertices];

	pVertices[0].vPosition = _float3(0.0f, 0.0f, 0.f);

	D3D11_SUBRESOURCE_DATA	VBInitialData{};
	VBInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &VBInitialData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);
	m_VBInstanceDesc.ByteWidth = m_iNumInstance * m_iInstanceVertexStride;
	m_VBInstanceDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_VBInstanceDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBInstanceDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_VBInstanceDesc.MiscFlags = 0;
	m_VBInstanceDesc.StructureByteStride = m_iInstanceVertexStride;

	m_pInstanceVertices = new IB_POINTINSTANCE_EFFECT[m_iNumInstance]; 
	m_pParticleParams = new POINT_INSTANCE_PARAMS[m_iNumInstance];

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		IB_POINTINSTANCE_EFFECT* pInstanceVertices = static_cast<IB_POINTINSTANCE_EFFECT*>(m_pInstanceVertices);

		_float		fScale = m_pGameInstance->Rand(pPointDesc->vSize.x, pPointDesc->vSize.y);
		_float		fLifeTime = m_pGameInstance->Rand(pPointDesc->vLifeTime.x, pPointDesc->vLifeTime.y);

		if (m_sData.IsCircle)
		{
			_vector Dir = XMVectorSet(m_pGameInstance->Rand(-1.f, 1.f), 0.f, m_pGameInstance->Rand(-1.f, 1.f), 0.f);
			XMStoreFloat4(&pInstanceVertices[i].vTranslation, XMVectorSetW(XMVector4Normalize(Dir) * m_sData.fOffset, 1.f));
		}
		else
		{
			pInstanceVertices[i].vTranslation = _float4(
				m_pGameInstance->Rand(pPointDesc->vCenter.x - pPointDesc->vRange.x * 0.5f, pPointDesc->vCenter.x + pPointDesc->vRange.x * 0.5f),
				m_pGameInstance->Rand(pPointDesc->vCenter.y - pPointDesc->vRange.y * 0.5f, pPointDesc->vCenter.y + pPointDesc->vRange.y * 0.5f),
				m_pGameInstance->Rand(pPointDesc->vCenter.z - pPointDesc->vRange.z * 0.5f, pPointDesc->vCenter.z + pPointDesc->vRange.z * 0.5f),
				1.f
			);
		}

		_vector vDir = XMVectorSetW(XMVector3Normalize(XMVectorSubtract(XMLoadFloat4(&pInstanceVertices[i].vTranslation), XMLoadFloat3(&m_vPivot))), 0.f);

		pInstanceVertices[i].vRight = _float4(fScale, 0.f, 0.f, 0.f);
		pInstanceVertices[i].vUp = _float4(0.f, fScale, 0.f, 0.f);
		pInstanceVertices[i].vLook = _float4(0.f, 0.f, fScale, 0.f);

		if (XMVectorGetX(XMVector3Length(vDir)) >= 0.1f)
		{
			_vector vLook = XMVector3Normalize(XMVectorSetW(XMVectorSubtract(XMLoadFloat4(m_pGameInstance->Get_CamPosition()), XMLoadFloat4(&pInstanceVertices[i].vTranslation)), 0.f));

			XMStoreFloat4(&pInstanceVertices[i].vUp, XMVectorScale(vDir, fScale));
			XMStoreFloat4(&pInstanceVertices[i].vRight, XMVectorScale(XMVector3Normalize(XMVector3Cross(XMVector3Normalize(XMLoadFloat4(&pInstanceVertices[i].vUp)), vLook)), fScale));
			XMStoreFloat4(&pInstanceVertices[i].vLook, XMVectorScale(XMVector3Normalize(XMVector3Cross(XMVector3Normalize(XMLoadFloat4(&pInstanceVertices[i].vUp)), XMLoadFloat4(&pInstanceVertices[i].vRight))), fScale));
		}

		pInstanceVertices[i].vLifeTime = _float2(0.f, fLifeTime);
		m_pParticleParams[i].vInitTranslation = pInstanceVertices[i].vTranslation;
		m_pParticleParams[i].fSize = fScale;
	}
     
    if (FAILED(Ready_ComputeShader()))
        return E_FAIL;
	 
	return S_OK;
}

HRESULT CVIBuffer_Point_Instance::Initialize_Clone(void* pArg)
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

HRESULT CVIBuffer_Point_Instance::Bind_Resources()
{
	ID3D11Buffer* pVertexBuffers[] = {
	m_pVB,
	m_pVBInstance,
	};

	_uint		iVertexStrides[] = {
		m_iVertexStride,
		m_iInstanceVertexStride,
	};

	_uint		iOffsets[] = {
		0,
		0
	};

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveType);

	return S_OK;
}

HRESULT CVIBuffer_Point_Instance::Render()
{
	m_pContext->DrawInstanced(1, m_iNumInstance, 0, 0);

    ////debug
    //m_pContext->CopyResource(m_pDebugStagingBuffer, m_pVBInstance);
    //
    //D3D11_MAPPED_SUBRESOURCE mappedResource;
    //if (SUCCEEDED(m_pContext->Map(m_pDebugStagingBuffer, 0, D3D11_MAP_READ, 0, &mappedResource)))
    //{
    //    IB_POINTINSTANCE_EFFECT aliveCount = *reinterpret_cast<IB_POINTINSTANCE_EFFECT*>(mappedResource.pData);
    //    m_pContext->Unmap(m_pStagingBuffer, 0);
    //}
    ////debug end
	
	return S_OK;
}

_bool CVIBuffer_Point_Instance::Update(_float fTimeDelta)
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

	return  m_bLoop ? false : IsFinish();	//다 끝나면 true 반환
}

void CVIBuffer_Point_Instance::UpdateGravity(_float fTimeDelta)
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

void CVIBuffer_Point_Instance::UpdateTurbulence(_float fTimeDelta, _float fAccTime)
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

void CVIBuffer_Point_Instance::Setting_Speed(SPEED_VALUE type, _float2 range)
{
	D3D11_MAPPED_SUBRESOURCE SubResource;
	if (SUCCEEDED(m_pContext->Map(m_pCB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource)))
	{
		POINT_INSTANCE_CB* pInstanceSpeedCB = reinterpret_cast<POINT_INSTANCE_CB*>(SubResource.pData);
		pInstanceSpeedCB->iSpeedType = static_cast<_uint>(type);
		pInstanceSpeedCB->fSpeedRange = range;
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
	JobDesc.pShader = m_ComputeShaders[ENUM_CLASS(CS_PASS::UPDATE_SPEED)];
	JobDesc.PassDesc = PassDesc;
	
	m_pGameInstance->Add_Job(COMPUTEJOB::UPDATE, JobDesc, true);
}

void CVIBuffer_Point_Instance::Remove_Speed(SPEED_VALUE type)
{
	//if (m_IsLoop == true)
	//	return;

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

void CVIBuffer_Point_Instance::Remove_Speed()
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

void CVIBuffer_Point_Instance::Setting_Pivot(_float3 pivot)
{
	m_vPivot = pivot;
}

HRESULT CVIBuffer_Point_Instance::Ready_SRV(void* pSysmem)
{
	ID3D11Buffer* pBuffer = { nullptr };

	D3D11_BUFFER_DESC ParticleParamsBufferDesc{};
	ParticleParamsBufferDesc.ByteWidth = sizeof(POINT_INSTANCE_PARAMS) * m_iNumInstance;
	ParticleParamsBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	ParticleParamsBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	ParticleParamsBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
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

	//  //Debug
	//  //여기서 Instance Buffer를 깔 수 있는 Staging Buffer를 만들어야됨
	//  //m_pVBInstance 이 내용을 system으로 넣어주는 Staing버퍼 만들기
	//  D3D11_BUFFER_DESC DebugStaingBufferDesc{};
	//  DebugStaingBufferDesc = m_VBInstanceDesc;
	//  DebugStaingBufferDesc.Usage = D3D11_USAGE_STAGING;
	//  DebugStaingBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	//  DebugStaingBufferDesc.BindFlags = 0;
	//  if (FAILED(m_pDevice->CreateBuffer(&DebugStaingBufferDesc, nullptr, &m_pDebugStagingBuffer)))
	//  	return E_FAIL;
	//  //Debug End


	return S_OK;
}

HRESULT CVIBuffer_Point_Instance::Ready_UAV()
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

	return S_OK;
}

HRESULT CVIBuffer_Point_Instance::Ready_CB()
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

HRESULT CVIBuffer_Point_Instance::Ready_ComputeShader()
{
	m_ComputeShaders[ENUM_CLASS(CS_PASS::MOVE)] = CComputeShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Engine_Shader_Point_Instance_Compute.hlsl"), "CS_MOVE");
	if (nullptr == m_ComputeShaders[ENUM_CLASS(CS_PASS::MOVE)])
		return E_FAIL;

	m_ComputeShaders[ENUM_CLASS(CS_PASS::GRAVITY)] = CComputeShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Engine_Shader_Point_Instance_Compute.hlsl"), "CS_UPDATE_GRAVITY");
	if (nullptr == m_ComputeShaders[ENUM_CLASS(CS_PASS::GRAVITY)])
		return E_FAIL;

	m_ComputeShaders[ENUM_CLASS(CS_PASS::UPDATE_SPEED)] = CComputeShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Engine_Shader_Point_Instance_Compute.hlsl"), "CS_UPDATE_SPEED");
	if (nullptr == m_ComputeShaders[ENUM_CLASS(CS_PASS::UPDATE_SPEED)])
		return E_FAIL;

	m_ComputeShaders[ENUM_CLASS(CS_PASS::RESET)] = CComputeShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Engine_Shader_Point_Instance_Compute.hlsl"), "CS_RESET");
	if (nullptr == m_ComputeShaders[ENUM_CLASS(CS_PASS::RESET)])
		return E_FAIL;

	m_ComputeShaders[ENUM_CLASS(CS_PASS::RESET_SPEED)] = CComputeShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Engine_Shader_Point_Instance_Compute.hlsl"), "CS_RESET_SPEED");
	if (nullptr == m_ComputeShaders[ENUM_CLASS(CS_PASS::RESET_SPEED)])
		return E_FAIL;

	m_ComputeShaders[ENUM_CLASS(CS_PASS::TURBULENCE)] = CComputeShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Engine_Shader_Point_Instance_Compute.hlsl"), "CS_TURBULENCE");
	if (nullptr == m_ComputeShaders[ENUM_CLASS(CS_PASS::TURBULENCE)])
		return E_FAIL;

    m_ComputeShaders[ENUM_CLASS(CS_PASS::RESET_DEAD_FLAG)] = CComputeShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Engine_Shader_Point_Instance_Compute.hlsl"), "CS_RESET_DEAD_FLAG");
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

_bool CVIBuffer_Point_Instance::IsFinish()
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

CVIBuffer_Point_Instance* CVIBuffer_Point_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const INSTANCE_DESC* pDesc)
{
	CVIBuffer_Point_Instance* pInstance = new CVIBuffer_Point_Instance(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pDesc)))
	{
		MSG_BOX(TEXT("Failed Created : CVIBuffer_Point_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Point_Instance::Clone(void* pArg)
{
	CVIBuffer_Point_Instance* pInstance = new CVIBuffer_Point_Instance(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CVIBuffer_Point_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Point_Instance::Free()
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
	//Safe_Release(m_pDebugStagingBuffer); 

	if (false == m_isCloned)
	{
		Safe_Delete_Array(m_pParticleParams);
		//Safe_Release(m_pSRVNoise);
        for (_uint i = 0; i < CS_PASS::END; ++i)
            Safe_Release(m_ComputeShaders[i]);
        Safe_Release(m_pLinearWrapSampler);

	}
}
