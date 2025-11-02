#include "UI_State_Panel.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_State.h"
#include "UI_TextBox.h"
#include "UI_Atlas_Icon.h"
CUI_State_Panel::CUI_State_Panel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Panel{pDevice, pContext}
{
}

CUI_State_Panel::CUI_State_Panel(const CUI_State_Panel& Prototype)
    : CUI_Panel(Prototype)
{
}

HRESULT CUI_State_Panel::Setting_PanelLevel(_int iPanelType, UI_PLAYER_STATE_DATA* Data, UI_PLAYER_STATE_DATA* pUPData)
{
    m_pData = Data;
    m_pUpData = pUPData;

    if (iPanelType == ENUM_CLASS(CUI_State::STATE_PANEL::DEFAULT_STATE))
    {
        for (auto child : m_Children)
        {
            if (child->Get_UIType() == ENUM_CLASS(UITYPE::TEXTURE))
            {
                m_pIcon = static_cast<CUI_Atlas_Icon*>(child);
                Safe_AddRef(child);
                m_pIcon->Set_Shader(2);
                m_pIcon->Set_Color({ 0.2f, 0.45f, 1.f, 1.f });
            }
            else
            {
                if (child->Get_Name() == "DefaultState_Value_Hp")
                {
                    m_pValue_TextBox = static_cast<CUI_TextBox*>(child);
                    Safe_AddRef(child);
                    m_eState = PLAYTER_STATE::MAXHP;    
                }
                else if (child->Get_Name() == "DefaultState_Value_Stamina")
                {
                    m_pValue_TextBox = static_cast<CUI_TextBox*>(child);
                    Safe_AddRef(child);
                    m_eState = PLAYTER_STATE::MAXSTAMINA;    
                }
                else if (child->Get_Name() == "DefaultState_Value_Damage")
                {
                    m_pValue_TextBox = static_cast<CUI_TextBox*>(child);
                    Safe_AddRef(child);
                    m_eState = PLAYTER_STATE::ATK;    
                }
                else if (child->Get_Name() == "DefaultState_Value_Def")
                {
                    m_pValue_TextBox = static_cast<CUI_TextBox*>(child);
                    Safe_AddRef(child);
                    m_eState = PLAYTER_STATE::DEF;    
                }
                else if (child->Get_Name() == "DefaultState_Value_Weight")
                {
                    m_pValue_TextBox = static_cast<CUI_TextBox*>(child);
                    Safe_AddRef(child);
                    m_eState = PLAYTER_STATE::WEIGHT;    
                }
                else if (child->Get_Name() == "DefaultState_Value_Agile")
                {
                    m_pValue_TextBox = static_cast<CUI_TextBox*>(child);
                    Safe_AddRef(child);
                    m_eState = PLAYTER_STATE::AGILE;    
                }
                else if (child->Get_Name() == "DefaultState_List_Up_Hp")
                {
                    m_pUpValue_TextBox = static_cast<CUI_TextBox*>(child);
                    Safe_AddRef(child);
                }
                else if (child->Get_Name() == "DefaultState_List_Up_Stamina")
                {
                    m_pUpValue_TextBox = static_cast<CUI_TextBox*>(child);
                    Safe_AddRef(child);
                }
                else if (child->Get_Name() == "DefaultState_List_Up_Damage")
                {
                    m_pUpValue_TextBox = static_cast<CUI_TextBox*>(child);
                    Safe_AddRef(child);
                }
                else if (child->Get_Name() == "DefaultState_List_Up_Def")
                {
                    m_pUpValue_TextBox = static_cast<CUI_TextBox*>(child);
                    Safe_AddRef(child);
                }
                else if (child->Get_Name() == "DefaultState_List_Up_Weight")
                {
                    m_pUpValue_TextBox = static_cast<CUI_TextBox*>(child);
                    Safe_AddRef(child);
                }
                else if (child->Get_Name() == "DefaultState_List_Up_Agile")
                {
                    m_pUpValue_TextBox = static_cast<CUI_TextBox*>(child);
                    Safe_AddRef(child);
                }
                else
                {
                    m_pName_TextBox = static_cast<CUI_TextBox*>(child);
                    Safe_AddRef(child);
                }
            }
        }
    }
    else if (iPanelType == ENUM_CLASS(CUI_State::STATE_PANEL::ADD_STATE))
    {
        for (auto child : m_Children)
        {
            if (child->Get_UIType() == ENUM_CLASS(UITYPE::TEXTURE))
            {
                m_pIcon = static_cast<CUI_Atlas_Icon*>(child);
                Safe_AddRef(child);
                m_pIcon->Set_Shader(2);
                m_pIcon->Set_Color({ 0.2f, 0.45f, 1.f, 1.f });
            }
            else
            {
                if (child->Get_Name() == "AddState_Value_Atk")
                {
                    m_pValue_TextBox = static_cast<CUI_TextBox*>(child);
                    Safe_AddRef(child);
                    m_eState = PLAYTER_STATE::STAMINAATK;
                }
                else if (child->Get_Name() == "AddState_Value_Regen")
                {
                    m_pValue_TextBox = static_cast<CUI_TextBox*>(child);
                    Safe_AddRef(child);
                    m_eState = PLAYTER_STATE::STAMINAREGEN;
                }
                else if (child->Get_Name() == "AddState_Value_Min")
                {
                    m_pValue_TextBox = static_cast<CUI_TextBox*>(child);
                    Safe_AddRef(child);
                    m_eState = PLAYTER_STATE::EVASION_STAMINADOWN;
                }
                else if (child->Get_Name() == "AddState_Value_MinDamage")
                {
                    m_pValue_TextBox = static_cast<CUI_TextBox*>(child);
                    Safe_AddRef(child);
                    m_eState = PLAYTER_STATE::DAMAGE_STAMINADOWN;
                }
                else if (child->Get_Name() == "AddState_Value_GuardDamage")
                {
                    m_pValue_TextBox = static_cast<CUI_TextBox*>(child);
                    Safe_AddRef(child);
                    m_eState = PLAYTER_STATE::GUARD_STAMINADOWN;
                }
                else if (child->Get_Name() == "AddState_List_Up_Atk")
                {
                    m_pUpValue_TextBox = static_cast<CUI_TextBox*>(child);
                    Safe_AddRef(child);
                }
                else if (child->Get_Name() == "AddState_List_Up_Regen")
                {
                    m_pUpValue_TextBox = static_cast<CUI_TextBox*>(child);
                }
                else if (child->Get_Name() == "AddState_List_Up_Min")
                {
                    m_pUpValue_TextBox = static_cast<CUI_TextBox*>(child);
                    Safe_AddRef(child);
                }
                else if (child->Get_Name() == "AddState_List_Up_MinDamage")
                {
                    m_pUpValue_TextBox = static_cast<CUI_TextBox*>(child);
                    Safe_AddRef(child);
                }
                else if (child->Get_Name() == "AddState_List_Up_GuardDamage")
                {
                    m_pUpValue_TextBox = static_cast<CUI_TextBox*>(child);
                    Safe_AddRef(child);
                }
                else
                {
                    m_pName_TextBox = static_cast<CUI_TextBox*>(child);
                    Safe_AddRef(child);
                }
            }
        }
    }
    else if (iPanelType == ENUM_CLASS(CUI_State::STATE_PANEL::ELEMENTAL))
    {
        for (auto child : m_Children)
        {
            if (child->Get_UIType() == ENUM_CLASS(UITYPE::TEXTURE))
            {
                m_pIcon = static_cast<CUI_Atlas_Icon*>(child);
                Safe_AddRef(child);
                m_pIcon->Set_Shader(2);
                m_pIcon->Set_Color({ 0.2f, 0.45f, 1.f, 1.f });
            }
            else
            {
                if (child->Get_Name() == "Elemental_Value_Fire")
                {
                    m_pValue_TextBox = static_cast<CUI_TextBox*>(child);
                    Safe_AddRef(child);
                    m_eState = PLAYTER_STATE::FIRE;
                }
                else if (child->Get_Name() == "Elemental_Value_Water")
                {
                    m_pValue_TextBox = static_cast<CUI_TextBox*>(child);
                    Safe_AddRef(child);
                    m_eState = PLAYTER_STATE::WATER;
                }
                else if (child->Get_Name() == "Elemental_Value_Lightning")
                {
                    m_pValue_TextBox = static_cast<CUI_TextBox*>(child);
                    Safe_AddRef(child);
                    m_eState = PLAYTER_STATE::LIGHTNING;
                }
                else if (child->Get_Name() == "Elemental_Value_Earth")
                {
                    m_pValue_TextBox = static_cast<CUI_TextBox*>(child);
                    Safe_AddRef(child);
                    m_eState = PLAYTER_STATE::EARTH;
                }
                else if (child->Get_Name() == "Elemental_Value_Chaos")
                {
                    m_pValue_TextBox = static_cast<CUI_TextBox*>(child);
                    Safe_AddRef(child);
                    m_eState = PLAYTER_STATE::CHAOS;
                }
                else if (child->Get_Name() == "Elemental_Value_Disease")
                {
                    m_pValue_TextBox = static_cast<CUI_TextBox*>(child);
                    Safe_AddRef(child);
                    m_eState = PLAYTER_STATE::DISEASE;
                }
                else if (child->Get_Name() == "Elemental_Value_Poison")
                {
                    m_pValue_TextBox = static_cast<CUI_TextBox*>(child);
                    Safe_AddRef(child);
                    m_eState = PLAYTER_STATE::POISON;
                }
                else if (child->Get_Name() == "Elemental_List_Up_Fire")
                {
                    m_pUpValue_TextBox = static_cast<CUI_TextBox*>(child);
                    Safe_AddRef(child);
                }
                else if (child->Get_Name() == "Elemental_List_Up_Water")
                {
                    m_pUpValue_TextBox = static_cast<CUI_TextBox*>(child);
                    Safe_AddRef(child);
                }
                else if (child->Get_Name() == "Elemental_List_Up_Lightning")
                {
                    m_pUpValue_TextBox = static_cast<CUI_TextBox*>(child);
                    Safe_AddRef(child);
                }
                else if (child->Get_Name() == "Elemental_List_Up_Earth")
                {
                    m_pUpValue_TextBox = static_cast<CUI_TextBox*>(child);
                    Safe_AddRef(child);
                }
                else if (child->Get_Name() == "Elemental_List_Up_Chaos")
                {
                    m_pUpValue_TextBox = static_cast<CUI_TextBox*>(child);
                    Safe_AddRef(child);
                }
                else if (child->Get_Name() == "Elemental_List_Up_Disease")
                {
                    m_pUpValue_TextBox = static_cast<CUI_TextBox*>(child);
                    Safe_AddRef(child);
                }
                else if (child->Get_Name() == "Elemental_List_Up_Poison")
                {
                    m_pUpValue_TextBox = static_cast<CUI_TextBox*>(child);
                    Safe_AddRef(child);
                }
                else
                {
                    m_pName_TextBox = static_cast<CUI_TextBox*>(child);
                    Safe_AddRef(child);
                }
            }
        }
    }
    return S_OK;
}

