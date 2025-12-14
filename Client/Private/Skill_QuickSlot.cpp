#include "Skill_QuickSlot.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "Skill_QuickSlot_List.h"
#include "UI_BackGround.h"
#include "UI_Default_Button.h"
#include "UI_TextBox.h"
#include "UI_Guide_Icon.h"
#include "UI_Default_Tex.h"

CSkill_QuickSlot::CSkill_QuickSlot(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Panel{ pDevice, pContext }
{
}

CSkill_QuickSlot::CSkill_QuickSlot(const CSkill_QuickSlot& Prototype)
    : CUI_Panel(Prototype)
{
}

void CSkill_QuickSlot::Equip_Check(_int iSkillIndex)
{
    for (_int i = 0; i < (_int)m_pList.size(); ++i)
    {
        m_pList[i]->UnEquipSlot(iSkillIndex);
    }
}

void CSkill_QuickSlot::On_Panel()
{
    if (m_IsUpdate)
        return;

    m_eAnimState = UIANIMSTATE::ON;
    m_fAccTime = 0.5f;
    m_IsUpdate = true;
    
    m_pGameInstance->Change_InputType(INPUT_TYPE::POPUP);
}

void CSkill_QuickSlot::Off_Panel()
{
    if (!m_IsUpdate)
        return;

    m_eAnimState = UIANIMSTATE::OFF;
    m_fAccTime = 1.f;

}

HRESULT CSkill_QuickSlot::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    CHECK_FAILED(Ready_Prototype(), E_FAIL);
    return S_OK;
}

HRESULT CSkill_QuickSlot::Initialize_Clone(void* pArg)
{
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);

    return S_OK;
}

void CSkill_QuickSlot::Priority_Update(_float fTimeDelta)
{
    if (!m_IsUpdate)
        return;

    __super::Priority_Update(fTimeDelta);
}

void CSkill_QuickSlot::Update(_float fTimeDelta)
{
    if (!m_IsUpdate)
        return;
    InputKey();
    UI_Animation(fTimeDelta);
    __super::Update(fTimeDelta);
}

void CSkill_QuickSlot::Late_Update(_float fTimeDelta)
{
    if (!m_IsUpdate)
        return;

    __super::Late_Update(fTimeDelta);
}

