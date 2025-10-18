#include "UIObject.h"
#include "GameInstance.h"

CUIObject::CUIObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CGameObject{ pDevice,pContext }
{
}

CUIObject::CUIObject(const CUIObject& Prototype)
    :CGameObject(Prototype)
{

}

void CUIObject::Get_Data(VTXINSTANCE_UI& pOutData)
{
    XMStoreFloat4(&pOutData.vRight, m_pTransformCom->Get_WorldMatrix().r[0]);
    XMStoreFloat4(&pOutData.vUp, m_pTransformCom->Get_WorldMatrix().r[1]);
    XMStoreFloat4(&pOutData.vLook, m_pTransformCom->Get_WorldMatrix().r[2]);
    XMStoreFloat4(&pOutData.vPosition, m_pTransformCom->Get_WorldMatrix().r[3]);

    pOutData.iTexPass = m_iTexPass;
    pOutData.iShaderPass = m_iShaderPass;
    pOutData.fAlpha = m_fAlpha;
    pOutData.vUV = m_vUV[m_iState];
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

void CUIObject::Add_Renderer()
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
        return;
}

void CUIObject::Update_Visible(_bool bisVisible)
{
    m_isVisible = bisVisible;
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
}

void CUIObject::Update_Scaling(_float fSize)
{
    m_pTransformCom->Scale(_float3{ m_vLocalSize.x * fSize, m_vLocalSize.y * fSize, 1.f });
}

void CUIObject::Update_Rotation(_float fAngle)
{
    m_pTransformCom->Rotation(XMVectorSet(0.f, 0.f, 1.f, 0.f), XMConvertToRadians(fAngle));
}

HRESULT CUIObject::Update_Switch(void* pArg)
{
    m_IsUpdate ? m_IsUpdate = false : m_IsUpdate = true;
    return S_OK;
}

void CUIObject::Bubble_EventCall()
{
    if (m_UIBubbleCallBack == nullptr)
        return;

    m_UIBubbleCallBack();
}

HRESULT CUIObject::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
    return S_OK;
}

void CUIObject::Insert_Bubble(std::function<void()> BubbleEvent)
{
    m_UIBubbleCallBack = BubbleEvent;
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
}
