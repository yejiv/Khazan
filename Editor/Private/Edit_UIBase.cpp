#include "Edit_UIBase.h"
#include "GameInstance.h"

CEdit_UIBase::CEdit_UIBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CUIObject{ pDevice, pContext }
{
}

CEdit_UIBase::CEdit_UIBase(const CEdit_UIBase& Prototype)
    : CUIObject(Prototype)
{
}

HRESULT CEdit_UIBase::Create_Child(_uint iPrototypeLevelIndex, const _wstring& strPrototypeTag, CUIObject::UIOBJECT_DESC* UIChildDesc, string szSeleteUIName, CUIObject* pParent)
{
    _bool isCreated = false;
    if (m_szName == szSeleteUIName)
    {
        UIChildDesc->fDepth = m_fDepth;
        Add_Child(static_cast<CUIObject*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iPrototypeLevelIndex, strPrototypeTag, UIChildDesc)));
        Update_Transform(pParent, m_vLocalPos);
        return S_OK;
    }
    else
    {
        for (auto& pChild : m_Children)
        {
            if (SUCCEEDED(static_cast<CEdit_UIBase*>(pChild)->Create_Child(iPrototypeLevelIndex, strPrototypeTag, UIChildDesc, szSeleteUIName)))
            {
                isCreated = true;
                break;
            }
        }
    }

    Update_Transform(pParent, m_vLocalPos);

    return E_FAIL;
}

HRESULT CEdit_UIBase::Save_UI(nlohmann::json& pOutData)
{
    nlohmann::json Data;

    Data["name"] = m_szName;
    Data["class"] = m_szClassName;

    string szType = UIType_EnumToString();
    Data["type"] = szType;
    if (m_eRenderType == UI_RENDER_TYPE::DEFAULT)
        Data["TexType"] = "Tex";
    else
        Data["TexType"] = "Atlas";

    Data["TexTag"] = m_szTexTag;
    Data["shaderPass"] = m_iShaderPass;
    Data["depth"] = m_fDepth;

    Data["LocalPos"]["x"] = m_vLocalPos.x;
    Data["LocalPos"]["y"] = m_vLocalPos.y;

    Data["LocalSize"]["x"] = m_vLocalSize.x;
    Data["LocalSize"]["y"] = m_vLocalSize.y;

    for (_int i = 0; i < m_vUVMinMax.size(); ++i)
    {
        nlohmann::json UVArray;
        UVArray["MinX"] = m_vUVMinMax[i].x;
        UVArray["MinY"] = m_vUVMinMax[i].y;
        UVArray["MaxX"] = m_vUVMinMax[i].z;
        UVArray["MaxY"] = m_vUVMinMax[i].w;

        Data["UV"].push_back(UVArray);
    }

    for (_int i = 0; i < m_EventNames.size(); ++i)
    {
        Data["Events"].push_back(m_EventNames[i]);
    }

    for (_int i = 0; i < m_Track.size(); ++i)
    {
        nlohmann::json Track;
        Track["TrackPosition"] = m_Track[i].fTrackPosition;
        Track["Alpha"] = m_Track[i].fAlpha;
        Track["Angle"] = m_Track[i].fAngle;
        Track["Size"] = m_Track[i].fSize;
        Track["Event"] = m_Track[i].szEvent;

        Track["Transloation"]["x"] = m_Track[i].vTransloation.x;
        Track["Transloation"]["y"] = m_Track[i].vTransloation.y;

        Data["Anime"].push_back(Track);
    }
    for (auto& pChild : m_Children)
    {
        nlohmann::json ChildData;
        static_cast<CEdit_UIBase*>(pChild)->Save_UI(ChildData);

        Data["Children"].push_back(ChildData);
    }

    pOutData = Data;

    return S_OK;
}

