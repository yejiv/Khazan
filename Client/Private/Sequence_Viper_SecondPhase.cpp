#include "Sequence_Viper_SecondPhase.h"
#include "GameInstance.h"
#include "ClientInstance.h"
#include "Transform.h"
#include "GameObject.h"
#include "Viper.h"
#include "Camera_Compre.h"
#include "AS_CutScene_Start_Viper.h"
#include "FSM_Viper.h"
#include "CharacterVirtual.h"
#include "Khazan_GSword.h"
#include "AI_Controller_Viper.h"
#include "SkipButton.h"
#include "Obelisk.h"

CSequence_Viper_SecondPhase::CSequence_Viper_SecondPhase(CViper* pViper, CKhazan_GSword* pKhazan)
    : m_pGameInstance{ CGameInstance::GetInstance() }
    , m_pClientInstance{ CClientInstance::GetInstance() }
    , m_pViper{ pViper }
    , m_pKhazan{ pKhazan }
{
    Safe_AddRef(m_pGameInstance);
    Safe_AddRef(m_pClientInstance);
}

HRESULT CSequence_Viper_SecondPhase::Initialize(const SEQ_REQ_PLAY_DESC& tDesc)
{


    return S_OK;
}

void CSequence_Viper_SecondPhase::Update(_float fTimeDelta)
{
    m_fTime += fTimeDelta;
    

    Skip_KeyInput(fTimeDelta);

    if (!m_isSkip)
    {
        if (m_fTime >= 3.f)
            m_isEnd = true;
    }
    else {
        m_fSkipTime += fTimeDelta;
        if (m_fSkipTime > 0.f && !m_isSkipFadeOut)
        {
            CSkipButton::SKIPBUTTON_DESC SkipBtnDesc{};
            SkipBtnDesc.isOpen = false;
            m_pClientInstance->UI_UpdateSwitch(TEXT("SkipButton"), &SkipBtnDesc);
            m_pClientInstance->Fade_Out();
            m_isSkipFadeOut = true;
        }

        if (m_fSkipTime > 1.f && !m_isSkipCameraSet)
        {
            m_pClientInstance->Camera_Force_AniEnd();
            m_isSkipCameraSet = true;
        }

        if (m_fSkipTime > 2.f && !m_isSkipFadeIn)
        {
            m_pClientInstance->Fade_In();
            m_isSkipFadeIn = true;
        }

        if (m_fSkipTime > 3.f && !m_isEnd)
        {
            m_isEnd = true;
        }
    }
}

void CSequence_Viper_SecondPhase::Pause()
{

}

void CSequence_Viper_SecondPhase::Resume()
{

}

void CSequence_Viper_SecondPhase::StopImmediate()
{

}

void CSequence_Viper_SecondPhase::Jump(_float fTime)
{
    m_fTime = fTime;
}

_bool CSequence_Viper_SecondPhase::IsEnd() const
{
    return m_isEnd;
}

void CSequence_Viper_SecondPhase::Push_Obelisk(CObelisk* pObelisk)
{
    m_Obelisks.push(pObelisk);
}

void CSequence_Viper_SecondPhase::Skip_KeyInput(_float fTimeDelta)
{
    if (!m_isSkipKey)
    {
        if (m_pGameInstance->Key_Down(DIK_G))
        {
            CSkipButton::SKIPBUTTON_DESC SkipBtnDesc{};
            SkipBtnDesc.isOpen = true;
            SkipBtnDesc.Event = [this]() {
                Set_Skip(true);
                };

            CClientInstance::GetInstance()->UI_UpdateSwitch(TEXT("SkipButton"), &SkipBtnDesc);

            m_isSkipKey = true;
        }
    }


}

CSequence_Viper_SecondPhase* CSequence_Viper_SecondPhase::Create(CViper* pViper, CKhazan_GSword* pKhazan)
{
    return new CSequence_Viper_SecondPhase(pViper, pKhazan);
}

void CSequence_Viper_SecondPhase::Free()
{
    __super::Free();

    Safe_Release(m_pGameInstance);

}
