#include "UIParent.h"

CUIParent::CUIParent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CUIObject{ pDevice,pContext }
{
}

CUIParent::CUIParent(const CUIParent& Prototype)
    :CUIObject(Prototype)
{

}

HRESULT CUIParent::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUIParent::Initialize_Clone(void* pArg)
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

void CUIParent::Priority_Update(_float fTimeDelta)
{
    for (auto Childe : m_Children)
        Childe->Priority_Update(fTimeDelta);
}

void CUIParent::Update(_float fTimeDelta)
{
    for (auto Childe : m_Children)
        Childe->Update(fTimeDelta);
}

void CUIParent::Late_Update(_float fTimeDelta)
{
    for (auto Childe : m_Children)
        Childe->Late_Update(fTimeDelta);
}

HRESULT CUIParent::Render()
{
    return S_OK;
}

void CUIParent::Add_Child(CUIObject* pChild)
{
    if (nullptr == pChild)
        return;

    m_Children.push_back(pChild);
    Safe_AddRef(pChild);
}

void CUIParent::Remove_Child(CUIObject* pChild)
{
    m_Children.erase(remove(m_Children.begin(), m_Children.end(), pChild), m_Children.end());
}

void CUIParent::Update_Transform(CUIObject* pParent, _float2 vPos)
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

void CUIParent::Update_Scaling(_float fSize)
{
    m_pTransformCom->Scale(_float3{ m_vLocalSize.x * fSize, m_vLocalSize.y * fSize, 1.f });

    for (auto& pChild : m_Children)
        pChild->Update_Scaling(fSize);
}

void CUIParent::Update_Rotation(_float fAngle)
{
    m_pTransformCom->Set_Quaternion(XMQuaternionRotationRollPitchYaw
    (XMConvertToRadians(m_vAngle.x)
        , XMConvertToRadians(m_vAngle.y)
        , XMConvertToRadians(m_vAngle.z + fAngle)
    ));

    for (auto& pChild : m_Children)
        pChild->Update_Rotation(fAngle);
}

void CUIParent::Update_Alpha(_float fAlpha)
{
    m_fAlpha = fAlpha;

    for (auto& pChild : m_Children)
        static_cast<CUIObject*>(pChild)->Update_Alpha(fAlpha);
}

HRESULT CUIParent::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
    return S_OK;
}

void CUIParent::Free()
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
