#include "Sequence_HeinMach_Field.h"
#include "Camera_Compre.h"
#include "UI_Announce_MapName.h"
#include "GameInstance.h"

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
	filePath += ".json";
	ifstream In(filePath);
	if (!In.is_open())
	{
		MSG_BOX(TEXT("UI JSON 파일 불러오기 실패"));
		In.close();
	}
	else
	{
		nlohmann::json jsonData;
		In >> jsonData;

		map<_wstring, vector<CAMERA_KEYFRAME>> Animations;
		for (auto Animation : jsonData["Animation"])
		{
			vector<CAMERA_KEYFRAME> KeyFrames;
			for (auto Ani : Animation["Animations"])
			{
				CAMERA_KEYFRAME KeyFrame{};
				KeyFrame.vTranslation.x = Ani["Translation"]["x"];
				KeyFrame.vTranslation.y = Ani["Translation"]["y"];
				KeyFrame.vTranslation.z = Ani["Translation"]["z"];
				KeyFrame.vLookAt.x = Ani["LookAt"]["x"];
				KeyFrame.vLookAt.y = Ani["LookAt"]["y"];
				KeyFrame.vLookAt.z = Ani["LookAt"]["z"];
				KeyFrame.vLookAt.w = Ani["LookAt"]["w"];
				KeyFrame.fSpeed = Ani["Speed"];
				KeyFrame.fTrackPosition = Ani["TrackPosition"];
				KeyFrame.isCurPos = Ani["isCurPos"];

				KeyFrames.push_back(KeyFrame);
			}

			Animations.emplace(AnsiToWString(Animation["Name"]), KeyFrames);
		}

		m_pCamera_Compre->Load_Animation(Animations);

	}

    return S_OK;
}

void CSequence_HeinMach_Field::Update(_float fTimeDelta)
{
    if (m_State != STATE::Playing) return;
	m_fTime += fTimeDelta;

    if (!m_isCameraStart)
    {
        m_pCamera_Compre->Set_Animation(TEXT("HeinMach"));

        m_isCameraStart = true;
    }

    if (m_fTime >= 5.f && !m_isFieldName)
    {
		EVENT_ANNOUNCE_MAPNAME Desc = {};
		//화면에 표시할 시간
		Desc.fTime = 2.f;

		//표시할 지역 이름
		Desc.iMapType = ENUM_CLASS(CUI_Announce_MapName::MAP_TYPE::HEINMACH);

		//페이드 아웃 시간
		Desc.fFadeOutTime = 2.0f;

		//디죨부 처리 여부
		Desc.isDissovle = true;

		//이벤트 발생시키기	
		m_pGameInstance->Emit_Event<EVENT_ANNOUNCE_MAPNAME>(ENUM_CLASS(EVENT_TYPE::ANNOUNCE_MAPNAME), Desc);

		m_isFieldName = true;
    }

    if (m_fTime >= 16.f)
        m_State = STATE::End;
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
