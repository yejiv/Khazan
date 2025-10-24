#include "Effect_Prefab.h"
#include "Effect_Point_Instance.h"
#include "Effect_Sprite.h"
#include "Effect_Mesh_Instance.h"
#include "GameInstance.h"

CEffect_Prefab::CEffect_Prefab(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
    : CGameObject{pDevice, pDeviceContext}
{
}

CEffect_Prefab::CEffect_Prefab(const CEffect_Prefab& Prototype)
    : CGameObject(Prototype)
{
}

HRESULT CEffect_Prefab::Initialize_Prototype()
{
    //Tool test
    GAMEOBJECT_DESC         Desc{};
    Desc.fSpeedPerSec = 10.f;
    Desc.fRotationPerSec = XMConvertToRadians(360.0f);

    if (FAILED(__super::Initialize_Clone(&Desc)))
        return E_FAIL;

    m_fCurTime = 0.f;

    //tmp
    m_bPlaying = true;
    m_IsLoop = true;
    m_PrevTrackIdx = -1;
    m_TrackIdx = 0;

    return S_OK;
}

HRESULT CEffect_Prefab::Initialize_Clone(void* pArg)
{
    return S_OK;
}

void CEffect_Prefab::Priority_Update(_float fTimeDelta)
{
    //if (!m_bPlaying) return;

    if(!m_bPlaying && m_IsLoop)
        ResetChildren();
    else if (!m_bPlaying)
        return ;

    /* [DEBUG] */
    if (m_pGameInstance->Key_Pressing(DIK_UP, fTimeDelta))
    {
        m_pTransformCom->Go_Straight(fTimeDelta);
    }
    if (m_pGameInstance->Key_Pressing(DIK_DOWN, fTimeDelta))
    {
        m_pTransformCom->Go_Backward(fTimeDelta);
    }
    if (m_pGameInstance->Key_Pressing(DIK_LEFT, fTimeDelta))
    {
        m_pTransformCom->Go_Left(fTimeDelta);
    }
    if (m_pGameInstance->Key_Pressing(DIK_RIGHT, fTimeDelta))
    {
        m_pTransformCom->Go_Right(fTimeDelta);
    }

    for (size_t i = 0; i < m_eEventTracks.size(); ++i)
    {
        EFFECT_EVENT track = m_eEventTracks[i];
        if (!m_bEventTriggered[i] && m_fCurTime >= track.fStartTime)
        {
            switch (track.eEventType)
            {
            case EffectEventType::ACTIVATE:
                m_Children[track.iElementIdx]->Active();
                break;
            case EffectEventType::ANIMATE_SPREAD:
                m_Children[track.iElementIdx]->SetSpreadData(&track);
                break;  
            case EffectEventType::ANIMATE_ROTATE:
                m_Children[track.iElementIdx]->SetRotateData(&track);
                break;
            case EffectEventType::ANIMATE_TWINLKE:
                m_Children[track.iElementIdx]->SetTwinkleData(&track);
                break; 
            case EffectEventType::ANIMATE_LINEAR_MOVE:
                m_Children[track.iElementIdx]->SetUpwardData(&track);
                break;
            case EffectEventType::SCROLL_SPEAD:
                m_Children[track.iElementIdx]->SetUpwardData(&track.fScrollSpeed);
                break;
            }
            m_bEventTriggered[i] = true;
        }
    }

    /* [DEBUG] */
    //_bool flag = false;
    //for (_uint i = 0; m_eEventTracks.size(); ++i)
    //{
    //   if (m_bEventTriggered[i] == true)
    //       flag = true;
    //}
    //if (flag)
    //    m_fCurTime += fTimeDelta;

}

void CEffect_Prefab::Update(_float fTimeDelta)
{
    if (!m_bPlaying && !m_IsLoop) 
        return; 

    _bool   isFin{true};

    for (auto& child : m_Children)
    {
        if(child->IsActive())
        {
            child->Update(fTimeDelta);
            isFin = false;
        }
    }

    if(isFin == true)
    {
        for (bool isTriggered : m_bEventTriggered)
            if (isTriggered == true)
                break;
        m_bPlaying = false;
    }
}

void CEffect_Prefab::Late_Update(_float fTimeDelta)
{
    for (auto& child : m_Children)
    {
        if (child->IsActive())
            child->Late_Update(fTimeDelta);
    }
}

HRESULT CEffect_Prefab::Render()
{
    return S_OK;
}

void CEffect_Prefab::Add_Effect_Element(_uint EffectType, void* pArg)
{
    class CEffect_Element* newEffect {nullptr};
    
    switch (EffectType)
    {
    case ENUM_CLASS(EffectType::POINT_INSTANCE):
        newEffect = CEffect_Point_Instance::Create(m_pDevice, m_pContext, pArg);
        newEffect->Set_EffectType(EffectType);
        break;
    case ENUM_CLASS(EffectType::MESH_INSTANCE):
        newEffect = CEffect_Mesh_Instance::Create(m_pDevice, m_pContext, pArg);
        newEffect->Set_EffectType(EffectType);
        break;
    case ENUM_CLASS(EffectType::SPRITE):
        newEffect = CEffect_Sprite::Create(m_pDevice, m_pContext, pArg);
        newEffect->Set_EffectType(EffectType);
        break;
    default :
        MSG_BOX(TEXT("Effect Type Error"));
        return;
    }

    newEffect->SetParentsMatrix(m_pTransformCom->Get_WorldMatrixPtr());
    m_Children.push_back(newEffect);
}

void CEffect_Prefab::Edit_Element(_uint ChildIdx)
{
    m_Children[ChildIdx]->Edit_Element();
}

void CEffect_Prefab::Edit_TimeTrack(_uint ChildIdx)
{
    vector<EFFECT_EVENT*> SelectedElement;
    vector<string> Names;
    vector<const char*> cstr_Names;

    for(_uint i = 0; i < (_uint)m_eEventTracks.size(); ++i)
    {
        if(m_eEventTracks[i].iElementIdx == ChildIdx)
            SelectedElement.push_back(&m_eEventTracks[i]);
    }

    for(_uint i = 0; i < (_uint)SelectedElement.size(); ++i)
        Names.push_back("TimeTrack" + to_string(i));

    for(const auto& name : Names)
        cstr_Names.push_back(name.c_str());

    ImGui::Begin("[Time Track List]");

    if(cstr_Names.size() > 0)
    {
        ImGui::ListBox("TimeTracks",&m_TrackIdx,cstr_Names.data(),(_int)cstr_Names.size());
        if (m_PrevTrackIdx != m_TrackIdx)
            m_sEditingData = *SelectedElement[m_TrackIdx];

        _int EventType = ENUM_CLASS(m_sEditingData.eEventType);

        ImGui::RadioButton("Active",&EventType,ENUM_CLASS(CEffect_Prefab::EffectEventType::ACTIVATE));
        ImGui::RadioButton("Spread",&EventType,ENUM_CLASS(CEffect_Prefab::EffectEventType::ANIMATE_SPREAD));
        ImGui::RadioButton("Rotate",&EventType,ENUM_CLASS(CEffect_Prefab::EffectEventType::ANIMATE_ROTATE));
        ImGui::RadioButton("Twinkle",&EventType,ENUM_CLASS(CEffect_Prefab::EffectEventType::ANIMATE_TWINLKE));
        ImGui::RadioButton("Up",&EventType,ENUM_CLASS(CEffect_Prefab::EffectEventType::ANIMATE_LINEAR_MOVE));
        ImGui::RadioButton("Dissolve",&EventType,ENUM_CLASS(CEffect_Prefab::EffectEventType::DISSOLVE));

        m_sEditingData.eEventType = (CEffect_Prefab::EffectEventType)EventType;

        ImGui::InputFloat("StartTime : ",reinterpret_cast<_float*>(&m_sEditingData.fStartTime));
        ImGui::InputFloat("fDuration : ",reinterpret_cast<_float*>(&m_sEditingData.fDuration));

        switch(m_sEditingData.eEventType)
        {
        case CEffect_Prefab::EffectEventType::ANIMATE_SPREAD:
            ImGui::InputFloat2("Spread speed : ",reinterpret_cast<_float*>(&m_sEditingData.fSpreadSpeed));
            ImGui::InputFloat3("pivot : ",reinterpret_cast<_float*>(&m_sEditingData.fPivot));
            ImGui::Checkbox("Gravity", &m_Gravity);
            break;

        case CEffect_Prefab::EffectEventType::ANIMATE_ROTATE:
            ImGui::InputFloat2("Rotate speed : ",reinterpret_cast<_float*>(&m_sEditingData.fRotationSpeed));
            ImGui::InputFloat3("pivot : ",reinterpret_cast<_float*>(&m_sEditingData.fPivot));
            break;

        case CEffect_Prefab::EffectEventType::ANIMATE_TWINLKE:
            ImGui::InputFloat2("Scale speed : ",reinterpret_cast<_float*>(&m_sEditingData.fScaleSpeed));
            break;

        case CEffect_Prefab::EffectEventType::ANIMATE_LINEAR_MOVE:
            ImGui::InputFloat2("Upward speed : ",reinterpret_cast<_float*>(&m_sEditingData.fUpwardSpeed));
            break;
        }

        if (ImGui::Button("Edit TimeTrack"))
        {
            *SelectedElement[m_TrackIdx] = m_sEditingData;
            m_sEditingData.bGravity = m_Gravity;
        }

        if(ImGui::Button("Delete TimeTrack"))
        {
            m_eEventTracks.erase(m_eEventTracks.begin() + (SelectedElement[m_TrackIdx] - m_eEventTracks.data()));
            auto iteratorToRemove = m_bEventTriggered.begin() + m_TrackIdx;
            m_bEventTriggered.erase(iteratorToRemove);
            m_TrackIdx = 0;
        }

        m_PrevTrackIdx = m_TrackIdx;
    }

    ImGui::End();
}

void CEffect_Prefab::Add_TimeTrack(EFFECT_EVENT TrackData)
{
    if(TrackData.iElementIdx + 1 > m_Children.size())
    {
        MSG_BOX(TEXT("Element Idx Error."));
        return;
    }

    if (m_Children[TrackData.iElementIdx]->Get_EffectType() == ENUM_CLASS(EffectType::SPRITE)
        && TrackData.eEventType != EffectEventType::ACTIVATE && TrackData.eEventType != EffectEventType::DISSOLVE)
    {
        MSG_BOX(TEXT("Only Sprite can be added Active, Dissolve"));
        return;
    }

    m_eEventTracks.push_back(TrackData);
    m_bEventTriggered.push_back(false);
}

void CEffect_Prefab::RevertChanges(_uint ChildIdx)
{
    m_Children[ChildIdx]->RevertChanges();
    m_PrevTrackIdx = -1;
    m_TrackIdx = 0;
}

void CEffect_Prefab::RemoveEffect(_uint ChildIdx)
{ 
    for(auto it = m_eEventTracks.begin(); it != m_eEventTracks.end(); )
    {
        if(it->iElementIdx == ChildIdx)
        {
            it = m_eEventTracks.erase(it);
            continue;
        }
        else if(it->iElementIdx > ChildIdx)
            it->iElementIdx -= 1;
        ++it; 
    }

    Safe_Release(m_Children[ChildIdx]);
    m_Children.erase(m_Children.begin() + ChildIdx);
}

_float CEffect_Prefab::Get_MaxTrack()
{
    _float rt {0};

    for(auto& track : m_eEventTracks)
    {
        if (track.fStartTime + track.fDuration > rt)
            rt = track.fStartTime + track.fDuration;
    }

    if (rt < 1.f)
        rt = 1.f;

    return rt ;
}

void CEffect_Prefab::Setting_Loop()
{    
    ImGui::Checkbox("Loop", &m_IsLoop); 
}

void CEffect_Prefab::ResetChildren()
{
    m_bPlaying = true;
    m_fCurTime = 0.f;
    for(_uint i = 0; i < m_bEventTriggered.size(); ++i)
        m_bEventTriggered[i] = false;
    for(_uint i = 0; i < m_Children.size(); ++i)
        m_Children[i]->Reset();
}

void CEffect_Prefab::Save(_wstring filename)
{
    std::ofstream os{ filename, std::ios::binary };

    os.write(reinterpret_cast<char*>(m_Children.size()), sizeof(size_t));
    os.write(reinterpret_cast<char*>(m_eEventTracks.size()), sizeof(size_t));

    for(auto& child : m_Children)
        child->Save_Data(os);

    for(_uint i = 0; i < m_eEventTracks.size(); ++i)
        os.write(reinterpret_cast<char*>(&m_eEventTracks[i]), sizeof(EFFECT_EVENT));

    os.close();
}

CEffect_Prefab* CEffect_Prefab::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext)
{
    CEffect_Prefab* pInstance = new CEffect_Prefab(pDevice, pDeviceContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX(TEXT("Failed to Created : CEffect_Prefab"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

CGameObject* CEffect_Prefab::Clone(void* pArg)
{
    CEffect_Prefab* pInstance = new CEffect_Prefab(*this);

    if (FAILED(pInstance->Initialize_Clone(pArg)))
    {
        MSG_BOX(TEXT("Failed to Created : CEffect_Prefab"));
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CEffect_Prefab::Free()
{
    __super::Free();

    for (auto& elements : m_Children)
        Safe_Release(elements);
}