HRESULT CEdit_UIBase::Load_UI(nlohmann::json& pInData, _uint iPrototypeLevelID)
{
    m_szName = pInData.value("name", "");
    m_szClassName = pInData.value("class", "");

    string TexType = {};
    if (TexType == "Tex")
        m_eRenderType = UI_RENDER_TYPE::DEFAULT;
    else
        m_eRenderType = UI_RENDER_TYPE::ATLAS;
    m_szTexTag = pInData.value("TexTag", "");

    if (m_szTexTag != "")
    {
        if (m_eRenderType == UI_RENDER_TYPE::DEFAULT)
        {
            _int size_needed = MultiByteToWideChar(CP_ACP, 0, m_szTexTag.c_str(), -1, nullptr, 0);
            _wstring szPrototypeTag(size_needed, 0);

            MultiByteToWideChar(CP_ACP, 0, m_szTexTag.c_str(), -1, &szPrototypeTag[0], size_needed);
            if (!szPrototypeTag.empty() && szPrototypeTag.back() == L'\0')
                szPrototypeTag.pop_back();

            m_pTexture = static_cast<CTexture*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, iPrototypeLevelID, szPrototypeTag.c_str(), nullptr));
        }
        else
        {
            _int size_needed = MultiByteToWideChar(CP_ACP, 0, m_szTexTag.c_str(), -1, nullptr, 0);
            _wstring szPrototypeTag(size_needed, 0);

            MultiByteToWideChar(CP_ACP, 0, m_szTexTag.c_str(), -1, &szPrototypeTag[0], size_needed);
            if (!szPrototypeTag.empty() && szPrototypeTag.back() == L'\0')
                szPrototypeTag.pop_back();

            m_pTexture_AtlasCom = static_cast<CTexture_Atlas*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, iPrototypeLevelID, szPrototypeTag.c_str(), nullptr));
        }
    }

    string szType = pInData.value("type", "");
    m_iUIType = UIType_StringToEnum(szType); // ← 직접 만든 변환 함수

    m_iShaderPass = pInData.value("shaderPass", -1);
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

    if (pInData.contains("UV"))
    {
        m_vUVMinMax.clear();
        for (auto& uv : pInData["UV"])
        {
            _float4 uvData;
            uvData.x = uv.value("MinX", 0.f);
            uvData.y = uv.value("MinY", 0.f);
            uvData.z = uv.value("MaxX", 0.f);
            uvData.w = uv.value("MaxY", 0.f);
            m_vUVMinMax.push_back(uvData);
        }
    }

    if (pInData.contains("Events"))
    {
        m_EventNames.clear();
        for (auto& Event : pInData["Events"])
            m_EventNames.push_back(Event.get<string>());
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


    if (pInData.contains("Children"))
    {
        for (auto& child : pInData["Children"])
        {
            CUIObject::UIOBJECT_DESC UIDesc{};
            UIDesc.szName = "";
            UIDesc.iUIType = 0;
            UIDesc.vLocalSize = { 1.f, 1.f };
            UIDesc.fDepth = 0;
            UIDesc.vLocalPos = { g_iWinSizeX >> 1 , g_iWinSizeY >> 1 };

            CEdit_UIBase* pChild = static_cast<CEdit_UIBase*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::GAMEOBJECT, iPrototypeLevelID, TEXT("Prototype_GameObject_UI_Base"), &UIDesc));

            pChild->Load_UI(child, iPrototypeLevelID);
            m_Children.push_back(pChild);
        }
    }

    __super::Update_Transform(nullptr, m_vLocalPos);
    return S_OK;
}

void CEdit_UIBase::Root_SeleteButton(string& szSeleteUIName, _int iNum, _int& iSeletRootUI, _int& iPosX, _int& iPosY, _int& iSizeX, _int& iSizeY)
{
    if (ImGui::Button(m_szName.c_str()))
    {
        iSeletRootUI = iNum;
        szSeleteUIName = m_szName;
        iPosX = (_int)m_vWorldPos.x;
        iPosY = (_int)m_vWorldPos.y;
        iSizeX = (_int)m_vLocalSize.x;
        iSizeY = (_int)m_vLocalSize.y;
    }
}

void CEdit_UIBase::SeleteButton(string& szSeleteUIName, _int iNum, _int& iPosX, _int& iPosY, _int& iSizeX, _int& iSizeY)
{
    _int iSpacing = iNum + 1;
    for (_int i = 0; i < iSpacing; ++i)
    {
        ImGui::Dummy(ImVec2(10.0f, 0.0f));
        ImGui::SameLine();
    }
    if (m_szName == szSeleteUIName)
    {
        ImVec2 pos = ImGui::GetCursorScreenPos();
        ImVec2 text_size = ImGui::CalcTextSize(m_szName.c_str());

        ImGui::GetWindowDrawList()->AddRectFilled(
            pos, ImVec2(pos.x + text_size.x, pos.y + text_size.y),
            IM_COL32(200, 200, 200, 100));

        ImGui::TextUnformatted(m_szName.c_str());
    }
    else
        if (ImGui::Button(m_szName.c_str()))
        {
            szSeleteUIName = m_szName;
            iPosX = (_int)m_vWorldPos.x;
            iPosY = (_int)m_vWorldPos.y;
            iSizeX = (_int)m_vLocalSize.x;
            iSizeY = (_int)m_vLocalSize.y;
        }
    for (auto& pChild : m_Children)
        static_cast<CEdit_UIBase*>(pChild)->SeleteButton(szSeleteUIName, iSpacing, iPosX, iPosY, iSizeX, iSizeY);

}