HRESULT CSkill_QuickSlot::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
{
    m_szName = pInData.value("name", "");
    string strTexType = pInData.value("TexType", "");

    string szType = pInData.value("type", "");
    m_iUIType = CClientInstance::GetInstance()->UIType_StringToEnum(szType);

    m_iTexPass = pInData.value("TexIndex", 0);
    m_iShaderPass = pInData.value("shaderPass", 0);

    m_fDepth = pInData.value("depth", 0.f);

    if (pInData.contains("LocalPos"))
    {
        m_vLocalPos.x = pInData["LocalPos"].value("x", 0.f);
        m_vLocalPos.y = pInData["LocalPos"].value("y", 0.f);
    }

    if (pInData.contains("LocalSize"))
    {
        m_vLocalSize.x = pInData["LocalSize"].value("x", 0.f);
        m_vLocalSize.y = pInData["LocalSize"].value("y", 0.f);
    }

    if (pInData.contains("Angle"))
    {
        m_vAngle.x = pInData["Angle"].value("x", 0.f);
        m_vAngle.y = pInData["Angle"].value("y", 0.f);
        m_vAngle.z = pInData["Angle"].value("z", 0.f);
    }

    if (pInData.contains("Color"))
    {
        m_vColor.x = pInData["Color"].value("x", 0.f);
        m_vColor.y = pInData["Color"].value("y", 0.f);
        m_vColor.z = pInData["Color"].value("z", 0.f);
        m_vColor.w = pInData["Color"].value("w", 0.f);
    }

    if (pInData.contains("UV"))
    {
        m_vUV.clear();
        for (auto& uv : pInData["UV"])
        {
            _float4 uvData;
            uvData.x = uv.value("MinX", 0.f);
            uvData.y = uv.value("MinY", 0.f);
            uvData.z = uv.value("MaxX", 0.f);
            uvData.w = uv.value("MaxY", 0.f);
            m_vUV.push_back(uvData);
        }
    }
    if (pInData.contains("Anime"))
    {
        m_Track.clear();
        for (auto& t : pInData["Anime"])
        {
            UIKEYFRAME track;
            track.fTrackPosition = t.value("TrackPosition", 0.f);
            track.fAlpha = t.value("Alpha", 1.f);
            track.fAngle = t.value("Angle", 0.f);
            track.fSize = t.value("Size", 1.f);
            track.szEvent = t.value("Event", "");

            if (t.contains("Transloation"))
            {
                track.vTransloation.x = t["Transloation"].value("x", 0.f);
                track.vTransloation.y = t["Transloation"].value("y", 0.f);
            }

            m_Track.push_back(track);
        }
    }

    m_pTransformCom->Scale(_float3{ m_vLocalSize.x, m_vLocalSize.y, 1.f });
    __super::Update_Rotation(0.f);

    if (pInData.contains("Children"))
    {
        for (auto& child : pInData["Children"])
        {
            string strName = child.value("name", "");

            if (strName == "Skill_Tap")
            {
                for (_int i = 0; i < 6; ++i)
                {
                    string strClass = child.value("class", "");
                    _wstring wstrClass = AnsiToWString(strClass);

                    CUIObject::UIOBJECT_DESC UIDesc{};
                    UIDesc.szName = "";
                    UIDesc.iUIType = 0;
                    UIDesc.vLocalSize = { 1.f, 1.f };
                    UIDesc.fDepth = 0;
                    UIDesc.vLocalPos = { g_iWinSizeX >> 1 , g_iWinSizeY >> 1 };

                    CUIObject* pChild = static_cast<CUIObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iPrototypeLevelID, wstrClass.c_str(), &UIDesc));

                    if (pChild == nullptr)
                    {
                        MSG_BOX(TEXT("자식 클론 생성 실패"));
                        return E_FAIL;
                    }
                    if (pChild->Load_UI(child, iPrototypeLevelID, pArg))
                        return E_FAIL;

                    pChild->Insert_Bubble([this](BUBBLEEVENT* pArg) {this->Bubble_EventCall(pArg); });
                    m_Children.push_back(pChild);
                    m_pList.push_back(static_cast<CSkill_QuickSlot_List*>(pChild));
                    Safe_AddRef(pChild);
                }
            }
            else
            {
                string strClass = child.value("class", "");
                _wstring wstrClass = AnsiToWString(strClass);

                CUIObject::UIOBJECT_DESC UIDesc{};
                UIDesc.szName = "";
                UIDesc.iUIType = 0;
                UIDesc.vLocalSize = { 1.f, 1.f };
                UIDesc.fDepth = 0;
                UIDesc.vLocalPos = { g_iWinSizeX >> 1 , g_iWinSizeY >> 1 };

                CUIObject* pChild = static_cast<CUIObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iPrototypeLevelID, wstrClass.c_str(), &UIDesc));

                if (pChild == nullptr)
                {
                    MSG_BOX(TEXT("자식 클론 생성 실패"));
                    return E_FAIL;
                }
                if (pChild->Load_UI(child, iPrototypeLevelID, pArg))
                    return E_FAIL;

                pChild->Insert_Bubble([this](BUBBLEEVENT* pArg) {this->Bubble_EventCall(pArg); });
                m_Children.push_back(pChild);
            }
        }
    }

    for (auto pChild : m_Children)
    {
        if (pChild->Get_Name() == "Name_Skill")
        {
            m_pSkillName = static_cast<CUI_TextBox*>(pChild);
            Safe_AddRef(m_pSkillName);
        }
    }
    _float2 vPos = {-20.f, -280.f};

    for (_int i = 0; i < (_int)m_pList.size(); ++i)
    {
        m_pList[i]->Update_Pos(i, vPos, 110.f);

        //데이터 셋팅
        //if(i == 0)
        //    m_pList[i]->Setting_Data(i, 106);
        //else if (i == 1)
        //    m_pList[i]->Setting_Data(i, 114);
        //else if (i == 2)
        //    m_pList[i]->Setting_Data(i, 103);

    }
    __super::Update_Transform(nullptr, m_vLocalPos);
    CHECK_FAILED(Ready_Object(), E_FAIL);

    m_vLocalPos.x = 400.f;

    return S_OK;
}

