#include "UI_BladeNexus_Map.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_TextBox.h"
#include "UI_BackGround.h"
#include "UI_Atlas_Icon.h"
#include "UI_Default_Tex.h"

#include "BladeNexus_Map_BG.h"
#include "BladeNexus_Map_List.h"
#include "UI_BladeNexus.h"

#include "Khazan_Spear.h"
#include "UI_Announce_MapName.h"

CUI_BladeNexus_Map::CUI_BladeNexus_Map(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Panel{ pDevice, pContext }
{
}

CUI_BladeNexus_Map::CUI_BladeNexus_Map(const CUI_BladeNexus_Map& Prototype)
    : CUI_Panel(Prototype)
{
}

void CUI_BladeNexus_Map::On_Panel(ONTYPE eType)
{
    if (m_IsUpdate)
        return;
   
    m_eOnType = eType;

    if (m_eOnType == ONTYPE::HEINMACH)
    {
        for (_int i = 0; i < (_int)m_pMapList.size(); ++i)
        {
            m_pMapList[i]->Setting_List(ENUM_CLASS(m_eOnType));
            m_pMapList[i]->Update_Visible(true);
        }
        m_pMapBg->Set_TexPass(0);
        m_pMapTex->Set_TexPass(1);
        m_iMaxSelete = 2;
    }
    else
    {
        for (_int i = 0; i < (_int)m_pMapList.size(); ++i)
        {
            m_pMapList[i]->Setting_List(ENUM_CLASS(m_eOnType));
            m_pMapList[i]->Update_Visible(true);
        }
        m_pMapBg->Set_TexPass(1);
        m_pMapTex->Set_TexPass(8);
        m_iMaxSelete = 0;
    }
    m_iSeleteIndex = 0;

    m_eAnimState = UIANIMSTATE::ON;
    m_fAccTime = 0.5f;
    m_IsUpdate = true;

    m_pGameInstance->Change_InputType(INPUT_TYPE::UI);

    for (_int i = 0; i < (_int)m_pMapList.size(); ++i)
    {
        if (m_iSeleteIndex == i)
            m_pMapList[i]->Set_Selete(true);
        else
            m_pMapList[i]->Set_Selete(false);
    }
}


void CUI_BladeNexus_Map::Off_Panel()
{
    if (!m_IsUpdate)
        return;

    CUI_BladeNexus::BLADENEXUS_ON_DESC Desc;

    Desc.eType = CUI_BladeNexus::ONTYPE::END;
    CClientInstance::GetInstance()->UI_UpdateSwitch(TEXT("BladeNexus"), &Desc);
    m_IsUpdate = false;
}

HRESULT CUI_BladeNexus_Map::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    CHECK_FAILED(Ready_Prototype(), E_FAIL);
    return S_OK;
}

HRESULT CUI_BladeNexus_Map::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;
    return S_OK;
}

void CUI_BladeNexus_Map::Priority_Update(_float fTimeDelta)
{
    if (m_pGameInstance->Key_Down(DIK_ESCAPE, INPUT_TYPE::UI))
    {
        Off_Panel();
    }
    if (!m_IsUpdate)
        return;

    UI_Animation(fTimeDelta);
    m_pBackGround->Priority_Update(fTimeDelta);
}

void CUI_BladeNexus_Map::Update(_float fTimeDelta)
{
    if (m_pGameInstance->Key_Down(DIK_8))
        On_Panel(ONTYPE::HEINMACH);

    if (m_pGameInstance->Key_Down(DIK_7))
        On_Panel(ONTYPE::EMBARS);

    if (!m_IsUpdate)
        return;

    __super::Update(fTimeDelta);
}