void CEdit_UIBase::Update_Option(string& szSeleteUIName, const string pFrameName, _int iTexType)
{
    if (m_szName == szSeleteUIName)
    {
        switch (static_cast<UITYPE>(m_iUIType))
        {
        case UITYPE::TAP:
            ImGui::RadioButton("Disable", &m_iUiState, 0);
            ImGui::SameLine();
            ImGui::RadioButton("Enable", &m_iUiState, 1);
            ImGui::SameLine();
            ImGui::RadioButton("Over", &m_iUiState, 2);
            ImGui::SameLine();
            ImGui::RadioButton("Selete", &m_iUiState, 3);

            ImGui::Text("Min : %.2f, %.2f", m_vUVMinMax[m_iUiState].x, m_vUVMinMax[m_iUiState].y);
            ImGui::SameLine();
            ImGui::Text("Max : %.2f, %.2f", m_vUVMinMax[m_iUiState].z, m_vUVMinMax[m_iUiState].w);

            if (ImGui::Button("SetUV"))
            {
                if (iTexType == ENUM_CLASS(UI_RENDER_TYPE::ATLAS))
                    Set_UVTexSet(szSeleteUIName, pFrameName);
                else
                    m_vUVMinMax[m_iUiState] = { 0.f, 0.f, 1.f, 1.f };
            }
            ImGui::Text("EventName : ");
            ImGui::SameLine();
            ImGui::Text(m_EventNames[m_iUiState].c_str());
            ImGui::InputText("##UIEventLabel", m_szEvent, MAX_PATH);
            ImGui::SameLine();
            if (ImGui::Button("EventSet"))
                m_EventNames[m_iUiState] = m_szEvent;
            break;
        case UITYPE::BUTTON:
            ImGui::RadioButton("Disable", &m_iUiState, 0);
            ImGui::SameLine();
            ImGui::RadioButton("Enable", &m_iUiState, 1);
            ImGui::SameLine();
            ImGui::RadioButton("Over", &m_iUiState, 2);
            ImGui::SameLine();
            ImGui::RadioButton("Selete", &m_iUiState, 3);

            ImGui::Text("Min : %.2f, %.2f", m_vUVMinMax[m_iUiState].x, m_vUVMinMax[m_iUiState].y);
            ImGui::SameLine();
            ImGui::Text("Max : %.2f, %.2f", m_vUVMinMax[m_iUiState].z, m_vUVMinMax[m_iUiState].w);

            if (ImGui::Button("SetUV"))
            {
                if (iTexType == ENUM_CLASS(UI_RENDER_TYPE::ATLAS))
                    Set_UVTexSet(szSeleteUIName, pFrameName);
                else
                    m_vUVMinMax[m_iUiState] = { 0.f, 0.f, 1.f, 1.f };
            }
            ImGui::Text("EventName : ");
            ImGui::SameLine();
            ImGui::Text(m_EventNames[m_iUiState].c_str());
            ImGui::InputText("##UIEventLabel", m_szEvent, MAX_PATH);
            ImGui::SameLine();
            if (ImGui::Button("EventSet"))
                m_EventNames[m_iUiState] = m_szEvent;
            break;

        case UITYPE::SLOT:
            ImGui::RadioButton("Disable", &m_iUiState, 0);
            ImGui::SameLine();
            ImGui::RadioButton("Enable", &m_iUiState, 1);
            ImGui::SameLine();
            ImGui::RadioButton("Over", &m_iUiState, 2);
            ImGui::SameLine();
            ImGui::RadioButton("Selete", &m_iUiState, 3);

            ImGui::Text("Min : %.2f, %.2f", m_vUVMinMax[m_iUiState].x, m_vUVMinMax[m_iUiState].y);
            ImGui::SameLine();
            ImGui::Text("Max : %.2f, %.2f", m_vUVMinMax[m_iUiState].z, m_vUVMinMax[m_iUiState].w);

            if (ImGui::Button("SetUV"))
            {
                if (iTexType == ENUM_CLASS(UI_RENDER_TYPE::ATLAS))
                    Set_UVTexSet(szSeleteUIName, pFrameName);
                else
                    m_vUVMinMax[m_iUiState] = { 0.f, 0.f, 1.f, 1.f };
            }
            ImGui::Text(m_EventNames[m_iUiState].c_str());
            ImGui::InputText("##UIEventLabel", m_szEvent, MAX_PATH);
            ImGui::SameLine();
            if (ImGui::Button("EventSet"))
                m_EventNames[m_iUiState] = m_szEvent;
            break;
        case UITYPE::SCROLLBAR:
            ImGui::Text("Min : %.2f, %.2f", m_vUVMinMax[0].x, m_vUVMinMax[0].y);
            ImGui::SameLine();
            ImGui::Text("Max : %.2f, %.2f", m_vUVMinMax[0].z, m_vUVMinMax[0].w);

            ImGui::RadioButton("Up", &m_iUpDownState, 0);
            ImGui::RadioButton("Down", &m_iUpDownState, 1);

            ImGui::Text(m_EventNames[m_iUpDownState].c_str());
            ImGui::InputText("##UIEventLabel", m_szEvent, MAX_PATH);
            ImGui::SameLine();
            if (ImGui::Button("EventSet"))
                m_EventNames[m_iUpDownState] = m_szEvent;

            break;
        case UITYPE::PROGRESSBAR:
            ImGui::Text("Min : %.2f, %.2f", m_vUVMinMax[0].x, m_vUVMinMax[0].y);
            ImGui::SameLine();
            ImGui::Text("Max : %.2f, %.2f", m_vUVMinMax[0].z, m_vUVMinMax[0].w);

            ImGui::InputFloat("Value", &m_fUiState, 0.01f, 0.01f);

            ImGui::Text(m_EventNames[0].c_str());
            ImGui::InputText("##UIEventLabel", m_szEvent, MAX_PATH);
            ImGui::SameLine();
            if (ImGui::Button("EventSet"))
                m_EventNames[0] = m_szEvent;
            break;
        }
    }

    for (auto& pChild : m_Children)
    {
        static_cast<CEdit_UIBase*>(pChild)->Update_Option(szSeleteUIName, pFrameName, iTexType);
    }
}

