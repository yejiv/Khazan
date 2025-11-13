#include "Inven_State_Panel.h"
#include "ClientInstance.h"
#include "GameInstance.h"
#include "UI_TextBox.h"

CInven_State_Panel::CInven_State_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CUI_Panel{pDevice, pContext}
{
}

CInven_State_Panel::CInven_State_Panel(const CInven_State_Panel& Prototype)
    :CUI_Panel(Prototype)
{
}

HRESULT CInven_State_Panel::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CInven_State_Panel::Initialize_Clone(void* pArg)
{
    __super::Initialize_Clone(pArg);
   
    m_pLevel = &CClientInstance::GetInstance()->Get_PlayerData().iLevel;

    m_pVitality = &CClientInstance::GetInstance()->Get_PlayerData().iVitality;
    m_pEndurance = &CClientInstance::GetInstance()->Get_PlayerData().iEndurance;
    m_pPower = &CClientInstance::GetInstance()->Get_PlayerData().iPower;
    m_pCompetency = &CClientInstance::GetInstance()->Get_PlayerData().iCompetency;
    m_pWill = &CClientInstance::GetInstance()->Get_PlayerData().iWill;

    m_pMaxHp = &CClientInstance::GetInstance()->Get_PlayerData().fMaxHp;
    m_pMaxStamina = &CClientInstance::GetInstance()->Get_PlayerData().fMaxStamina;
    m_pAtk = &CClientInstance::GetInstance()->Get_PlayerData().fDamage;
    m_pDef = &CClientInstance::GetInstance()->Get_PlayerData().fGuard;

    return S_OK;
}

void CInven_State_Panel::Priority_Update(_float fTimeDelta)
{
}

void CInven_State_Panel::Update(_float fTimeDelta)
{
    Text_Update();
}

void CInven_State_Panel::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CInven_State_Panel::Render()
{
    return S_OK;
}

HRESULT CInven_State_Panel::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
    CHECK_FAILED(__super::Load_UI(pInData, iPrototypeLevelID, pArg), E_FAIL);

    m_pUIText.resize(TEXT_TYPE::END);
    for (auto pChild : m_Children)
    {
        string strName = pChild->Get_Name();

        if ("State_Level_Text" == strName)
        {
            m_pUIText[TEXT_TYPE::LEVEL] = static_cast<CUI_TextBox*>(pChild);
            Safe_AddRef(pChild);
        }
        else if ("State_1_Text" == strName)
        {
            m_pUIText[TEXT_TYPE::VITALITY] = static_cast<CUI_TextBox*>(pChild);
            Safe_AddRef(pChild);
        }
        else if ("State_2_Text" == strName)
        {
            m_pUIText[TEXT_TYPE::ENDURANCE] = static_cast<CUI_TextBox*>(pChild);
            Safe_AddRef(pChild);
        }
        else if ("State_3_Text" == strName)
        {
            m_pUIText[TEXT_TYPE::STRENGTH] = static_cast<CUI_TextBox*>(pChild);
            Safe_AddRef(pChild);
        }
        else if ("State_4_Text" == strName)
        {
            m_pUIText[TEXT_TYPE::WILL] = static_cast<CUI_TextBox*>(pChild);
            Safe_AddRef(pChild);
        }
        else if ("State_5_Text" == strName)
        {
            m_pUIText[TEXT_TYPE::COMPETECNCY] = static_cast<CUI_TextBox*>(pChild);
            Safe_AddRef(pChild);
        }
        else if ("State_MaxHp_Text" == strName)
        {
            m_pUIText[TEXT_TYPE::MAXHP] = static_cast<CUI_TextBox*>(pChild);
            Safe_AddRef(pChild);
        }
        else if ("State_MaxStamina_Text" == strName)
        {
            m_pUIText[TEXT_TYPE::MAXSTAMINA] = static_cast<CUI_TextBox*>(pChild);
            Safe_AddRef(pChild);
        }
        else if ("State_Atk_Text" == strName)
        {
            m_pUIText[TEXT_TYPE::ATK] = static_cast<CUI_TextBox*>(pChild);
            Safe_AddRef(pChild);
        }
        else if ("State_Def_Text" == strName)
        {
            m_pUIText[TEXT_TYPE::DEF] = static_cast<CUI_TextBox*>(pChild);
            Safe_AddRef(pChild);
        }

    }
    return S_OK;
}

void CInven_State_Panel::Text_Update()
{
    m_pUIText[TEXT_TYPE::LEVEL]->Set_Text(to_wstring(*m_pLevel));
    m_pUIText[TEXT_TYPE::VITALITY]->Set_Text(to_wstring(*m_pVitality));
    m_pUIText[TEXT_TYPE::ENDURANCE]->Set_Text(to_wstring(*m_pEndurance));
    m_pUIText[TEXT_TYPE::STRENGTH]->Set_Text(to_wstring(*m_pPower));
    m_pUIText[TEXT_TYPE::WILL]->Set_Text(to_wstring(*m_pWill));
    m_pUIText[TEXT_TYPE::COMPETECNCY]->Set_Text(to_wstring(*m_pCompetency));
    m_pUIText[TEXT_TYPE::MAXHP]->Set_Text(to_wstring((_int)*m_pMaxHp));
    m_pUIText[TEXT_TYPE::MAXSTAMINA]->Set_Text(to_wstring((_int)*m_pMaxStamina));
    m_pUIText[TEXT_TYPE::ATK]->Set_Text(to_wstring((_int)*m_pAtk));
    m_pUIText[TEXT_TYPE::DEF]->Set_Text(to_wstring((_int)*m_pDef));

}

CInven_State_Panel* CInven_State_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CInven_State_Panel* pInstance = new CInven_State_Panel(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed Created : CInven_State_Panel"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CInven_State_Panel::Clone(void* pArg)
{
    CInven_State_Panel* pInstance = new CInven_State_Panel(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CInven_State_Panel"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CInven_State_Panel::Free()
{
    __super::Free();

    for (auto pText : m_pUIText)
        Safe_Release(pText);
    m_pUIText.clear();
}
