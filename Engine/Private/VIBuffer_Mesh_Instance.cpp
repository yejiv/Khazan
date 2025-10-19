#include "VIBuffer_Mesh_Instance.h"
#include "GameInstance.h"

CVIBuffer_Mesh_Instance::CVIBuffer_Mesh_Instance(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CVIBuffer_Instance { pDevice, pDeviceContext }
{
}

CVIBuffer_Mesh_Instance::CVIBuffer_Mesh_Instance(const CVIBuffer_Mesh_Instance& Prototype)
	: CVIBuffer_Instance { Prototype }
	, m_vPivot{ Prototype.m_vPivot }
	//, m_pSpeeds{ Prototype.m_pSpeeds }
	, m_IsLoop{ Prototype.m_IsLoop }
	, m_fRotationPerSec{ Prototype.m_fRotationPerSec }
	, m_fOffset{ Prototype.m_fOffset }
	, m_fRange{ Prototype.m_fRange }
	, m_fScale{ Prototype.m_fScale } 
{
}

void CVIBuffer_Mesh_Instance::Reset()
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	VTXINSTANCE_PARTICLE* pInstanceVertices = static_cast<VTXINSTANCE_PARTICLE*>(m_pInstanceVertices);

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXINSTANCE_PARTICLE* pVertices = static_cast<VTXINSTANCE_PARTICLE*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		pVertices[i].vLifeTime.x = 0.f;
		pVertices[i].vTranslation = pInstanceVertices[i].vTranslation;
	}

	Remove_Speed();

	m_pContext->Unmap(m_pVBInstance, 0);
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
	m_iInstanceVertexStride = sizeof(VTXINSTANCE_PARTICLE);
	m_iNumVertices = tMeshInfo.iNumVertices;
	m_iVertexStride = sizeof(MESHINSTANCE_PARTICLE);
	m_iNumIndices = tMeshInfo.iNumFace * 3;
	m_iIndexStride = 4;
	m_iNumVertexBuffers = 2;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_vPivot = pMeshDesc->vPivot;
	m_fRange = pMeshDesc->vRange;
	m_bIsCircle = pMeshDesc->IsCircle;
	m_fRotationPerSec = pMeshDesc->fRotationPerSec;
	m_fOffset = pMeshDesc->fOffset;

	D3D11_BUFFER_DESC		VBDesc{};
	VBDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	VBDesc.Usage = D3D11_USAGE_DEFAULT;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = 0;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	MESHINSTANCE_PARTICLE* pVertices = new MESHINSTANCE_PARTICLE[m_iNumVertices];
	m_pVertexPositions = new _float3[m_iNumVertices];
	ZeroMemory(m_pVertexPositions, sizeof(_float3) * m_iNumVertices);
	for (_uint i = 0; i < m_iNumVertices; ++i)
	{
		memcpy(&pVertices[i].vPosition, &tMeshInfo.vecVertices[i].position, sizeof(_float3));
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

	m_pInstanceVertices = new VTXINSTANCE_PARTICLE[m_iNumInstance];
	for (_uint i = 0; i < ENUM_CLASS(SPEED_VALUE::SPEED_END); ++i)
	{
		m_fSpeed[i] = new _float[m_iNumInstance];
		ZeroMemory(m_fSpeed[i], sizeof(_float) * m_iNumInstance);
	}

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		VTXINSTANCE_PARTICLE* pInstanceVertices = static_cast<VTXINSTANCE_PARTICLE*>(m_pInstanceVertices);

		_float		fScale = m_pGameInstance->Rand(pMeshDesc->vSize.x, pMeshDesc->vSize.y);
		_float		fLifeTime = m_pGameInstance->Rand(pMeshDesc->vLifeTime.x, pMeshDesc->vLifeTime.y);

		_matrix		RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), XMConvertToRadians(m_pGameInstance->Rand(0, 360)));

		XMStoreFloat4(&pInstanceVertices[i].vRight, XMVector4Transform(XMVectorSet(fScale, 0.f, 0.f, 0.f), RotationMatrix));
		XMStoreFloat4(&pInstanceVertices[i].vUp, XMVector4Transform(XMVectorSet(0.f, fScale, 0.f, 0.f), RotationMatrix));
		XMStoreFloat4(&pInstanceVertices[i].vLook, XMVector4Transform(XMVectorSet(0.f, 0.f, fScale * pMeshDesc->fSizeRatio, 0.f), RotationMatrix));

		if (m_bIsCircle)
		{
			_vector Dir = XMVectorSet(m_pGameInstance->Rand(-1.f, 1.f), 0.f, m_pGameInstance->Rand(-1.f, 1.f), 0.f);
			XMStoreFloat4(&pInstanceVertices[i].vTranslation, XMVectorSetW(XMVector4Normalize(Dir) * m_fOffset, 1.f));
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
		m_fRange = pMeshDesc->vRange;
		m_fScale = pMeshDesc->vSize;
	}

	return S_OK;
}

