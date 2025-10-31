#include "UI_State_MainPanel.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_State.h"
#include "UI_State_Panel.h"

#include "UI_TextBox.h"
CUI_State_MainPanel::CUI_State_MainPanel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Panel{ pDevice, pContext }
{
}

CUI_State_MainPanel::CUI_State_MainPanel(const CUI_State_MainPanel& Prototype)
    : CUI_Panel(Prototype)
{
}

HRESULT CUI_State_MainPanel::Setting_PanelLevel(_int iPanelType, UI_PLAYER_STATE_DATA* Data)
{
    m_iPanelType = iPanelType;
    m_pData = Data;

    if (m_iPanelType == ENUM_CLASS(CUI_State::STATE_PANEL::LEVEL))
    {
        for (auto child : m_Children)
        {
            if (child->Get_Name() == "Level_Num")
            {
                m_pValue_Text.emplace("Level_Num", static_cast<CUI_TextBox*>(child));
                Safe_AddRef(child);
                continue;
            }
            else if (child->Get_Name() == "Up_Count")
            {
                m_pValue_Text.emplace("Up_Count", static_cast<CUI_TextBox*>(child));
                Safe_AddRef(child);
                continue;
            }
        }
    }
    else if (m_iPanelType == ENUM_CLASS(CUI_State::STATE_PANEL::LACHRYMA))
    {
        for (auto child : m_Children)
        {
            if (child->Get_Name() == "Lachryma_PennyValue")
            {
                m_pValue_Text.emplace("Lachryma_PennyValue", static_cast<CUI_TextBox*>(child));
                Safe_AddRef(child);
                continue;
            }
            else if (child->Get_Name() == "Lachryma_UpValue")
            {
                m_pValue_Text.emplace("Lachryma_UpValue", static_cast<CUI_TextBox*>(child));
                Safe_AddRef(child);
                continue;
            }
        }
    }
    else if (m_iPanelType == ENUM_CLASS(CUI_State::STATE_PANEL::DEFAULT_STATE))
    {
        for (auto child : m_Children)
        {
            _bool isSetting = true;
            if (child->Get_Name() == "DefaultState_List_Hp")
            {
                m_pState_Panel.emplace("DefaultState_List_Hp", static_cast<CUI_State_Panel*>(child));
                Safe_AddRef(child);
            }
            else if (child->Get_Name() == "DefaultState_List_Stamin")
            {
                m_pState_Panel.emplace("DefaultState_List_Stamin", static_cast<CUI_State_Panel*>(child));
                Safe_AddRef(child);
            }
            else if (child->Get_Name() == "DefaultState_List_Damage")
            {
                m_pState_Panel.emplace("DefaultState_List_Damage", static_cast<CUI_State_Panel*>(child));
                Safe_AddRef(child);
            }
            else if (child->Get_Name() == "DefaultState_List_Def")
            {
                m_pState_Panel.emplace("DefaultState_List_Def", static_cast<CUI_State_Panel*>(child));
                Safe_AddRef(child);
            }
            else if (child->Get_Name() == "DefaultState_List_Weight")
            {
                m_pState_Panel.emplace("DefaultState_List_Weight", static_cast<CUI_State_Panel*>(child));
                Safe_AddRef(child);
            }
            else if (child->Get_Name() == "DefaultState_List_Agile")
            {
                m_pState_Panel.emplace("DefaultState_List_Agile", static_cast<CUI_State_Panel*>(child));
                Safe_AddRef(child);
            }
            else
                isSetting = false;
            
            if(isSetting)
                static_cast<CUI_State_Panel*>(child)->Setting_PanelLevel(m_iPanelType, Data);
        }
    }
    else if (m_iPanelType == ENUM_CLASS(CUI_State::STATE_PANEL::ADD_STATE))
    {
        for (auto child : m_Children)
        {
            _bool isSetting = true;
            if (child->Get_Name() == "AddState_List_Atk")
            {
                m_pState_Panel.emplace("AddState_List_Atk", static_cast<CUI_State_Panel*>(child));
                Safe_AddRef(child);
            }
            else if (child->Get_Name() == "AddState_List_Regen")
            {
                m_pState_Panel.emplace("AddState_List_Regen", static_cast<CUI_State_Panel*>(child));
                Safe_AddRef(child);
            }
            else if (child->Get_Name() == "AddState_List_Min")
            {
                m_pState_Panel.emplace("AddState_List_Min", static_cast<CUI_State_Panel*>(child));
                Safe_AddRef(child);
            }
            else if (child->Get_Name() == "AddState_List_MinDamage")
            {
                m_pState_Panel.emplace("AddState_List_MinDamage", static_cast<CUI_State_Panel*>(child));
                Safe_AddRef(child);
            }
            else if (child->Get_Name() == "AddState_List_GuardDamage")
            {
                m_pState_Panel.emplace("AddState_List_GuardDamage", static_cast<CUI_State_Panel*>(child));
                Safe_AddRef(child);
            }
            else
                isSetting = false;

            if (isSetting)
                static_cast<CUI_State_Panel*>(child)->Setting_PanelLevel(m_iPanelType, Data);
        }
    }
    else if (m_iPanelType == ENUM_CLASS(CUI_State::STATE_PANEL::ELEMENTAL))
    {
        for (auto child : m_Children)
        {

            _bool isSetting = true;
            if (child->Get_Name() == "Elemental_List_Fire")
            {
                m_pState_Panel.emplace("Elemental_List_Fire", static_cast<CUI_State_Panel*>(child));
                Safe_AddRef(child);
            }
            else if (child->Get_Name() == "Elemental_List_Water")
            {
                m_pState_Panel.emplace("Elemental_List_Water", static_cast<CUI_State_Panel*>(child));
                Safe_AddRef(child);
            }
            else if (child->Get_Name() == "Elemental_List_Lightning")
            {
                m_pState_Panel.emplace("Elemental_List_Lightning", static_cast<CUI_State_Panel*>(child));
                Safe_AddRef(child);
            }
            else if (child->Get_Name() == "Elemental_List_Earth")
            {
                m_pState_Panel.emplace("Elemental_List_Earth", static_cast<CUI_State_Panel*>(child));
                Safe_AddRef(child);
            }
            else if (child->Get_Name() == "Elemental_List_Chaos")
            {
                m_pState_Panel.emplace("Elemental_List_Chaos", static_cast<CUI_State_Panel*>(child));
                Safe_AddRef(child);
            }
            else if (child->Get_Name() == "Elemental_List_Disease")
            {
                m_pState_Panel.emplace("Elemental_List_Disease", static_cast<CUI_State_Panel*>(child));
                Safe_AddRef(child);
            }
            else if (child->Get_Name() == "Elemental_List_Poison")
            {
                m_pState_Panel.emplace("Elemental_List_Poison", static_cast<CUI_State_Panel*>(child));
                Safe_AddRef(child);
            }
            else
                isSetting = false;

            if (isSetting)
                static_cast<CUI_State_Panel*>(child)->Setting_PanelLevel(m_iPanelType, Data);
         
        }
    }
    return S_OK;
}

