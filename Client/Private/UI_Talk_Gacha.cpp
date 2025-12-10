#include "UI_Talk_Gacha.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_WorldTextBox.h"
#include "UI_WorldTex.h"
#include "UI_WorldList.h"

#include "MiniGame_Gacha.h"

CUI_Talk_Gacha::CUI_Talk_Gacha(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Panel{ pDevice, pContext }
{
}

CUI_Talk_Gacha::CUI_Talk_Gacha(const CUI_Talk_Gacha& Prototype)
    : CUI_Panel(Prototype)
{
}

void CUI_Talk_Gacha::On_Panel()
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

void CUI_Talk_Gacha::Off_Panel()
{
    m_eAnimState = UIANIMSTATE::OFF;
}

void CUI_Talk_Gacha::Update_UITransform(_vector vPos)
{
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetZ(XMVectorSetX(XMVectorSetY(vPos, XMVectorGetY(vPos) + m_vLocalSize.y * 0.87f), XMVectorGetX(vPos) - 0.5f), XMVectorGetZ(vPos) - 0.9f));

    _float offsetY = sin(m_fSpeedWeight * 2.f) * 1.f * 0.5f
        + sin(m_fSpeedWeight * 2.f * 0.5f) * 1.f * 0.5f;
    m_pTransformCom->Set_State(STATE::POSITION, XMVectorSetY(m_pTransformCom->Get_State(STATE::POSITION), XMVectorGetY(m_pTransformCom->Get_State(STATE::POSITION)) + offsetY * 0.02f));

    m_pTransformCom->Scale({ 1.15f, 1.f, 1.f });
    m_pTransformCom->Rotation(XMConvertToRadians(3.f), XMConvertToRadians(-70.f), XMConvertToRadians(6.f));
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

    m_pList[0]->Set_LocalPos({ -0.f,0.03f,-0.01f,1.f });
    m_pList[1]->Set_LocalPos({ -0.f,-0.08f,-0.01f,1.f });
    m_pList[2]->Set_LocalPos({ -0.f,-0.19f,-0.01f,1.f });
    m_pList[3]->Set_LocalPos({ -0.f,-0.3f,-0.01f,1.f });


    m_pList[0]->Set_LocalSize({ 0.8f, 0.1f,1.f });
    m_pList[1]->Set_LocalSize({ 0.8f, 0.1f,1.f });
    m_pList[2]->Set_LocalSize({ 0.8f, 0.1f,1.f });
    m_pList[3]->Set_LocalSize({ 0.8f, 0.1f,1.f });

    m_pNameBG->Update_UITransform(m_pTransformCom->Get_WorldMatrix());
    m_BG_Line[0]->Set_LocalPos({ 0.f,0.49f,-0.001f,1.f });
    m_BG_Line[0]->Set_LocalSize({ 1.14f,0.01f,1.f });
    m_BG_Line[0]->Set_Color({ 0.722f, 0.573f, 0.333f, 0.8f });
    m_BG_Line[0]->Set_ShaderPass(19);

    m_BG_Line[1]->Set_LocalPos({ 0.f,-0.49f,-0.001f,1.f });
    m_BG_Line[1]->Set_LocalSize({ 1.14f,0.01f,1.f });
    m_BG_Line[1]->Set_Color({ 0.722f, 0.573f, 0.333f, 0.8f });
    m_BG_Line[1]->Set_ShaderPass(19);

    m_BG_Line[2]->Set_LocalPos({ 0.563f,0.f,-0.001f,1.f });
    m_BG_Line[2]->Set_LocalSize({ 0.01f, 0.995f,1.f });
    m_BG_Line[2]->Set_Color({ 0.722f, 0.573f, 0.333f, 0.8f });
    m_BG_Line[2]->Set_ShaderPass(19);

    m_BG_Line[3]->Set_LocalPos({ -0.563f,0.f,-0.001f,1.f });
    m_BG_Line[3]->Set_LocalSize({ 0.01f, 0.995f,1.f });
    m_BG_Line[3]->Set_Color({ 0.722f, 0.573f, 0.333f, 0.8f });
    m_BG_Line[3]->Set_ShaderPass(19);

    m_BG_Line[4]->Set_LocalPos({ 0.f,0.1f,-0.001f,1.f });
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
    m_pNameBG->Set_Color({ 0.722f, 0.573f, 0.333f, 0.8f });
    m_pNameBG->Set_ShaderPass(19);
}

HRESULT CUI_Talk_Gacha::Initialize_Prototype(_int iPorotoLevel)
{
    m_iPrototypeIndex = iPorotoLevel;

    CHECK_FAILED(Ready_Prototype(), E_FAIL);
    return S_OK;
}

HRESULT CUI_Talk_Gacha::Initialize_Clone(void* pArg)
{
    m_vColor = { 1.f,1.f, 1.f, 0.9f };
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    CHECK_FAILED(Ready_Component(), E_FAIL);
    CHECK_FAILED(Ready_Children(), E_FAIL);

    CClientInstance::GetInstance()->Add_RootUI(AnsiToWString(m_szName), this);

    return S_OK;
}
void CUI_Talk_Gacha::Priority_Update(_float fTimeDelta)
{
         m_pGacha->Priority_Update(fTimeDelta);

    if (!m_IsUpdate)
        return;
}

