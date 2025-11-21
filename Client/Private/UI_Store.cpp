#include "UI_Store.h"
#include "GameInstance.h"
#include "ClientInstance.h"

#include "UI_TextBox.h"
#include "UI_Atlas_Icon.h"

#include "UI_BackGround.h"

#include "Store_List.h"
#include "Store_Slot.h"
#include "Amount.h"
CUI_Store::CUI_Store(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUI_Panel{ pDevice, pContext }
{
}

CUI_Store::CUI_Store(const CUI_Store& Prototype)
    : CUI_Panel(Prototype)
{
}

void CUI_Store::On_Panel()
{
    if (m_IsUpdate)
        return;
    m_eAnimState = UIANIMSTATE::ON;
    m_fAccTime = 0.f;
    m_IsUpdate = true;
    __super::Update_Alpha(m_fAccTime);

    m_iSeleteIndex = 0;
    Update_Selete();
}

void CUI_Store::Off_Panel()
{
    static_cast<CAmount*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Amount")))->Off_Panel();
    if (m_strReturnName == "")
    {
        m_eAnimState = UIANIMSTATE::OFF;
        m_fAccTime = 1.f;
    }
    else
    {
        m_IsUpdate = false;
        CClientInstance::GetInstance()->UI_UpdateSwitch(AnsiToWString(m_strReturnName));
        m_strReturnName = "";
    }

}

HRESULT CUI_Store::Initialize_Prototype(_uint iLevel)
{
    m_iLevel = iLevel;
    CHECK_FAILED(Ready_Prototype(), E_FAIL);
    return S_OK;
}

HRESULT CUI_Store::Initialize_Clone(void* pArg)
{
    if (FAILED(__super::Initialize_Clone(pArg)))
        return E_FAIL;

    return S_OK;
}

void CUI_Store::Priority_Update(_float fTimeDelta)
{
    if (!m_IsUpdate)
        return;

    UI_Animation(fTimeDelta);
    __super::Priority_Update(fTimeDelta);
}

void CUI_Store::Update(_float fTimeDelta)
{
    if (!m_IsUpdate)
        return;

    if (m_pGameInstance->Key_Down(DIK_ESCAPE, INPUT_TYPE::UI))
        Off_Panel();
    

    if (m_pGameInstance->Key_Down(DIK_S, INPUT_TYPE::UI))
    {
        ++m_iSeleteIndex;
        if (m_iSeleteIndex >= m_iMaxSeleteIndex)
            m_iSeleteIndex = 0;
        Update_Selete();
    }
    else if (m_pGameInstance->Key_Down(DIK_W, INPUT_TYPE::UI))
    {
        --m_iSeleteIndex;
        if (m_iSeleteIndex < 0)
            m_iSeleteIndex = m_iMaxSeleteIndex - 1;
        Update_Selete();
    }

    __super::Update(fTimeDelta);
}

void CUI_Store::Late_Update(_float fTimeDelta)
{
    if (!m_IsUpdate)
        return;

    __super::Late_Update(fTimeDelta);
}

HRESULT CUI_Store::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID, void* pArg)
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
    _float2 vPos = {};
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

            if (strName == "Item_List")
            {
                if (child.contains("LocalPos"))
                {
                    vPos.x = child["LocalPos"].value("x", 0.f);
                    vPos.y = child["LocalPos"].value("y", 0.f);
                }
                for (_int i = 0; i < 8; ++i)
                {
                    CStore_List* pChild = static_cast<CStore_List*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iPrototypeLevelID, wstrClass.c_str(), &UIDesc));

                    if (pChild == nullptr)
                    {
                        MSG_BOX(TEXT("자식 클론 생성 실패"));
                        return E_FAIL;
                    }
                    if (pChild->Load_UI(child, iPrototypeLevelID, pArg))
                        return E_FAIL;

                    pChild->Insert_Bubble([this](BUBBLEEVENT* pArg) {this->Bubble_EventCall(pArg); });

                    m_StoreList.push_back(pChild);
                    Add_Child(pChild);
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

            }
        }
    }
    __super::Update_Transform(nullptr, m_vLocalPos);

    for (_int i = 0; i < (_int)m_StoreList.size(); ++i)
    {
        m_StoreList[i]->Update_Pos(i, vPos, 100.f, this);

    }

    CHECK_FAILED(Ready_Object(), E_FAIL);
    return S_OK;
}

void CUI_Store::Bubble_EventCall(BUBBLEEVENT* pArg)
{
   
}

HRESULT CUI_Store::Update_Switch(void* pArg)
{
    if (pArg == nullptr)
        return E_FAIL;

    STOER_DESC* pDesc = static_cast<STOER_DESC*>(pArg);
    if (pDesc->isOpen)
    {
        static_cast<CAmount*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("Amount")))->On_Panel();
        On_Panel();
        m_strReturnName = pDesc->szName;

        _int iMaxIndex = (_int)pDesc->ItemIndex.size();

        if (iMaxIndex <= 8)
            m_iMaxSeleteIndex = iMaxIndex;
        else
            m_iMaxSeleteIndex = 8;

        for (_int i = 0; i < (_int)m_StoreList.size(); ++i)
        {
            if (iMaxIndex > i)
                m_StoreList[i]->Setting_List(pDesc->ItemIndex[i]);
            else
                m_StoreList[i]->Setting_List(-1);
        }

    }

    return S_OK;
}


HRESULT CUI_Store::Ready_Prototype()
{
    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Store_List"),
        CStore_List::Create(m_pDevice, m_pContext)), E_FAIL);

    CHECK_FAILED(m_pGameInstance->Add_Prototype(m_iLevel, TEXT("Prototype_GameObject_UI_Store_Slot"),
        CStore_Slot::Create(m_pDevice, m_pContext)), E_FAIL);

    return S_OK;
}

HRESULT CUI_Store::Ready_Object()
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

void CUI_Store::UI_Animation(_float fTimeDelta)
{
    if (m_eAnimState == UIANIMSTATE::ON)
    {
        m_fAccTime += fTimeDelta * 3.f;

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
        }
        __super::Update_Alpha(m_fAccTime);
    }
}

void CUI_Store::Update_Selete()
{
    for (_int i = 0; i < (_int)m_StoreList.size(); ++i)
    {
        if(m_iSeleteIndex == i)
            m_StoreList[i]->Set_Selete(true);
        else
            m_StoreList[i]->Set_Selete(false);
    }
}

CUI_Store* CUI_Store::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iLevel)
{
    CUI_Store* pInstance = new CUI_Store(pDevice, pContext);
    if (FAILED(pInstance->Initialize_Prototype(iLevel)))
    {
        MSG_BOX(TEXT("Failed Created : CUI_Store"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

CGameObject* CUI_Store::Clone(void* pArg)
{
    CUI_Store* pInstance = new CUI_Store(*this);
    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed Cloned : CUI_Store"));
        Safe_Release(pInstance);
    }
    return pInstance;
}

void CUI_Store::Free()
{
    __super::Free();

    Safe_Release(m_pBackGround);
}