_bool CEdit_UIBase::Update_ClassName(string& szSeleteUIName)
{
    if (m_szName == szSeleteUIName)
    {
        ImGui::Text("UI_ClassName : ");
        ImGui::SameLine();
        ImGui::Text(m_szClassName.c_str());
        return true;
    }

    for (auto& pChild : m_Children)
        if (static_cast<CEdit_UIBase*>(pChild)->Update_ClassName(szSeleteUIName) == true)
            return true;

    return false;
}

_bool CEdit_UIBase::ReName(string& szSeleteUIName, string szChangeUIName)
{
    if (m_szName == szSeleteUIName)
    {
        m_szName = szChangeUIName;
        return true;
    }

    for (auto& pChild : m_Children)
        if (static_cast<CEdit_UIBase*>(pChild)->ReName(szSeleteUIName, szChangeUIName) == true)
            return true;

    return false;
}

_bool CEdit_UIBase::Set_ClassName(string& szSeleteUIName, string szChangeUIName)
{
    if (m_szName == szSeleteUIName)
    {
        m_szClassName = szChangeUIName;
        return true;
    }

    for (auto& pChild : m_Children)
        if (static_cast<CEdit_UIBase*>(pChild)->Set_ClassName(szSeleteUIName, szChangeUIName) == true)
            return true;

    return false;
}

_bool CEdit_UIBase::Move_UI(string& szSeleteUIName, _float fSetX, _float fSetY, CUIObject* pParent, _bool isParent)
{
    if (m_szName == szSeleteUIName && isParent)
    {
        m_vLocalPos = { fSetX, fSetY };
        Update_Transform(nullptr, m_vLocalPos);
        return true;
    }
    else if (m_szName == szSeleteUIName)
    {
        m_vLocalPos.x = (pParent->Get_WolrdPos().x - fSetX) * -1.f;
        m_vLocalPos.y = (pParent->Get_WolrdPos().y - fSetY) * -1.f;
        m_vWorldPos.x = m_vLocalPos.x;
        m_vWorldPos.y = m_vLocalPos.y;
        m_pTransformCom->Set_State(STATE::POSITION, XMVectorSet(m_vWorldPos.x - m_iWinSizeX * 0.5f, -m_vWorldPos.y + m_iWinSizeY * 0.5f, 0.f, 1.f));
        return true;
    }

    for (auto& pChild : m_Children)
    {
        static_cast<CEdit_UIBase*>(pChild)->Move_UI(szSeleteUIName, fSetX, fSetY, this, false);
    }

    Update_Transform(nullptr, m_vLocalPos);
    return false;

}

_bool CEdit_UIBase::Scaling_UI(string& szSeleteUIName, _float fSizeX, _float fSizeY)
{
    if (m_szName == szSeleteUIName)
    {
        m_vLocalSize = { fSizeX , fSizeY };

        m_pTransformCom->Scale(_float3{ m_vLocalSize.x, m_vLocalSize.y, 1.f });
        return true;
    }

    for (auto& pChild : m_Children)
    {
        if (static_cast<CEdit_UIBase*>(pChild)->Scaling_UI(szSeleteUIName, fSizeX, fSizeY))
        {
            return true;
        }
    }
    return false;
}

void CEdit_UIBase::Set_Alpha(_float fAlpha)
{
    m_fAlpha = fAlpha;

    for (auto& pChild : m_Children)
        static_cast<CEdit_UIBase*>(pChild)->Set_Alpha(fAlpha);
}

