#include "UI_Talk_Trader.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_WorldTextBox.h"
#include "UI_WorldTex.h"
#include "UI_WorldList.h"

#include "UI_Inven.h"
#include "UI_Store.h"

CUI_Talk_Trader::CUI_Talk_Trader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Panel{ pDevice, pContext }
{
}

CUI_Talk_Trader::CUI_Talk_Trader(const CUI_Talk_Trader& Prototype)
    : CUI_Panel(Prototype)
{
}

void CUI_Talk_Trader::On_Panel()
{
    m_IsUpdate = true;
    m_iSelete = 0;
    Update_Selete();
    m_eType = TALK_TYPE::START;
    m_pGameInstance->Change_InputType(INPUT_TYPE::WORLD_UI);
    Setting_Talk();

    m_eAnimState = UIANIMSTATE::ON;
    m_fAccTime = 0.f;
}

void CUI_Talk_Trader::Off_Panel()
{
    m_eAnimState = UIANIMSTATE::OFF;
}

void CUI_Talk_Trader::Update_UITransform(_vector vPos)
{
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetZ(XMVectorSetX(XMVectorSetY(vPos, XMVectorGetY(vPos) + m_vLocalSize.y * 0.87f), XMVectorGetX(vPos) - 0.f), XMVectorGetZ(vPos) - 1.f));
//    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetZ(XMVectorSetX(XMVectorSetY(vPos, XMVectorGetY(vPos) + m_vLocalSize.y * 0.87f), XMVectorGetX(vPos) + 0.9f), XMVectorGetZ(vPos) + 0.6f));
    _float offsetY = sin(m_fSpeedWeight * 2.f) * 1.f * 0.5f
        + sin(m_fSpeedWeight * 2.f * 0.5f) * 1.f * 0.5f;
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetY(m_pTransformCom->Get_State(STATE::POSITION), XMVectorGetY(m_pTransformCom->Get_State(STATE::POSITION)) + offsetY * 0.02f));

    m_pTransformCom->Scale({ 1.15f, 1.f, 1.f });
    m_pTransformCom->Rotation(XMConvertToRadians(20.f), XMConvertToRadians(-120.f), XMConvertToRadians(3.f));
    m_vColor = { 1.f,1.f, 1.f, 0.8f };

    m_pName->Update_UITransform(m_pTransformCom->Get_WorldMatrix());
    m_pName->Set_LocalPos({ -0.53f, 0.425f, -0.01f, 1.f });
    m_pName->Set_LocalSize({ 0.5f, 0.5f, 1.f });
    m_pName->Set_Color({ 1.f, 1.f, 1.f,1.f });


    m_pText1->Update_UITransform(m_pTransformCom->Get_WorldMatrix());
    m_pText1->Set_LocalPos({ -0.51f, 0.33f, -0.01f, 1.f });
    m_pText1->Set_LocalSize({ 0.4f, 0.4f, 1.f });
    m_pText1->Set_Color({ 1.f,1.f,1.f,1.f });

    m_pText2->Update_UITransform(m_pTransformCom->Get_WorldMatrix());
    m_pText2->Set_LocalPos({ -0.51f, 0.27f, -0.01f, 1.f });
    m_pText2->Set_LocalSize({ 0.4f, 0.4f, 1.f });
    m_pText2->Set_Color({ 1.f,1.f,1.f,1.f });

    m_pText3->Update_UITransform(m_pTransformCom->Get_WorldMatrix());
    m_pText3->Set_LocalPos({ -0.51f, 0.21f, -0.01f, 1.f });
    m_pText3->Set_LocalSize({ 0.4f, 0.4f, 1.f });
    m_pText3->Set_Color({ 1.f,1.f,1.f,1.f });

    for (auto pLine : m_BG_Line)
        pLine->Update_UITransform(m_pTransformCom->Get_WorldMatrix());

    for (auto pList : m_pList)
        pList->Update_UITransform(m_pTransformCom->Get_WorldMatrix());

    for (auto pGuide : m_Key_Guide)
        pGuide->Update_UITransform(m_pTransformCom->Get_WorldMatrix());

    m_pList[0]->Set_LocalPos({ -0.f,-0.08f,-0.01f,1.f });
    m_pList[1]->Set_LocalPos({ -0.f,-0.19f,-0.01f,1.f });
    m_pList[2]->Set_LocalPos({ -0.f,-0.3f,-0.01f,1.f });

    m_pList[0]->Set_LocalSize({ 0.8f, 0.1f,1.f });
    m_pList[1]->Set_LocalSize({ 0.8f, 0.1f,1.f });
    m_pList[2]->Set_LocalSize({ 0.8f, 0.1f,1.f });

    m_pNameBG->Update_UITransform(m_pTransformCom->Get_WorldMatrix());
    m_BG_Line[0]->Set_LocalPos({ 0.f,0.49f,-0.001f,1.f });
    m_BG_Line[0]->Set_LocalSize({ 1.14f,0.01f,1.f });
    m_BG_Line[0]->Set_Color({ 0.655f, 0.875f, 1.f, 0.8f });
    m_BG_Line[0]->Set_ShaderPass(19);

    m_BG_Line[1]->Set_LocalPos({ 0.f,-0.49f,-0.001f,1.f });
    m_BG_Line[1]->Set_LocalSize({ 1.14f,0.01f,1.f });
    m_BG_Line[1]->Set_Color({ 0.655f, 0.875f, 1.f, 0.8f });
    m_BG_Line[1]->Set_ShaderPass(19);

    m_BG_Line[2]->Set_LocalPos({ 0.563f,0.f,-0.001f,1.f });
    m_BG_Line[2]->Set_LocalSize({ 0.01f, 0.995f,1.f });
    m_BG_Line[2]->Set_Color({ 0.655f, 0.875f, 1.f, 0.8f });
    m_BG_Line[2]->Set_ShaderPass(19);

    m_BG_Line[3]->Set_LocalPos({ -0.563f,0.f,-0.001f,1.f });
    m_BG_Line[3]->Set_LocalSize({ 0.01f, 0.995f,1.f });
    m_BG_Line[3]->Set_Color({ 0.655f, 0.875f, 1.f, 0.8f });
    m_BG_Line[3]->Set_ShaderPass(19);

    m_BG_Line[4]->Set_LocalPos({ 0.f,0.f,-0.001f,1.f });
    m_BG_Line[4]->Set_LocalSize({ 1.0f,0.01f,1.f });
    m_BG_Line[4]->Set_Color({ 0.8f, 0.8f, 0.8f, 0.5f });
    m_BG_Line[4]->Set_ShaderPass(19);

    m_Key_Guide[0]->Set_LocalPos({ -0.07f,-0.42f,-0.001f,1.f });
    m_Key_Guide[0]->Set_LocalSize({ 0.08f, 0.08f,1.f });


    m_Key_Guide[1]->Set_LocalPos({ -0.f,-0.42f,-0.001f,1.f });
    m_Key_Guide[1]->Set_LocalSize({ 0.05f, 0.05f,1.f });


    m_Key_Guide[2]->Set_LocalPos({ 0.07f,-0.42f,-0.001f,1.f });
    m_Key_Guide[2]->Set_LocalSize({ 0.08f, 0.08f,1.f });

    m_pNameBG->Set_LocalPos({ -0.34f,0.4f,-0.002f,1.f });
    m_pNameBG->Set_LocalSize({ 0.45f, 0.075f,1.f });
    m_pNameBG->Set_Color({ 0.655f, 0.875f, 1.f, 0.8f });
    m_pNameBG->Set_ShaderPass(19);
}

