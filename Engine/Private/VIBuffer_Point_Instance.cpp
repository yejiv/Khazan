#include "VIBuffer_Point_Instance.h"

#include "GameInstance.h"
#include "ComputeShader.h"

CVIBuffer_Point_Instance::CVIBuffer_Point_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer_Instance{ pDevice, pContext }
{
}

CVIBuffer_Point_Instance::CVIBuffer_Point_Instance(const CVIBuffer_Point_Instance& Prototype)
	: CVIBuffer_Instance(Prototype)
	, m_vPivot{ Prototype.m_vPivot }
	, m_pSpeeds{ Prototype.m_pSpeeds }
	, m_isLoop { Prototype.m_isLoop }
	, m_pComputeShader{ Prototype.m_pComputeShader }
	, m_pSRV { Prototype.m_pSRV }
	, m_pUAV { Prototype.m_pUAV }
	, m_pCB { Prototype.m_pCB }
	, m_pStructuredBuffer { Prototype.m_pStructuredBuffer}
{
}

HRESULT CVIBuffer_Point_Instance::Initialize_Prototype(const INSTANCE_DESC* pDesc)
{
	const POINT_INSTANCE_DESC* pPointDesc= static_cast<const POINT_INSTANCE_DESC*>(pDesc);

	m_vPivot = pPointDesc->vPivot;
	m_isLoop = pPointDesc->isLoop;	
	
	m_iInstanceVertexStride = sizeof(VTXINSTANCE_PARTICLE);
	m_iNumInstance = pPointDesc->iNumInstance;
	m_iNumVertices = 1;
	m_iVertexStride = sizeof(VTXPOS);	
	m_iNumVertexBuffers = 2;
	m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

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

	// Default Code
	//	m_VBInstanceDesc.ByteWidth = m_iNumInstance * m_iInstanceVertexStride;
	//	m_VBInstanceDesc.Usage = D3D11_USAGE_DYNAMIC;
	//	m_VBInstanceDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//	m_VBInstanceDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//	m_VBInstanceDesc.MiscFlags = 0;
	//	m_VBInstanceDesc.StructureByteStride = m_iInstanceVertexStride;
	
	// Compute Shader
	m_VBInstanceDesc.ByteWidth = m_iNumInstance * m_iInstanceVertexStride;
	m_VBInstanceDesc.Usage = D3D11_USAGE_DEFAULT;
	m_VBInstanceDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBInstanceDesc.CPUAccessFlags = 0;
	m_VBInstanceDesc.MiscFlags = 0;
	m_VBInstanceDesc.StructureByteStride = m_iInstanceVertexStride;

	m_pInstanceVertices = new VTXINSTANCE_PARTICLE[m_iNumInstance];
	m_pSpeeds = new _float[m_iNumInstance];

	PARTICLE_PARAMS* pParticleParams = new PARTICLE_PARAMS[m_iNumInstance];

	for (size_t i = 0; i < m_iNumInstance; i++)
	{			
		VTXINSTANCE_PARTICLE* pInstanceVertices = static_cast<VTXINSTANCE_PARTICLE*>(m_pInstanceVertices);

		_float		fScale = m_pGameInstance->Rand(pPointDesc->vSize.x, pPointDesc->vSize.y);
		_float		fLifeTime = m_pGameInstance->Rand(pPointDesc->vLifeTime.x, pPointDesc->vLifeTime.y);
		m_pSpeeds[i] = m_pGameInstance->Rand(pPointDesc->vSpeed.x, pPointDesc->vSpeed.y);

		pInstanceVertices[i].vRight = _float4(fScale, 0.f, 0.f, 0.f);
		pInstanceVertices[i].vUp = _float4(0.f, fScale, 0.f, 0.f);
		pInstanceVertices[i].vLook = _float4(0.f, 0.f, fScale, 0.f);
		pInstanceVertices[i].vTranslation = _float4(
			m_pGameInstance->Rand(pPointDesc->vCenter.x - pPointDesc->vRange.x * 0.5f, pPointDesc->vCenter.x + pPointDesc->vRange.x * 0.5f), 
			m_pGameInstance->Rand(pPointDesc->vCenter.y - pPointDesc->vRange.y * 0.5f, pPointDesc->vCenter.y + pPointDesc->vRange.y * 0.5f),
			m_pGameInstance->Rand(pPointDesc->vCenter.z - pPointDesc->vRange.z * 0.5f, pPointDesc->vCenter.z + pPointDesc->vRange.z * 0.5f),
			1.f
		);

		pInstanceVertices[i].vLifeTime = _float2(0.f, fLifeTime);

		// Compute Shader
		pParticleParams[i].fSpeed = m_pSpeeds[i];
		pParticleParams[i].vInitTranslation = pInstanceVertices[i].vTranslation;
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

HRESULT CVIBuffer_Point_Instance::Initialize_Clone(void* pArg)
{
 	if (FAILED(__super::Initialize_Clone(pArg)))
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

	return S_OK;
}

void CVIBuffer_Point_Instance::Spread(_float fTimeDelta)
{
	//	D3D11_MAPPED_SUBRESOURCE	SubResource{};
	//	
	//	VTXINSTANCE_PARTICLE* pInstanceVertices = static_cast<VTXINSTANCE_PARTICLE*>(m_pInstanceVertices);
	//	
	//	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	//	
	//	VTXINSTANCE_PARTICLE* pVertices = static_cast<VTXINSTANCE_PARTICLE*>(SubResource.pData);
	//	
	//	for (size_t i = 0; i < m_iNumInstance; i++)
	//	{
	//		_vector	vMoveDir = XMVector3Normalize(XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_vPivot), 0.f));
	//	
	//		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + vMoveDir * m_pSpeeds[i] * fTimeDelta);
	//		pVertices[i].vLifeTime.x += fTimeDelta;
	//	
	//		if (true == m_isLoop)
	//		{
	//			if (pVertices[i].vLifeTime.x >= pVertices[i].vLifeTime.y)
	//			{
	//				pVertices[i].vLifeTime.x = 0.f;
	//				pVertices[i].vTranslation = pInstanceVertices[i].vTranslation;
	//			}
	//		}
	//	}
	//	
	//	m_pContext->Unmap(m_pVBInstance, 0);

	// 상수 버퍼 업데이트
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
	JobDesc.pShader = m_pComputeShader;
	JobDesc.PassDesc = PassDesc;

	m_pGameInstance->Add_Job(COMPUTEJOB::UPDATE, JobDesc, true);

	m_pContext->CopyResource(m_pVBInstance, m_pStructuredBuffer);
}

void CVIBuffer_Point_Instance::Drop(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	VTXINSTANCE_PARTICLE* pInstanceVertices = static_cast<VTXINSTANCE_PARTICLE*>(m_pInstanceVertices);

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXINSTANCE_PARTICLE* pVertices = static_cast<VTXINSTANCE_PARTICLE*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		_vector	vMoveDir = XMVectorSet(0.f, -1.f, 0.f, 0.f);

		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + vMoveDir * m_pSpeeds[i] * fTimeDelta);
		pVertices[i].vLifeTime.x += fTimeDelta;

		if (true == m_isLoop)
		{
			if (pVertices[i].vLifeTime.x >= pVertices[i].vLifeTime.y)
			{
				pVertices[i].vLifeTime.x = 0.f;
				pVertices[i].vTranslation = pInstanceVertices[i].vTranslation;
			}
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

HRESULT CVIBuffer_Point_Instance::Ready_ShaderResourceView(void* pSysmem)
{
	// 1. SRV로 쓸 파티클 파라미터 버퍼 만들기
	ID3D11Buffer* pBuffer = { nullptr };

	D3D11_BUFFER_DESC ParticleParamsBufferDesc{};
	ParticleParamsBufferDesc.ByteWidth = sizeof(PARTICLE_PARAMS) * m_iNumInstance;
	ParticleParamsBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	ParticleParamsBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	ParticleParamsBufferDesc.CPUAccessFlags = 0;
	ParticleParamsBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	ParticleParamsBufferDesc.StructureByteStride = sizeof(PARTICLE_PARAMS);

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

HRESULT CVIBuffer_Point_Instance::Ready_UnorderedAccessView()
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

HRESULT CVIBuffer_Point_Instance::Ready_ConstantBuffer()
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

HRESULT CVIBuffer_Point_Instance::Ready_ComputeShader()
{
	// 4. 컴퓨트 셰이더 생성
	m_pComputeShader = CComputeShader::Create(m_pDevice, m_pContext, TEXT("../Bin/ShaderFiles/Engine_Shader_Compute.hlsl"), "CS_SPREAD");
	if (nullptr == m_pComputeShader)
		return E_FAIL;

	return S_OK;
}

CVIBuffer_Point_Instance* CVIBuffer_Point_Instance::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const INSTANCE_DESC* pDesc)
{
	CVIBuffer_Point_Instance* pInstance = new CVIBuffer_Point_Instance(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pDesc)))
	{
		MSG_BOX(TEXT("Failed to Create : CVIBuffer_Point_Instance"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Point_Instance::Clone(void* pArg)
{
	CVIBuffer_Point_Instance* pInstance = new CVIBuffer_Point_Instance(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Clone : CVIBuffer_Point_Instance"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Point_Instance::Free()
{
	__super::Free();

	if (false == m_isCloned)
		Safe_Delete_Array(m_pSpeeds);

	Safe_Release(m_pComputeShader);
	Safe_Release(m_pStructuredBuffer);
	Safe_Release(m_pCB);
	Safe_Release(m_pUAV);
	Safe_Release(m_pSRV);
}
