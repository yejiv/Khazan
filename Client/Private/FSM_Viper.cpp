#include "FSM_Viper.h"

#pragma region PHASE1.h

#include "AS_Idle_Viper.h"
#include "AS_Walk_Viper.h"
#include "AS_Run_Viper.h"
#include "AS_Quick2Hit_Viper.h"
#include "AS_Slow2Hit_VIper.h"
#include "AS_LockOn_Viper.h"
#include "AS_StingSlashCombo_Viper.h"
#include "AS_Slow3Hit_Viper.h"
#include "AS_SlashBackJump_Viper.h"
#include "AS_TurnAttack_Viper.h"
#include "AS_JumpSmash_Viper.h"
#include "AS_Devour_Viper.h"
#include "AS_SideMove_P1_Viper.h"
#include "AS_5HitCombo_Viper.h"
#include "AS_ThrowBlade_Viper.h"
#include "AS_StingGrab_Viper.h"
#include "AS_Groggy_Viper.h"
#include "AS_Hit_Viper.h"
#include "AS_CutScene_Start_Viper.h"
#include "AS_CutScene_2Phase_Viper.h"

#pragma endregion

#pragma region PHASE2.h
#include "AS_P2_Run_Viper.h"
#include "AS_P2_HandStomp_Viper.h"
#include "AS_P2_HandStompStr_Viper.h"
#include "AS_P2_HandSwing2Hit_Viper.h"
#include "AS_P2_HandUpper_Viper.h"
#include "AS_P2_SlashDobule_Viper.h"
#include "AS_P2_SlashStomp_Viper.h"
#include "AS_P2_HandSwing3Hit.h"
#include "AS_P2_FakeRunAttack_Viper.h"
#include "AS_P2_DashUpper_Viper.h"
#include "AS_P2_DashUpperStr_Viper.h"
#include "AS_P2_BackJump_Viper.h"
#include "AS_P2_SideMove_Viper.h"
#include "AS_P2_JumpAttack_Viper.h"
#include "AS_P2_SwingCombo_VIper.h"
#include "AS_P2_ThrowRock_VIper.h"
#include "AS_P2_DashDrift_Viper.h"
#include "AS_P2_SwingRound_Viper.h"
#include "AS_P2_Roar_Viper.h"
#include "AS_P2_LockOn_Viper.h"

#pragma endregion


CFSM_Viper::CFSM_Viper()
{

}


CAS_CutScene_Start_Viper* CFSM_Viper::Get_CutScene_Start_Viper()
{
    CAS_CutScene_Start_Viper* pCutScneState = static_cast<CAS_CutScene_Start_Viper*>(m_States[ENUM_CLASS(VIPER_STATE_P1::CUTSCENE_START)]);

    return pCutScneState;
}

HRESULT CFSM_Viper::Initialize()
{
#pragma region PHASE1

    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::CUTSCENE_START), CAS_CutScene_Start_Viper::Create())))
        return E_FAIL;

    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::IDLE), CAS_Idle_Viper::Create())))
        return E_FAIL;

    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::WALK), CAS_Walk_Viper::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::RUN), CAS_Run_Viper::Create())))
        return E_FAIL;

    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::LOCKON), CAS_LockOn_Viper::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::QUICK2HIT), CAS_Quick2Hit_Viper::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::SLOW2HIT), CAS_Slow2Hit_VIper::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::STINGSLASHCOMBO), CAS_StingSlashCombo_Viper::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::THROWBLADE), CAS_ThrowBlade_Viper::Create())))
        return E_FAIL;


    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::SLOW3HIT), CAS_Slow3Hit_Viper::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::SLASHBACKJUMP), CAS_SlashBackJump_Viper::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::TURNATTACK), CAS_TurnAttack_Viper::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::JUMPSMASH), CAS_JumpSmash_Viper::Create())))
        return E_FAIL;


    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::DIVOUR), CAS_Devour_Viper::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::SIDEMOVE), CAS_SideMove_P1_Viper::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::COMBO5HIT), CAS_5HitCombo_Viper::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::STINGGRAB), CAS_StingGrab_Viper::Create())))
        return E_FAIL;

#pragma endregion


#pragma region PHASE2

    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::P2_RUN), CAS_P2_Run_Viper::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::P2_HANDSTOMP), CAS_P2_HandStomp_Viper::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::P2_HANDSTOMPSTR), CAS_P2_HandStompStr_Viper::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::P2_HANDSWING2HIT), CAS_P2_HandSwing2Hit_Viper::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::P2_HANDUPPER), CAS_P2_HandUpper_Viper::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::P2_SLASHDOUBLE), CAS_P2_SlashDobule_Viper::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::P2_SLASHSTOMP), CAS_P2_SlashStomp_Viper::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::P2_HANDSWING3HIT), CAS_P2_HandSwing3Hit::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::P2_FAKERUNATTACK), CAS_P2_FakeRunAttack_Viper::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::P2_DASHUPPER), CAS_P2_DashUpper_Viper::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::P2_DASTUPPERSTR), CAS_P2_DashUpperStr_Viper::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::P2_BACKJUMP), CAS_P2_BackJump_Viper::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::P2_SIDEMOVE), CAS_P2_SideMove_Viper::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::P2_JUMPATTACK), CAS_P2_JumpAttack_Viper::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::P2_SWINGCOMBO), CAS_SwingCombo_VIper::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::P2_THROWROCK), CAS_P2_ThrowRock_VIper::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::P2_DASHDRIFT), CAS_P2_DashDrift_Viper::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::P2_SWINGROUND), CAS_P2_SwingRound_Viper::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::P2_ROAR), CAS_P2_Roar_Viper::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::P2_LOCKON), CAS_P2_LockOn_Viper::Create())))
        return E_FAIL;

#pragma endregion

    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::GROGGY), CAS_Groggy_Viper::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::HIT), CAS_Hit_Viper::Create())))
        return E_FAIL;
    if (FAILED(Add_State(ENUM_CLASS(VIPER_STATE_P1::CUTSCENE_PHASE2), CAS_CutScene_2Phase_Viper::Create())))
        return E_FAIL;


    m_pCurrentState = m_States[ENUM_CLASS(VIPER_STATE_P1::IDLE)];
    Safe_AddRef(m_pCurrentState);
    if (nullptr == m_pCurrentState)
        return E_FAIL;

    return S_OK;
}

void CFSM_Viper::Update(CGameObject* pOwner, _float fTimeDelta)
{
    __super::Update(pOwner,fTimeDelta);
}

CFSM_Viper* CFSM_Viper::Create()
{
    CFSM_Viper* pInstance = new CFSM_Viper();
    if (FAILED(pInstance->Initialize()))
    {
        Safe_Release(pInstance);
        MSG_BOX(TEXT("Failed Create : CFSM_Viper"));
    }
    return pInstance;
}

void CFSM_Viper::Free()
{
    __super::Free();
}