HRESULT CUI_Talk_Trader::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CUI_Talk_Trader::Initialize_Clone(void* pArg)
{
    m_vColor = { 1.f,1.f, 1.f, 0.9f };
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    CHECK_FAILED(Ready_Component(), E_FAIL);
    CHECK_FAILED(Ready_Children(), E_FAIL);
    
    CClientInstance::GetInstance()->Add_RootUI(AnsiToWString(m_szName), this);

    Ready_Item();
    return S_OK;
}
void CUI_Talk_Trader::Priority_Update(_float fTimeDelta)
{
    if (!m_IsUpdate)
        return;

}

void CUI_Talk_Trader::Update(_float fTimeDelta)
{
    if (!m_IsUpdate)
        return;

    m_fSpeedWeight += fTimeDelta;

    UI_Animation(fTimeDelta);
    if (m_eType == TALK_TYPE::START)
        List_Selete();

    if (m_pGameInstance->Key_Down(DIK_F, INPUT_TYPE::WORLD_UI))
    {
        m_pGameInstance->PlaySoundOnce(TEXT("UI_common_click2(SFX).wav"));

        if (m_iSelete == 0 && m_eType == TALK_TYPE::START)
        {
            CUI_Store::STOER_DESC Desc;
            Desc.isOpen = true;
            Desc.szName = m_szName;
            Desc.ItemIndex = m_ItemIndex;
            m_isUIOpen = true;
            CClientInstance::GetInstance()->UI_UpdateSwitch(TEXT("Store"), &Desc);
            Off_Panel();
        }
        else if (m_iSelete == 1 && m_eType == TALK_TYPE::START)
        {
            CUI_Inven::INVEN_ONOFF_DESC Desc = {};
            Desc.isOpen = true;
            Desc.eState = CUI_Inven::INVEN_STATE::SALE;
            Desc.szName = m_szName;
            m_isUIOpen = true;
            CClientInstance::GetInstance()->UI_UpdateSwitch(TEXT("Inven"), &Desc);
            m_pGameInstance->PlaySoundOnce(TEXT("UI_skill_close (SFX).wav"));
            Off_Panel();
        }
        else if (m_iSelete == 2 && m_eType == TALK_TYPE::START)
        {
            Off_Panel();
            m_isUIOpen = false;
            m_pGameInstance->PlaySoundOnce(TEXT("UI_skill_close (SFX).wav"));
            m_pGameInstance->StopByChannel(&m_pChannel);

            m_pGameInstance->Emit_Event<EventObject>(ENUM_CLASS(EVENT_TYPE::OBJECT_INTERACT), EventObject::OffEvent());
        }

        Setting_Talk();
    }

}