void CUI_BladeNexus_Map::Late_Update(_float fTimeDelta)
{
    if (!m_IsUpdate)
        return;

    _bool isKeyInput = false;
    if (m_pGameInstance->Key_Down(DIK_W, INPUT_TYPE::UI))
    {
        m_iSeleteIndex -= 1;
        isKeyInput = true;

        if (m_iSeleteIndex < 0)
            m_iSeleteIndex = m_iMaxSelete;
    
    }
    else if (m_pGameInstance->Key_Down(DIK_S, INPUT_TYPE::UI))
    {
        m_iSeleteIndex += 1;
        isKeyInput = true;

        if (m_iSeleteIndex > m_iMaxSelete)
            m_iSeleteIndex = 0;
    }
    if (isKeyInput)
    {
        for (_int i = 0; i < (_int)m_pMapList.size(); ++i)
        {
            if (m_iSeleteIndex == i)
                m_pMapList[i]->Set_Selete(true);
            else
                m_pMapList[i]->Set_Selete(false);
        }
    }
    __super::Late_Update(fTimeDelta);
}

HRESULT CUI_BladeNexus_Map::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
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

            string strClass = child.value("class", "");
            _wstring wstrClass = AnsiToWString(strClass);

            CUIObject::UIOBJECT_DESC UIDesc{};
            UIDesc.szName = "";
            UIDesc.iUIType = 0;
            UIDesc.vLocalSize = { 1.f, 1.f };
            UIDesc.fDepth = 0;
            UIDesc.vLocalPos = { g_iWinSizeX >> 1 , g_iWinSizeY >> 1 };
            if ("Map_List" == strName)
            {
                _float2 vPos = {};
                if (child.contains("LocalPos"))
                {
                    vPos.x = child["LocalPos"].value("x", 0.f);
                    vPos.y = child["LocalPos"].value("y", 0.f);
                }

                for (_int i = 0; i < 3; ++i)
                {
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
                    m_pMapList.push_back(static_cast<CUI_BladeNexus_Map_List*>(pChild));
                    Safe_AddRef(pChild);
                    static_cast<CUI_BladeNexus_Map_List*>(pChild)->Update_Pos(i, vPos, 70.f);
                }
            }
            else
            {
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

                if (strName == "MapBg")
                {
                    m_pMapBg = static_cast<CBladeNexus_Map_BG*>(pChild);
                    Safe_AddRef(pChild);
                }
                else if (strName == "MapImge")
                {
                    m_pMapTex = static_cast<CUI_Default_Tex*>(pChild);
                    Safe_AddRef(pChild);

                }
                
            }
        }
    }

    __super::Update_Transform(nullptr, m_vLocalPos);
    CHECK_FAILED(Ready_Object(), E_FAIL);

    return S_OK;
}

void CUI_BladeNexus_Map::Bubble_EventCall(BUBBLEEVENT* pArg)
{
    BUBBLE_MAP_DESC* pDesc = static_cast<BUBBLE_MAP_DESC*>(pArg);

    if (pDesc->isClick)
    {
        m_pGameInstance->Change_InputType(INPUT_TYPE::GAMEPLAY);

      
 

        _int iNexusIndex = { -1};
        if (m_eOnType == ONTYPE::HEINMACH)
        {
            if (pDesc->iIndex == 0)
                iNexusIndex = 0;
            else if (pDesc->iIndex == 1)
                iNexusIndex = 1;
            else if (pDesc->iIndex == 2)
                iNexusIndex = 2;
        }
        else  if (m_eOnType == ONTYPE::EMBARS)
        {
            if (pDesc->iIndex == 0)
                iNexusIndex = 4;
            else if (pDesc->iIndex == 1)
                iNexusIndex = 5;
            else if (pDesc->iIndex == 2)
                iNexusIndex = 6;
        }
        m_iNexusIndex = iNexusIndex;
        
        if (m_iNexusIndex > -1)
        {
            CClientInstance::GetInstance()->Fade_Out([this]() {this->Move_Player(); });
        }
           
        m_IsUpdate = false;
    }
    else
    {
        if (m_eOnType == ONTYPE::HEINMACH)
        {
            if (pDesc->iIndex == 0) m_pMapTex->Set_TexPass(1);
            else if (pDesc->iIndex == 1) m_pMapTex->Set_TexPass(2);
            else if (pDesc->iIndex == 2) m_pMapTex->Set_TexPass(4);
        }
        else
        {
            if (pDesc->iIndex == 0) m_pMapTex->Set_TexPass(8);
        }

        m_iSeleteIndex = pDesc->iIndex;
        for (_int i = 0; i < (_int)m_pMapList.size(); ++i)
        {
            if(pDesc->iIndex == i)
                m_pMapList[i]->Set_Selete(true);
            else
                m_pMapList[i]->Set_Selete(false);

        }
    }

}

