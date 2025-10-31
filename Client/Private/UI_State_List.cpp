#include "UI_State_List.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_TextBox.h"
#include "UI_Atlas_Icon.h"
#include "UI_Default_Tex.h"
#include "UI_State_Button.h"

CUI_State_List::CUI_State_List(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Tap{ pDevice, pContext }
{
}

CUI_State_List::CUI_State_List(const CUI_State_List& Prototype)
    : CUI_Tap(Prototype)
{
}

void CUI_State_List::Setting_List(_int iIndex, _float2 vPos, _float fOffSetY)
{
    m_iIndex = iIndex;
    m_vLocalPos.x = vPos.x;
    m_vLocalPos.y = vPos.y + iIndex * fOffSetY;

    if (m_iIndex == 0)
    {
        m_pName_TextBox->Set_Text(TEXT("활력"));
        m_pStateIcon->Set_Texture(CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_Bullet_03_Body.png", 2), 2);
    }
    else if (m_iIndex == 1)
    {
        m_pName_TextBox->Set_Text(TEXT("지구력"));
        m_pStateIcon->Set_Texture(CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_Bullet_04_Heart.png", 2), 2);
    }
    else if (m_iIndex == 2)
    {
        m_pName_TextBox->Set_Text(TEXT("힘"));
        m_pStateIcon->Set_Texture(CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_Bullet_07_Strength.png", 2), 2);
    }
    else if (m_iIndex == 3)
    {
        m_pName_TextBox->Set_Text(TEXT("의지"));
        m_pStateIcon->Set_Texture(CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_Bullet_06_Force.png", 2), 2);
    }
    else if (m_iIndex == 4)
    {
        m_pName_TextBox->Set_Text(TEXT("역량"));
        m_pStateIcon->Set_Texture(CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_Bullet_05_Skill.png", 2), 2);
    }
    __super::Update_Transform(nullptr, m_vWorldPos);
}

HRESULT CUI_State_List::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
   
    return S_OK;

}

HRESULT CUI_State_List::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);
    CHECK_FAILED(Ready_Component(), E_FAIL);
    return S_OK;
}

void CUI_State_List::Priority_Update(_float fTimeDelta)
{
    m_iState = ENUM_CLASS(UISTATE::ENABLE);
    __super::Priority_Update(fTimeDelta);
}

void CUI_State_List::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CUI_State_List::Late_Update(_float fTimeDelta)
{
    CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);
    __super::Late_Update(fTimeDelta);
}

HRESULT CUI_State_List::Render()
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

HRESULT CUI_State_List::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
    string strTexTag = pInData.value("TexTag", "");
    wstring wstrTexTag = AnsiToWString(strTexTag);

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), wstrTexTag.c_str(),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    if (FAILED(__super::Load_UI(pInData, iPrototypeLevelID, pArg)))
        return E_FAIL;


    if (m_szName == "State_List")
    {
        m_pButton.resize(2);
        for (auto child : m_Children)
        {
            string strName = child->Get_Name();
            if (strName == "Level_List_Tex")
                m_pName_TextBox = static_cast<CUI_TextBox*>(child);
            else if (strName == "State_List_L" || strName == "State_List_R")
                m_pTexture.push_back(static_cast<CUI_Default_Tex*>(child));
            else if (strName == "Level_List_Icon")
                m_pStateIcon = static_cast<CUI_Atlas_Icon*>(child);
            else if (strName == "State_Level")
                m_pCulLevel_TextBox = static_cast<CUI_TextBox*>(child);
            else if (strName == "State_Level_Up")
                m_pUpLevel_TextBox = static_cast<CUI_TextBox*>(child);
            else if (strName == "State_Level_Up")
                m_pUpIcon = static_cast<CUI_Default_Tex*>(child);
            else if (strName == "State_Button_UP")
                m_pButton[0] = static_cast<CUI_State_Button*>(child);
            else if (strName == "State_Button_Down")
                m_pButton[1] = static_cast<CUI_State_Button*>(child);
        }
    }

    return S_OK;
}

HRESULT CUI_State_List::Ready_Component()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    return S_OK;

}

CUI_State_List* CUI_State_List::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    CUI_State_List* pInstance = new CUI_State_List(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        MSG_BOX(TEXT("Failed Created : CUI_State_List"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_State_List::Clone(void* pArg)
{
    CUI_State_List* pInstance = new CUI_State_List(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CUI_State_List"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CUI_State_List::Free()
{
    __super::Free();

    Safe_Release(m_pName_TextBox);
    Safe_Release(m_pCulLevel_TextBox);
    Safe_Release(m_pUpLevel_TextBox);
    Safe_Release(m_pStateIcon);
    Safe_Release(m_pUpIcon);

    for (auto pButton : m_pButton)
        Safe_Release(pButton);
    m_pButton.clear();

    for (auto pTex : m_pTexture)
        Safe_Release(pTex);
    m_pTexture.clear();

    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);

}