HRESULT CEdit_UIBase::Set_AtlasTextTure(string& szSeleteUIName, _uint iPrototypeLevelID, const _wstring& strPrototypeTag, const string pFrameName, _int iTexType)
{
    if (m_szName == szSeleteUIName)
    {
        m_iTexType = iTexType;
        if (static_cast<UI_RENDER_TYPE>(iTexType) == UI_RENDER_TYPE::ATLAS)
        {
            Safe_Release(m_pTexture_AtlasCom);

            int size_needed = WideCharToMultiByte(CP_ACP, 0, strPrototypeTag.c_str(), -1, nullptr, 0, nullptr, nullptr);
            string result(size_needed, 0);

            WideCharToMultiByte(CP_ACP, 0, strPrototypeTag.c_str(), -1, &result[0], size_needed, nullptr, nullptr);

            if (!result.empty() && result.back() == '\0')
                result.pop_back();

            m_szTexTag = result;
            m_pTexture_AtlasCom = static_cast<CTexture_Atlas*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, iPrototypeLevelID, strPrototypeTag, nullptr));

            if (m_pTexture_AtlasCom != nullptr)
            {
                m_vUVMinMax[0] = m_pTexture_AtlasCom->FindTexFrame(pFrameName);
                m_iShaderPass = 1;
                m_eRenderType = UI_RENDER_TYPE::ATLAS;
            }
        }
        else
        {
            Safe_Release(m_pTexture);
            int size_needed = WideCharToMultiByte(CP_ACP, 0, strPrototypeTag.c_str(), -1, nullptr, 0, nullptr, nullptr);
            string result(size_needed, 0);

            WideCharToMultiByte(CP_ACP, 0, strPrototypeTag.c_str(), -1, &result[0], size_needed, nullptr, nullptr);

            if (!result.empty() && result.back() == '\0')
                result.pop_back();

            m_szTexTag = result;
            m_pTexture = static_cast<CTexture*>(m_pGameInstance->Clone_Prototype(PROTOTYPE::COMPONENT, iPrototypeLevelID, strPrototypeTag, nullptr));
            if (m_pTexture != nullptr)
            {
                m_vUVMinMax[0] = { 0.f, 0.f, 1.f, 1.f };
                m_iShaderPass = 1;
                m_eRenderType = UI_RENDER_TYPE::DEFAULT;
            }
        }
        return S_OK;
    }

    for (auto& pChild : m_Children)
    {
        if (SUCCEEDED(static_cast<CEdit_UIBase*>(pChild)->Set_AtlasTextTure(szSeleteUIName, iPrototypeLevelID, strPrototypeTag, pFrameName, iTexType)))
        {
            return S_OK;
        }
    }
    return E_FAIL;
}

_bool CEdit_UIBase::Set_UVTexSet(string& szSeleteUIName, const string pFrameName)
{
    if (m_szName == szSeleteUIName)
    {
        m_vUVMinMax[m_iUiState] = m_pTexture_AtlasCom->FindTexFrame(pFrameName);
        return true;
    }

    for (auto& pChild : m_Children)
    {
        if (static_cast<CEdit_UIBase*>(pChild)->Set_UVTexSet(szSeleteUIName, pFrameName))
        {
            return true;
        }
    }
    return false;
}

HRESULT CEdit_UIBase::Set_AtlasTexSize(string& szSeleteUIName, const string pFrameName, _float fSize)
{
    if (m_szName == szSeleteUIName)
    {
        if (m_pTexture_AtlasCom != nullptr)
        {
            _float2 vSize = m_pTexture_AtlasCom->FindTexSize(pFrameName);

            m_vLocalSize.x = vSize.x * fSize;
            m_vLocalSize.y = vSize.y * fSize;

            m_pTransformCom->Scale(_float3{ m_vLocalSize.x, m_vLocalSize.y, 1.f });
        }
        return S_OK;
    }

    for (auto& pChild : m_Children)
    {
        if (SUCCEEDED(static_cast<CEdit_UIBase*>(pChild)->Set_AtlasTexSize(szSeleteUIName, pFrameName, fSize)))
        {
            return S_OK;
        }
    }
    return E_FAIL;
}

_bool CEdit_UIBase::Anim_Empty(string& szSeleteUIName)
{
    if (m_szName == szSeleteUIName)
    {
        if (m_Track.empty())
            return true;
        else
            return false;
    }
    for (auto& pChild : m_Children)
    {
        if (static_cast<CEdit_UIBase*>(pChild)->Anim_Empty(szSeleteUIName))
            return true;
    }
    return false;
}