HRESULT CUI_BladeNexus_Map::Update_Switch(void* pArg)
{
    return S_OK;
}

HRESULT CUI_BladeNexus_Map::Ready_Prototype()
{

    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_Component_UI_TS_Tex"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Map/Map_Tex_%d.png"), 9)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_Component_UI_TS_BG"),
        CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/UI/Map/Map_Bg_%d.png"), 2)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_BladeNexus_Map_BG"),
        CBladeNexus_Map_BG::Create(m_pDevice, m_pContext)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_BladeNexus_Map_List"),
        CUI_BladeNexus_Map_List::Create(m_pDevice, m_pContext, m_iLevel)), E_FAIL);

    return S_OK;
}

HRESULT CUI_BladeNexus_Map::Ready_Object()
{
    UIOBJECT_DESC Desc = {};
    Desc.fDepth = 5.5f;
    Desc.iUIType = ENUM_CLASS(UITYPE::TEXTURE);
    Desc.szName = "BackGround";
    Desc.vLocalSize = { g_iWinSizeX, g_iWinSizeY };
    Desc.vLocalPos = { g_iWinSizeX >> 1, g_iWinSizeY >> 1 };

    m_pBackGround = static_cast<CUI_BackGround*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_GameObject_UI_BackGround"), &Desc));
    if (m_pBackGround == nullptr)
        return E_FAIL;
    m_pBackGround->Setting_BG(CUI_BackGround::UIBGTYPE::BLADENEXUS);
    m_Children.push_back(m_pBackGround);
    Safe_AddRef(m_pBackGround);

    return S_OK;
}

void CUI_BladeNexus_Map::UI_Animation(_float fTimeDelta)
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
        }
    }
}

void CUI_BladeNexus_Map::Move_Player()
{
    CClientInstance::GetInstance()->Fade_Out();
    CKhazan_Spear* pKhazan = static_cast<CKhazan_Spear*>(m_pGameInstance->Find_GameObject(m_pGameInstance->Get_CurrentLevelID(), TEXT("Layer_Creature_Player"), 0));
    if (pKhazan == nullptr)
        MSG_BOX(TEXT("플레이어 없음"));
    pKhazan->Set_Position(CClientInstance::GetInstance()->Find_BladeNexus(m_iNexusIndex)->vPos);


    CClientInstance::GetInstance()->Fade_In();

    EVENT_ANNOUNCE_MAPNAME Desc = {};
    Desc.fTime = 2.f;
    Desc.iMapType = ENUM_CLASS(CUI_Announce_MapName::MAP_TYPE::DEFAULT);
    Desc.fFadeOutTime = 1.0f;
    Desc.isDissovle = true;
    Desc.wstrName = CClientInstance::GetInstance()->Find_BladeNexus(m_iNexusIndex)->strName;
    m_pGameInstance->Emit_Event<EVENT_ANNOUNCE_MAPNAME>(ENUM_CLASS(EVENT_TYPE::ANNOUNCE_MAPNAME), Desc);
}

CUI_BladeNexus_Map* CUI_BladeNexus_Map::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    CUI_BladeNexus_Map* pInstance = new CUI_BladeNexus_Map(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        MSG_BOX(TEXT("Failed Created : CUI_BladeNexus_Map"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_BladeNexus_Map::Clone(void* pArg)
{
    CUI_BladeNexus_Map* pInstance = new CUI_BladeNexus_Map(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CUI_BladeNexus_Map"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CUI_BladeNexus_Map::Free()
{
    __super::Free();
    Safe_Release(m_pBackGround);

    for (auto pList : m_pMapList)
        Safe_Release(pList);
    m_pMapList.clear();

    Safe_Release(m_pMapBg);
    Safe_Release(m_pMapTex);
}
