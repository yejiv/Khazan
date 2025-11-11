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

void CUI_State_List::Setting_List(_int iIndex, _float2 vPos, _float fOffSetY, _int* pValue, _int* pUpValue, _int* pPoint)
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
    m_pValue = pValue;
    m_pUpValue = pUpValue;
    m_pPoint = pPoint;

    __super::Update_Transform(nullptr, m_vWorldPos);
}

void CUI_State_List::Setting_Button(_int* pUpValue)
{
    m_pUpValue = pUpValue;
}

void CUI_State_List::Setting_Type(CUI_State::UI_TYPE eType)
{
    m_eUI_Type = eType;

    if (m_eUI_Type == CUI_State::UI_TYPE::DEFAULT)
        m_pCulLevel_TextBox->Setting_Pivot(220, 17);
    else
        m_pCulLevel_TextBox->Setting_Pivot(118, 17);
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
    m_iState = ENUM_CLASS(UISTATE::ENABLE);
    return S_OK;
}

void CUI_State_List::Priority_Update(_float fTimeDelta)
{
    if (m_iIndex > -1 && m_eUI_Type == CUI_State::UI_TYPE::UPAGERD)
    {
        for (auto pButton : m_pButton)
            pButton->Priority_Update(fTimeDelta);
    }

}

void CUI_State_List::Update(_float fTimeDelta)
{
    if (m_iIndex > -1 && m_eUI_Type == CUI_State::UI_TYPE::UPAGERD)
    {
        if (ButtonOver(g_hWnd))
        {            
            CUI_State::BubbleDesc Desc = {};
            Desc.eListType = static_cast<CUI_State::STATE_LIST>(m_iIndex);
            Desc.isClick = false;
            Desc.isList = true;
            m_UIBubbleCallBack(&Desc);
        }

        if (*m_pPoint > 0)
            m_pButton[0]->Update(fTimeDelta);

        if (*m_pUpValue > 0)
            m_pButton[1]->Update(fTimeDelta);
    }
    else
    {
        if (*m_pUpValue > 0)
        {
            m_pName_TextBox->Set_Color({ 1.f, 1.f, 1.f, 1.f });

            for (auto pTex : m_pTexture)
                pTex->Set_Color({1.f,1.f,1.f,1.f});
            m_vColor.w = 1.f;

            if (ButtonClick(g_hWnd, false, true, INPUT_TYPE::UI))
            {
                CUI_State::BubbleDesc Desc = {};
                Desc.isList = false;
                m_UIBubbleCallBack(&Desc);
            }
        }
        else
        {
            m_pName_TextBox->Set_Color({ 1.f, 1.f, 1.f, 0.6f });

            for (auto pTex : m_pTexture)
                pTex->Set_Color({ 1.f,1.f,1.f,0.6f });

            m_vColor.w = 0.6f;
        }


    }


}

