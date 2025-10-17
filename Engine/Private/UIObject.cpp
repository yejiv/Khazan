#include "UIObject.h"
#include "GameInstance.h"

CUIObject::CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject{ pDevice,pContext }
{
}

CUIObject::CUIObject(const CUIObject& Prototype)
    :CGameObject( Prototype )
{

}

HRESULT CUIObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUIObject::Initialize_Clone(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    UIOBJECT_DESC* pDesc = static_cast<UIOBJECT_DESC*>(pArg);

    m_iUIType = pDesc->iUIType;

    if (m_iUIType < 0)
        return E_FAIL;

    m_vLocalPos = pDesc->vLocalPos;
    m_vLocalSize = pDesc->vLocalSize;
    m_szName = pDesc->szName;
    m_UIBubbleCallBack = pDesc->BubbleEvent;
    m_fDepth = pDesc->fDepth;

    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    D3D11_VIEWPORT			Viewport{};
    _uint			iNumViewports = { 1 };

    m_pContext->RSGetViewports(&iNumViewports, &Viewport);

    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.f, 1.f));

    m_iWinSizeX = static_cast<const _uint>(Viewport.Width);
    m_iWinSizeY = static_cast<const _uint>(Viewport.Height);

    m_pTransformCom->Scale(_float3{ m_vLocalSize.x, m_vLocalSize.y, 1.f });
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(
        m_vLocalPos.x - m_iWinSizeX * 0.5f,
        -m_vLocalPos.y + m_iWinSizeY * 0.5f, 0.f, 1.f));

    return S_OK;
}

void CUIObject::Priority_Update(_float fTimeDelta)
{
    for (auto Childe : m_Children)
        Childe->Priority_Update(fTimeDelta);
}

void CUIObject::Update(_float fTimeDelta)
{
    for (auto Childe : m_Children)
        Childe->Update(fTimeDelta);
}

void CUIObject::Late_Update(_float fTimeDelta)
{
    for (auto Childe : m_Children)
        Childe->Late_Update(fTimeDelta);
}

HRESULT CUIObject::Render()
{
    return S_OK;
}

void CUIObject::Add_Child(CUIObject* pChild)
{
    if (nullptr == pChild)
        return;

    m_Children.push_back(pChild);

}

void CUIObject::Remove_Child(CUIObject* pChild)
{
    m_Children.erase(remove(m_Children.begin(), m_Children.end(), pChild), m_Children.end());
}

void CUIObject::Update_Visible(_bool bisVisible)
{
    m_isVisible = bisVisible;

    for (auto& pChild : m_Children)
    {
        pChild->Update_Visible(bisVisible);
    }
}

void CUIObject::Update_Transform(CUIObject* pParent, _float2 vPos)
{
    if (pParent == nullptr)
    {
        m_vWorldPos.x = vPos.x;
        m_vWorldPos.y = vPos.y;
    }
    else
    {
        m_vWorldPos.x = pParent->Get_WolrdPos().x + m_vLocalPos.x;
        m_vWorldPos.y = pParent->Get_WolrdPos().y + m_vLocalPos.y;
    }
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_vWorldPos.x - m_iWinSizeX * 0.5f, -m_vWorldPos.y + m_iWinSizeY * 0.5f, 0.f, 1.f));
    for (auto& pChild : m_Children)
    {
        pChild->Update_Transform(this, m_vWorldPos);
    }
}

void CUIObject::Update_Scaling(_float fSize)
{
    m_pTransformCom->Scale(_float3{ m_vLocalSize.x * fSize, m_vLocalSize.y * fSize, 1.f });

    for (auto& pChild : m_Children)
        pChild->Update_Scaling(fSize);
}

void CUIObject::Update_Rotation(_float fAngle)
{
    m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(fAngle));

    for (auto& pChild : m_Children)
        pChild->Update_Rotation(fAngle);
}

_float2 CUIObject::Compute_AlignedPos(_float2 vPos, _float2 vSize)
{
    switch (m_eAlignment)
    {
    case UI_ALIGNMENT::TOP_LEFT:
        return vPos;
    case UI_ALIGNMENT::TOP_CENTER:
        return _float2(vPos.x + vSize.x * 0.5f, vPos.y);
    case UI_ALIGNMENT::TOP_RIGHT:
        return _float2(vPos.x + vSize.x, vPos.y);
    case UI_ALIGNMENT::MIDDLE_LEFT:
        return _float2(vPos.x, vPos.y + vSize.y * 0.5f);
    case UI_ALIGNMENT::MIDDLE_CENTER:
        return _float2(vPos.x + vSize.x * 0.5f, vPos.y + vSize.y * 0.5f);
    case UI_ALIGNMENT::MIDDLE_RIGHT:
        return _float2(vPos.x + vSize.x, vPos.y + vSize.y * 0.5f);
    case UI_ALIGNMENT::BOTTOM_LEFT:
        return _float2(vPos.x, vPos.y + vSize.y);
    case UI_ALIGNMENT::BOTTOM_CENTER:
        return _float2(vPos.x + vSize.x * 0.5f, vPos.y + vSize.y);
    case UI_ALIGNMENT::BOTTOM_RIGHT:
        return _float2(vPos.x + vSize.x, vPos.y + vSize.y);
    }

    return _float2(0.f, 0.f);
}

_bool CUIObject::IsPick(HWND hWnd)
{
    _float fX = m_vWorldPos.x;
    _float fY = m_vWorldPos.y;

    RECT	rcRect = { LONG(fX - (m_vWorldSize.x * 0.5f)), LONG(fY - (m_vWorldSize.y * 0.5f)), LONG(fX + (m_vWorldSize.x * 0.5f)), LONG(fY + (m_vWorldSize.y * 0.5f)) };

    POINT ptMouse{};
    GetCursorPos(&ptMouse);
    ScreenToClient(hWnd, &ptMouse);

    return PtInRect(&rcRect, ptMouse);
}

void CUIObject::Free()
{
    __super::Free();

    for (auto& pChild : m_Children)
    {
        if (pChild != nullptr)
        {
            Safe_Release(pChild);
        }
    }
    m_Children.clear();
}
