#include "UIObject.h"
#include "GameInstance.h"

CUIObject::CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject{ pDevice,pContext }
{
}

CUIObject::CUIObject(const CUIObject& Prototype)
    :CGameObject{ Prototype }
{

}

CTexture* CUIObject::Set_Texture(CGameObject* pGameObject, CTexture* pTexture)
{   
    if (nullptr == pGameObject)
        return nullptr;

    if (nullptr == pTexture)
        return nullptr;

    pTexture = static_cast<CTexture*>(pGameObject->Get_Component(TEXT("Com_Texture")));
    if (nullptr == pTexture)
        return nullptr;

    Safe_AddRef(pTexture);

    return pTexture;

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

    // Transform Initialize
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    m_eSpaceType = pDesc->eSpaceTeype;
    strcpy_s(m_szName, pDesc->szName);

    if (UISPACETYPE::SCREEN == m_eSpaceType)
        Initialize_Screen_UI(pDesc);
    else if (UISPACETYPE::WORLD == m_eSpaceType)
        Initialize_World_UI(pDesc);
    else
        return E_FAIL;

    return S_OK;
}

void CUIObject::Priority_Update(_float fTimeDelta)
{
}

void CUIObject::Update(_float fTimeDelta)
{
}

void CUIObject::Late_Update(_float fTimeDelta)
{
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

    if (nullptr == pChild->m_pParent)
        pChild->m_pParent = this;


    Update_Transform();
}

void CUIObject::Remove_Child(CUIObject* pChild)
{
    m_Children.erase(remove(m_Children.begin(), m_Children.end(), pChild), m_Children.end());
    if (this == pChild->m_pParent)
        pChild->m_pParent = nullptr;
}

void CUIObject::Update_Visible(_bool bisVisible)
{
    m_isVisible = bisVisible;

    for (auto& pChild : m_Children)
    {
        pChild->Update_Visible(bisVisible);
    }
}

void CUIObject::Update_Transform(CTransform* pTargetTransform)
{
    if (m_pParent)
    {
        m_vWorldPos.x = m_pParent->m_vWorldPos.x + m_vLocalPos.x;
        m_vWorldPos.y = m_pParent->m_vWorldPos.y + m_vLocalPos.y;
        m_vWorldPos.z = m_pParent->m_vWorldPos.z + m_vLocalPos.z;
    }
    else
        m_vWorldPos = m_vLocalPos;

    m_pTransformCom->Scale(_float3(m_vLocalSize.x, m_vLocalSize.y, 1.f));

    if (UISPACETYPE::SCREEN == m_eSpaceType)
    {
        m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(
            m_vWorldPos.x - m_iWinSizeX * 0.5f,
            -m_vWorldPos.y + m_iWinSizeY * 0.5f, m_vWorldPos.z, 1.f));

        for (auto& pChild : m_Children)
        {
            pChild->Update_Transform();
        }

    }
    else if (UISPACETYPE::WORLD == m_eSpaceType)
    {
        if (m_pTransformCom)
        {
            m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetW(XMLoadFloat3(&m_vWorldPos), 1.f));
        }

        for (auto& pChild : m_Children)
        {
            pChild->Update_Transform();
        }
    }
}

_float2 CUIObject::Compute_AlignedPos(_float2 vPos, _float2 vSize)
{
    switch (m_eAlignment)
    {
    case UI_Alignment::TOP_LEFT:
        return vPos;
    case UI_Alignment::TOP_CENTER:
        return _float2(vPos.x - vSize.x * 0.5f, vPos.y);
    case UI_Alignment::TOP_RIGHT:
        return _float2(vPos.x - vSize.x, vPos.y);
    case UI_Alignment::MIDDLE_LEFT:
        return _float2(vPos.x, vPos.y - vSize.y * 0.5f);
    case UI_Alignment::MIDDLE_CENTER:
        return _float2(vPos.x - vSize.x * 0.5f, vPos.y - vSize.y * 0.5f);
    case UI_Alignment::MIDDLE_RIGHT:
        return _float2(vPos.x - vSize.x, vPos.y - vSize.y * 0.5f);
    case UI_Alignment::BOTTOM_LEFT:
        return _float2(vPos.x, vPos.y - vSize.y);
    case UI_Alignment::BOTTOM_CENTER:
        return _float2(vPos.x - vSize.x * 0.5f, vPos.y - vSize.y);
    case UI_Alignment::BOTTOM_RIGHT:
        return _float2(vPos.x - vSize.x, vPos.y - vSize.y);
    }
}


_bool CUIObject::Update_Picking(HWND hWnd)
{
    _bool bChildClicked = false;

    for (auto& pChild : m_Children)
    {
        if (pChild && pChild->Update_Picking(hWnd))
        {
            bChildClicked = true;
        }
    }

    POINT ptMouse{};
    GetCursorPos(&ptMouse);
    ScreenToClient(hWnd, &ptMouse);

    RECT rc{
        LONG(m_vWorldPos.x - m_vLocalSize.x * 0.5f),
        LONG(m_vWorldPos.y - m_vLocalSize.y * 0.5f),
        LONG(m_vWorldPos.x + m_vLocalSize.x * 0.5f),
        LONG(m_vWorldPos.y + m_vLocalSize.y * 0.5f)
    };

    if (PtInRect(&rc, ptMouse))
    {
        m_isHovered = true;
        return true;
    }

    m_isHovered = false;
    return false;

}

void CUIObject::Broadcast_Click()
{
    // 함수를 호출하는 객체가 Broadcast를 실행하고
    OnClick.Broadcast();
    // 호출한 객체가 부모 컴포짓이 있다면
    if (m_pParent)
    {
        // 부모의 Click을 호출하면서
        m_pParent->Broadcast_Click();
    }
    // 자식에서 부모 컴포짓으로 이벤트를 전달 시킨다.
}

void CUIObject::Broadcast_Hover(_float fMousePosX, _float fMousePosY)
{
    OnHover.Broadcast(fMousePosX,fMousePosY);
    if (m_pParent)
        m_pParent->Broadcast_Hover(fMousePosX,fMousePosY);
}

HRESULT CUIObject::Initialize_Screen_UI(UIOBJECT_DESC* pDesc)
{

    m_vLocalPos = pDesc->vLocalPos;
    m_vLocalSize = pDesc->vLocalSize;

    D3D11_VIEWPORT			Viewport{};

    _uint			iNumViewports = { 1 };

    m_pContext->RSGetViewports(&iNumViewports, &Viewport);

    XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
    XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(Viewport.Width, Viewport.Height, 0.0f, 1.f));

    m_iWinSizeX = static_cast<const _uint>(Viewport.Width);
    m_iWinSizeY = static_cast<const _uint>(Viewport.Height);

    m_pTransformCom->Scale(m_vLocalSize);
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(
        m_vLocalPos.x - m_iWinSizeX * 0.5f,
        -m_vLocalPos.y + m_iWinSizeY * 0.5f, m_vLocalPos.z, 1.f));

    return S_OK;
}

HRESULT CUIObject::Initialize_World_UI(UIOBJECT_DESC* pDesc)
{

    m_vLocalPos = pDesc->vLocalPos;
    m_vLocalSize = pDesc->vLocalSize;

    // 추후에 추가 내용이 있으면 추가

    return S_OK;
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
    m_pParent = nullptr;
}
