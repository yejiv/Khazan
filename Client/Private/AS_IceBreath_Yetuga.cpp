#include "AS_IceBreath_Yetuga.h"
#include "AI_Controller.h"
#include "Yetuga.h"
#include "GameInstance.h"
#include "BlackBoard.h"
#include "FSM_Yetuga.h"
#include "Body_Yetuga.h"

CAS_IceBreath_Yetuga::CAS_IceBreath_Yetuga()
{
    m_IceBreathChannels.resize(10);
}

void CAS_IceBreath_Yetuga::Enter(CStateMachine* pFSM, CGameObject* pOwner)
{
    m_eState = PHASE::START;

    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));

    if (PHASE::START == m_eState)
    {
        pModel->Set_Animation(21);
        pYetuga->Look_Target();
    }

    m_IceBreathChannels =
    {
        pYetuga->Get_SoundChannel(ENUM_CLASS(MONSFX::EFFECT1)),
        pYetuga->Get_SoundChannel(ENUM_CLASS(MONSFX::EFFECT2)),
        pYetuga->Get_SoundChannel(ENUM_CLASS(MONSFX::EFFECT3)),
    };
    CBlackBoard* BB = pYetuga->Get_Controller()->Get_BlackBoard();


    m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_icebreath1_01 (SFX).wav"), 1.f);

}

void CAS_IceBreath_Yetuga::Update(CStateMachine* pFSM, CGameObject* pOwner, _float fTimeDelta)
{
    CYetuga* pYetuga = static_cast<CYetuga*>(pOwner);
    CModel* pModel = static_cast<CModel*>(pYetuga->Get_Body()->Get_Component(TEXT("Com_Model")));
    CBlackBoard* BB = pYetuga->Get_Controller()->Get_BlackBoard();


    switch (m_eState)
    {
    case PHASE::START:
        if (BB->Get_Value<_bool>(pYetuga->Get_Name(), "isReadyiceBreath"))
        {
            BB->Set_Value<_bool>(pYetuga->Get_Name(), "isReadyiceBreath", false);
            pModel->Set_Animation(49);
            m_pGameInstance->PlaySoundOnce(TEXT("Mon_efx_yetuga_icebreath1_shot_01 (SFX).wav"), 1.f);

            m_eState = PHASE::ROOP;
        }
        break;
    case PHASE::ROOP:
    {
        if (!BB->Get_Value<_bool>(pYetuga->Get_Name(), "isIceCreate"))
            break;

        m_fIceBreathTimeAcc += fTimeDelta;

        if (m_fIceBreathTimeAcc < m_fIceBreathSoundInterval)
            break;
        _uint iRand = m_pGameInstance->Rand(0, (_uint)pYetuga->Get_IceBreathSound().size() - 1);

        FMOD_CHANNEL** pChannel = m_IceBreathChannels[m_iIceBreathSoundChannelIndex];

        m_iIceBreathSoundChannelIndex = (m_iIceBreathSoundChannelIndex + 1) % m_IceBreathChannels.size();

        m_pGameInstance->PlaySoundOnce(
            pYetuga->Get_IceBreathSound()[iRand],
            pYetuga->Get_Position(),
            pChannel,
            10.f);

        m_fIceBreathTimeAcc = 0.f;
    }

    break;
    }

    if (pModel->Play_Animation(fTimeDelta))
    {

        if (PHASE::START == m_eState)
            BB->Set_Value<_bool>(pYetuga->Get_Name(), "isReadyiceBreath", true);

        if (PHASE::ROOP == m_eState)
        {
            CBlackBoard* BB = pYetuga->Get_Controller()->Get_BlackBoard();
            BB->Set_Value<_bool>(pYetuga->Get_Name(), "isIceBreathFinished", true);
        }

    }


}

void CAS_IceBreath_Yetuga::Exit(CStateMachine* pFSM, CGameObject* pOwner)
{
    for (auto& pChannel : m_IceBreathChannels)
    {
        if (pChannel && *pChannel)
            m_pGameInstance->StopByChannel(pChannel);
    }

    m_pGameInstance->Stop_Effect(ENUM_CLASS(LEVEL::HEINMACH), TEXT("Yetuga_Ice"));
}

CAS_IceBreath_Yetuga* CAS_IceBreath_Yetuga::Create()
{
    return new CAS_IceBreath_Yetuga();
}

void CAS_IceBreath_Yetuga::Free()
{
    __super::Free();
}