_bool CEdit_UIBase::Create_Anim(string& szSeleteUIName)
{
    if (m_szName == szSeleteUIName)
    {
        for (_int i = 0; i < 2; ++i)
        {
            UIKEYFRAME Desc = {};
            Desc.fAlpha = 1.f;
            Desc.fAngle = 0.f;
            Desc.fSize = 1.f;
            Desc.vTransloation = m_vWorldPos;
            Desc.fTrackPosition = (_float)i;

            m_Track.push_back(Desc);
        }
        return true;
    }
    for (auto& pChild : m_Children)
    {
        if (static_cast<CEdit_UIBase*>(pChild)->Create_Anim(szSeleteUIName))
            return true;
    }
    return false;
}

_bool CEdit_UIBase::Add_Anim(string& szSeleteUIName)
{
    if (m_szName == szSeleteUIName)
    {
        UIKEYFRAME Desc = {};
        Desc.fAlpha = 1.f;
        Desc.fSize = 1.f;
        Desc.fAngle = 0.f;
        Desc.vTransloation = m_vWorldPos;
        Desc.fTrackPosition = m_Track.back().fTrackPosition + 1.f;
        m_Track.push_back(Desc);
        return true;
    }
    for (auto& pChild : m_Children)
    {
        if (static_cast<CEdit_UIBase*>(pChild)->Add_Anim(szSeleteUIName))
            return true;
    }
    return false;
}

_bool CEdit_UIBase::Set_Anim(string& szSeleteUIName)
{
    if (m_szName == szSeleteUIName)
    {
        for (_int i = 0; i < m_Track.size(); ++i)
        {
            ImGui::BeginGroup();
            ImGui::PushID(i);
            ImGui::Text("Track %d", i);

            _float fTimeDelta = m_Track[i].fTrackPosition;
            ImGui::SameLine();
            ImGui::RadioButton("##SeleteTrack", &m_iSeleteTrackIndex, i);
            if (ImGui::InputFloat("Time", &fTimeDelta))
            {
                if (i == 0)
                    m_Track[i].fTrackPosition = fTimeDelta;
                else if (fTimeDelta > m_Track[i - 1].fTrackPosition)
                    m_Track[i].fTrackPosition = fTimeDelta;
            }
            ImGui::InputFloat("Size", &m_Track[i].fSize);
            ImGui::InputFloat("Alpha", &m_Track[i].fAlpha);
            ImGui::InputFloat("Angle", &m_Track[i].fAngle);

            ImGui::InputFloat("PosX", &m_Track[i].vTransloation.x);
            ImGui::InputFloat("PosY", &m_Track[i].vTransloation.y);

            ImGui::Text("EventName : ");
            ImGui::SameLine();
            ImGui::Text(m_Track[i].szEvent.c_str());
            ImGui::InputText("##UITrackEventLabel", m_szTrackEvent, MAX_PATH);
            if (ImGui::Button("TrackEventSet"))
                m_Track[i].szEvent = m_szTrackEvent;

            ImGui::PopID();
            ImGui::EndGroup();
            ImGui::Separator();
        }
        return true;
    }
    for (auto& pChild : m_Children)
    {
        if (static_cast<CEdit_UIBase*>(pChild)->Set_Anim(szSeleteUIName))
            return true;
    }
    return false;
}

_bool CEdit_UIBase::Set_AnimPos(string& szSeleteUIName)
{
    if (m_szName == szSeleteUIName)
    {
        m_Track[m_iSeleteTrackIndex].vTransloation = m_vWorldPos;
        return true;
    }
    for (auto& pChild : m_Children)
    {
        if (static_cast<CEdit_UIBase*>(pChild)->Set_AnimPos(szSeleteUIName))
            return true;
    }
    return false;
}

_bool CEdit_UIBase::Get_LastTime(string& szSeleteUIName, _float& fOutTime)
{
    if (m_szName == szSeleteUIName)
    {
        fOutTime = m_Track.back().fTrackPosition;
        return true;
    }
    for (auto& pChild : m_Children)
    {
        if (static_cast<CEdit_UIBase*>(pChild)->Get_LastTime(szSeleteUIName, fOutTime))
            return true;
    }
    return false;
}

_bool CEdit_UIBase::Play_Animation(string& szSeleteUIName, _float& fAccTime)
{
    if (m_szName == szSeleteUIName)
    {
        Update_Track(fAccTime);
        return true;
    }
    for (auto& pChild : m_Children)
    {
        if (static_cast<CEdit_UIBase*>(pChild)->Play_Animation(szSeleteUIName, fAccTime))
            return true;
    }
    return false;
}

_bool CEdit_UIBase::ReSet_Track(string& szSeleteUIName)
{
    if (m_szName == szSeleteUIName)
    {
        m_iCurrentKeyFrameIndex = 0;
        return true;
    }
    for (auto& pChild : m_Children)
    {
        if (static_cast<CEdit_UIBase*>(pChild)->ReSet_Track(szSeleteUIName))
            return true;
    }
    return false;
}