HRESULT CUI_State_MainPanel::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;

}

HRESULT CUI_State_MainPanel::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);
    return S_OK;
}

void CUI_State_MainPanel::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CUI_State_MainPanel::Update(_float fTimeDelta)
{
    if (m_iPanelType == ENUM_CLASS(CUI_State::STATE_PANEL::LEVEL))
    {
        m_pValue_Text.at("Level_Num")->Set_Text(to_wstring(m_pData->iLevel));
        m_pValue_Text.at("Up_Count")->Set_Text(to_wstring(m_pData->iUPPoint));
    }
    else if (m_iPanelType == ENUM_CLASS(CUI_State::STATE_PANEL::LACHRYMA))
    {
        m_pValue_Text.at("Lachryma_PennyValue")->Set_Text(to_wstring(m_pData->iLachryma));
        m_pValue_Text.at("Lachryma_UpValue")->Set_Text(to_wstring(m_pData->iUpLachryma));
    }

    __super::Update(fTimeDelta);
}

void CUI_State_MainPanel::Late_Update(_float fTimeDelta)
{
    __super::Late_Update(fTimeDelta);
}

HRESULT CUI_State_MainPanel::Render()
{
    return S_OK;
}

HRESULT CUI_State_MainPanel::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
    if (FAILED(__super::Load_UI(pInData, iPrototypeLevelID, pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_State_MainPanel::Mapping_Child()
{
    return E_NOTIMPL;
}

CUI_State_MainPanel* CUI_State_MainPanel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    CUI_State_MainPanel* pInstance = new CUI_State_MainPanel(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        MSG_BOX(TEXT("Failed Created : CUI_State_MainPanel"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_State_MainPanel::Clone(void* pArg)
{
    CUI_State_MainPanel* pInstance = new CUI_State_MainPanel(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CUI_State_MainPanel"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CUI_State_MainPanel::Free()
{
    __super::Free();

    for (auto TextBox : m_pValue_Text)
        Safe_Release(TextBox.second);

    for (auto State : m_pState_Panel)
        Safe_Release(State.second);

    m_pValue_Text.clear();

    m_pData = nullptr;
}
