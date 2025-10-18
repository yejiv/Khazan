#include "VIBuffer_Instance_UI.h"

CVIBuffer_Instance_UI::CVIBuffer_Instance_UI(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
	: CVIBuffer_Instance{ pDevice, pDeviceContext }
{
}

CVIBuffer_Instance_UI::CVIBuffer_Instance_UI(const CVIBuffer_Instance_UI& Prototype)
	: CVIBuffer_Instance(Prototype)
{
	m_pInstanceVertices = new VTXINSTANCE_UI[Prototype.m_iNumInstance];
	memcpy(m_pInstanceVertices, Prototype.m_pInstanceVertices, sizeof(VTXINSTANCE_UI) * m_iNumInstance);
}

HRESULT CVIBuffer_Instance_UI::Initialize_Prototype(const INSTANCE_DESC* pDesc)
{
	m_iInstanceVertexStride = sizeof(VTXINSTANCE_UI);
	m_iNumInstance = pDesc->iNumInstance;
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
	m_VBInstanceDesc.ByteWidth = m_iNumInstance * m_iInstanceVertexStride;
	m_VBInstanceDesc.Usage = D3D11_USAGE_DYNAMIC;
	m_VBInstanceDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBInstanceDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	m_VBInstanceDesc.MiscFlags = 0;
	m_VBInstanceDesc.StructureByteStride = m_iInstanceVertexStride;

	m_pInstanceVertices = new VTXINSTANCE_UI[m_iNumInstance];

	for (size_t i = 0; i < m_iNumInstance; i++)
	{
		VTXINSTANCE_UI* pInstanceVertices = m_pInstanceVertices;

		pInstanceVertices[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		pInstanceVertices[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		pInstanceVertices[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);
		pInstanceVertices[i].vPosition = _float4(0.f, 0.f, 0.f, 1.f);
		pInstanceVertices[i].vUV = _float4(0.f, 0.f, 1.f, 1.f);
		pInstanceVertices[i].fAlpha = 1.f;
		pInstanceVertices[i].iShaderPass = 0;
		pInstanceVertices[i].iTexPass = 0;
	}

	return S_OK;
}

HRESULT CVIBuffer_Instance_UI::Initialize_Clone(void* pArg)
{
	D3D11_SUBRESOURCE_DATA InitialDesc = {};
	InitialDesc.pSysMem = m_pInstanceVertices;

	if (FAILED(m_pDevice->CreateBuffer(&m_VBInstanceDesc, &InitialDesc, &m_pVBInstance)))
		return E_FAIL;

	return S_OK;
}

HRESULT CVIBuffer_Instance_UI::Bind_Resources()
{
	ID3D11Buffer* pVertexBuffers[] = { m_pVB, m_pVBInstance, };
	_uint		iVertexStrides[] = { m_iVertexStride, m_iInstanceVertexStride, };
	_uint		iOffsets[] = { 0, 0 };

	m_pContext->IASetVertexBuffers(0, m_iNumVertexBuffers, pVertexBuffers, iVertexStrides, iOffsets);
	m_pContext->IASetPrimitiveTopology(m_ePrimitiveType);

	return S_OK;
}

HRESULT CVIBuffer_Instance_UI::Render()
{
	if (m_iRenderCount > 0)
	{
		m_pContext->DrawInstanced(1, m_iRenderCount, 0, 0);
		Reset();
	}
	return S_OK;
}

HRESULT CVIBuffer_Instance_UI::Add_UIInstance(const VTXINSTANCE_UI* pUIData)
{
	memcpy(&m_pInstanceVertices[m_iRenderCount], pUIData, sizeof(VTXINSTANCE_UI));
	++m_iRenderCount;
	return S_OK;
}

HRESULT CVIBuffer_Instance_UI::Update_Buffer()
{
	D3D11_MAPPED_SUBRESOURCE	SubResource{};

	m_pContext->Map(m_pVBInstance, 0, D3D11_MAP_WRITE_NO_OVERWRITE, 0, &SubResource);
	VTXINSTANCE_UI* pVertices = static_cast<VTXINSTANCE_UI*>(SubResource.pData);

	memcpy(pVertices, m_pInstanceVertices, sizeof(VTXINSTANCE_UI) * m_iNumInstance);

	m_pContext->Unmap(m_pVBInstance, 0);
	return S_OK;
}

HRESULT CVIBuffer_Instance_UI::Reset()
{
	for (size_t i = 0; i < m_iRenderCount; i++)
	{
		VTXINSTANCE_UI* pInstanceVertices = m_pInstanceVertices;

		pInstanceVertices[i].vRight = _float4(1.f, 0.f, 0.f, 0.f);
		pInstanceVertices[i].vUp = _float4(0.f, 1.f, 0.f, 0.f);
		pInstanceVertices[i].vLook = _float4(0.f, 0.f, 1.f, 0.f);
		pInstanceVertices[i].vPosition = _float4(0.f, 0.f, 0.f, 1.f);
		pInstanceVertices[i].vUV = _float4(0.f, 0.f, 1.f, 1.f);
		pInstanceVertices[i].fAlpha = 1.f;
		pInstanceVertices[i].iShaderPass = 0;
		pInstanceVertices[i].iTexPass = 0;
	}
	// Melong
	m_iRenderCount = 0;
	return S_OK;
}

CVIBuffer_Instance_UI* CVIBuffer_Instance_UI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext, const INSTANCE_DESC* pDesc)
{
	CVIBuffer_Instance_UI* pInstance = new CVIBuffer_Instance_UI(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Prototype(pDesc)))
	{
		MSG_BOX(TEXT("Failed Created : CVIBuffer_Instance_UI"));
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent* CVIBuffer_Instance_UI::Clone(void* pArg)
{
	CVIBuffer_Instance_UI* pInstance = new CVIBuffer_Instance_UI(*this);

	if (FAILED(pInstance->Initialize_Clone(pArg)))
	{
		MSG_BOX(TEXT("Failed Cloned : CVIBuffer_Instance_UI"));
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Instance_UI::Free()
{
	__super::Free();
	Safe_Delete_Array(m_pInstanceVertices);

}