void CUI_State_Panel::State_Hover()
{
    m_isUpgrade = true;
}

void CUI_State_Panel::State_On(_float fValue)
{
}

HRESULT CUI_State_Panel::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    return S_OK;

}

HRESULT CUI_State_Panel::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);
    CHECK_FAILED(Ready_Component(), E_FAIL);
    return S_OK;
}

void CUI_State_Panel::Priority_Update(_float fTimeDelta)
{
    m_isUpgrade = false;
}

void CUI_State_Panel::Update(_float fTimeDelta)
{
    Update_Text();
}

void CUI_State_Panel::Late_Update(_float fTimeDelta)
{
    if(m_pTextureCom != nullptr)
    {
        Update_UpGradeText(fTimeDelta);
    }

    if (m_pTextureCom != nullptr && m_isUpgrade)
    {
        CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
    }

    m_pName_TextBox->Late_Update(fTimeDelta);
    m_pValue_TextBox->Late_Update(fTimeDelta);
}

HRESULT CUI_State_Panel::Render()
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

HRESULT CUI_State_Panel::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
    string strTexTag = pInData.value("TexTag", "");
 
    if ("" != strTexTag)
    {
        wstring wstrTexTag = AnsiToWString(strTexTag);

        if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), wstrTexTag.c_str(),
            TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
            return E_FAIL;

    }

    if (FAILED(__super::Load_UI(pInData, iPrototypeLevelID, pArg)))
        return E_FAIL;

    m_iShaderPass = 0;
    return S_OK;
}

