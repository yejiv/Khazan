
#include "Skill_QuickSLot_List.h"
#include "ClientInstance.h"
#include "GameInstance.h"

#include "UI_TextBox.h"
#include "UI_Atlas_Icon.h"
#include "Skill_QuickSlot.h"
#include "UI_Default_Tex.h"

CSkill_QuickSlot_List::CSkill_QuickSlot_List(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Tap{ pDevice, pContext }
{
}

CSkill_QuickSlot_List::CSkill_QuickSlot_List(const CSkill_QuickSlot_List& Prototype)
    : CUI_Tap(Prototype)
{
}

void CSkill_QuickSlot_List::UnEquipSlot(_int iSkillIndex)
{
    if (m_iEquipSkillIndex == iSkillIndex)
    {
        m_iEquipSkillIndex = 0;

        m_pLineIcon->Set_Texture(CClientInstance::GetInstance()->Get_AtlasUV("T_SkillSlot_Bg_Deco_UI.png", 4), 4);

        m_pSkillIcon->Set_Texture(CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_Skill_NotEquip_UI.png", 4),4);
        m_pSkillIcon->Set_Color({ 1.f, 1.f, 1.f,1.f });
        m_pSkillIcon->Update_Scaling(1.f);
        wstring wstrTemp = TEXT("장착 중인 스킬이 없습니다.");
        m_pSkillName->Set_Text(wstrTemp);
        m_pGameInstance->Emit_Event(ENUM_CLASS(EVENT_TYPE::SKILL_QUICKSLOT), EVENT_SKILL_SLOT{ m_iIndex,m_iEquipSkillIndex , false });
        if (m_iIndex == 0)
            CClientInstance::GetInstance()->UnBindSkillToButton(CONTROL_BUTTON::CTRL_LB);
        else if (m_iIndex == 1)
            CClientInstance::GetInstance()->UnBindSkillToButton(CONTROL_BUTTON::CTRL_RB);
        else if (m_iIndex == 2)
            CClientInstance::GetInstance()->UnBindSkillToButton(CONTROL_BUTTON::CTRL_F);
        else if (m_iIndex == 3)
            CClientInstance::GetInstance()->UnBindSkillToButton(CONTROL_BUTTON::Q);
        else if (m_iIndex == 4)
            CClientInstance::GetInstance()->UnBindSkillToButton(CONTROL_BUTTON::E);
        else if (m_iIndex == 5)
            CClientInstance::GetInstance()->UnBindSkillToButton(CONTROL_BUTTON::R);
    }
}

void CSkill_QuickSlot_List::Set_SkillIndex(_int iSKillIndex)
{
    m_iSkillIndex = iSKillIndex;
}

void CSkill_QuickSlot_List::Update_Pos(_int iIndex, _float2 vPos, _float fOffSetY)
{
    m_iIndex = iIndex;
    m_vLocalPos.x = vPos.x;
    m_vLocalPos.y = vPos.y + iIndex * fOffSetY;

    if (m_iIndex == 1)
    {
        m_pGuideIcon[2]->Set_Texture(CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_Mouse_Rmb.png", 3), 3);
    }
    else if (m_iIndex == 2)
    {
        m_pGuideIcon[2]->Set_Texture(CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_KB_F.png", 3), 3);
    }
    else if (m_iIndex == 3)
    {
        m_pGuideIcon[0]->Set_Texture(CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_KB_Q.png", 3), 3);
        m_pGuideIcon[1]->Update_Visible(false);
        m_pGuideIcon[2]->Update_Visible(false);
    }
    else if (m_iIndex == 4)
    {
        m_pGuideIcon[0]->Set_Texture(CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_KB_E.png", 3), 3);
        m_pGuideIcon[1]->Update_Visible(false);
        m_pGuideIcon[2]->Update_Visible(false);
    }
    else if (m_iIndex == 5)
    {
        m_pGuideIcon[0]->Set_Texture(CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_KB_R.png", 3), 3);
        m_pGuideIcon[1]->Update_Visible(false);
        m_pGuideIcon[2]->Update_Visible(false);
    }
}

HRESULT CSkill_QuickSlot_List::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;

    return S_OK;
}

HRESULT CSkill_QuickSlot_List::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);
    CHECK_FAILED(Ready_Componet(), E_FAIL);

    return S_OK;
}

void CSkill_QuickSlot_List::Priority_Update(_float fTimeDelta)
{
    if (ButtonOver(g_hWnd))
        m_pHover->Update_Visible(true);
    else
        m_pHover->Update_Visible(false);

    if (ButtonClick(g_hWnd, false, true, INPUT_TYPE::POPUP))
    {
        m_iEquipSkillIndex = m_iSkillIndex;
        m_pLineIcon->Set_Texture(CClientInstance::GetInstance()->Get_AtlasUV("T_SkillSlot_Bg_SpecialLine_UI.png", 4), 4);

        const SKILL_DB* pDB = CClientInstance::GetInstance()->Get_Data<SKILL_DB>(m_iEquipSkillIndex);
        m_pSkillIcon->Set_Texture(CClientInstance::GetInstance()->Get_AtlasUV(WStringToAnsi(pDB->wstrIcon), pDB->iTexPass), pDB->iTexPass);
        m_pSkillIcon->Set_Color({ 0.929f, 0.741f, 0.376f,1.f });
        m_pSkillIcon->Update_Scaling(0.5f);
        m_pSkillName->Set_Text(pDB->wstrName);

        CSkill_QuickSlot::BUBBLE_DESC Desc;
        Desc.iIndex = m_iIndex;
        Desc.iSkillIndex = m_iEquipSkillIndex;
        
        m_UIBubbleCallBack(&Desc);

        m_pGameInstance->Emit_Event(ENUM_CLASS(EVENT_TYPE::SKILL_QUICKSLOT), EVENT_SKILL_SLOT{ m_iIndex,m_iEquipSkillIndex , true });
        
        if (m_iIndex == 0)
            CClientInstance::GetInstance()->BindSkillToButton(CONTROL_BUTTON::CTRL_LB, 1 << pDB->iIndex);
        else if (m_iIndex == 1)
            CClientInstance::GetInstance()->BindSkillToButton(CONTROL_BUTTON::CTRL_RB, 1 << pDB->iIndex);
        else if (m_iIndex == 2)
            CClientInstance::GetInstance()->BindSkillToButton(CONTROL_BUTTON::CTRL_F, 1 << pDB->iIndex);
        else if (m_iIndex == 3)
            CClientInstance::GetInstance()->BindSkillToButton(CONTROL_BUTTON::Q, 1 << pDB->iIndex);
        else if (m_iIndex == 4)
            CClientInstance::GetInstance()->BindSkillToButton(CONTROL_BUTTON::E, 1 << pDB->iIndex);
        else if (m_iIndex == 5)
            CClientInstance::GetInstance()->BindSkillToButton(CONTROL_BUTTON::R, 1 << pDB->iIndex);
    }
}

void CSkill_QuickSlot_List::Update(_float fTimeDelta)
{

}

void CSkill_QuickSlot_List::Late_Update(_float fTimeDelta)
{

    CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
    __super::Late_Update(fTimeDelta);
}

HRESULT CSkill_QuickSlot_List::Render()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_iTexPass)))
        return E_FAIL;
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);

    m_pShaderCom->Begin(m_iShaderPass);
    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CSkill_QuickSlot_List::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
    __super::Load_UI(pInData, iPrototypeLevelID, pArg);
    /*m_iShaderPass = 6;
    m_iState = ENUM_CLASS(UISTATE::ENABLE);*/

    for (auto pChild : m_Children)
    {
        string strName = pChild->Get_Name();
        if (strName == "Skill_Slot_Line")
        {
            m_pLineIcon = static_cast<CUI_Atlas_Icon*>(pChild);
            Safe_AddRef(m_pLineIcon);
        }
        else if (strName == "Skill_Slot_Plus")
        {
            m_pSkillIcon = static_cast<CUI_Atlas_Icon*>(pChild);
            Safe_AddRef(m_pSkillIcon);
        }
        else if (strName == "Skill_Slot_Text")
        {
            m_pSkillName = static_cast<CUI_TextBox*>(pChild);
            Safe_AddRef(m_pSkillName);
        }
        else if (strName == "Skill_Key_1" || strName == "Skill_Key_2" || strName == "Skill_Key_3")
        {
            m_pGuideIcon.push_back(static_cast<CUI_Atlas_Icon*>(pChild));
            Safe_AddRef(pChild);
        }
        else if (strName == "Hover")
        {
            m_pHover = static_cast<CUI_Default_Tex*>(pChild);
            Safe_AddRef(m_pHover);
        }
    }

    m_pSkillIcon->Set_ShaderPass(2);

    m_iState = ENUM_CLASS(UISTATE::ENABLE);
    return S_OK;
}

HRESULT CSkill_QuickSlot_List::Ready_Componet()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_Common_MenuList"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

CSkill_QuickSlot_List* CSkill_QuickSlot_List::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    CSkill_QuickSlot_List* pInstance = new CSkill_QuickSlot_List(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        MSG_BOX(TEXT("Failed Created : CSkill_QuickSlot_List"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CSkill_QuickSlot_List::Clone(void* pArg)
{
    CSkill_QuickSlot_List* pInstance = new CSkill_QuickSlot_List(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CSkill_QuickSlot_List"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CSkill_QuickSlot_List::Free()
{
    __super::Free();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);

    Safe_Release(m_pLineIcon);
    Safe_Release(m_pSkillIcon);
    Safe_Release(m_pSkillName);
    Safe_Release(m_pHover);

    for (auto pIcon : m_pGuideIcon)
        Safe_Release(pIcon);
    m_pGuideIcon.clear();

}