void CUI_State_List::Late_Update(_float fTimeDelta)
{
    CClientInstance::GetInstance()->Add_UIRender(UI_RENDER_TYPE::DEFAULT, this);

    m_iIndex > -1 ? List_RenderUpdate(fTimeDelta) : Button_RenderUpdate(fTimeDelta);
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

void CUI_State_List::Bubble_EventCall(BUBBLEEVENT* pArg)
{
    STATE_LIST_BUBBLE* pDesc = static_cast<STATE_LIST_BUBBLE*>(pArg);

    if (pDesc->eType == STATE_BUTTON_TYPE::UP)
    {
        if (*m_pPoint <= 0)
            return;

        *m_pUpValue += 1;
        *m_pPoint -= 1;

        CUI_State::BubbleDesc Desc = {};
        Desc.eListType = static_cast<CUI_State::STATE_LIST>(m_iIndex);
        Desc.isClick = true;
        Desc.isUp = true;
        Desc.isList = true;
        m_UIBubbleCallBack(&Desc);
    }
    else
    {
        if (*m_pUpValue <= 0)
            return;

        *m_pUpValue -= 1;
        *m_pPoint += 1;

        CUI_State::BubbleDesc Desc = {};
        Desc.eListType = static_cast<CUI_State::STATE_LIST>(m_iIndex);
        Desc.isClick = true;
        Desc.isUp = false;
        Desc.isList = true;
        m_UIBubbleCallBack(&Desc);
    }
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
        m_pButton.resize(ENUM_CLASS(STATE_BUTTON_TYPE::END));
        for (auto child : m_Children)
        {
            string strName = child->Get_Name();
            Safe_AddRef(child);
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
            else if (strName == "State_Up_Icon")
                m_pUpIcon = static_cast<CUI_Default_Tex*>(child);
            else if (strName == "State_Button_UP")
                m_pButton[ENUM_CLASS(STATE_BUTTON_TYPE::UP)] = static_cast<CUI_State_Button*>(child);
            else if (strName == "State_Button_Down")
                m_pButton[ENUM_CLASS(STATE_BUTTON_TYPE::DOWN)] = static_cast<CUI_State_Button*>(child);
            else if (strName == "State_List_Hover")
                m_pHover = static_cast<CUI_Default_Tex*>(child);
            else
                Safe_Release(child);
        }

        m_pButton[ENUM_CLASS(STATE_BUTTON_TYPE::UP)]->Set_Type(STATE_BUTTON_TYPE::UP);
        m_pButton[ENUM_CLASS(STATE_BUTTON_TYPE::DOWN)]->Set_Type(STATE_BUTTON_TYPE::DOWN);
    }
    else if(m_szName == "State_Button")
    {
        for (auto child : m_Children)
        {
            string strName = child->Get_Name();
            Safe_AddRef(child);
            if (strName == "State_Button_Text")
                m_pName_TextBox = static_cast<CUI_TextBox*>(child);
            else if (strName == "State_Button_L" || strName == "State_Button_R")
                m_pTexture.push_back(static_cast<CUI_Default_Tex*>(child));
            else if (strName == "ButtonHover")
                m_pHover = static_cast<CUI_Default_Tex*>(child);
            else
                Safe_Release(child);
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

void CUI_State_List::List_RenderUpdate(_float fTimeDelta)
{
    if (m_eUI_Type == CUI_State::UI_TYPE::DEFAULT)
    {
        m_pCulLevel_TextBox->Set_Text(to_wstring(*m_pValue));
        m_pName_TextBox->Late_Update(fTimeDelta);
        m_pCulLevel_TextBox->Late_Update(fTimeDelta);
        m_pStateIcon->Late_Update(fTimeDelta);

        for (auto pTex : m_pTexture)
            pTex->Late_Update(fTimeDelta);
    }
    else
    {
        m_pCulLevel_TextBox->Set_Text(to_wstring(*m_pValue));
        m_pUpLevel_TextBox->Set_Text(to_wstring(*m_pValue + *m_pUpValue));

        m_pName_TextBox->Late_Update(fTimeDelta);
        m_pCulLevel_TextBox->Late_Update(fTimeDelta);
        m_pStateIcon->Late_Update(fTimeDelta);

        if (ButtonOver(g_hWnd))
            m_pHover->Late_Update(fTimeDelta);

        for (auto pTex : m_pTexture)
            pTex->Late_Update(fTimeDelta);

        if(*m_pPoint > 0)
            m_pButton[0]->Late_Update(fTimeDelta);

        if (*m_pUpValue > 0)
        {
            m_pUpLevel_TextBox->Late_Update(fTimeDelta);
            m_pUpIcon->Late_Update(fTimeDelta);
            m_pButton[1]->Late_Update(fTimeDelta);
        }
    }
}

void CUI_State_List::Button_RenderUpdate(_float fTimeDelta)
{
    m_pName_TextBox->Late_Update(fTimeDelta);
    for (auto pTex : m_pTexture)
        pTex->Late_Update(fTimeDelta);

    if (*m_pUpValue > 0 && ButtonOver(g_hWnd))
        m_pHover->Late_Update(fTimeDelta);
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

    Safe_Release(m_pHover);

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