void CUI_Talk_Gacha::Update(_float fTimeDelta)
{
    m_pGacha->Update(fTimeDelta);

    if (m_eType == TALK_TYPE::MINIGAME)
    {
        if (CMiniGame_Gacha::GACHA_STATE::END == m_pGacha->Get_GachaState())
        {
            m_eType = TALK_TYPE::SUCCES;
            m_isSucces = m_pGacha->isSucces();
            // 미니게임 끝난 위치
            CClientInstance::GetInstance()->Camera_ReturnToPreviousPose(0.5f);
        }
    }
    if (!m_IsUpdate)
        return;

    m_fSpeedWeight += fTimeDelta;

    UI_Animation(fTimeDelta);
    if (m_eType == TALK_TYPE::START)
        List_Selete();

    if (m_pGameInstance->Key_Down(DIK_F, INPUT_TYPE::WORLD_UI))
    {
        //m_pGameInstance->PlaySoundOnce(TEXT("UI_common_click2(SFX).wav"));

        if ((m_eType == TALK_TYPE::START && m_iSelete == 0) || (m_eType == TALK_TYPE::START && m_iSelete == 1) || (m_eType == TALK_TYPE::START && m_iSelete == 2))
        {
            m_eType = TALK_TYPE::TALK;
            m_iTalkIndex = 0;
            Update_Selete();
        }
        else if (m_eType == TALK_TYPE::TALK)
        {
            ++m_iTalkIndex;
            if (m_iTalkIndex >= m_iMaxTalk)
            {
                m_eType = TALK_TYPE::MINIGAME;
                Update_Selete();
                m_pGacha->Start_MiniGame((CMiniGame_Gacha::MINIGAME_LEVEL)m_iSelete);                
                m_iSelete = 0;
            }
        }
        else if (m_iSelete == 3 && m_eType == TALK_TYPE::START)
        {
            Off_Panel();
            m_isUIOpen = false;
            m_pGameInstance->PlaySoundOnce(TEXT("UI_skill_close (SFX).wav"));
            m_pGameInstance->StopByChannel(&m_pChannel);

            m_pGameInstance->Emit_Event<EventObject>(ENUM_CLASS(EVENT_TYPE::OBJECT_INTERACT), EventObject::OffEvent());
        }
        else if (m_eType == TALK_TYPE::SUCCES)
        {
            ++m_iTalkIndex;
            if (m_iTalkIndex >= m_iMaxTalk)
            {
                m_eType = TALK_TYPE::START;
                m_iSelete = 0;
                Update_Selete();
            }
        }
        Setting_Talk();
    }

}

void CUI_Talk_Gacha::Late_Update(_float fTimeDelta)
{
    m_pGacha->Late_Update(fTimeDelta);

    if (!m_IsUpdate)
        return;

    if (m_eType == TALK_TYPE::MINIGAME)
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

HRESULT CUI_Talk_Gacha::Render()
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

HRESULT CUI_Talk_Gacha::Update_Switch(void* pArg)
{
    On_Panel();

    return S_OK;
}

HRESULT CUI_Talk_Gacha::Ready_Prototype()
{
    if (FAILED(m_pGameInstance->Add_Prototype(m_iPrototypeIndex, TEXT("Prototype_GameObject_MiniGame_Gacha"),
        CMiniGame_Gacha::Create(m_pDevice, m_pContext, m_iPrototypeIndex))))
        return E_FAIL;

    return S_OK;
}

HRESULT CUI_Talk_Gacha::Ready_Component()
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

HRESULT CUI_Talk_Gacha::Ready_Children()
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

    for (_int i = 0; i < 4; ++i)
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

    m_pName->Set_Text(TEXT("단진"));
    m_pName->Set_TextTag(TEXT("Blade_Medium_20"));
    m_pText1->Set_TextTag(TEXT("Blade_Medium_20"));
    m_pText2->Set_TextTag(TEXT("Blade_Medium_20"));
    m_pText3->Set_TextTag(TEXT("Blade_Medium_20"));

    m_pGacha = static_cast<CMiniGame_Gacha*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, m_iPrototypeIndex, TEXT("Prototype_GameObject_MiniGame_Gacha")));
    CHECK_NULLPTR(m_pGacha, E_FAIL);

    return S_OK;
}

void CUI_Talk_Gacha::Update_Selete()
{
    for (_int i = 0; i < (_int)m_pList.size(); ++i)
    {
        if (m_iSelete == i)
            m_pList[i]->Set_Selete(true);
        else
            m_pList[i]->Set_Selete(false);
    }
}

