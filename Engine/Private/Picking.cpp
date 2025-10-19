#include "Picking.h"
#include "GameInstance.h"

CPicking::CPicking(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : m_pDevice{ pDevice }
    , m_pContext{ pContext }
    , m_pGameInstance{ CGameInstance::GetInstance() }
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pDevice);
    Safe_AddRef(m_pContext);
}

HRESULT CPicking::Initialize(HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
    D3D11_TEXTURE2D_DESC	TextureDesc;
    ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

    /* 깊이 버퍼의 픽셀은 백버퍼의 픽셀과 갯수가 동일해야만 깊이 텍스트가 가능해진다. */
    /* 픽셀의 수가 다르면 아에 렌더링을 못함. */
    TextureDesc.Width = iWinSizeX;
    TextureDesc.Height = iWinSizeY;
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = 1;
    TextureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;

    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.SampleDesc.Count = 1;

    TextureDesc.Usage = D3D11_USAGE_STAGING/* 정적 */;
    /* 추후에 어떤 용도로 바인딩 될 수 있는 View타입의 텍스쳐를 만들기위한 Texture2D입니까? */
    TextureDesc.BindFlags = 0;
    TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
    TextureDesc.MiscFlags = 0;

    if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &m_pTexture2D)))
        return E_FAIL;

    m_hWnd = hWnd;
    m_pPixels = new _float4[iWinSizeX * iWinSizeY];
    m_iWinSizeX = iWinSizeX;
    m_iWinSizeY = iWinSizeY;

    return S_OK;
}

void CPicking::Update()
{
    /* 매 프레임 월드 정보를 저장한 타겟을 복사받아오자. */
    // m_pContext->CopyResource();

}

_bool CPicking::isPicked(_float3* pOut)
{
    if (FAILED(m_pGameInstance->Copy_RT_Resource(TEXT("Target_World"), m_pTexture2D)))
        return false;

    D3D11_MAPPED_SUBRESOURCE        SubResource{};
    if (FAILED(m_pContext->Map(m_pTexture2D, 0, D3D11_MAP_READ, 0, &SubResource)))
        return false;

    memcpy(m_pPixels, SubResource.pData, sizeof(_float4) * m_iWinSizeX * m_iWinSizeY);

    m_pContext->Unmap(m_pTexture2D, 0);

    GetCursorPos(&m_ptMouse);
    ScreenToClient(m_hWnd, &m_ptMouse);

    if (0 > m_ptMouse.x || static_cast<_long>(m_iWinSizeX) < m_ptMouse.x || 0 > m_ptMouse.y || static_cast<_long>(m_iWinSizeY) < m_ptMouse.y)
        return false;

    _uint       iIndex = m_ptMouse.y * m_iWinSizeX + m_ptMouse.x;

    if (0.0f == m_pPixels[iIndex].x && 0.0f == m_pPixels[iIndex].y && 0.0f == m_pPixels[iIndex].z && 0.0f == m_pPixels[iIndex].w)
        return false;

    _vector     vPosition = { m_pPixels[iIndex].x, m_pPixels[iIndex].y, m_pPixels[iIndex].z, 1.f };

    XMStoreFloat3(pOut, vPosition);

    return true;
}

_bool CPicking::isPicked(_float3* pOut, _uint* iObjectID)
{
    if (FAILED(m_pGameInstance->Copy_RT_Resource(TEXT("Target_World"), m_pTexture2D)))
        return false;

    D3D11_MAPPED_SUBRESOURCE        SubResource{};
    if (FAILED(m_pContext->Map(m_pTexture2D, 0, D3D11_MAP_READ, 0, &SubResource)))
        return false;

    memcpy(m_pPixels, SubResource.pData, sizeof(_float4) * m_iWinSizeX * m_iWinSizeY);

    m_pContext->Unmap(m_pTexture2D, 0);

    GetCursorPos(&m_ptMouse);
    ScreenToClient(m_hWnd, &m_ptMouse);

    // 화면밖으로 나가있을때 피킹 예외처리
    if (0 > m_ptMouse.x || static_cast<_long>(m_iWinSizeX) < m_ptMouse.x || 0 > m_ptMouse.y || static_cast<_long>(m_iWinSizeY) < m_ptMouse.y)
        return false;

    _uint       iIndex = m_ptMouse.y * m_iWinSizeX + m_ptMouse.x;

    if (0.0f == m_pPixels[iIndex].x && 0.0f == m_pPixels[iIndex].y && 0.0f == m_pPixels[iIndex].z && 0.0f == m_pPixels[iIndex].w)
        return false;

    _vector     vPosition = { m_pPixels[iIndex].x, m_pPixels[iIndex].y, m_pPixels[iIndex].z, m_pPixels[iIndex].w };

    *iObjectID = static_cast<_uint>(m_pPixels[iIndex].w);
    XMStoreFloat3(pOut, vPosition);

    return true;
}

CPicking* CPicking::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, HWND hWnd, _uint iWinSizeX, _uint iWinSizeY)
{
    CPicking* pInstance = new CPicking(pDevice, pContext);

    if (FAILED(pInstance->Initialize(hWnd, iWinSizeX, iWinSizeY)))
    {
        MSG_BOX(TEXT("Failed to Created : CPicking"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CPicking::Free()
{
    __super::Free();

    Safe_Delete_Array(m_pPixels);

    Safe_Release(m_pTexture2D);

    Safe_Release(m_pGameInstance);
    Safe_Release(m_pDevice);
    Safe_Release(m_pContext);
}
