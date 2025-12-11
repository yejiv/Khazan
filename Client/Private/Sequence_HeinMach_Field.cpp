#include "Sequence_HeinMach_Field.h"
#include "Camera_Compre.h"
#include "UI_Announce_MapName.h"
#include "GameInstance.h"
#include "ClientInstance.h"
#include "UI_HUD.h"

CSequence_HeinMach_Field::CSequence_HeinMach_Field(CCamera_Compre* pCamera)
    : m_pCamera_Compre { pCamera }
	, m_pGameInstance { CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CSequence_HeinMach_Field::Initialize(const SEQ_REQ_PLAY_DESC& tDesc)
{
    m_Id = tDesc.tId;
	m_fTime = tDesc.fStartTime;
    m_State = STATE::Playing;

	string filePath = "../../Client/Bin/Data/Camera/Animation/HeinMach";
    CClientInstance::GetInstance()->Camera_Set_Animation_Json(filePath);
    CClientInstance::GetInstance()->Set_PlayerInput(false, true);    

    return S_OK;
}

void CSequence_HeinMach_Field::Update(_float fTimeDelta)
{
    if (m_State != STATE::Playing) return;
	m_fTime += fTimeDelta;

    if (!m_isCameraStart)
    {
        static_cast<CUI_HUD*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("HUD")))->Switch_Panel(false);
        m_pCamera_Compre->Set_Animation(TEXT("HeinMach"));

        m_isCameraStart = true;
    }

    if (m_fTime >= 5.f && !m_isFieldName)
    {
		EVENT_ANNOUNCE_MAPNAME Desc = {};
		Desc.fTime = 2.f;

		Desc.iMapType = ENUM_CLASS(CUI_Announce_MapName::MAP_TYPE::HEINMACH);

		Desc.fFadeOutTime = 2.0f;

		Desc.isDissovle = true;

		m_pGameInstance->Emit_Event<EVENT_ANNOUNCE_MAPNAME>(ENUM_CLASS(EVENT_TYPE::ANNOUNCE_MAPNAME), Desc);

		m_isFieldName = true;
    }

    if (m_fTime > 14.3f && !m_isPlayerInput)
    {
        CClientInstance::GetInstance()->Set_PlayerInput(true);
        static_cast<CUI_HUD*>(CClientInstance::GetInstance()->Get_RootUI(TEXT("HUD")))->Switch_Panel(true);
        CClientInstance::GetInstance()->BGM_HeinMach_Dawn();
    }

    if (m_fTime >= 16.f)
    {              
        m_State = STATE::End;
    }
        
}

void CSequence_HeinMach_Field::Pause()
{
    if (m_State == STATE::Playing)
        m_State = STATE::Paused;
}

void CSequence_HeinMach_Field::Resume()
{
    if (m_State == STATE::Paused)
        m_State = STATE::Playing;
}

void CSequence_HeinMach_Field::StopImmediate()
{
    m_State = STATE::End;
}

void CSequence_HeinMach_Field::Jump(_float fTime)
{
	m_fTime = fTime;
}

_bool CSequence_HeinMach_Field::IsEnd() const
{
    return m_State == STATE::End;
}

CSequence_HeinMach_Field* CSequence_HeinMach_Field::Create(CCamera_Compre* pCamera)
{
    return new CSequence_HeinMach_Field(pCamera);
}

void CSequence_HeinMach_Field::Free()
{
    __super::Free();

	Safe_Release(m_pGameInstance);
	m_pCamera_Compre = nullptr;
}