HRESULT CUI_State_Panel::Ready_Component()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

void CUI_State_Panel::Update_Text()
{
    _wstring wstrTemp = {};
    _int iTextValue = 4;
    _float fValue = 0;
    switch (m_eState)
    {
    case PLAYTER_STATE::MAXHP:
        m_pValue_TextBox->Set_Text(to_wstring(m_pData->iMaxHp));
        m_pUpValue_TextBox->Set_Text(to_wstring(m_pUpData->iMaxHp));
        break;
    case PLAYTER_STATE::MAXSTAMINA:
        m_pValue_TextBox->Set_Text(to_wstring(m_pData->iMaxStamina));
        m_pUpValue_TextBox->Set_Text(to_wstring(m_pUpData->iMaxStamina));
        break;
    case PLAYTER_STATE::ATK:
        m_pValue_TextBox->Set_Text(to_wstring(m_pData->iAtk));
        m_pUpValue_TextBox->Set_Text(to_wstring(m_pUpData->iAtk));
        break;
    case PLAYTER_STATE::DEF:
        m_pValue_TextBox->Set_Text(to_wstring(m_pData->iDef)); 
        m_pUpValue_TextBox->Set_Text(to_wstring(m_pUpData->iDef));
        break;
    case PLAYTER_STATE::WEIGHT:
        fValue = m_pData->fWeight / m_pData->fMaxWeight * 100.f;
        fValue >= 100.f ? iTextValue = 5 : iTextValue = 4;

        wstrTemp = to_wstring(m_pData->fWeight).substr(0, 4) + TEXT(" / ") + to_wstring(m_pData->fMaxWeight).substr(0, 4);
        wstrTemp += TEXT(" (") + to_wstring(fValue).substr(0, iTextValue) + TEXT("%)");
        m_pValue_TextBox->Set_Text(wstrTemp);
       
        fValue = m_pUpData->fWeight;
        fValue <= 0.f ? iTextValue = 3 : iTextValue = 4;
        wstrTemp = to_wstring(fValue).substr(0, iTextValue) + TEXT("%");
        m_pUpValue_TextBox->Set_Text(wstrTemp);
        break;

    case PLAYTER_STATE::AGILE:              
        if (m_pData->fAgile >= 100.f)
        {
            m_pValue_TextBox->Set_Text(TEXT("A"));
            m_pValue_TextBox->Set_Color({0.2f, 0.2f, 0.9f, 1.f});
        }
        else if (m_pData->fAgile >= 75.f)
        {
            m_pValue_TextBox->Set_Text(TEXT("B"));
            m_pValue_TextBox->Set_Color({ 0.6f, 0.6f, 0.2f, 1.f });
        }
        else if (m_pData->fAgile >= 50.f)
        {
            m_pValue_TextBox->Set_Text(TEXT("C"));
            m_pValue_TextBox->Set_Color({ 0.2f, 0.8f, 0.2f, 1.f });
        }
        else if (m_pData->fAgile >= 25.f)
        {
            m_pValue_TextBox->Set_Text(TEXT("D"));
            m_pValue_TextBox->Set_Color({ 0.2f, 0.2f, 0.2f, 1.f });
        }
        break;

    case PLAYTER_STATE::STAMINAATK:
        fValue = m_pData->fStaminaAttack;
        fValue <= 0.f ? iTextValue = 3 : iTextValue = 4;
        wstrTemp = to_wstring(fValue).substr(0, iTextValue) + TEXT("%");
        m_pValue_TextBox->Set_Text(wstrTemp);

        fValue = m_pUpData->fStaminaAttack;
        fValue <= 0.f ? iTextValue = 3 : iTextValue = 4;
        wstrTemp = to_wstring(fValue).substr(0, iTextValue) + TEXT("%");
        m_pUpValue_TextBox->Set_Text(wstrTemp);
        break;
    case PLAYTER_STATE::STAMINAREGEN:
        fValue = m_pData->fStaminaRegen;
        fValue <= 0.f ? iTextValue = 3 : iTextValue = 4;
        wstrTemp = to_wstring(fValue).substr(0, iTextValue) + TEXT("%");
        m_pValue_TextBox->Set_Text(wstrTemp);

        fValue = m_pUpData->fStaminaRegen;
        fValue <= 0.f ? iTextValue = 3 : iTextValue = 4;
        wstrTemp = to_wstring(fValue).substr(0, iTextValue) + TEXT("%");
        m_pUpValue_TextBox->Set_Text(wstrTemp);
        break;
    case PLAYTER_STATE::EVASION_STAMINADOWN:
        fValue = m_pData->fEvasion_StaminaDown;
        fValue <= 0.f ? iTextValue = 3 : iTextValue = 4;
        wstrTemp = to_wstring(fValue).substr(0, iTextValue) + TEXT("%");
        m_pValue_TextBox->Set_Text(wstrTemp);

        fValue = m_pUpData->fEvasion_StaminaDown;
        fValue <= 0.f ? iTextValue = 3 : iTextValue = 4;
        wstrTemp = to_wstring(fValue).substr(0, iTextValue) + TEXT("%");
        m_pUpValue_TextBox->Set_Text(wstrTemp);
        break;
    case PLAYTER_STATE::DAMAGE_STAMINADOWN:
        fValue = m_pData->fDamage_StaminaDown;
        fValue <= 0.f ? iTextValue = 3 : iTextValue = 4;
        wstrTemp = to_wstring(fValue).substr(0, iTextValue) + TEXT("%");
        m_pValue_TextBox->Set_Text(wstrTemp);

        fValue = m_pUpData->fDamage_StaminaDown;
        fValue <= 0.f ? iTextValue = 3 : iTextValue = 4;
        wstrTemp = to_wstring(fValue).substr(0, iTextValue) + TEXT("%");
        m_pUpValue_TextBox->Set_Text(wstrTemp);
        break;
    case PLAYTER_STATE::GUARD_STAMINADOWN:
        fValue = m_pData->fGuard_StaminaDown;
        fValue <= 0.f ? iTextValue = 3 : iTextValue = 4;
        wstrTemp = to_wstring(fValue).substr(0, iTextValue) + TEXT("%");
        m_pValue_TextBox->Set_Text(wstrTemp);

        fValue = m_pUpData->fGuard_StaminaDown;
        fValue <= 0.f ? iTextValue = 3 : iTextValue = 4;
        wstrTemp = to_wstring(fValue).substr(0, iTextValue) + TEXT("%");
        m_pUpValue_TextBox->Set_Text(wstrTemp);
        break;

    case PLAYTER_STATE::FIRE:
        m_pValue_TextBox->Set_Text(to_wstring(m_pData->iFire));
        m_pUpValue_TextBox->Set_Text(to_wstring(m_pUpData->iFire));
        break;
    case PLAYTER_STATE::WATER:         
        m_pValue_TextBox->Set_Text(to_wstring(m_pData->iWater)); 
        m_pUpValue_TextBox->Set_Text(to_wstring(m_pUpData->iWater));
        break;
    case PLAYTER_STATE::LIGHTNING:  
        m_pValue_TextBox->Set_Text(to_wstring(m_pData->iLightning)); 
        m_pUpValue_TextBox->Set_Text(to_wstring(m_pUpData->iLightning));
        break;
    case PLAYTER_STATE::EARTH:  
        m_pValue_TextBox->Set_Text(to_wstring(m_pData->iEarth));
        m_pUpValue_TextBox->Set_Text(to_wstring(m_pUpData->iEarth));
        break;
    case PLAYTER_STATE::CHAOS:   
        m_pValue_TextBox->Set_Text(to_wstring(m_pData->iChaos));
        m_pUpValue_TextBox->Set_Text(to_wstring(m_pUpData->iChaos));
        break;
    case PLAYTER_STATE::DISEASE:      
        m_pValue_TextBox->Set_Text(to_wstring(m_pData->iDisease)); 
        m_pUpValue_TextBox->Set_Text(to_wstring(m_pUpData->iDisease));
        break;
    case PLAYTER_STATE::POISON:     
        m_pValue_TextBox->Set_Text(to_wstring(m_pData->iPoison));
        m_pUpValue_TextBox->Set_Text(to_wstring(m_pUpData->iPoison));
        break;
    }

}