void CUI_Talk_Gacha::Setting_Talk()
{
    if (m_eAnimState == UIANIMSTATE::OFF)
        return;

    if (m_eType == TALK_TYPE::START)
    {
        m_BG_Line[4]->Update_Visible(true);
        m_pList[0]->Update_Visible(true);
        m_pList[1]->Update_Visible(true);
        m_pList[2]->Update_Visible(true);
        m_pList[3]->Update_Visible(true);

        m_pText1->Update_Visible(true);
        m_pText2->Update_Visible(true);
        m_pText3->Update_Visible(false);

        m_pList[0]->Setting_Icon(2, CClientInstance::GetInstance()->Get_AtlasUV("T_Ico_Dialog_LevelUp.png", 2));
        m_pList[1]->Setting_Icon(2, CClientInstance::GetInstance()->Get_AtlasUV("T_Ico_Dialog_LevelUp.png", 2));
        m_pList[2]->Setting_Icon(2, CClientInstance::GetInstance()->Get_AtlasUV("T_Ico_Dialog_LevelUp.png", 2));
        m_pList[3]->Setting_Icon(2, CClientInstance::GetInstance()->Get_AtlasUV("T_Ico_Dialog_Exit.png", 2));

        m_pList[0]->Set_Text(TEXT("쉬움"));
        m_pList[1]->Set_Text(TEXT("중간"));
        m_pList[2]->Set_Text(TEXT("어려움"));
        m_pList[3]->Set_Text(TEXT("대화를 마친다."));

        //m_pGameInstance->PlaySoundOnce(TEXT("dialogue_sub_06_20640118_dan (Korean(KR)).wav"), 1.f, &m_pChannel);

        m_pText1->Set_Text(TEXT("카잔님 다시 만나시는 군요"));
        m_pText2->Set_Text(TEXT("저랑 야바위 한판 하실래요?"));

    }
    else if (m_eType == TALK_TYPE::TALK)
    {
        m_BG_Line[4]->Update_Visible(false);
        m_pList[0]->Update_Visible(false);
        m_pList[1]->Update_Visible(false);
        m_pList[2]->Update_Visible(false);
        m_pList[3]->Update_Visible(false);
        m_pText3->Update_Visible(true);
        m_iMaxTalk = 1;
        switch (m_iSelete)
        {
        case 0:
            m_pText1->Set_Text(TEXT("쉬움이라 흠"));
            m_pText2->Set_Text(TEXT("겉모습과 다르게 겁쟁이시군요"));
            m_pText3->Set_Text(TEXT("그래도 행운을 빌어요~"));
            break;
        case 1:
            m_pText1->Set_Text(TEXT("야바위에 확률은 50%인거 아시나요?"));
            m_pText2->Set_Text(TEXT("맞추거나 못맞추거나!"));
            m_pText3->Set_Text(TEXT("행운을 빌어요~"));
            break;
        case 2:
            m_pText1->Set_Text(TEXT("보기와 다르게 화끈하시군요"));
            m_pText2->Set_Text(TEXT("맞추시면 행운의 요정을 보상으로 드리지요"));
            m_pText3->Set_Text(TEXT("행운을 빌어요~"));
            break;
        }
    }
    else if (m_eType == TALK_TYPE::SUCCES)
    {
        m_BG_Line[4]->Update_Visible(false);
        m_pList[0]->Update_Visible(false);
        m_pList[1]->Update_Visible(false);
        m_pList[2]->Update_Visible(false);
        m_pList[3]->Update_Visible(false);
        m_pText3->Update_Visible(true);
        m_iMaxTalk = 1;

        if (m_isSucces)
        {
            m_pText1->Set_Text(TEXT("성공하셨군요!!"));
            m_pText2->Set_Text(TEXT("대단한 눈썰미네요"));
            m_pText3->Set_Text(TEXT("보상을 드리겠습니다~"));
        }
        else
        {
            m_pText1->Set_Text(TEXT("아쉽게 실패하셨네요"));
            m_pText2->Set_Text(TEXT("눈썰미를 조금 더 키우고 오시는 게"));
            m_pText3->Set_Text(TEXT("한번 더 도전하시겠어요?"));
        }
    }
}

void CUI_Talk_Gacha::List_Selete()
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

void CUI_Talk_Gacha::UI_Animation(_float fTimeDelta)
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
            m_pGameInstance->Change_InputType(INPUT_TYPE::GAMEPLAY);

            if (m_isUIOpen)
                m_pGameInstance->Change_InputType(INPUT_TYPE::UI);
            else
                m_pGameInstance->Change_InputType(INPUT_TYPE::GAMEPLAY);
        }
    }
}

CUI_Talk_Gacha* CUI_Talk_Gacha::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _int iPorotoLevel)
{
    CUI_Talk_Gacha* pInstance = new CUI_Talk_Gacha(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(iPorotoLevel)))
    {
        MSG_BOX(TEXT("Failed Created : CUI_Talk_Gacha"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_Talk_Gacha::Clone(void* pArg)
{
    CUI_Talk_Gacha* pInstance = new CUI_Talk_Gacha(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CUI_Talk_Gacha"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CUI_Talk_Gacha::Free()
{
    //if(m_szName != "")
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
    Safe_Release(m_pGacha);
}
