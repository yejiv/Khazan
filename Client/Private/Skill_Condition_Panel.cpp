#include "Skill_Condition_Panel.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_Atlas_Icon.h"
#include "UI_TextBox.h"
#include "UI_Default_Tex.h"

CSkill_Condition_Panel::CSkill_Condition_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Panel{ pDevice, pContext }
{
}

CSkill_Condition_Panel::CSkill_Condition_Panel(const CSkill_Condition_Panel& Prototype)
    : CUI_Panel(Prototype)
{
}

void CSkill_Condition_Panel::Setting_Condition(_bool isOnPreSkill, _wstring wstrPreSkillName)
{
    if (!isOnPreSkill)
    {
        m_pGetText->Set_Color({ 0.8941f, 0.223f, 0.223f, 1.f});
        m_pIcon->Set_Color({ 0.8941f, 0.223f, 0.223f, 1.f});
        m_pSKilName->Set_Color({ 0.8941f, 0.223f, 0.223f, 1.f});
    }
    else
    {
        m_pGetText->Set_Color({ 1.f,1.f,1.f, 1.f });
        m_pIcon->Set_Color({ 0.278f, 0.827f, 0.251f, 1.f });
        m_pSKilName->Set_Color({ 0.278f, 0.827f, 0.251f, 1.f });
    }
    _wstring wstrName = TEXT("[") + wstrPreSkillName + TEXT("]");
    m_pSKilName->Set_Text(wstrName);
}

HRESULT CSkill_Condition_Panel::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CSkill_Condition_Panel::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    return S_OK;
}

void CSkill_Condition_Panel::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CSkill_Condition_Panel::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CSkill_Condition_Panel::Late_Update(_float fTimeDelta)
{
    if(m_isVisible)
        __super::Late_Update(fTimeDelta);
}

HRESULT CSkill_Condition_Panel::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
    __super::Load_UI(pInData, iPrototypeLevelID, pArg);

    for (auto Child : m_Children)
    {
        string strName = Child->Get_Name();
        if (strName == "Skill_Condition_Skill_Name")
        {
            m_pSKilName = static_cast<CUI_TextBox*>(Child);
            Safe_AddRef(m_pSKilName);
        }
        else if (strName == "Skill_Condition_Skill_Get")
        {
            m_pGetText = static_cast<CUI_TextBox*>(Child);
            Safe_AddRef(m_pGetText);
        }
        else if (strName == "Skill_Condition_Skill_Get_Icon")
        {
            m_pIcon = static_cast<CUI_Atlas_Icon*>(Child);
            Safe_AddRef(m_pIcon);
            m_pIcon->Set_ShaderPass(2);
        }
    }


    return S_OK;
}

CSkill_Condition_Panel* CSkill_Condition_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSkill_Condition_Panel* pInstance = new CSkill_Condition_Panel(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed Created : CSkill_Condition_Panel"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CSkill_Condition_Panel::Clone(void* pArg)
{
    CSkill_Condition_Panel* pInstance = new CSkill_Condition_Panel(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CSkill_Condition_Panel"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CSkill_Condition_Panel::Free()
{
    __super::Free();

    Safe_Release(m_pGetText);
    Safe_Release(m_pSKilName);
    Safe_Release(m_pIcon);
}
