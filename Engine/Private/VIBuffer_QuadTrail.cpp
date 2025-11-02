#include "VIBuffer_QuadTrail.h"

CVIBuffer_QuadTrail::CVIBuffer_QuadTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer { pDevice, pContext }
{
}

CVIBuffer_QuadTrail::CVIBuffer_QuadTrail(const CVIBuffer_QuadTrail& Prototype)
	: CVIBuffer{ Prototype }
{
}

HRESULT CVIBuffer_QuadTrail::Initialize_Prototype()
{
	/*매번 업데이트때 갱신(1)*/
	//필요한 인덱스 개수 = ((총 정점 개수 / 2) - 1) * 6


	m_iNumVertices = 2048;
	m_iNumIndices = ((m_iNumVertices / 2) - 1) * 6;
	m_iVertexStride = sizeof(VTXPOSTEX);
	m_iIndexStride = 2;
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	D3D11_BUFFER_DESC		VBDesc{};

	VBDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	VBDesc.Usage = D3D11_USAGE_DYNAMIC;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	///*매번 업데이트때 갱신*/
	//VTXPOSTEX* pVertices = new VTXPOSTEX[m_iNumVertices];
	//
	//pVertices[0].vPosition = _float3(-0.5f, 0.5f, 0.f);
	//pVertices[0].vTexcoord = _float2(0.f, 0.f);	
	//
	//pVertices[1].vPosition = _float3(0.5f, 0.5f, 0.f);
	//pVertices[1].vTexcoord = _float2(1.f, 0.f);
	//
	//pVertices[2].vPosition = _float3(0.5f, -0.5f, 0.f);
	//pVertices[2].vTexcoord = _float2(1.f, 1.f);
	//
	//pVertices[3].vPosition = _float3(-0.5f, -0.5f, 0.f);
	//pVertices[3].vTexcoord = _float2(0.f, 1.f);
	//
	//D3D11_SUBRESOURCE_DATA	VBInitialData{};
	//VBInitialData.pSysMem = pVertices;
	
	//if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &VBInitialData, &m_pVB)))
	//	return E_FAIL;
	
	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, nullptr, &m_pVB)))
		return E_FAIL;

	//Safe_Delete_Array(pVertices);

	D3D11_BUFFER_DESC		IBDesc{};
	IBDesc.ByteWidth = m_iNumVertices * m_iIndexStride;
	IBDesc.Usage = D3D11_USAGE_DEFAULT;
	IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	IBDesc.CPUAccessFlags = 0;
	IBDesc.MiscFlags = 0;
	IBDesc.StructureByteStride = m_iIndexStride;

	_ushort* pIndices = new _ushort[m_iNumIndices];
	
	_uint Idx{};

	for (_uint i = 0; i < (m_iNumVertices - 2); i += 2)
	{
		pIndices[Idx++] = i + 3;
		pIndices[Idx++] = i + 1;
		pIndices[Idx++] = i + 0;

		pIndices[Idx++] = i + 2;
		pIndices[Idx++] = i + 3;
		pIndices[Idx++] = i + 0;
	}
	
	D3D11_SUBRESOURCE_DATA	IBInitialData{};
	IBInitialData.pSysMem = pIndices;
	
	if (FAILED(m_pDevice->CreateBuffer(&IBDesc, &IBInitialData, &m_pIB)))
		return E_FAIL;
	
	Safe_Delete_Array(pIndices);

	return S_OK;
}

HRESULT CVIBuffer_QuadTrail::Initialize_Clone(void* pArg)
{
	return S_OK;
}

void CVIBuffer_QuadTrail::Update(deque<TRAIL_POINT>& vertices)
{
	if (vertices.size() < 2)
		return;

	m_iNumDrawVertices = vertices.size() * 2;
	m_iNumDrawIndices = ((m_iNumDrawVertices / 2) - 1) * 6;

	D3D11_MAPPED_SUBRESOURCE SubResource;
	if (SUCCEEDED(m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource)))
	{
		VTXPOSTEX* pVertices = reinterpret_cast<VTXPOSTEX*>(SubResource.pData);
		_uint dataIdx{};
		for (_uint i = 0; i < m_iNumDrawVertices; i += 2)
		{
			XMStoreFloat3(&pVertices[i].vPosition, XMLoadFloat4(&vertices[dataIdx].vTop));
			pVertices[i].vTexcoord = _float2((_float)i / (_float)(m_iNumDrawVertices - 2), 0);

			XMStoreFloat3(&pVertices[i + 1].vPosition, XMLoadFloat4(&vertices[dataIdx++].vBottom));
			pVertices[i + 1].vTexcoord = _float2((_float)i / (_float)(m_iNumDrawVertices - 2), 1);
		}
		m_pContext->Unmap(m_pVB, 0);
	}
}

HRESULT CVIBuffer_QuadTrail::Render()
{
	m_pContext->DrawIndexed(m_iNumDrawIndices, 0, 0);	//Update에서 매번 갱신

	return S_OK;
}

HRESULT CVIBuffer_QuadTrail::Bind_Resources()
{
	ID3D11Buffer* pVertexBuffers[] = {
			m_pVB,
	};

	_uint		iVertexStrides[] = {
		m_iVertexStride,
	};

	_uint		iOffsets[] = {
		0
	};

	m_pContext->IASetVertexBuffers(0, m_iNumDrawVertices, pVertexBuffers, iVertexStrides, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveType);

	return S_OK;
}

CVIBuffer_QuadTrail* CVIBuffer_QuadTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_QuadTrail* pInstance = new CVIBuffer_QuadTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_QuadTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_QuadTrail::Clone(void* pArg)
{
	CVIBuffer_QuadTrail* pInstance = new CVIBuffer_QuadTrail(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CVIBuffer_QuadTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_QuadTrail::Free()
{
	__super::Free();


}
