#include "Popup_Reset.h"
#include "ClientInstance.h"
#include "GameInstance.h"

#include "UI_TextBox.h"
#include "UI_Default_Tex.h"
#include "UI_BackGround.h"
#include "Store_Slot.h"

CPopup_Reset::CPopup_Reset(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Panel{ pDevice, pContext }
{
}

CPopup_Reset::CPopup_Reset(const CPopup_Reset& Prototype)
    : CUI_Panel(Prototype)
{
}

HRESULT CPopup_Reset::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CPopup_Reset::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);
    CHECK_FAILED(Ready_Componet(), E_FAIL);
    CHECK_FAILED(Ready_Object(), E_FAIL);

    m_fValue = 0.75f;

    return S_OK;
}

void CPopup_Reset::Priority_Update(_float fTimeDelta)
{
}

void CPopup_Reset::Update(_float fTimeDelta)
{
    if (!m_IsUpdate)
        return;

    if (m_pGameInstance->Key_Down(DIK_F, INPUT_TYPE::POPUP))
    {
        m_Event();
        m_IsUpdate = false;
        m_pGameInstance->Change_InputType(INPUT_TYPE::UI);
    }
    else if (m_pGameInstance->Key_Down(DIK_ESCAPE, INPUT_TYPE::POPUP))
    {
        m_IsUpdate = false;
        m_pGameInstance->Change_InputType(INPUT_TYPE::UI);
        m_Event = nullptr;
    }
}

void CPopup_Reset::Late_Update(_float fTimeDelta)
{
    if (!m_IsUpdate)
        return;

    CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
    __super::Late_Update(fTimeDelta);
}

HRESULT CPopup_Reset::Render()
{
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix), E_FAIL);
    CHECK_FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);
    CHECK_FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_iTexPass), E_FAIL);

    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fValue", &m_fValue, sizeof(_float)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4)), E_FAIL);

    m_pShaderCom->Begin(15);
    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CPopup_Reset::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
    __super::Load_UI(pInData, iPrototypeLevelID, pArg);

    for (auto pChild : m_Children)
    {
        string strName = pChild->Get_Name();
        if (strName == "Info_1")
        {
            m_pSkillPoint = static_cast<CUI_TextBox*>(pChild);
            Safe_AddRef(m_pSkillPoint);
        }
        else if (strName == "Line_0" || strName == "Line_1" || strName == "Line_2" || strName == "Line_3" || strName == "Line_4")
        {
            pChild->Set_Color({ 1.f, 1.f, 1.f, 0.7f });
        }
    }

    return S_OK;
}

HRESULT CPopup_Reset::Update_Switch(void* pArg)
{
    POPUP_RESET_DESC* pDesc = static_cast<POPUP_RESET_DESC*>(pArg);

    _int iLevel = CClientInstance::GetInstance()->Get_PlayerData().iSkillLevel;
    _int iCulPoint = CClientInstance::GetInstance()->Get_PlayerData().iSkilPoint;

    _wstring wstrText = TEXT("돌려 받는 스킬 포인트 : ") + to_wstring(iLevel - iCulPoint);

    m_pSkillPoint->Set_Text(wstrText);
    m_Event = pDesc->Event;
    m_IsUpdate = true;
    m_pGameInstance->Change_InputType(INPUT_TYPE::POPUP);
    return S_OK;
}

HRESULT CPopup_Reset::Ready_Componet()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_GuideTex"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CPopup_Reset::Ready_Object()
{
    UIOBJECT_DESC Desc = {};
    Desc.fDepth = 2.2f;
    Desc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
    Desc.szName = "BackGround";
    Desc.vLocalSize = { g_iWinSizeX, g_iWinSizeY };
    Desc.vLocalPos = {0, 0 };

    m_pBackGround = static_cast<CUI_BackGround*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_BackGround"), &Desc));
    if (m_pBackGround == nullptr)
        return E_FAIL;
    m_pBackGround->Setting_BG(CUI_BackGround::UIBGTYPE::END);
    m_pBackGround->Set_Color({ 0.0f, 0.0f, 0.0f, 0.6f });
    m_Children.push_back(m_pBackGround);
    Safe_AddRef(m_pBackGround);

    return S_OK;
}

CPopup_Reset* CPopup_Reset::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CPopup_Reset* pInstance = new CPopup_Reset(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed Created : CPopup_Reset"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CPopup_Reset::Clone(void* pArg)
{
    CPopup_Reset* pInstance = new CPopup_Reset(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CPopup_Reset"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CPopup_Reset::Free()
{
    __super::Free();
    Safe_Release(m_pSkillPoint);
    Safe_Release(m_pBackGround);
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);
}