void CUI_Talk_Trader::Late_Update(_float fTimeDelta)
{
    if (!m_IsUpdate)
        return;

    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
        return;
    for (auto pLine : m_BG_Line)
        pLine->Late_Update(fTimeDelta);
    m_pNameBG->Late_Update(fTimeDelta);
    m_pText1->Late_Update(fTimeDelta);
    m_pText2->Late_Update(fTimeDelta);
    m_pText3->Late_Update(fTimeDelta);
    m_pName->Late_Update(fTimeDelta);


    for (auto pList : m_pList)
        pList->Late_Update(fTimeDelta);

    for (auto pGuide : m_Key_Guide)
        pGuide->Late_Update(fTimeDelta);
}

HRESULT CUI_Talk_Trader::Render()
{
    if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::VIEW))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", m_pGameInstance->Get_Transform_Float4x4(D3DTS::PROJ))))
        return E_FAIL;

    if (FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix")))
        return E_FAIL;

    if (FAILED(m_pTextureCom->Bind_Shader_Resource(m_pShaderCom, "g_Texture", 0)))
        return E_FAIL;
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vColor, sizeof(_float4)), E_FAIL);

    m_pShaderCom->Begin(17);
    m_pVIBufferCom->Bind_Resources();
    m_pVIBufferCom->Render();
    return S_OK;
}

HRESULT CUI_Talk_Trader::Update_Switch(void* pArg)
{
    On_Panel();

    return S_OK;
}

