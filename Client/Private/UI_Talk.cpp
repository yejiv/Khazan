#include "UI_Talk.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_WorldTextBox.h"
CUI_Talk::CUI_Talk(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Panel{ pDevice, pContext }
{
}

CUI_Talk::CUI_Talk(const CUI_Talk& Prototype)
    : CUI_Panel(Prototype)
{
}

void CUI_Talk::Update_UITransform(_vector vPos)
{
   
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetZ(XMVectorSetX(XMVectorSetY(vPos, XMVectorGetY(vPos) + m_vLocalSize.y * 0.8f), XMVectorGetX(vPos) - 0.5f), XMVectorGetZ(vPos) - 1.4f));
    m_pTransformCom->Scale({1.f, 1.f, 1.f});
    m_pText->Update_UITransform(m_pTransformCom->Get_WorldMatrix());
    m_pTransformCom->Rotation({ 0.f,1.f,0.f,0.f }, XMConvertToRadians(110.f));
    m_vColor = { 1.f,1.f, 1.f, 0.8f };
}

HRESULT CUI_Talk::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Talk::Initialize_Clone(void* pArg)
{
    m_vColor = { 1.f,1.f, 1.f, 0.9f };
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    CHECK_FAILED(Ready_Component(), E_FAIL);
    CHECK_FAILED(Ready_Children(), E_FAIL);
    return S_OK;
}
void CUI_Talk::Priority_Update(_float fTimeDelta)
{
}

void CUI_Talk::Update(_float fTimeDelta)
{

}

void CUI_Talk::Late_Update(_float fTimeDelta)
{
 //   CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
    //m_pTransformCom->LookAt(XMLoadFloat4(m_pGameInstance->Get_CamPosition()));
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
        return;
    m_pText->Late_Update(fTimeDelta);
}

HRESULT CUI_Talk::Render()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4)), E_FAIL);

    m_pShaderCom->Begin(17);
    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();
    return S_OK;
}

HRESULT CUI_Talk::Ready_Component()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_UI_BackGround"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Talk::Ready_Children()
{
    CUIObject::UIOBJECT_DESC Desc = {};
    Desc.iUIType = ENUM_CLASS(UITYPE::PANEL);
    Desc.vLocalPos = { 0.f, 0.f };
    Desc.vLocalSize = { 1.7f, 1.7f };
    Desc.szName = "TalkUI";
    m_pText = static_cast<CUI_WorldTextBox*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_WorldText"), &Desc));

    if (m_pText == nullptr)
        return E_FAIL;

    return S_OK;
}

CUI_Talk* CUI_Talk::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Talk* pInstance = new CUI_Talk(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed Created : CUI_Talk"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_Talk::Clone(void* pArg)
{
    CUI_Talk* pInstance = new CUI_Talk(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CUI_Talk"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CUI_Talk::Free()
{
    __super::Free();
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);

    Safe_Release(m_pText);
}
