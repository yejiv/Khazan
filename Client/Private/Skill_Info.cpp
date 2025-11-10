#include "Skill_Info.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_Atlas_Icon.h"
#include "UI_TextBox.h"
#include "UI_Default_Tex.h"

#include "Skill_Info_Panel.h"
#include "Skill_Info_Tex.h"
#include "Skill_Condition_Panel.h"

CSkill_Info::CSkill_Info(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Panel{ pDevice, pContext }
{
}

CSkill_Info::CSkill_Info(const CSkill_Info& Prototype)
    : CUI_Panel(Prototype)
{
}

HRESULT CSkill_Info::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;

    if (FAILED(Ready_Prototype()))
        return E_FAIL;

    return S_OK;
}

HRESULT CSkill_Info::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    CHECK_FAILED(Ready_Componet(), E_FAIL);
    return S_OK;
}

void CSkill_Info::Priority_Update(_float fTimeDelta)
{
    m_IsUpdate = false;
    __super::Priority_Update(fTimeDelta);
}

void CSkill_Info::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CSkill_Info::Late_Update(_float fTimeDelta)
{
    if (!m_IsUpdate)
        return;

    CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
    __super::Late_Update(fTimeDelta);

}

HRESULT CSkill_Info::Render()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_iTexPass)))
        return E_FAIL;

    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4)), E_FAIL);

    m_pShaderCom->Begin(m_iShaderPass);
    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();

    return S_OK;
}

HRESULT CSkill_Info::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
    __super::Load_UI(pInData, iPrototypeLevelID, pArg);

    for (auto Child : m_Children)
    {
        string strName = Child->Get_Name();

        if (strName == "Skill_Name")
        {
            m_pSKillName = static_cast<CUI_TextBox*>(Child);
        }
        else if (strName == "Skill_Stamina_Text")
        {
            m_pSkillGaugeText = static_cast<CUI_TextBox*>(Child);
            Safe_AddRef(m_pSkillGaugeText);
        }
        else if (strName == "Point_Icon")
        {
            m_pPointIcon = static_cast<CUI_Atlas_Icon*>(Child);
            Safe_AddRef(m_pPointIcon);
        }
        else if (strName == "Point_Count")
        {
            m_pPointText = static_cast<CUI_TextBox*>(Child);
            Safe_AddRef(m_pPointText);
        }
        else if (strName == "Get_Icon")
        {
            m_pGetIcon = static_cast<CUI_Atlas_Icon*>(Child);
            Safe_AddRef(m_pGetIcon);
        }
        else if (strName == "Equip_Text")
        {
            m_pEquip_Text = static_cast<CUI_TextBox*>(Child);
            Safe_AddRef(m_pEquip_Text);
        }
        else if (strName == "Equip_Deco")
        {
            m_pEquip_Deco = static_cast<CUI_Default_Tex*>(Child);
            Safe_AddRef(m_pEquip_Deco);
        }
        else if (strName == "Skill_Info")
        {
            m_pInfoPanel = static_cast<CSkill_Info_Panel*>(Child);
            Safe_AddRef(m_pInfoPanel);
        }
        else if (strName == "Skill_Condition")
        {
            m_pConditionPanel = static_cast<CSkill_Condition_Panel*>(Child);
            Safe_AddRef(m_pConditionPanel);
        }
        else if (strName == "Skill_Info_Tex")
        {
            m_pInfo_Tex = static_cast<CSkill_Info_Tex*>(Child);
            Safe_AddRef(m_pInfo_Tex);
        }
    }


    return S_OK;
}

