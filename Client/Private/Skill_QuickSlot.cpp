#include "Skill_QuickSlot.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "Skill_QuickSlot_List.h"
#include "UI_BackGround.h"
#include "UI_Default_Button.h"
#include "UI_TextBox.h"
#include "UI_Guide_Icon.h"
#include "UI_Default_Tex.h"

CSkill_QuickSlot::CSkill_QuickSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Panel{ pDevice, pContext }
{
}

CSkill_QuickSlot::CSkill_QuickSlot(const CSkill_QuickSlot& Prototype)
    : CUI_Panel(Prototype)
{
}

void CSkill_QuickSlot::On_Panel()
{
    if (m_IsUpdate)
        return;

    m_eAnimState = UIANIMSTATE::ON;
    m_fAccTime = 0.5f;
    m_IsUpdate = true;
    
    m_pGameInstance->Change_InputType(INPUT_TYPE::POPUP);
}

void CSkill_QuickSlot::Off_Panel()
{
    if (!m_IsUpdate)
        return;

    m_eAnimState = UIANIMSTATE::OFF;
    m_fAccTime = 1.f;

    m_pGameInstance->Change_InputType(INPUT_TYPE::UI);
}

HRESULT CSkill_QuickSlot::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    CHECK_FAILED(Ready_Prototype(), E_FAIL);
    return S_OK;
}

HRESULT CSkill_QuickSlot::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    return S_OK;
}

void CSkill_QuickSlot::Priority_Update(_float fTimeDelta)
{
    if (!m_IsUpdate)
        return;

    __super::Priority_Update(fTimeDelta);
}

void CSkill_QuickSlot::Update(_float fTimeDelta)
{
    if (!m_IsUpdate)
        return;
    InputKey();
    UI_Animation(fTimeDelta);
    __super::Update(fTimeDelta);
}

void CSkill_QuickSlot::Late_Update(_float fTimeDelta)
{
    if (!m_IsUpdate)
        return;

    __super::Late_Update(fTimeDelta);
}

HRESULT CSkill_QuickSlot::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
    __super::Load_UI(pInData, iPrototypeLevelID, pArg);

    /*for (auto pChild : m_Children)
    {
        string strName = pChild->Get_Name();

        if (strName == "Spear")
        {
            m_SkillTap[TAP_TYPE::SPEAR] = static_cast<CSkill_Tap*>(pChild);
            Safe_AddRef(pChild);
            m_SkillTap[TAP_TYPE::SPEAR]->Setting_Panel(TAP_TYPE::SPEAR);
        }
        else if (strName == "GS")
        {
            m_SkillTap[TAP_TYPE::GS] = static_cast<CSkill_Tap*>(pChild);
            Safe_AddRef(pChild);
            m_SkillTap[TAP_TYPE::GS]->Setting_Panel(TAP_TYPE::GS);
        }
        else if (strName == "Public")
        {
            m_SkillTap[TAP_TYPE::PUBLIC] = static_cast<CSkill_Tap*>(pChild);
            Safe_AddRef(pChild);
            m_SkillTap[TAP_TYPE::PUBLIC]->Setting_Panel(TAP_TYPE::PUBLIC);
        }
    }*/

    CHECK_FAILED(Ready_Object(), E_FAIL);
    return S_OK;
}

void CSkill_QuickSlot::Bubble_EventCall(BUBBLEEVENT* pArg)
{
}

HRESULT CSkill_QuickSlot::Update_Switch(void* pArg)
{
    On_Panel();

    return S_OK;
}

HRESULT CSkill_QuickSlot::Ready_Prototype()
{
    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Skill_QuickSlot_List"),
        CSkill_QuickSlot_List::Create(m_pDevice, m_pContext, m_iLevel)), E_FAIL);

    return S_OK;
}

HRESULT CSkill_QuickSlot::Ready_Object()
{
    UIOBJECT_DESC Desc = {};
    Desc.fDepth = 2.9f;
    Desc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
    Desc.szName = "BackGround";
    Desc.vLocalSize = { g_iWinSizeX, g_iWinSizeY };
    Desc.vLocalPos = { g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

    m_pBackGround = static_cast<CUI_BackGround*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_BackGround"), &Desc));
    if (m_pBackGround == nullptr)
        return E_FAIL;
    m_pBackGround->Setting_BG(CUI_BackGround::UIBGTYPE::DEFAULT);
    m_pBackGround->Set_Color({ 0.0f, 0.0f, 0.0f, 1.f });
    m_Children.push_back(m_pBackGround);
    Safe_AddRef(m_pBackGround);

    return S_OK;

}

void CSkill_QuickSlot::UI_Animation(_float fTimeDelta)
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
        }
    }
}

void CSkill_QuickSlot::InputKey()
{
    if (m_pGameInstance->Key_Down(DIK_ESCAPE, INPUT_TYPE::POPUP))
        Off_Panel();
}

CSkill_QuickSlot* CSkill_QuickSlot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    CSkill_QuickSlot* pInstance = new CSkill_QuickSlot(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        MSG_BOX(TEXT("Failed Created : CSkill_QuickSlot"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CSkill_QuickSlot::Clone(void* pArg)
{
    CSkill_QuickSlot* pInstance = new CSkill_QuickSlot(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CSkill_QuickSlot"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CSkill_QuickSlot::Free()
{
    __super::Free();

    Safe_Release(m_pBackGround);

}