HRESULT CEdit_UIBase::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CEdit_UIBase::Initialize_Clone(void* pArg)
{
    m_vFrameColor = { 0.f, 1.f, 0.f, 1.f };
    m_fAlpha = 1.f;
    CHECK_FAILED(__super::Initialize_Clone(pArg), E_FAIL);
    CHECK_FAILED(Ready_Component(), E_FAIL);

    switch (static_cast<UITYPE>(m_iUIType))
    {
    case UITYPE::TAP:
        m_vUVMinMax.resize(4);
        m_EventNames.resize(4);
        break;
    case UITYPE::BUTTON:
        m_vUVMinMax.resize(4);
        m_EventNames.resize(4);
        break;
    case UITYPE::SLOT:
        m_vUVMinMax.resize(4);
        m_EventNames.resize(4);
        break;
    case UITYPE::SCROLLBAR:
        m_vUVMinMax.resize(1);
        m_EventNames.resize(2);
        break;
    case UITYPE::PROGRESSBAR:
        m_vUVMinMax.resize(1);
        m_EventNames.resize(1);
        break;
    default:
        m_vUVMinMax.resize(1);
        break;
    }
    return S_OK;
}

void CEdit_UIBase::Priority_Update(_float fTimeDelta)
{
    __super::Priority_Update(fTimeDelta);
}

void CEdit_UIBase::Update(_float fTimeDelta)
{
    __super::Update(fTimeDelta);
}

void CEdit_UIBase::Late_Update(_float fTimeDelta)
{
    if (FAILED(m_pGameInstance->Add_RenderGroup(RENDERGROUP::UI, this)))
        return;

    if (m_iUIType == ENUM_CLASS(UITYPE::SLOT) && m_iUiState == 0)
        return;

    if (m_iUIType == ENUM_CLASS(UITYPE::TAP) && m_iUiState == 0)
        return;

    __super::Late_Update(fTimeDelta);
}

HRESULT CEdit_UIBase::Render()
{
    CHECK_FAILED(m_pTransformCom->Bind_Shader_Resource(m_pShaderCom, "g_WorldMatrix"), E_FAIL);

    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vColor", &m_vFrameColor, sizeof(_float4)), E_FAIL);
    CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float)), E_FAIL);

    if (m_iShaderPass == 1)
    {
        CHECK_FAILED(m_pShaderCom->Bind_RawValue("g_vUVMinMax", &m_vUVMinMax[m_iUiState], sizeof(_float4)), E_FAIL);
        if (m_eRenderType == UI_RENDER_TYPE::ATLAS)
            CHECK_FAILED(m_pTexture_AtlasCom->Bind_Shader_Texture(m_pShaderCom, "g_Texture"), E_FAIL);
        else
            CHECK_FAILED(m_pTexture->Bind_Shader_Resource(m_pShaderCom, "g_Texture", m_iTexIndex), E_FAIL);

    }
    CHECK_FAILED(m_pShaderCom->Begin(m_iShaderPass), E_FAIL);

    CHECK_FAILED(m_pVIBufferCom->Bind_Resources(), E_FAIL);
    CHECK_FAILED(m_pVIBufferCom->Render(), E_FAIL);

    return S_OK;
}

HRESULT CEdit_UIBase::Ready_Component()
{
    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_Shader_VtxPosTex_Edit_UI"),
        TEXT("Com_Shader"), reinterpret_cast<CComponent**>(&m_pShaderCom), nullptr)))
        return E_FAIL;

    if (FAILED(CGameObject::Add_Component(ENUM_CLASS(LEVEL::STATIC), TEXT("Prototype_Component_VIBuffer_Rect"),
        TEXT("Com_VIBuffer"), reinterpret_cast<CComponent**>(&m_pVIBufferCom), nullptr)))
        return E_FAIL;

    return S_OK;
}