HRESULT CVIBuffer_Mesh_Instance::Initialize_Clone(void* pArg)
{
	if (FAILED(__super::Initialize_Clone(pArg)))
		return E_FAIL;

	return S_OK;
}

void CVIBuffer_Mesh_Instance::Update(_float fTimeDelta)
{
	D3D11_MAPPED_SUBRESOURCE SubResource = {};

	VTXINSTANCE_PARTICLE* pInstanceVertices = static_cast<VTXINSTANCE_PARTICLE*>(m_pInstanceVertices);

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);

	VTXINSTANCE_PARTICLE* pVertices = static_cast<VTXINSTANCE_PARTICLE*>(SubResource.pData);

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		//Scale
		pVertices[i].vRight.x += m_fSpeed[ENUM_CLASS(SPEED_VALUE::SCALE_SPEED)][i];
		pVertices[i].vUp.y += m_fSpeed[ENUM_CLASS(SPEED_VALUE::SCALE_SPEED)][i];
		pVertices[i].vLook.z += m_fSpeed[ENUM_CLASS(SPEED_VALUE::SCALE_SPEED)][i];

		//Rotation
		if (m_fSpeed[ENUM_CLASS(SPEED_VALUE::ROTATION_SPEED)][i])
		{
			_matrix		RotationMatrix = XMMatrixRotationAxis(XMVectorSet(0.f, 1.f, 0.f, 0.f), m_fSpeed[ENUM_CLASS(SPEED_VALUE::ROTATION_SPEED)][i] * fTimeDelta);
			_vector		Pivot_Pos = XMVectorSetW(XMLoadFloat3(&m_vPivot), 1.f);
			_matrix		Pivot_World = XMMatrixTranslationFromVector(Pivot_Pos);

			_matrix final_Matrix = RotationMatrix * Pivot_World;
			_vector pos = XMLoadFloat4(&pVertices[i].vTranslation);
			pos = XMVector4Transform(pos, final_Matrix);
			XMStoreFloat4(&pVertices[i].vTranslation, pos);
		}

		//Spread
		_vector	vMoveDir = XMVector3Normalize(XMVectorSetW(XMLoadFloat4(&pVertices[i].vTranslation) - XMLoadFloat3(&m_vPivot), 0.f));
		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + vMoveDir * m_fSpeed[ENUM_CLASS(SPEED_VALUE::SPREAD_SPEED)][i] * fTimeDelta);

		//MoveLinear
		vMoveDir = XMVectorSet(0.f, 1.f, 0.f, 0.f);
		XMStoreFloat4(&pVertices[i].vTranslation, XMLoadFloat4(&pVertices[i].vTranslation) + vMoveDir * m_fSpeed[ENUM_CLASS(SPEED_VALUE::UPWARD_SPEED)][i] * fTimeDelta);

		pVertices[i].vLifeTime.x += fTimeDelta;

		if (pVertices[i].vLifeTime.x >= pVertices[i].vLifeTime.y)
		{
			pVertices[i].vLifeTime.x = 0.f;
			pVertices[i].vTranslation = pInstanceVertices[i].vTranslation;
			pVertices[i].bDead = true;
		}

		if (pVertices[i].vRight.x <= 0.f)
		{
			_float		fScale = m_pGameInstance->Rand(m_fScale.x, m_fScale.y);
			pVertices[i].vRight = _float4(fScale, 0.f, 0.f, 0.f);
			pVertices[i].vUp = _float4(0.f, fScale, 0.f, 0.f);
			pVertices[i].vLook = _float4(0.f, 0.f, fScale, 0.f);
		}
	}

	m_pContext->Unmap(m_pVBInstance, 0);
}

void CVIBuffer_Mesh_Instance::Setting_Speed(SPEED_VALUE type, _float2 range)
{
	for (size_t i = 0; i < m_iNumInstance; i++)
		m_fSpeed[ENUM_CLASS(type)][i] = m_pGameInstance->Rand(range.x, range.y);
}

void CVIBuffer_Mesh_Instance::Remove_Speed(SPEED_VALUE type)
{
	ZeroMemory(m_fSpeed[ENUM_CLASS(type)], sizeof(_float) * m_iNumInstance);
}

void CVIBuffer_Mesh_Instance::Remove_Speed()
{
	for(_uint i = 0; i < ENUM_CLASS(SPEED_VALUE::SPEED_END); ++i) 
		ZeroMemory(m_fSpeed[i],sizeof(_float) * m_iNumInstance); 
}

void CVIBuffer_Mesh_Instance::Setting_Pivot(_float3 pivot)
{
	m_vPivot = pivot;
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

	if (false == m_isCloned)
	{
		for (_uint i = 0; i < ENUM_CLASS(SPEED_VALUE::SPEED_END); ++i) 
			Safe_Delete(m_fSpeed[i]);  
	}
}

