#include "VIBuffer_LineTrail.h"

CVIBuffer_LineTrail::CVIBuffer_LineTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer { pDevice, pContext }
{
}

CVIBuffer_LineTrail::CVIBuffer_LineTrail(const CVIBuffer_LineTrail& Prototype)
	: CVIBuffer{ Prototype }
{
}

HRESULT CVIBuffer_LineTrail::Initialize_Prototype()
{
	m_iNumVertices = 2048;
	m_iNumIndices = ((m_iNumVertices / 2) - 1) * 6;
	m_iVertexStride = sizeof(VTXPOSTEX);
	m_iIndexStride = 2;
	m_iNumVertexBuffers = 1;
	m_eIndexFormat = DXGI_FORMAT_R16_UINT;
	m_ePrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

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

HRESULT CVIBuffer_LineTrail::Initialize_Clone(void* pArg)
{
	D3D11_BUFFER_DESC		VBDesc{};

	VBDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
	VBDesc.Usage = D3D11_USAGE_DYNAMIC;
	VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	VBDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	VBDesc.MiscFlags = 0;
	VBDesc.StructureByteStride = m_iVertexStride;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, nullptr, &m_pVB)))
		return E_FAIL;

	LINE_BUFFER_DESC* dsc = static_cast<LINE_BUFFER_DESC*>(pArg);
	m_fOffset = dsc->fOffset;

	return S_OK;
}

void CVIBuffer_LineTrail::Update(deque<_float4>& vertices, const _float4* CamPos)
{
	if (vertices.size() < 2)
		return;

	_int NumDrawVertices = vertices.size() * 2;
	m_iNumDrawIndices = ((NumDrawVertices / 2) - 1) * 6;

	D3D11_MAPPED_SUBRESOURCE SubResource;
	if (SUCCEEDED(m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource)))
	{
		VTXPOSTEX* pVertices = reinterpret_cast<VTXPOSTEX*>(SubResource.pData);
		_uint dataIdx{};
		for (_uint i = 0; i < NumDrawVertices - 2; i += 2)
		{
			//위아래 정점 빌보딩해서 직접 만들기
			_vector vDir = XMLoadFloat4(&vertices[dataIdx]) - XMLoadFloat4(&vertices[dataIdx + 1]);
			_vector vLookDir = XMVector4Normalize(XMLoadFloat4(CamPos) - XMLoadFloat4(&vertices[dataIdx]));
			_vector vRight = XMVector4Normalize(XMVector3Cross(vDir, vLookDir)) * m_fOffset * 0.5f;

			XMStoreFloat3(&pVertices[i].vPosition, XMLoadFloat4(&vertices[dataIdx]) + vRight);
			pVertices[i].vTexcoord = _float2((_float)i / (_float)(NumDrawVertices - 2), 0);

			XMStoreFloat3(&pVertices[i + 1].vPosition, XMLoadFloat4(&vertices[dataIdx++]) - vRight);
			pVertices[i + 1].vTexcoord = _float2((_float)i / (_float)(NumDrawVertices - 2), 1);
		}

		//마지막 쌍 예외처리
		_vector vDir = XMLoadFloat4(&vertices[dataIdx - 1]) - XMLoadFloat4(&vertices[dataIdx]);
		_vector vLookDir = XMVector4Normalize(XMLoadFloat4(CamPos) - XMLoadFloat4(&vertices[dataIdx]));
		_vector vRight = XMVector4Normalize(XMVector3Cross(vDir, vLookDir)) * m_fOffset * 0.5f;

		XMStoreFloat3(&pVertices[NumDrawVertices - 2].vPosition, XMLoadFloat4(&vertices[dataIdx]) + vRight);
		pVertices[NumDrawVertices - 2].vTexcoord = _float2((_float)(NumDrawVertices - 2) / (_float)(NumDrawVertices - 2), 0);

		XMStoreFloat3(&pVertices[NumDrawVertices - 1].vPosition, XMLoadFloat4(&vertices[dataIdx++]) - vRight);
		pVertices[NumDrawVertices - 1].vTexcoord = _float2((_float)(NumDrawVertices - 2) / (_float)(NumDrawVertices - 2), 1);

		m_pContext->Unmap(m_pVB, 0);
	}
}

void CVIBuffer_LineTrail::Update(deque<_float4>& vertices)
{
	if (vertices.size() < 2)
		return;

	_int NumDrawVertices = vertices.size() * 2;
	m_iNumDrawIndices = ((NumDrawVertices / 2) - 1) * 6;

	D3D11_MAPPED_SUBRESOURCE SubResource;
	if (SUCCEEDED(m_pContext->Map(m_pVB, 0, D3D11_MAP_WRITE_DISCARD, 0, &SubResource)))
	{
		VTXPOSTEX* pVertices = reinterpret_cast<VTXPOSTEX*>(SubResource.pData);
		_uint dataIdx{};
		for (_uint i = 0; i < NumDrawVertices - 2; i += 2)
		{
			//위아래 정점 빌보딩해서 직접 만들기
			_vector vDir = XMLoadFloat4(&vertices[dataIdx]) - XMLoadFloat4(&vertices[dataIdx + 1]);
			_vector vRight = XMVector4Normalize(XMVectorSet(-XMVectorGetY(vDir), XMVectorGetX(vDir), 0.f, 0.f)) * m_fOffset * 0.5f;

			XMStoreFloat3(&pVertices[i].vPosition, XMLoadFloat4(&vertices[dataIdx]) + vRight);
			pVertices[i].vTexcoord = _float2((_float)i / (_float)(NumDrawVertices - 2), 0);

			XMStoreFloat3(&pVertices[i + 1].vPosition, XMLoadFloat4(&vertices[dataIdx++]) - vRight);
			pVertices[i + 1].vTexcoord = _float2((_float)i / (_float)(NumDrawVertices - 2), 1);
		}

		//마지막 쌍 예외처리
		_vector vDir = XMLoadFloat4(&vertices[dataIdx - 1]) - XMLoadFloat4(&vertices[dataIdx]);
		_vector vRight = XMVector4Normalize(XMVectorSet(-XMVectorGetY(vDir), XMVectorGetX(vDir), 0.f, 0.f)) * m_fOffset * 0.5f;

		XMStoreFloat3(&pVertices[NumDrawVertices - 2].vPosition, XMLoadFloat4(&vertices[dataIdx]) + vRight);
		pVertices[NumDrawVertices - 2].vTexcoord = _float2((_float)(NumDrawVertices - 2) / (_float)(NumDrawVertices - 2), 0);

		XMStoreFloat3(&pVertices[NumDrawVertices - 1].vPosition, XMLoadFloat4(&vertices[dataIdx++]) - vRight);
		pVertices[NumDrawVertices - 1].vTexcoord = _float2((_float)(NumDrawVertices - 2) / (_float)(NumDrawVertices - 2), 1);

		m_pContext->Unmap(m_pVB, 0);
	}
}

HRESULT CVIBuffer_LineTrail::Render()
{
	m_pContext->DrawIndexed(m_iNumDrawIndices, 0, 0);	//Update에서 매번 갱신

	return S_OK;
}

CVIBuffer_LineTrail* CVIBuffer_LineTrail::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_LineTrail* pInstance = new CVIBuffer_LineTrail(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_LineTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_LineTrail::Clone(void* pArg)
{
	CVIBuffer_LineTrail* pInstance = new CVIBuffer_LineTrail(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CVIBuffer_LineTrail"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_LineTrail::Free()
{
	__super::Free();


}