void CEdit_UIBase::Update_Track(_float& fAccTime)
{
    if (fAccTime >= m_Track.back().fTrackPosition)
    {
        fAccTime = 0.f;
        m_iCurrentKeyFrameIndex = 0;
    }
    if (fAccTime == 0.f)
        m_iCurrentKeyFrameIndex = 0;

    while (fAccTime >= m_Track[m_iCurrentKeyFrameIndex + 1].fTrackPosition)
        m_iCurrentKeyFrameIndex++;

    _float fRatio = (fAccTime - m_Track[m_iCurrentKeyFrameIndex].fTrackPosition) /
        (m_Track[m_iCurrentKeyFrameIndex + 1].fTrackPosition - m_Track[m_iCurrentKeyFrameIndex].fTrackPosition);

    //((1.f - fRatio) * First) + (fRatio * Second);
    //Size
    _float fSize = ((1.f - fRatio) * m_Track[m_iCurrentKeyFrameIndex].fSize) + (fRatio * m_Track[m_iCurrentKeyFrameIndex + 1].fSize);
    __super::Update_Scaling(fSize);

    //Alpha
    _float fAlpha = ((1.f - fRatio) * m_Track[m_iCurrentKeyFrameIndex].fAlpha) + (fRatio * m_Track[m_iCurrentKeyFrameIndex + 1].fAlpha);
    Set_Alpha(fAlpha);

    //Angle
    _float fAngle = ((1.f - fRatio) * m_Track[m_iCurrentKeyFrameIndex].fAngle) + (fRatio * m_Track[m_iCurrentKeyFrameIndex + 1].fAngle);
    __super::Update_Rotation(fAngle);

    //Pos
    _int iTrackIndex[4] = {};

    if (m_iCurrentKeyFrameIndex <= 0)
        iTrackIndex[0] = 0;
    else
        iTrackIndex[0] = m_iCurrentKeyFrameIndex - 1;

    iTrackIndex[1] = m_iCurrentKeyFrameIndex;
    iTrackIndex[2] = m_iCurrentKeyFrameIndex + 1;

    if (m_iCurrentKeyFrameIndex + 1 >= m_Track.size() - 1)
        iTrackIndex[3] = m_iCurrentKeyFrameIndex + 1;
    else
        iTrackIndex[3] = m_iCurrentKeyFrameIndex + 2;

    _vector p0 = { m_Track[iTrackIndex[0]].vTransloation.x, m_Track[iTrackIndex[0]].vTransloation.y, 1.f };
    _vector p1 = { m_Track[iTrackIndex[1]].vTransloation.x, m_Track[iTrackIndex[1]].vTransloation.y, 1.f };
    _vector p2 = { m_Track[iTrackIndex[2]].vTransloation.x, m_Track[iTrackIndex[2]].vTransloation.y, 1.f };
    _vector p3 = { m_Track[iTrackIndex[3]].vTransloation.x, m_Track[iTrackIndex[3]].vTransloation.y, 1.f };

    _float2 fPos = {};
    XMStoreFloat2(&fPos, XMVectorCatmullRom(p0, p1, p2, p3, fRatio));

    //m_vLocalPos.x = ((m_vWorldPos.x - m_vLocalPos.x) - fPos.x) * -1.f;
    //m_vLocalPos.y = ((m_vWorldPos.y - m_vLocalPos.y) - fPos.y) * -1.f;
    //m_vWorldPos.x = m_vLocalPos.x;
    //m_vWorldPos.y = m_vLocalPos.y;

    Update_Transform(nullptr, fPos);
}

string CEdit_UIBase::UIType_EnumToString()
{
    string szUIType = {};

    switch (static_cast<UITYPE>(m_iUIType))
    {
    case UITYPE::PANEL:
        szUIType = "PANEL";
        break;
    case UITYPE::TAP:
        szUIType = "TAP";
        break;
    case UITYPE::BUTTON:
        szUIType = "BUTTON";
        break;
    case UITYPE::SLOT:
        szUIType = "SLOT";
        break;
    case UITYPE::SCROLLBAR:
        szUIType = "SCROLLBAR";
        break;
    case UITYPE::PROGRESSBAR:
        szUIType = "PROGRESSBAR";
        break;
    }
    return szUIType;

}

_uint CEdit_UIBase::UIType_StringToEnum(string szUIType)
{
    UITYPE eUIType = {};

    if (szUIType == "PANEL")
        eUIType = UITYPE::PANEL;
    if (szUIType == "TAP")
        eUIType = UITYPE::TAP;
    if (szUIType == "BUTTON")
        eUIType = UITYPE::BUTTON;
    if (szUIType == "SLOT")
        eUIType = UITYPE::SLOT;
    if (szUIType == "PROGRESSBAR")
        eUIType = UITYPE::PROGRESSBAR;
    if (szUIType == "SCROLLBAR")
        eUIType = UITYPE::SCROLLBAR;

    return ENUM_CLASS(eUIType);
}

CEdit_UIBase* CEdit_UIBase::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CEdit_UIBase* pInstance = new CEdit_UIBase(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CEdit_UIBase"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CEdit_UIBase::Clone(void* pArg)
{
    CEdit_UIBase* pInstance = new CEdit_UIBase(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CEdit_UIBase"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEdit_UIBase::Free()
{
    __super::Free();
    Safe_Release(m_pShaderCom);
    Safe_Release(m_pVIBufferCom);
    Safe_Release(m_pTexture_AtlasCom);
    Safe_Release(m_pTexture);
}