void CUI_State_Panel::Update_UpGradeText(_float fTimeDelta)
{
    _bool isRender = false;

    switch (m_eState)
    {
    case PLAYTER_STATE::MAXHP:                  m_pUpData->iMaxHp > 0 ? isRender = true : isRender = false; break;
    case PLAYTER_STATE::MAXSTAMINA:             m_pUpData->iMaxStamina > 0 ? isRender = true : isRender = false; break;
    case PLAYTER_STATE::ATK:                    m_pUpData->iAtk > 0 ? isRender = true : isRender = false; break;
    case PLAYTER_STATE::DEF:                    m_pUpData->iDef > 0 ? isRender = true : isRender = false; break;
    case PLAYTER_STATE::WEIGHT:                 m_pUpData->fWeight > 0 ? isRender = true : isRender = false; break;
    case PLAYTER_STATE::AGILE:                  m_pUpData->fAgile > 0 ? isRender = true : isRender = false; break;
    case PLAYTER_STATE::STAMINAATK:             m_pUpData->fStaminaAttack > 0 ? isRender = true : isRender = false; break;
    case PLAYTER_STATE::STAMINAREGEN:           m_pUpData->fStaminaRegen > 0 ? isRender = true : isRender = false; break;
    case PLAYTER_STATE::EVASION_STAMINADOWN:    m_pUpData->fEvasion_StaminaDown > 0 ? isRender = true : isRender = false; break;
    case PLAYTER_STATE::DAMAGE_STAMINADOWN:     m_pUpData->fDamage_StaminaDown > 0 ? isRender = true : isRender = false; break;
    case PLAYTER_STATE::GUARD_STAMINADOWN:      m_pUpData->fGuard_StaminaDown > 0 ? isRender = true : isRender = false; break;
    case PLAYTER_STATE::FIRE:                   m_pUpData->iFire > 0 ? isRender = true : isRender = false; break;
    case PLAYTER_STATE::WATER:                  m_pUpData->iWater > 0 ? isRender = true : isRender = false; break;
    case PLAYTER_STATE::LIGHTNING:              m_pUpData->iLightning > 0 ? isRender = true : isRender = false; break;
    case PLAYTER_STATE::EARTH:                  m_pUpData->iEarth > 0 ? isRender = true : isRender = false; break;
    case PLAYTER_STATE::CHAOS:                  m_pUpData->iChaos > 0 ? isRender = true : isRender = false; break;
    case PLAYTER_STATE::DISEASE:                m_pUpData->iDisease > 0 ? isRender = true : isRender = false; break;
    case PLAYTER_STATE::POISON:                 m_pUpData->iPoison > 0 ? isRender = true : isRender = false; break;
    }

    if (!isRender)
        return;

    m_pIcon->Late_Update(fTimeDelta);
    m_pUpValue_TextBox->Late_Update(fTimeDelta);
}

CUI_State_Panel* CUI_State_Panel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    CUI_State_Panel* pInstance = new CUI_State_Panel(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        MSG_BOX(TEXT("Failed Created : CUI_State_Panel"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_State_Panel::Clone(void* pArg)
{
    CUI_State_Panel* pInstance = new CUI_State_Panel(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CUI_State_Panel"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CUI_State_Panel::Free()
{
    __super::Free();

    Safe_Release(m_pValue_TextBox);
    Safe_Release(m_pIcon);
    Safe_Release(m_pName_TextBox);
    Safe_Release(m_pUpValue_TextBox);

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);

    m_pData = nullptr;
}