HRESULT CUI_Talk_Trader::Ready_Component()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_UI"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Texture_UI_BackGround"),
        TEXT("Com_Texture"), reinterpret_cast<CComponent**>(&m_pTextureCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Talk_Trader::Ready_Children()
{
    CUIObject::UIOBJECT_DESC Desc = {};
    Desc.iUIType = ENUM_CLASS(UITYPE::PANEL);
    Desc.vLocalPos = { 0.f, 0.f };
    Desc.vLocalSize = { 1.f, 1.f };
    Desc.szName = "TalkUI";
    m_pText1 = static_cast<CUI_WorldTextBox*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_WorldText"), &Desc));
    CHECK_NULLPTR(m_pText1, E_FAIL);
    Add_Child(m_pText1);

    m_pText2 = static_cast<CUI_WorldTextBox*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_WorldText"), &Desc));
    CHECK_NULLPTR(m_pText2, E_FAIL);
    Add_Child(m_pText2);

    m_pText3 = static_cast<CUI_WorldTextBox*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_WorldText"), &Desc));
    CHECK_NULLPTR(m_pText3, E_FAIL);
    Add_Child(m_pText3);

    m_pName = static_cast<CUI_WorldTextBox*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_WorldText"), &Desc));
    CHECK_NULLPTR(m_pName, E_FAIL);
    Add_Child(m_pName);

    for (_int i = 0; i < 5; ++i)
    {
        CUI_WorldTex* pBG_Line = static_cast<CUI_WorldTex*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_WorldTex"), &Desc));
        CHECK_NULLPTR(pBG_Line, E_FAIL);

        if (i <= 1)
            pBG_Line->Setting_Texture(1, CClientInstance::GetInstance()->Get_AtlasUV("T_BGDeco_ItemInfo_01.png", 1));
        else if (i < 4)
            pBG_Line->Setting_Texture(1, CClientInstance::GetInstance()->Get_AtlasUV("T_BGDeco_ItemInfo_03.png", 1));
        else
            pBG_Line->Setting_Texture(1, CClientInstance::GetInstance()->Get_AtlasUV("T_Img_Line_ItemInfo.png", 1));
        m_BG_Line.push_back(pBG_Line);
        Add_Child(pBG_Line);
    }

    m_pNameBG = static_cast<CUI_WorldTex*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_WorldTex"), &Desc));
    CHECK_NULLPTR(m_pNameBG, E_FAIL);
    m_pNameBG->Setting_Texture(6, TEXT("Prototype_Component_UI_Common_MenuList"));
    Add_Child(m_pNameBG);

    for (_int i = 0; i < 3; ++i)
    {
        CUI_WorldList* pBG_Line = static_cast<CUI_WorldList*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_WorldList"), &Desc));
        CHECK_NULLPTR(pBG_Line, E_FAIL);
        m_pList.push_back(pBG_Line);
        Add_Child(pBG_Line);
    }

    Update_Selete();
    m_IsUpdate = false;

    for (_int i = 0; i < 3; ++i)
    {
        CUI_WorldTex* pGuide = static_cast<CUI_WorldTex*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_WorldTex"), &Desc));
        CHECK_NULLPTR(pGuide, E_FAIL);

        m_Key_Guide.push_back(pGuide);
        Add_Child(pGuide);
    }

    m_Key_Guide[0]->Setting_Texture(4, CClientInstance::GetInstance()->Get_AtlasUV("Loading_4.png", 4));
    m_Key_Guide[1]->Setting_Texture(3, CClientInstance::GetInstance()->Get_AtlasUV("T_Icon_KB_F.png", 3));
    m_Key_Guide[2]->Setting_Texture(4, CClientInstance::GetInstance()->Get_AtlasUV("Loading_3.png", 4));

    m_pName->Set_Text(TEXT("두이무크"));
    m_pName->Set_TextTag(TEXT("Blade_Medium_20"));
    m_pText1->Set_TextTag(TEXT("Blade_Medium_20"));
    m_pText2->Set_TextTag(TEXT("Blade_Medium_20"));
    m_pText3->Set_TextTag(TEXT("Blade_Medium_20"));

    return S_OK;
}

void CUI_Talk_Trader::Ready_Item()
{
    m_ItemIndex.push_back(5001);
    m_ItemIndex.push_back(5002);
    m_ItemIndex.push_back(5003);
    m_ItemIndex.push_back(5004);
    m_ItemIndex.push_back(5005);
    m_ItemIndex.push_back(3001);
    m_ItemIndex.push_back(3002);
}

void CUI_Talk_Trader::Update_Selete()
{
    for (_int i = 0; i < 3; ++i)
    {
        if (m_iSelete == i)
            m_pList[i]->Set_Selete(true);
        else
            m_pList[i]->Set_Selete(false);
    }
}

void CUI_Talk_Trader::Setting_Talk()
{
    if (m_eAnimState == UIANIMSTATE::OFF)
        return;

    if (m_eType == TALK_TYPE::START)
    {
        m_pGameInstance->PlaySoundOnce(TEXT("dialogue_sub_1060001_dui (Korean(KR)).wav"),1.f, &m_pChannel);
        m_BG_Line[4]->Update_Visible(true);
        m_pList[0]->Update_Visible(true);
        m_pList[1]->Update_Visible(true);
        m_pList[2]->Update_Visible(true);

        m_pText1->Update_Visible(true);
        m_pText2->Update_Visible(true);
        m_pText3->Update_Visible(false);

        m_pList[0]->Setting_Icon(2, CClientInstance::GetInstance()->Get_AtlasUV("T_Ico_Dialog_LevelUp.png", 2));
        m_pList[1]->Setting_Icon(2, CClientInstance::GetInstance()->Get_AtlasUV("T_Ico_Dialog_LevelUp.png", 2));
        m_pList[2]->Setting_Icon(2, CClientInstance::GetInstance()->Get_AtlasUV("T_Ico_Dialog_Exit.png", 2));

        m_pList[0]->Set_Text(TEXT("구매"));
        m_pList[1]->Set_Text(TEXT("판매"));
        m_pList[2]->Set_Text(TEXT("대화를 마친다."));

        m_pText1->Set_Text(TEXT("잘오셨어요."));
        m_pText2->Set_Text(TEXT("마침 좋은 물건들이 들어왔습니다."));
    }
}

void CUI_Talk_Trader::List_Selete()
{
    if (m_pGameInstance->Key_Down(DIK_S, INPUT_TYPE::WORLD_UI))
    {
        ++m_iSelete;

        if (m_iSelete >= (_int)m_pList.size())
            m_iSelete = 0;

        Update_Selete();
    }
    else if (m_pGameInstance->Key_Down(DIK_W, INPUT_TYPE::WORLD_UI))
    {
        --m_iSelete;

        if (m_iSelete < 0)
            m_iSelete = (_int)m_pList.size() - 1;

        Update_Selete();
    }
}

void CUI_Talk_Trader::UI_Animation(_float fTimeDelta)
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

            if(m_isUIOpen)
                m_pGameInstance->Change_InputType(INPUT_TYPE::UI);
            else
                m_pGameInstance->Change_InputType(INPUT_TYPE::GAMEPLAY);
        }
    }
}

CUI_Talk_Trader* CUI_Talk_Trader::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CUI_Talk_Trader* pInstance = new CUI_Talk_Trader(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed Created : CUI_Talk_Trader"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_Talk_Trader::Clone(void* pArg)
{
    CUI_Talk_Trader* pInstance = new CUI_Talk_Trader(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CUI_Talk_Trader"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CUI_Talk_Trader::Free()
{
    //if (m_szName != "")
    //    CClientInstance::GetInstance()->Release_RootUI(AnsiToWString(m_szName));

    __super::Free();
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pTextureCom);
    Safe_Release(m_pVIBufferCom);

    Safe_Release(m_pName);
    Safe_Release(m_pText1);
    Safe_Release(m_pText2);
    Safe_Release(m_pText3);

    for (auto pLine : m_BG_Line)
        Safe_Release(pLine);
    m_BG_Line.clear();

    for (auto pGuide : m_Key_Guide)
        Safe_Release(pGuide);
    m_Key_Guide.clear();

    for (auto pList : m_pList)
        Safe_Release(pList);
    m_pList.clear();

    Safe_Release(m_pNameBG);
}
