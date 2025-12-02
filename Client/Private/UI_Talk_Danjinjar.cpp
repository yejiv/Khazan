#include "UI_Talk_Danjinjar.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_WorldTextBox.h"
#include "UI_WorldTex.h"
#include "UI_WorldList.h"


CUI_Talk_Danjinjar::CUI_Talk_Danjinjar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Panel{ pDevice, pContext }
{
}

CUI_Talk_Danjinjar::CUI_Talk_Danjinjar(const CUI_Talk_Danjinjar& Prototype)
    : CUI_Panel(Prototype)
{
}

void CUI_Talk_Danjinjar::On_Panel()
{
    m_IsUpdate = true;
    m_eType = TALK_TYPE::START;
    m_pGameInstance->Change_InputType(INPUT_TYPE::UI);
    m_eAnimState = UIANIMSTATE::ON;
    m_fAccTime = 0.f;
}

void CUI_Talk_Danjinjar::Off_Panel()
{
    m_eAnimState = UIANIMSTATE::OFF;
}

void CUI_Talk_Danjinjar::Update_UITransform(_vector vPos)
{
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetZ(XMVectorSetX(XMVectorSetY(vPos, XMVectorGetY(vPos)), XMVectorGetX(vPos)), XMVectorGetZ(vPos)));

    _float offsetY = sin(m_fSpeedWeight * 2.f) * 1.f * 0.5f
        + sin(m_fSpeedWeight * 2.f * 0.5f) * 1.f * 0.5f;
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetY(m_pTransformCom->Get_State(STATE::POSITION), XMVectorGetY(m_pTransformCom->Get_State(STATE::POSITION)) + offsetY * 0.02f));

    m_pTransformCom->Scale({ 2.625f, 1.f, 1.f });
    m_pTransformCom->Scale({ 0.1f, 0.1f, 0.1f });

    m_vColor = { 1.f,1.f, 1.f, 1.f };
    //m_pTransformCom->LookAt_Revers(XMLoadFloat4(m_pGameInstance->Get_CamPosition()));

    m_pText1->Update_UITransform(m_pTransformCom->Get_WorldMatrix());
    m_pText1->Set_LocalPos({ -0.4f, 0.3f, -0.001f, 1.f });
    m_pText1->Set_LocalSize({ 1.75f, 1.75f, 1.f });
    m_pText1->Set_Color({ 1.f,1.f,1.f,1.f });
    m_pText1->Set_Text(TEXT(""));

    m_pText2->Update_UITransform(m_pTransformCom->Get_WorldMatrix());
    m_pText2->Set_LocalPos({ -0.4f, 0.15f, -0.001f, 1.f });
    m_pText2->Set_LocalSize({ 1.75f, 1.75f, 1.f });
    m_pText2->Set_Color({ 1.f,1.f,1.f,1.f });
    m_pText2->Set_Text(TEXT(""));
}

HRESULT CUI_Talk_Danjinjar::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Talk_Danjinjar::Initialize_Clone(void* pArg)
{
    m_vColor = { 1.f,1.f, 1.f, 0.9f };
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    CHECK_FAILED(Ready_Component(), E_FAIL);
    CHECK_FAILED(Ready_Children(), E_FAIL);

    return S_OK;
}
void CUI_Talk_Danjinjar::Priority_Update(_float fTimeDelta)
{
    if (!m_IsUpdate)
        return;

}

void CUI_Talk_Danjinjar::Update(_float fTimeDelta)
{
    m_IsUpdate = true;
    if (!m_IsUpdate)
        return;

    UI_Animation(fTimeDelta);
}

void CUI_Talk_Danjinjar::Late_Update(_float fTimeDelta)
{
    if (!m_IsUpdate)
        return;

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
        return;

    m_pText1->Late_Update(fTimeDelta);
    m_pText2->Late_Update(fTimeDelta);

}

HRESULT CUI_Talk_Danjinjar::Render()
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

HRESULT CUI_Talk_Danjinjar::Ready_Component()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_World_Talk"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Talk_Danjinjar::Ready_Children()
{
    CUIObject::UIOBJECT_DESC Desc = {};
    Desc.iUIType = ENUM_CLASS(UITYPE::PANEL);
    Desc.vLocalPos = { 0.f, 0.f };
    Desc.vLocalSize = { 1.f, 1.f };
    Desc.szName = "TalkUI";
    m_pText1 = static_cast<CUI_WorldTextBox*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_WorldText"), &Desc));
    CHECK_NULLPTR(m_pText1, E_FAIL);
    Add_Child(m_pText1);

    m_pText2 = static_cast<CUI_WorldTextBox*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_WorldText"), &Desc));
    CHECK_NULLPTR(m_pText2, E_FAIL);
    Add_Child(m_pText2);

    m_IsUpdate = false;

    m_pText1->Set_TextTag(TEXT("Blade_Medium_20"));
    m_pText2->Set_TextTag(TEXT("Blade_Medium_20"));

    return S_OK;
}

void CUI_Talk_Danjinjar::UI_Animation(_float fTimeDelta)
{
    if (m_eAnimState == UIANIMSTATE::ON)
    {
        m_fAccTime += fTimeDelta * 3.f;
        __super::Update_Alpha(m_fAccTime);

        if (m_fAccTime >= 1.f)
        {
            m_fAccTime = 1.f;
            m_eAnimState = UIANIMSTATE::END;
        }
    }
    else if (m_eAnimState == UIANIMSTATE::OFF)
    {
        m_fAccTime -= fTimeDelta * 3.f;
        __super::Update_Alpha(m_fAccTime);

        if (m_fAccTime <= 0.f)
        {
            m_fAccTime = 0.f;
            m_eAnimState = UIANIMSTATE::END;
            m_IsUpdate = false;
            m_pGameInstance->Change_InputType(INPUT_TYPE::GAMEPLAY);
        }
    }
}

CUI_Talk_Danjinjar* CUI_Talk_Danjinjar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Talk_Danjinjar* pInstance = new CUI_Talk_Danjinjar(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed Created : CUI_Talk_Danjinjar"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_Talk_Danjinjar::Clone(void* pArg)
{
    CUI_Talk_Danjinjar* pInstance = new CUI_Talk_Danjinjar(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CUI_Talk_Danjinjar"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CUI_Talk_Danjinjar::Free()
{
    __super::Free();
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);

    Safe_Release(m_pText1);
    Safe_Release(m_pText2);

}
