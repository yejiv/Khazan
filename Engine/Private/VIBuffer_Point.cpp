#include "VIBuffer_Point.h"

CVIBuffer_Point::CVIBuffer_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CVIBuffer { pDevice, pContext }
{
}

CVIBuffer_Point::CVIBuffer_Point(const CVIBuffer_Point& Prototype)
	: CVIBuffer{ Prototype }
{
}

HRESULT CVIBuffer_Point::Initialize_Prototype()
{
	m_iNumVertices = 1;
	m_iVertexStride = sizeof(VTXPOS);
    //m_iNumIndices = 6;
    m_iIndexStride = 2;
    m_iNumVertexBuffers = 1;
    m_eIndexFormat = DXGI_FORMAT_R16_UINT;
    m_ePrimitiveType = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;

    D3D11_BUFFER_DESC		VBDesc{};
    VBDesc.ByteWidth = m_iNumVertices * m_iVertexStride;
    VBDesc.Usage = D3D11_USAGE_DEFAULT;
    VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    VBDesc.CPUAccessFlags = 0;
    VBDesc.MiscFlags = 0;
    VBDesc.StructureByteStride = m_iVertexStride;

    VTXPOS* pVertices = new VTXPOS[m_iNumVertices];

	pVertices[0].vPosition = _float3(0.f, 0.f, 0.f);

	D3D11_SUBRESOURCE_DATA	VBInitialData{};
	VBInitialData.pSysMem = pVertices;

	if (FAILED(m_pDevice->CreateBuffer(&VBDesc, &VBInitialData, &m_pVB)))
		return E_FAIL;

	Safe_Delete_Array(pVertices);

	return S_OK;
}

HRESULT CVIBuffer_Point::Initialize_Clone(void* pArg)
{
	return S_OK;
}

HRESULT CVIBuffer_Point::Bind_Resources()
{

    ID3D11Buffer* pVertexBuffers[] = {
    m_pVB,
    };

    _uint		iVertexStrides[] = {
        m_iVertexStride,
    };

    _uint		iOffsets[] = {
        0,
    };

    m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
    m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

    return S_OK;
}

HRESULT CVIBuffer_Point::Render()
{
    m_pContext->Draw(1, 0);    
    return S_OK;
}

CVIBuffer_Point* CVIBuffer_Point::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CVIBuffer_Point* pInstance = new CVIBuffer_Point(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX(TEXT("Failed to Created : CVIBuffer_Point"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CVIBuffer_Point::Clone(void* pArg)
{
	CVIBuffer_Point* pInstance = new CVIBuffer_Point(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed to Cloned : CVIBuffer_Point"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Point::Free()
{
	__super::Free();


}