HRESULT CSkill_Info::Update_Switch(void* pArg)
{
    SKILLINFO_DESC* pDesc = static_cast<SKILLINFO_DESC*>(pArg);

    m_IsUpdate = true;

    m_vLocalPos = pDesc->iOffsetPos;
    Update_Transform(nullptr, m_vLocalPos);

    pDesc->isEquip ? m_pEquip_Deco->Update_Visible(true) : m_pEquip_Deco->Update_Visible(false);
    pDesc->isEquip ? m_pEquip_Text->Update_Visible(true) : m_pEquip_Text->Update_Visible(false);

    if (pDesc->isMaxLevel)
    {
        m_pPointIcon->Update_Visible(false);
        m_pPointText->Update_Visible(false);
    }
    else
    {
        m_pPointIcon->Update_Visible(true);
        m_pPointText->Update_Visible(true);
    }

    if (pDesc->isGet)
        m_pGetIcon->Update_Visible(true);
    else
        m_pGetIcon->Update_Visible(false);

    if (m_iSkillIndex == pDesc->iSkillIndex)
        return S_OK;

    m_iSkillIndex = pDesc->iSkillIndex;
    const SKILL_DB* pSkillData = CClientInstance::GetInstance()->Get_Data<SKILL_DB>(m_iSkillIndex);
    m_pSKillName->Set_Text(pSkillData->wstrName);
    if (pSkillData->iGauge > 0)
    {
        _wstring wstrGauge = TEXT("투지 ") + to_wstring(pSkillData->iGauge) + TEXT(" 소모");
        m_pSkillGaugeText->Update_Visible(true);
        m_pSkillGaugeText->Set_Text(wstrGauge);
    }
    else
        m_pSkillGaugeText->Update_Visible(false);


    m_pInfoPanel->Setting_Info(pSkillData->wstrInfo, {0.f, 0.f});

    if(pSkillData->iPreSkill == 0)
       m_pConditionPanel->Update_Visible(false);
    else
    {
        m_pConditionPanel->Update_Visible(true);
        m_pConditionPanel->Setting_Condition(pDesc->isOnPreSkill, CClientInstance::GetInstance()->Get_Data<SKILL_DB>(pSkillData->iPreSkill)->wstrName);
    }

    m_pInfo_Tex->Setting_Tex(m_iSkillIndex);

    
    return S_OK;
}

HRESULT CSkill_Info::Ready_Prototype()
{
    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Skill_Info_Tex"),
        CSkill_Info_Tex::Create(m_pDevice, m_pContext)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Skill_Info_Panel"),
        CSkill_Info_Panel::Create(m_pDevice, m_pContext)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Skill_Condition_Panel"),
        CSkill_Condition_Panel::Create(m_pDevice, m_pContext)), E_FAIL);
    return S_OK;
}

HRESULT CSkill_Info::Ready_Componet()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_UI_ItemInfo_BG"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

CSkill_Info* CSkill_Info::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    CSkill_Info* pInstance = new CSkill_Info(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        MSG_BOX(TEXT("Failed Created : CSkill_Info"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CSkill_Info::Clone(void* pArg)
{
    CSkill_Info* pInstance = new CSkill_Info(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CSkill_Info"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CSkill_Info::Free()
{
    __super::Free();

   // for (auto Icon : m_pEffectIcon)
   //    Safe_Release(Icon);
   //m_pEffectIcon.clear();
   //
   //for (auto Text : m_pEffectText)
   //    Safe_Release(Text);
   //m_pEffectText.clear();
   //
   //for (auto Text : m_pEffectValue)
   //    Safe_Release(Text);
   //m_pEffectValue.clear();
   
   Safe_Release(m_pSKillName);
   Safe_Release(m_pSkillGaugeText);

   Safe_Release(m_pPointIcon);
   Safe_Release(m_pPointText);
   Safe_Release(m_pGetIcon);

   Safe_Release(m_pInfoPanel);
   Safe_Release(m_pConditionPanel);
   Safe_Release(m_pInfo_Tex);

   Safe_Release(m_pEquip_Deco);
   Safe_Release(m_pEquip_Text);

   Safe_Release(m_pShaderCom);
   Safe_Release(m_pTextureCom);
   Safe_Release(m_pVIBufferCom);
}