void CSkill_QuickSlot::Bubble_EventCall(BUBBLEEVENT* pArg)
{
    BUBBLE_DESC* pDesc = static_cast<BUBBLE_DESC*>(pArg);

    for (_int i = 0; i < (_int)m_pList.size(); ++i)
    {
        if (pDesc->iIndex == i)
            continue;

        m_pList[i]->UnEquipSlot(pDesc->iSkillIndex);
    }
    Off_Panel();
}

HRESULT CSkill_QuickSlot::Update_Switch(void* pArg)
{
    SKILLQUICK_DESC* pDesc = static_cast<SKILLQUICK_DESC*>(pArg);

    m_iSkillIndex = pDesc->iSkillIndex;

    m_pSkillName->Set_Text(CClientInstance::GetInstance()->Get_Data<SKILL_DB>(m_iSkillIndex)->wstrName);

    for (auto pList : m_pList)
        pList->Set_SkillIndex(m_iSkillIndex);

    On_Panel();

    return S_OK;
}

HRESULT CSkill_QuickSlot::Ready_Prototype()
{
    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Skill_QuickSlot_List"),
        CSkill_QuickSlot_List::Create(m_pDevice, m_pContext, m_iLevel)), E_FAIL);

    return S_OK;
}

HRESULT CSkill_QuickSlot::Ready_Object()
{
    UIOBJECT_DESC Desc = {};
    Desc.fDepth = 2.8f;
    Desc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
    Desc.szName = "BackGround";
    Desc.vLocalSize = { g_iWinSizeX, g_iWinSizeY };
    Desc.vLocalPos = { g_iWinSizeX>>1, g_iWinSizeY>>1 };

    m_pBackGround = static_cast<CUI_BackGround*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_BackGround"), &Desc));
    if (m_pBackGround == nullptr)
        return E_FAIL;
    m_pBackGround->Setting_BG(CUI_BackGround::UIBGTYPE::DEFAULT);
    m_pBackGround->Set_Color({ 0.0f, 0.0f, 0.0f, 0.8f });
    m_Children.push_back(m_pBackGround);
    Safe_AddRef(m_pBackGround);

    return S_OK;

}

void CSkill_QuickSlot::UI_Animation(_float fTimeDelta)
{
    if (m_eAnimState == UIANIMSTATE::ON)
    {
        m_fAccTime += fTimeDelta * 2.f;

        if (m_fAccTime >= 1.f)
        {
            m_fAccTime = 1.f;
            m_eAnimState = UIANIMSTATE::END;
        }
        __super::Update_Alpha(m_fAccTime);
    }
    else if (m_eAnimState == UIANIMSTATE::OFF)
    {
        m_fAccTime -= fTimeDelta * 3.f;


        if (m_fAccTime <= 0.f)
        {
            m_fAccTime = 0.f;
            m_eAnimState = UIANIMSTATE::END;
            m_IsUpdate = false;

            m_pGameInstance->Change_InputType(INPUT_TYPE::UI);
        }
        __super::Update_Alpha(m_fAccTime);
    }
}

void CSkill_QuickSlot::InputKey()
{
    if (m_pGameInstance->Key_Down(DIK_ESCAPE, INPUT_TYPE::POPUP))
        Off_Panel();
}

CSkill_QuickSlot* CSkill_QuickSlot::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    CSkill_QuickSlot* pInstance = new CSkill_QuickSlot(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        MSG_BOX(TEXT("Failed Created : CSkill_QuickSlot"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CSkill_QuickSlot::Clone(void* pArg)
{
    CSkill_QuickSlot* pInstance = new CSkill_QuickSlot(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CSkill_QuickSlot"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CSkill_QuickSlot::Free()
{
    __super::Free();

    Safe_Release(m_pBackGround);

    Safe_Release(m_pSkillName);
    for(auto pChild : m_pList)
        Safe_Release(pChild);
    m_pList.clear();


}
